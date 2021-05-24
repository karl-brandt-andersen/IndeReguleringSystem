#include "WiFiHandler.h"

const char* host = "192.168.4.1";
const char SLP[] = "SLP: ";


void WifiSetup(){
  WiFi.begin(ssid, password);         //Connect to reguerings hotspot.

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");                //Keep trying to connect, until connected.
  }
  
  /*  Debug messages.  */
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
}


int SendData(float TMP, float CO2){
  int sleepTime = 0;                        //make a variable to collect sleeping 

  WiFiClient client;                        //Use WiFiClient class to create TCP connections
  const int httpPort = 80;                  //Send on port 80.
  if (!client.connect(host, httpPort)) {    //Connect to server.
    Serial.println("connection failed");
    return 0;
  } else Serial.println("connection good");

  char DataBuff [30];         //Create buffer
  sprintf(DataBuff, "TMP: %0.1f\r\nCO2: %0.0f", TMP, CO2);    //Write string to buffer.
  client.print(String("GET ") + "/S HTTP/1.1\r\n" + "Host: " + host + "\r\n" + DataBuff + "\r\n\r\n");  //Send http header with DataBuff as additional header data.

  /* ^ Laver en HTTP header som ligner:
   * GET /S HTTP/1.1\r\n
   * Host: 192.168.1.10\r\n    
   * TMP: 9.2\r\n    
   * CO2: 350\r\n    
   * \r\n\r\n
  */
 
  unsigned long timeout = millis();
  while (client.available() == 0) {       //wait for data to be returned.
    if (millis() - timeout > 5000) {      //if more than 5 seconds have elapsed, its timend out.
      Serial.println(">>> Client Timeout !");
      client.stop();                      //Stops the connection.
      return 0;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');   //Read recived data line by line.
    Serial.println(line);                         //Print lines to Serial port.
    int OK = 5;                                   
    for (int i = 0; i < 5; i++) {                 //Loop for every element in the string SLP ("SLP: ")
      if (line[i] == SLP[i]) OK--;                //If Recived data equals the string SLP. decrement OK
    }
    if (!OK) {                    //If we decrementet to 0, we have a match
      line.remove(0, 4);          //Remove first part of header.
      sleepTime = line.toInt();   //Convert remaining data to an integer.
      client.stop();              //Stop the connection by sending FIN to host.
    }
  }
  delay(100);                     //Wait a little, to ACK on FIN ACK. (TCP 3-way handshake) 
  return sleepTime;               //Reuturn the recived sleeping interval.
}