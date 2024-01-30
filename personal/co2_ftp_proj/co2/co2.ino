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
#define WIFI_TIMEOUT 5000
#define WIFI_WAIT 500

typedef struct {
  char wifi_ssid[M_STR_LEN];  //63 + 1 for terminator char
  char wifi_pass[M_STR_LEN];  //it is 32 but choosing 63 to fit with the rest of the variables

  // couldnt find clear limits so setting to 63 chars
  char ftp_server[M_STR_LEN];
  char ftp_user[M_STR_LEN];
  char ftp_pass[M_STR_LEN];

  char ftp_path[M_PATH_LEN];  //gives headroom for file renaming lim 255 also a future feature

  char read_freq = 1;                                          // readings per hr
} settings_t;
settings_t set;

BMP280 bthSen;                                               //barometer, temperature and humidity sensor
SCD30 cSen;                                                    // co2 sensor
ESP32_FTPClient* ftp = NULL;                                    // used as pointer to have the ability to change the credentials
Adafruit_SSD1306 disp;                                         // oled display
Preferences mem;

int load_set();
int update_set();
int save_set();
int check_connect();

void setup() {
  mem.begin("co2", false);
  Serial.begin(115200);
  delay(10);

  
  Wire.begin();
  disp.begin();
  bthSen.begin();
  cSen.begin();
}

/**
  loads settings from permament memory
  returns 1 if success
  returns 0 if no settings found in memory (first run)
  setNum 
**/
int load_set() {
  size_t setNum = mem.getBytes("settings", &set, sizeof(set));
  //check_connect();  could be added
  if (setNum)
    return 1;
  return 0;
}
int save_set() {
  mem.putBytes("settings", &set, sizeof(set));
  return 1;
}

/**
  Takes the credentials stored in settings (set)
  and checks if they are valid in the current location
  returns 0 if unable to connect
  returns 1 if both ftp and wifi connects

  plan to be changed to 0 = pass, 1 = ftp fail, 2 = wifi fail
**/
int check_connect() {
  // connect to wifi, start timer, if passes timeout credentials are invalid
  WiFi.begin(set.wifi_ssid, set.wifi_pass);
  long int tStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - tStart > WIFI_TIMEOUT)
      return 0;
    else
     delay(WIFI_WAIT);
  }

  // do same for ftp server
  ftp = new ESP32_FTPClient(set.ftp_server, set.ftp_user, set.ftp_pass);
  ftp->OpenConnection();
  if (!ftp->isConnected())
    return 0;

  //ensure the state of connection is known
  ftp->CloseConnection();
  return 1;
}


void loop() {
  // put your main code here, to run repeatedly:
}
