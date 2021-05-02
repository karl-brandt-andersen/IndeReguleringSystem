#include "WiFiServer.h"

const char * headerKeys[] = {"TMP", "CO2"} ;
const size_t numberOfHeaders = 2;
WebServer server(80);

void HandleSensor(){
  float TMP = server.header("TMP").toFloat();
  int CO2 = server.header("CO2").toInt();
  Serial.println(TMP);
  Serial.println(CO2);
  if (CO2 && TMP)server.sendHeader("SLP", SensorSnoozeTime);
  server.send(200, "text/plain", "");
}

void WiFisetup(void) {

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password,5,1,4); //Hidden network on CH5, maximum 4 clients.
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("ESP soft AP ip is : ");
  Serial.println(WiFi.softAPIP());
  Serial.print("ESP local ip is : ");
  Serial.println(WiFi.localIP());
  

  server.on("/", handleRoot);
  server.on("/S", HandleSensor);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  server.collectHeaders(headerKeys, numberOfHeaders);
  Serial.println("HTTP server started");
}

void handleRoot() {
    char temp[400];
  snprintf(temp, 400,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
  </body>\
</html>");
  server.send(200, "text/html", temp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleServer(){
    server.handleClient();
}