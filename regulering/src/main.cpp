#include <Arduino.h>
#include <hardwarePWM.h>
#include <WiFiServer.h>

#define fanSensePin 22
#define fanSetup pinMode(fanSensePin, INPUT_PULLUP);       //Set tacho pin to input with pullup to vcc

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
  setPWM(20);
  Serial.begin(115200);
  Serial.println("this is the regulering");
  fanSetup;       //Set tacho pin to input with pullup to vcc
  WiFisetup();
}

void loop() {
  printf("Speed: %.2f RPM\n", GetFanRPM());
  delay(500);
  handleServer();
}
