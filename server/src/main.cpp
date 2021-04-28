#include <Arduino.h>

#include <WiFi.h>


const char* wifi_network_ssid = "A.S";
const char* wifi_network_password = "1elefantmed8sko";

const char* soft_ap_ssid = "klimaServer";
const char* soft_ap_password = "password";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(wifi_network_ssid, wifi_network_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFI");
  }

  Serial.print("ESP soft AP ip is : ");
  Serial.println(WiFi.softAPIP());

  Serial.print("ESP local ip is : ");
  Serial.println(WiFi.localIP());
  
  

}

void loop() {
  // put your main code here, to run repeatedly:

}