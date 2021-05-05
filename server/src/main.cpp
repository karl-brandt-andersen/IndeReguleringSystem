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



// information for wifi connectivity, 
// if time, this can be made into a webserver, so when the server starts, it can host a webpage, where it's possible to input wifi information
const char* wifi_network_ssid = "AAU-1-DAY";
const char* wifi_network_password = "flag81safe";


// setup of AP server, this is what the sensors, and keypad connects too
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

int sep1 = 0;
int sep2;

int tint;

char temparr[10];
char co2arr[10];
double temp, co2;

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
      
      // it 

      test = (char*)data;
      Serial.println(test);

      int ii;
      Serial.println(len);
      for(int i = 0; i < len; i++){
        Serial.println(i);
        
        //Serial.println(ii);
        CurrentChar = data[i];
        
        if(dataSpreader == 1){
          temparr[ii] = data[i];
          sep1 = sep1+2;
         // Serial.write(data[i]);
         // Serial.println();
         // Serial.write(temparr[ii]);
         // Serial.println();
        }
        if(dataSpreader == 2){
          co2arr[ii] = data[i];
          sep1 = sep1+2;
        }
        if(CurrentChar == "44"){
          dataSpreader++;
          sep1 = i;

          Serial.println(dataSpreader);
        }

        ii = sep1 - i;
      }

      temparr[9] = '\0';
      co2arr[9] = '\0';


      temp = atof(temparr);
      co2 = atof(co2arr);


      Serial.println(temp);
      Serial.println(co2);

      Serial.println((char*)temparr);
      Serial.println((char*)co2arr);


      Serial.println(temparr);
      dataSpreader = 0;
      
       /* huge comment 

      // making the char array into a string that can written into an csv file at ez.
      test = (char*)data;
      Serial.println(test);
      //Serial.println((char*)data);











      //tint = atoi(*data)
      
      */

      /* this is outdated. ill try to make it send more ivformation instead. if i can


      // this for loop should essential be moved into it's own func.
      // since the data in the body will be written in a csv format when sent, it will need to be broken down into the diffrent data parts
      for (size_t i = 0; i < len; i++) {
        Serial.write(data[i]);
        Serial.println();

        //Serial.println(index);
        //Serial.println(total);

        CurrentChar = data[i];
        //Serial.println("CC: " + CurrentChar); // print char value in ascii
        //Serial.println("len:" + len);
        //Serial.println("i:" + i);
        //Serial.println("data:" + dataSpreader);

        if (CurrentChar == "44")
        {
          dataSpreader++;
        }
        // the mothode to conver .csv to doubles, what if i just convert multiple doubles to a csv string.
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

      Serial.println(sep1);
      Serial.println(sep2);

      unsigned long long int m = 1;
      double ret = 0;
      for (int j = sep2-1; j >= sep1; j--) {
        ret += (data[j] - '0') * m;
        m *= 10;
        Serial.println(ret);


    }



      */




      //Serial.println("ret:");
      //Serial.println(ret);
      //Serial.println(co2);

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
 // this is a mthode to convert char array to double, but it keeps getting overflow
double toDouble(const char* s, int start, int stop) {
    unsigned long long int m = 1;
    float ret = 0;
    for (int i = stop; i >= start; i--) {
        ret += (s[i] - '0') * m;
        m *= 10;
    }
    return ret;
}


