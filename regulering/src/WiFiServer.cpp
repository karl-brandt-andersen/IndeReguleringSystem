#include "WiFiServer.h"

const char * headerKeys[] = {"TMP", "CO2", "STE", "SCO"};
const size_t numberOfHeaders = 4;
WebServer server(80);
float TMP = 22;
float setTMP = 24.6;
int CO2 = 0;
int setCO2 = 0;

float GetTemp(){
	return TMP;
}
int GetCO2(){
	return CO2;	
}
float GetSetTemp(){
	return setTMP;
}
int GetSetCO2(){
	return setCO2;	
}

void HandleSensor(){
  TMP = server.header("TMP").toFloat();		//read TMP header field
  CO2 = server.header("CO2").toInt();		//read CO2 header field
  Serial.println(TMP);
  Serial.println(CO2);
  if (CO2 && TMP) server.sendHeader("SLP", SensorSnoozeTime);	//If we got data, send sleeping interval back.
  server.send(200, "text/plain", "");		//Send header without any html data.
}

void HandleKontrol(){
  float tempTMP = server.header("STE").toFloat();	//read STE (set temperature) header field
  int tempCO2 = server.header("SCO").toInt();		//read SCO (set CO2) header field
  if (tempTMP) setTMP = tempTMP;					//If its valid, save it.
  if (tempCO2) setCO2 = tempCO2;					//If its valid, save it.
  Serial.println(tempTMP);
  Serial.println(tempCO2);
  server.sendHeader("TMP", String(TMP));			//Send current temperaure in return
  server.sendHeader("CO2", String(CO2));			//Send current Co2 in return
  server.send(200, "text/plain", "");				//Send header without any html data.
}

void WiFisetup(void) {

  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password,5,1,4); //Hidden network on CH5, maximum 4 clients.
  //WiFi.begin(ssid, password); 	//Dont connect, not used

  server.on("/", handleRoot);		//Root of server.
  server.on("/S", HandleSensor);	//Sensor handling
  server.on("/K", HandleKontrol);	//Controlpanel handling.
  server.onNotFound(handleNotFound);//Error handling.
  server.begin();					//Starts the server.
  server.collectHeaders(headerKeys, numberOfHeaders); //Make the library collect specific headers.
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
  server.send(200, "text/html", temp); //On root send a generic website. (not used)
}

void handleNotFound() {
  String message = "File Not Found\n\n";	//If user request a directory that does not exists, notify user.
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
    server.handleClient();	//Handles http requests.
}
