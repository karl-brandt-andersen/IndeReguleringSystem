#include <Arduino.h>
#include <WiFiHandler.h>
#include <OneWire.h>

// Setup a oneWire instance to communicate with any OneWire device
OneWire  ds(23);  // on pin 23 (a 4.7K resistor is necessary)

const uint8_t mesure[9] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};	//Read sequence for MH-Z19B
int CO2 = 0;		//CO2 data
float TMP = 0;		//Temprature data.

void GetCO2();
char getCheckSum(char *packet);
void Sleep(int Time);
int getTemp();

void setup() {
  Serial.begin(115200);	//Debug serial port.
  Serial2.begin(9600);	//MH-Z19B (CO2 sensor) serial port.

  // We start by connecting to a WiFi network
  WifiSetup();
  
}

void loop() {
  while (getTemp());	//Update TMP with temprature, will loop until succsesfull reading.
  GetCO2();				//Update CO2 with CO2 concentration.
  Serial.printf("TMP: %2.2f, CO2: %d\n", TMP, CO2);	//Debug
  Sleep(SendData(TMP, CO2));	//Deepsleep for the amount given by the server
  delay(10000);			//Wait before retrying.
}

void Sleep(int Time) {			//Deepsleep
  if (Time) {	//if the time is 0 we dont want to sleep.
    Serial.println("Sleeping");	
    esp_sleep_enable_timer_wakeup(Time * 1000000);	//Sleep time in seconds. (normally in uS)
    esp_deep_sleep_start();						
  }
}

void GetCO2() {								
  Serial2.write(mesure, 9);					//Request CO2 reading
  delay(300);
  if (Serial2.available()) {				//If we got an answer. we want to read it.
    byte i = 0;
	char inByte[9];					
    while (Serial2.available()) 
      inByte[i++] = Serial2.read();			//Read buffer to inByte
    if (inByte[8] == getCheckSum(inByte)) 	//Check if checsum match.
      CO2 = inByte[2] * 256 + inByte[3];	//Update CO2 with new data.
  }
}

char getCheckSum(char *packet) {	//Calculates checksum
  char i, checksum = 0;
  for ( i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xFF - checksum;
  checksum += 1;
  return checksum;
}

int getTemp(){
  byte i;
  byte data[12];
  byte addr[8];
  float celsius;
  
  while ( !ds.search(addr)) {
    ds.reset_search();    //Search for DS18b20
    delay(250);
	return 1;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 1;           //If CRC is invalid abort.
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);              // Wait for conversion
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         	// Read Scratchpad and data.

  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();    //Copy 9 bytes of data to the buffer data.
  }

  int16_t raw = (data[1] << 8) | data[0];	//make 2 temprature bytes into 1 16 bit byte.
  
	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
	if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	//// default is 12 bit resolution, 750 ms conversion time
  
  celsius = (float)raw / 16.0;    //Calculate celsius.
  TMP = celsius;
  return 0;
}