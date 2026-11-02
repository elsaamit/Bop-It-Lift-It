#include "esp32_bno055"
#include "touch_sensor_test"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
// #include <WebServer.h> 
// #include <WebSocketServer.h>
// #include <WiFi.h>

/*
This file is the main execution of the Lift-It Game
  - it will be wirelessly integrated with an App to display the instructions.

Components:
1. ES32-WROOM  
2. BNO055 9-axis accelerometer
3. # of capactative touch sensors

Start button is initation through the software
*/

//        TYPES
enum CurlType { CURL_NONE, CURL_BICEP, CURL_HAMMER, CURL_PREACHER};
enum GameState { ST_IDEL, ST_WAIT_START, ST_IN_REP, ST_FEEBACK, ST_GAME_OVER};

const char* curlName(CurlType c) {
  switch (c) {
    case CURL_BICEP:    return "BICEP";
    case CURL_HAMMER:   return "HAMMER";
    case CURL_PREACHER: return "PREACHER";
    default:            return "NONE";
  }
}

const char* stateName(GameState s) {
  switch (s) {
    case ST_IDLE:       return "IDLE";
    case ST_WAIT_START: return "WAIT_START";
    case ST_IN_REP:     return "IN_REP";
    case ST_FEEDBACK:   return "FEEDBACK";
    case ST_GAME_OVER:  return "GAME_OVER";
  }
  return "UNKNOWN";
}

// ESP32 DevKit Pins
const int PIN_SDA = 19;       // IMU SDA
const int PIN_SCL = 21;       // IMU SCL pin
const int PIN_TOUCH = 32;     // capacative touch sensor #1
const int PIN_TOUCH2 = 35;    // capactative touch sensor #2
// const int PIN_TOUCH2 = 35;    // capactative touch sensor #2



void setup() {
  //pin set up

}

void loop() {
  // put your main code here, to run repeatedly:

}
