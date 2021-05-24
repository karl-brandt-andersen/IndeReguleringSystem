#include <WiFi.h>


#define ssid  "klimaServer"
#define password "password"

void WifiSetup();
int SendData(float stemp, int sco2);
int getCO2();
float getTMP();