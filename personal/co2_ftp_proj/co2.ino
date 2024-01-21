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


#define WIFI_SSID ""
#define WIFI_PASS ""

char ftp_server[] = "";
char ftp_user[]   = "";
char ftp_pass[]   = "";

BMP280 bmp280;
SCD30 airSensor;
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);
SSD1306 display;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  Wire.begin();
  display.begin();
  bmp280.begin();
  airSensor.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}
