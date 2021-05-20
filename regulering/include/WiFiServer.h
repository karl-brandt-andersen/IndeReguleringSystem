#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define ssid  "SSID"                    //LAN
#define password "password"             //LAN
#define soft_ap_ssid "klimaServer"
#define soft_ap_password "password"
#define SensorSnoozeTime "60"

void HandleSensor();
void HandleKontrol();
void WiFisetup();
void handleRoot();
void handleNotFound();
void handleServer();
float GetTemp();
int GetCO2();
float GetSetTemp();
int GetSetCO2();