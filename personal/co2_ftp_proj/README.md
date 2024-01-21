OVERVIEW:
    -Indoor air quality tracker. Contains a co2 sensor (SCD30), barometer, temp and humidity sensor(BMP280).
    -GUI is displayed through 128x64 SSD1306 I2C display
    -Microcontroller is ESP32-WROOM-32D, for bluetooth and wifi comunication.
    -All connections are done through the I2C bus with future UI elements being attached to standard GPIO pins.
    

TODO:
    -Get ftp connection working
    -Get intial wifi/password setup
    -Setup EEPROM storage for critical values like passwords
    -(optional) setup bluetooth modification of values instead of serial
    -Get date/time sync with ftp server
    -Get sensor reading working
    -Get upload and curate sensor data on ftp server
    -Get rudimentary Serial UI
    -Get rudimentary GUI
    -Polish GUI and include a serverless mode.
    -(optional) create animations and image error messages
    -Design a case for the electronics

DONE:
    -Setup initial circuit.
