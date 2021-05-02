#include <Arduino.h>
#include <WiFiHandler.h>




void Sleep(int Time) {
  if (Time) {
    Serial.println("Sleeping");
    esp_sleep_enable_timer_wakeup(Time * 1000000);
    esp_deep_sleep_start();
  }
}


void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WifiSetup();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  float TEMP = random(10, 30);
  float CO2 = random(10, 30);
  Sleep(SendData(TEMP, CO2));
  delay(500);
}