#include "WiFiHandler.h"

const char* host = "192.168.4.1";
const char SLP[] = "SLP: ";


void WifiSetup(){
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
}


int SendData(float TMP, float CO2){
  int sleepTime = 0;

  WiFiClient client; // Use WiFiClient class to create TCP connections
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return 0;
  } else Serial.println("connection good");

  char DataBuff [30];
  sprintf(DataBuff, "TMP: %0.1f\r\nCO2: %0.0f", TMP, CO2);
  client.print(String("GET ") + "/S HTTP/1.1\r\n" + "Host: " + host + "\r\n" + DataBuff + "\r\n\r\n");

  /* ^ Laver en HTTP header som ligner:
   * GET /S HTTP/1.1\r\n
   * Host: 192.168.1.10\r\n    
   * TMP: 9.2\r\n    
   * CO2: 350\r\n    
   * \r\n\r\n
  */
 
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return 0;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    int OK = 5;
    for (int i = 0; i < 5; i++) {
      if (line[i] == SLP[i]) OK--;
    }
    if (!OK) {
      line.remove(0, 4);
      sleepTime = line.toInt();
      client.stop();
    }
  }
  delay(100); //Giver tid til at vi kan give ACK på modtaget FIN. 
  return sleepTime;
}