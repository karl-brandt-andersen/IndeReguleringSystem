#include <Arduino.h>
#include <WiFiHandler.h>
#include <OneWire.h>

// Setup a oneWire instance to communicate with any OneWire device
OneWire  ds(23);  // on pin 2 (a 4.7K resistor is necessary)

const uint8_t mesure[9] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
int CO2 = 0;
float TMP = 0;

void GetCO2();
char getCheckSum(char *packet);
void Sleep(int Time);
int getTemp();

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  // We start by connecting to a WiFi network
  WifiSetup();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  while (getTemp());
  GetCO2();
  Serial.printf("TMP: %2.2f, CO2: %d\n", TMP, CO2);
  Sleep(SendData(TMP, CO2));
  delay(10000);
}

void Sleep(int Time) {
  if (Time) {
    Serial.println("Sleeping");
    esp_sleep_enable_timer_wakeup(Time * 1000000);
    esp_deep_sleep_start();
  }
}

void GetCO2() {
  Serial2.write(mesure, 9);
  delay(300);
  if (Serial2.available()) {
    char i = 0, inByte[9];
    while (Serial2.available()) 
      inByte[i++] = Serial2.read();
    if (inByte[8] == getCheckSum(inByte)) 
      CO2 = inByte[2] * 256 + inByte[3];
  }
}

char getCheckSum(char *packet) {
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
    ds.reset_search();
    delay(250);
	return 1;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 1;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  
	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
	if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	//// default is 12 bit resolution, 750 ms conversion time
  
  celsius = (float)raw / 16.0;
  TMP = celsius;
  return 0;
}
/*
ROM = 28 58 3A FD C 0 0 FF
  Chip = DS18B20
  Data = 1 80 1 4B 46 1F FF 10 10 56  CRC=56
  Temperature = 24.00 Celsius, 75.20 Fahrenheit
No more addresses.
*/