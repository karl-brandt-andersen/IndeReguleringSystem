#include <Arduino.h>
#include <hardwarePWM.h>
#include <WiFiServer.h>

#define fanSensePin 22
#define fanSetup pinMode(fanSensePin, INPUT_PULLUP);       //Set tacho pin to input with pullup to vcc


const int ledPin = 21;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10;
 

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
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, 1024);
  Serial.begin(115200);
  Serial.println("this is the regulering");
  fanSetup;       //Set tacho pin to input with pullup to vcc
  WiFisetup();
}

void loop() {
  printf("Speed: %.2f RPM\n", GetFanRPM());
  printf("TMP: %0.2f, CO2: %d\n", GetTemp(), GetCO2());
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
}
