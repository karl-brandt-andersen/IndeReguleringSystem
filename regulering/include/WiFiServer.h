#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define ssid  "SSID"                    //LAN
#define password "password"             //LAN
#define soft_ap_ssid "klimaServer"
#define soft_ap_password "password"
#define SensorSnoozeTime "20"

void HandleSensor();
void WiFisetup();
void handleRoot();
void handleNotFound();
void handleServer();