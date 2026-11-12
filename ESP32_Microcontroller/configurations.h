// Pins, tuning values, and shared types
// Change pins here once instead of hunting through every file.
// #pragma once
#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H
#include <Arduino.h>

// AS OF 4:30 11/8:
//    UPDATED TO be configured to the webapp code 
//    still integrated the button for tare
// need to still test

//  Pins (ESP32-WROOM DevKit)  ---> UPDATED 10:54 AM, 11/9/26
const int PIN_TARE   = 13;  // tare (reset axis) button --> IO13
const int PIN_SDA    = 8;  // BNO055 SDA   --> IO8
const int PIN_SCL    = 9;  // BNO055 SCL   --> IO9
// const int PIN_OLED_SDA = 33; // OLED SDA     // prototyping purposes
// const int PIN_OLED_SCL = 32; // OLED SCL     // prototyping purposes
const int PIN_TOUCH  = 4;  // touch sensor #1 (IO4)
const int PIN_TOUCH2 = 5;  // touch sensor #2 (IO5)

// reps detecting (need to calibrate)
const float TOP_ANGLE_DEG           = 90.0;  // rep counts as "at the top" past this
const float BOTTOM_ANGLE_DEG        = 25.0;  // back below this = rep finished
const float HAMMER_RATIO            = 0.7;   // handle this vertical = hammer curl
const float PREACHER_TOUCH_FRACTION = 0.6;   // both hands on for 60% of the rep = preacher

//  Timing of the Game
const unsigned long SETTLE_MS      = 3000;  // the wait in WAIT_START ebfore auto-tare
const unsigned long REP_TIMEOUT_MS = 12000;
const unsigned long FEEDBACK_MS    = 3000;  // how long the screen stays on
const unsigned long SAMPLE_MS      = 30;   // 100 Hz sensor loop (BNO055 fusion rate)
const unsigned long DRAW_MS         =  30; //  refresh rate
const int MAX_MISSES = 3;

// CONNECTION TO THE WEB APP: WIFI LINK
// false = esp32 joins the existing network
// true = esp32 makes its own wifi network/hotspot
const bool WIFI_USE_AP = true;
const char* const WIFI_SSID = "LiftIt-ESP32";
const char* const WIFI_PASS = "JDliftit26"; //password has to be 8+ characters
const uint16_t WS_PORT = 8081;   // matches the port in websocket.js

//  Shared types 
enum CurlType  { CURL_NONE, CURL_BICEP, CURL_HAMMER, CURL_PREACHER };
enum GameState { ST_IDLE, ST_WAIT_START, ST_IN_REP, ST_FEEDBACK, ST_GAME_OVER };

inline const char* curlAction(CurlType c) {
  switch (c) {    // updated to match web app naming structure
    case CURL_BICEP:    return "bicep";
    case CURL_HAMMER:   return "hammer";
    case CURL_PREACHER: return "preacher";
    default:            return "none";
  }
}

// the text shown in the app for each move
inline const char* curlPrompt(CurlType c) {
  switch (c) {
    case CURL_BICEP:    return "Bicep curl!";
    case CURL_HAMMER:   return "Hammer curl!";
    case CURL_PREACHER: return "Double curl!";
    default:            return "";
  }
}

inline const char* stateName(GameState s) {
  switch (s) {
    case ST_IDLE:       return "IDLE";
    case ST_WAIT_START: return "WAIT_START";
    case ST_IN_REP:     return "IN_REP";
    case ST_FEEDBACK:   return "FEEDBACK";
    case ST_GAME_OVER:  return "GAME_OVER";
  }
  return "UNKNOWN";
}



struct Vec3 { 
  float x, y, z; 
};

inline float mag(const Vec3& v) { 
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); 
  }

// Angle between two vectors in degrees (0 if either is ~zero)
inline float angleDeg(const Vec3& a, const Vec3& b) {
  float ma = mag(a), mb = mag(b);
  if (ma < 0.1f || mb < 0.1f){
    return 0;
  } 
  float c = (a.x * b.x + a.y * b.y + a.z * b.z) / (ma * mb);
  c = constrain(c, -1.0f, 1.0f);
  return acosf(c) * 180.0f / PI;
}

#endif // configurations_h