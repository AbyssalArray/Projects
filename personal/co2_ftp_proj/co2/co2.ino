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
#include <Preferences.h>


// 1 Activate and 0 deactivate debug lines
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif


#define EEPROM_SIZE 512  //esp32
#define M_STR_LEN 64
#define M_PATH_LEN 200

typedef struct {
  char wifi_ssid[M_STR_LEN];  //63 + 1 for terminator char
  char wifi_pass[M_STR_LEN];  //it is 32 but choosing 63 to fit with the rest of the variables

  // couldnt find clear limits so setting to 63 chars
  char ftp_server[M_STR_LEN];
  char ftp_user[M_STR_LEN];
  char ftp_pass[M_STR_LEN];

  char ftp_path[M_PATH_LEN];  //gives headroom for file renaming lim 255

  char read_freq = 1;                                          // readings per hr
} settings_t;
settings_t set;

BMP280 bthSen;                                               //barometer, temperature and humidity sensor
SCD30 cSen;                                                    // co2 sensor
ESP32_FTPClient* ftp = NULL;  //initialising here to keep it global for now.
Adafruit_SSD1306 disp;                                         // oled display
Preferences mem;

void load_set();
void update_set();
void save_set();
void check_connect();

void setup() {
  mem.begin("co2", false);
  Serial.begin(115200);
  delay(10);

  
  Wire.begin();
  disp.begin();
  bthSen.begin();
  cSen.begin();
}
void save_set() {
  mem.putBytes("settings", &set, sizeof(set));

}

void loop() {
  // put your main code here, to run repeatedly:
}
