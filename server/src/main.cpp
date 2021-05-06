#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#include <ESPAsyncWebServer.h>

#include <AsyncTCP.h>

#include <Arduino_JSON.h>

void serverSetup();
void HttpListner();
void APIMaker();
void toDouble();
void dataPharser();

// it's possible to make a id and pass for the post and get commands, might be a future edition.

// information for wifi connectivity, 
// if time, this can be made into a webserver, so when the server starts, it can host a webpage, where it's possible to input wifi information
//const char* wifi_network_ssid = "A.S";
//const char* wifi_network_password = "1elefantmed8sko";

const char* wifi_network_ssid = "AAU-1-DAY";
const char* wifi_network_password = "seed34chin";


// setup of AP server, this is what the sensors, and keypad connects too
const char* soft_ap_ssid = "klimaServer";
const char* soft_ap_password = "password";



const char* thingsspeak = "https://api.thingspeak.com/update";
String apiWriteKey = "8QN9SCFZ23U6THNR";


int dataSpreader = 0;

String test;
String CurrentChar;

int sep1 = 0;

char temparr[10];
char co2arr[10];
double temp, co2;
                        // change the length of the arrays if you want to have longe amounts of data, (This is already more than enough)
double setTemp, setco2;
char setTemparr[10];
char setCo2arr[10];


int ii;

// Create A server
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  serverSetup();
  APIMaker();
}




void loop() {

}


void APIMaker(){

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){ // this is a test httpGET 
    Serial.println("request recieved");
    request->send(200, "plain/text", "hello from server");
  });

  // What happens when the server gets an HTTP POST with the /sensData 
  // the data needs to be in thsi format "7,IP,temp,co2" for the sensors or "8,IP,settemp,setco2"
  // for the panel. the 7 and 8 is for the server to understand what is what. 
  server.on(
    "/sensData",
    HTTP_POST,
    [](AsyncWebServerRequest *request){
    Serial.println("post");},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      
      CurrentChar = data[0];

      test = (char*)data;
      Serial.println(test);

      // Splitting the code into usefull doubles that contain temp and co2
      if(CurrentChar == "55"){ //-------------------------no default statement--------
        Serial.println(len);
        for(int i = 0; i < len; i++){
          //Serial.println(i);
          
          CurrentChar = data[i];
          
          if(dataSpreader == 2){
            temparr[ii] = data[i];
            sep1 = sep1+2;
          }

          if(dataSpreader == 3){
            co2arr[ii] = data[i];
            sep1 = sep1+2;
          }

          if(CurrentChar == "44"){
            dataSpreader++;
            sep1 = i;

            //Serial.println(dataSpreader);
          }

          ii = sep1 - i;
        }
        
        // this could have been made more nice, but that takes time. So fuck that and let me die
      } else if(CurrentChar == "56") {
        Serial.println("working");

        for(int i = 0; i < len; i++){ // could make if statement that sets the 0 and the end of the digits, istead of a fixed place in the arr
          CurrentChar = data[i];
          if(dataSpreader == 2){
            setTemparr[ii] = data[i];
            sep1 = sep1+2;
          }
          if(dataSpreader == 3){
            setCo2arr[ii] = data[i];
            sep1 = sep1+2;
          }
          if(CurrentChar == "44"){
            dataSpreader++;
            sep1 = i;
          }

          ii = sep1 - i;
        }

      }
      // making sure that the is an end to the char arrays, else there will come an overflow
        temparr[9] = '\0';
        co2arr[9] = '\0';
        setTemparr[9] = '\0';
        setCo2arr[9] = '\0';

        temp = atof(temparr);
        co2 = atof(co2arr);
        setTemp = atof(setTemparr);
        setco2 =  atof(setCo2arr);

        Serial.println(temp);
        Serial.println(co2);
        Serial.println(setTemp);
        //Serial.println((char*)temparr);
        //Serial.println((char*)co2arr);


        //Serial.println(temparr);
        dataSpreader = 0;
      // returning a sentence (200 means ok) here shall there be an input command in microsecond of how long a sensor needs to sleep
      // the command shall be adjustable, so that it's possible to choose how often the sensors need update.
      request->send(200);
    }
    
  
  );


  server.begin();
  Serial.println("Server is hosting and waiting");
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
  // printing the IP's for the server, so it's easy to find it in testing.
  Serial.print("ESP soft AP ip is : ");
  Serial.println(WiFi.softAPIP());

  Serial.print("ESP local ip is : ");
  Serial.println(WiFi.localIP());

}




// this code is for a sensor to connect to the server, also used by control pannel
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


// don't know yet, might be usefull later, if we need to connect to thingsspeak api for a quick finnish.
/*
void api(){
  
  
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      http.begin(thingsspeak);

      http.addHeader("Content-Type", "");
    }
    {
    
    }
    
  }


}
*/

