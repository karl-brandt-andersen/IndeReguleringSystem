#include <Arduino.h>
#include <hardwarePWM.h>
#include <WiFiServer.h>
#include <OneWire.h>

// Setup a oneWire instance to communicate with any OneWire device
OneWire  ds(19);  // on pin 2 (a 4.7K resistor is necessary)

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
int timeConstant = 60;

float desiredValueTemp = 22;
float outsideTemp = 13;
float IntergralSumValueHeat = 0;
float lastError = 0;

float PropValueTemp = 50;
float InteValueTemp = 2;
float DiffValueTemps = 2;

void motorOne(int speed);
void motorTwo(int speed);
float PICalculationVent(int readValue);
float PIDCalculationHeat(float readValue);
void PIRegulationVent(float CorrectionValue);


float PIDCalculationHeat(float readValue){
	float error = desiredValueTemp - readValue;
  //printf("error: %0.2f\n", error);
  long time = millis() - lastHeat;

  long workingTime = time/1000;

	IntergralSumValueHeat += error*workingTime;

  //printf("IntergralSumValueHeat: %0.2f\n", IntergralSumValueHeat);
	float correctionValue = PropValueTemp*error+IntergralSumValueHeat*InteValueTemp+ (error-lastError)/workingTime*DiffValueTemps;
	//printf("correctionValue: %0.2f\n", correctionValue);
  lastError = error;
  lastHeat += time;
	return correctionValue;
}


float PICalculationVent(int readValue){
	
	int error = readValue-desiredValueCarbon;
	float correctionValue;
	IntergralSumValueVent += (float)(error-lastvalue)*timeConstant;
	correctionValue = IntergralSumValueVent*(PropValueVent/InteValueVent)+PropValueVent*error;
	return correctionValue;
}

int RegulationVent(int level){
  int amount =  4;
  int CO = level;
  float Volume = 36.98;

  float massCarbonDes = 68.83119814*CO;

  float prod = 0.00001207479;
  float co2MassPart = 0.000626;
  float dense = 1.225;

  float flow = -(amount*prod*Volume*dense)/((Volume*dense*co2MassPart-massCarbonDes)*dense);

  float flowProportionality, flowOffset;

  int adc = flow*flowProportionality + flowOffset;

  return adc;



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

/*  /FLYT EFTER TEST */

int HeaterTemp();

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
  Serial.begin(115200);
  Serial.println("A");
  PWMsetup;   //Heater
  setPWM(75); //Heater 75%
  ledcSetup(ledChannel, freq, resolution); // configure LED PWM functionalitites
  ledcAttachPin(fanPin, ledChannel);  // attach the channel to the GPIO to be controlled
  ledcWrite(ledChannel, 300);
  fanSetup;       //Set tacho pin to input with pullup to vcc
  WiFisetup();
}

void loop() {
  while (HeaterTemp());
  printf("Speed: %.2f RPM\n", GetFanRPM());
  printf("TMP: %0.2f, CO2: %d\n", GetTemp(), GetCO2());
  desiredValueTemp = GetSetTemp();
  printf("setTMP: %0.2f, setCO2: %d\n", desiredValueTemp, GetSetCO2());
 
  float heat = PIDCalculationHeat(Htemp);
  printf("heat : %0.2f. Htemp: %0.2f\n", heat, Htemp);
  if (heat < 0) heat = 0;
  if (heat > 100) heat = 100;
  printf("%0.2f", heat);
  setPWM(heat);
  //PIRegulationVent(PICalculationVent(GetCO2()));
   delay(500);

  if (Serial.available()){
    printf("Restart\n");
    Serial.read();
    IntergralSumValueHeat = 0;
  }


  handleServer();
  
}

int HeaterTemp(){
  byte i;
  byte data[12];
  byte addr[8];
  float celsius;
  
  while ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
	return 1;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 1;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  
	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
	if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	//// default is 12 bit resolution, 750 ms conversion time
  
  celsius = (float)raw / 16.0;
  Htemp = celsius;

  return 0;
}