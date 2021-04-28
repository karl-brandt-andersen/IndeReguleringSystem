#include <Arduino.h>
#include <hardwarePWM.h>


void setup() {
  PWMsetupActiveLOW;
  setPWM(30);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("this is the regulering");
  delay(500);
}