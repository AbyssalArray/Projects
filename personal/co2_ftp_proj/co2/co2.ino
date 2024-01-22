#include "Arduino.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include <BMP280.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>
#include <splash.h>


// 1 Activate and 0 deactivate debug lines
#define DEBUG  1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif


#define EEPROM_SIZE 512   //esp32 

char wifi_ssid[64]; //63 + 1 for terminator char 
char wifi_pass[64]; //it is 32 but choosing 63 to fit with the rest of the variables

// couldnt find clear limits so setting to 63 chars
char ftp_server[64];
char ftp_user[64];
char ftp_pass[64];

char ftp_path[200];  //gives headroom for file renaming lim 255 

char read_freq = 1;  // readings per hr

BMP280 bthSen;      //barometer, temperature and humidity sensor
SCD30 cSen;         // co2 sensor
ESP32_FTPClient ftp(ftp_server,ftp_user,ftp_pass, 5000, 2);  //initialising here to keep it global for now.
Adafruit_SSD1306 disp;  // oled display

void setup() {
  Serial.begin(115200);
  delay(10);
  Wire.begin();
  disp.begin();
  bthSen.begin();
  cSen.begin();

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    debugln("failed to initialise EEPROM"); 
    // need to add GUI error here
    while(1) delay(1000000);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
