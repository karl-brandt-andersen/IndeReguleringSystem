#include <Arduino.h>
#include <hardwarePWM.h>

#define fanSensePin 22

float GetFanRPM(){
  float puls = pulseIn(fanSensePin, HIGH);  //Mål pulsen på sense i uS
  /*Få uS til S, X*=1000000, 
    Tag reciproc for at få hZ, X = 1/X, 
    Gang med 4 fordi der er 2 perioder i en rotation og vi måler en halv.*/
  if (puls) puls = 4000000/ puls;           
  return puls;
}

void setup() {
  PWMsetup;
  setPWM(20);
  Serial.begin(115200);
  Serial.println("this is the regulering");

  pinMode(fanSensePin, INPUT_PULLUP);       //Set tacho pin to input with pullup to vcc
}

void loop() {
  printf("Speed: %.2f RPM\n", GetFanRPM());
  delay(500);
}
