#include <WiFi.h>


#define ssid  "klimaServer"
#define password "password"

void WifiSetup();
int SendData(float TMP, float CO2);