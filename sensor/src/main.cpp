#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#include <ESPAsyncWebServer.h>

#include <AsyncTCP.h>

void sensorWifiSetup();
int sendSensorData();


const char* soft_ap_ssid = "klimaServer";
const char* soft_ap_password = "password";

const char* serverName = "http://192.168.4.1/sensData";

String dataString;

int type = 7;

double temp = 23.432;
double co2 = 670;

IPAddress ip;

//------------------------------------- TODO-----------------
/*
making the wifi reconnect when it loses the signal, if it cant connect for some time
it should go to sleep, and try again.

if the server sends the wrong signal it should make
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensorWifiSetup();
  ip = WiFi.localIP();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("this is the sensor");

  sendSensorData();
  delay(1000);


}



void sensorWifiSetup(){
  WiFi.begin(soft_ap_ssid, soft_ap_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to AP WiFI");
  }

  Serial.print("ESP local ip is : ");
  Serial.println(WiFi.localIP());


}

int sendSensorData(int temp, int co2){
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
       http.begin(serverName);
       dataString = "7,32.3,5564,654"; // making a string from the temp and co2 inputs of the func
       Serial.println(dataString);
       http.addHeader("Content-Type", "text/plain");
       int httpResponseCode = http.POST(dataString);

       Serial.println(httpResponseCode);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }

    
}