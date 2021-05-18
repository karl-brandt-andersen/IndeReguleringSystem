#include <Arduino.h>
#include <hardwarePWM.h>
#include <WiFiServer.h>

#define fanSensePin 22
#define fanSetup pinMode(fanSensePin, INPUT_PULLUP);       //Set tacho pin to input with pullup to vcc
float Htemp = 20.0;

const int fanPin = 21;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10;

/*  FLYT EFTER TEST*/
int desiredValueCarbon;
float IntergralSumValueVent = 0;
float mi = 1;
int lastvalue;
long lastHeat = 0;

float PropValueVent = 1;
float InteValueVent = 1;

int minimumVent = 300; //juster
int timeConstant = 0;
int maximumResolution = (int)(pow(2,12)-1);

float desiredValueTemp = 20;
float outsideTemp = 10;
float IntergralSumValueHeat = 0;
float lastError = 0;

float PropValueTemp = 1;
float InteValueTemp = 1;
float DiffValueTemps = 1;

void motorOne(int speed);
void motorTwo(int speed);
float PICalculationVent(int readValue);
float PIDCalculationHeat(float readValue);
void PIRegulationVent(float CorrectionValue);


float PIDCalculationHeat(float readValue){
	float error = desiredValueTemp - readValue;
  //printf("error: %0.2f\n", error);
  long time = millis() - lastHeat;
	IntergralSumValueHeat += error*time;
  
  //printf("IntergralSumValueHeat: %0.2f\n", IntergralSumValueHeat);
	float correctionValue = PropValueTemp*error+IntergralSumValueHeat*InteValueTemp+ (error-lastError)/time*DiffValueTemps;
	//printf("correctionValue: %0.2f\n", correctionValue);
  lastError = error;
  lastHeat += time;
	return correctionValue/1000;
}


float PICalculationVent(int readValue){
	
	int error = readValue-desiredValueCarbon;
	float correctionValue;
	IntergralSumValueVent += (float)(error-lastvalue)*timeConstant;
	correctionValue = IntergralSumValueVent*(PropValueVent/InteValueVent)+PropValueVent*error;
	return correctionValue;
}

void PIRegulationVent(float correctionValue){
	
	if (correctionValue<minimumVent){
		motorOne(minimumVent);
		motorTwo(minimumVent);
	} else{
		motorOne(correctionValue);
		motorTwo(correctionValue);
	}
}

void motorOne(int speed){
    if (speed > 1024) speed = 1024;
	  ledcWrite(ledChannel, 1024);
}


void motorTwo(int speed){
	/* WIFI SEND, NOT IMPEMENTED */
}

void setUpPWM(int i, int z){
	ledcSetup(0, 5000, 12);
	ledcSetup(2,5000,12);
	
	ledcAttachPin(16, 0);
	ledcAttachPin(17,2);
}

/*  /FLYT EFTER TEST */

 

float GetFanRPM(){
  float puls = pulseIn(fanSensePin, HIGH);  //Mål pulsen på sense i uS
  /*Få uS til S, K=1000000, 
    Gang K med 1/4 fordi der er 2 perioder i en rotation og vi måler en halv.
    Gang K med 60 for at få RPM.
    RPM = K/puls = 1000000/(puls*4)*60 */
  if (puls) puls = 15000000/puls;           
  return puls;
}

void setup() {
  PWMsetup;
  setPWM(75);
    // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(fanPin, ledChannel);
  ledcWrite(ledChannel, 0*1024);
  Serial.begin(115200);
  Serial.println("this is the regulering");
  fanSetup;       //Set tacho pin to input with pullup to vcc
  WiFisetup();
}

void loop() {
  //printf("Speed: %.2f RPM\n", GetFanRPM());
  //printf("TMP: %0.2f, CO2: %d\n", GetTemp(), GetCO2());
  delay(500);
  handleServer();
  /*
  for (int i = 1020; i > 50; i=i-10){
    ledcWrite(ledChannel, i);
    delay(3000);
    int rpm = GetFanRPM();
    float CFM = 52/(rpm/200);
    printf("%.2f; %d; %.2f\n", rpm,i, CFM);
  }
  */
  float heat = PIDCalculationHeat(5.0);
  printf("heat : %0.2f\n", heat);
  delay(500);
  heat = PIDCalculationHeat(13);
  printf("heat : %0.2f\n", heat);
  delay(500);
  heat = PIDCalculationHeat(20);
  printf("heat : %0.2f\n", heat);
  setPWM(heat > 100? 100: heat)
  //PIRegulationVent(PICalculationVent(GetCO2()));
  
}
