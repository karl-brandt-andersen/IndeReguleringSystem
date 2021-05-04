#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#include <ESPAsyncWebServer.h>

#include <AsyncTCP.h>

#include <Arduino_JSON.h>

void serverSetup();
void HttpListner();
void APIMaker();



const char* wifi_network_ssid = "A.S";
const char* wifi_network_password = "1elefantmed8sko";

const char* soft_ap_ssid = "klimaServer";
const char* soft_ap_password = "password";

const char* thingsspeak = "https://api.thingspeak.com/update";
String apiWriteKey = "8QN9SCFZ23U6THNR";

unsigned long lastTime = 0;

unsigned long timerDelay = 10000;

int dataSpreader = 0;

String test;
String CurrentChar;

char datanum0[20];
char datanum1[20];
char datanum2[20];

int sep1;
int sep2;

int tint;


// Create A server
AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);



  serverSetup();


  APIMaker();
}




void loop() {
  // HttpListner();
}


void APIMaker(){

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){ // this is a test httpGET 
    Serial.println("request recieved");
    request->send(200, "plain/text", "hello from server");
  });

  server.on(
    "/sensData",
    HTTP_POST,
    [](AsyncWebServerRequest *request){
    Serial.println("post");},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      
      // making the char array into a string that can written into an csv file at ez.
      test = (char*)data;
      Serial.println(test);
      Serial.println((char*)data);

      //tint = atoi(*data)
      



      // this for loop should essential be moved into it's own func.
      // since the data in the body will be written in a csv format when sent, it will need to be broken down into the diffrent data parts
      for (size_t i = 0; i < len; i++) {
        Serial.write(data[i]);
        Serial.println();

        //Serial.println(index);
        //Serial.println(total);

        CurrentChar = data[i];
        Serial.println("CC: " + CurrentChar);
        Serial.println("len:" + len);
        Serial.println("i:" + i);
        Serial.println("data:" + dataSpreader);

        if (CurrentChar == ";")
        {
          dataSpreader++;
        }
        
        switch (dataSpreader)
        {
        case 0: // the first thing that will be sent is the device ip
          
          break;
        case 1: // second will be temp
          sep1 = i;
          break;
        case 2: // third will be co2
          sep2 = i;
          break;

        default:
          break;
        }

        if (i == len - 1)
        {
          dataSpreader = 0;
        }
        
      } 

      





      request->send(200);
    }

  
  );


  server.begin();
  Serial.println("Server is hosting and waiting");
}

 

void jsonDecoder(){

}


void serverSetup(){
  // wifi mode
  WiFi.mode(WIFI_MODE_APSTA);
  // starting the AP and connecting to the intetnet
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

void sendSensorData(){
  
}



void api(){
  
  
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      http.begin(thingsspeak);

      http.addHeader("Content-Type", "");
    }
    {
      /* code */
    }
    
  }


}

void dataToVar(){

  
}