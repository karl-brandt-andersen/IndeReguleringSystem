#include <Arduino.h>
#include <hardwarePWM.h>
#include <WiFiServer.h>
#include <OneWire.h>

// Setup a oneWire instance to communicate with any OneWire device
OneWire  ds(19);  // on pin 19 (a 4.7K resistor is necessary)

#define fanSensePin 22
#define fanSetup pinMode(fanSensePin, INPUT_PULLUP);       //Set tacho pin to input with pullup to vcc
float Htemp = 20.0;

void motorOne(int speed);
void motorTwo(int speed);
float PIDCalculationHeat(float readValue);
int RegulationVent(int level);
void PIRegulationVent(float CorrectionValue);
int HeaterTemp();

// setting PWM properties
const int fanPin = 21;      //PWM pin to control voltage on fan.
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10;

// PID controller
long lastHeat = 0;
int minimumVent = 300;

float desiredValueTemp = 22;
float IntergralSumValueHeat = 0;
float lastError = 0;

float PropValueTemp = 50;
float InteValueTemp = 2;
float DiffValueTemps = 2;

float PIDCalculationHeat(float readValue){
	float error = desiredValueTemp - readValue;
  long time = millis() - lastHeat;
  long workingTime = time/1000;
	IntergralSumValueHeat += error*workingTime;
	float correctionValue = PropValueTemp*error + IntergralSumValueHeat*InteValueTemp + (error-lastError)/workingTime*DiffValueTemps;
  lastError = error;
  lastHeat += time;
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
  PWMsetup;                   //Heater PWM setup.
  setPWM(75);                 //Heater 75%
  ledcSetup(ledChannel, freq, resolution);  // configure LED PWM functionalitites
  ledcAttachPin(fanPin, ledChannel);        // attach the channel to the GPIO to be controlled
  ledcWrite(ledChannel, 300); //Set voltage on motor.
  fanSetup;                   //Set tacho pin to input with pullup to vcc
  WiFisetup();                
}

void loop() {
  while (HeaterTemp());             //Update Htemp with temperature redo if it gets an error.
  desiredValueTemp = GetSetTemp();  //Set temperature target to what the controlpanel set temprature.
  /* Debug messages */
  printf("Speed: %.2f RPM\n", GetFanRPM()); 
  printf("TMP: %0.2f, CO2: %d\n", GetTemp(), GetCO2());
  printf("setTMP: %0.2f, setCO2: %d\n", desiredValueTemp, GetSetCO2());

  float heat = PIDCalculationHeat(Htemp); //Get PID output.
  printf("heat : %0.2f. Htemp: %0.2f\n", heat, Htemp);  //Heater system debug messages.
  if (heat < 0) heat = 0;       //Heater cant heat negative
  if (heat > 100) heat = 100;   //Heater cant heat more than 100%
  setPWM(heat);                 //Set heater to heat with given percentage.
  delay(500);

  /* Debug reset for PID */
  if (Serial.available()){
    printf("Restart\n");
    Serial.read();
    IntergralSumValueHeat = 0;
  }

  handleServer();   //Handle HTTP clients.
}

int HeaterTemp(){
  byte i;
  byte data[12];
  byte addr[8];
  float celsius;
  
  while ( !ds.search(addr)) {
    ds.reset_search();    //Search for DS18b20
    delay(250);
	return 1;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 1;           //If CRC is invalid abort.
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);              // Wait for conversion
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);           // Read Scratchpad and data.

  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();    //Copy 9 bytes of data to the buffer data.
  }

  int16_t raw = (data[1] << 8) | data[0]; //make 2 temprature bytes into 1 16 bit byte.
  
	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
	if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	//// default is 12 bit resolution, 750 ms conversion time
  
  celsius = (float)raw / 16.0;    //Calculate celsius.
  Htemp = celsius;
  return 0;
}