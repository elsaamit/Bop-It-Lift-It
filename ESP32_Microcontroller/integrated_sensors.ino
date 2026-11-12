/*
  integrated_sensors.ino
  Sensor + screen helpers for the Lift-It game.
  

  Functions the main file can use:
    imuBegin()  --->  start the BNO055 (call in setup after Wire.begin)
    imuGravity()   --->      current gravity direction as a Vec3
    imuHandleRatio() --->    0 = handle level, 1 = handle vertical (hammer grip)
    bothHandsOn()  --->     true when both touch sensors are touched
    displayBegin() --->   start the OLED (call in setup after Wire1.begin)
    showMessage(), showWaitStart(), showInRep(), showFeedback(), sendGameOver()
    toneSuccess(), toneFail()
    printDebug()  --->      live values to Serial Monitor
*/

#include "configurations.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// BNO055
Adafruit_BNO055 BNO28(55, 0x28, &Wire);
Adafruit_BNO055 BNO29(55, 0x29, &Wire);
Adafruit_BNO055* BNO = nullptr;

// Which sensor axis runs along the dumbbell handle: 0 = x, 1 = y, 2 = z.
// Check with printDebug(): hold the dumbbell in a hammer grip and see which
// gravity value is near +/-9.8. That is your handle axis.
const int HANDLE_AXIS = 0;

bool imuBegin() {
  if (BNO28.begin())      BNO = &BNO28;
  else if (BNO29.begin()) BNO = &BNO29;
  else {
    Serial.println("BNO055 not found! Check wiring.");
    return false;
  }
  delay(1000);
  BNO->setExtCrystalUse(true);   // change to false if readings act strangely
  Serial.println("BNO055 ready.");
  return true;
}

Vec3 imuGravity() {
  if (!BNO) return { 0, 0, 0 };
  imu::Vector<3> g = BNO->getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  return { (float)g.x(), (float)g.y(), (float)g.z() };
}

float imuHandleRatio() {
  Vec3 g = imuGravity();
  float m = mag(g);
  if (m < 0.1f) return 0;
  float along = (HANDLE_AXIS == 0) ? g.x : (HANDLE_AXIS == 1) ? g.y : g.z;
  return fabs(along) / m;
}


// Touch modules (preacher: HIGH when touched)

bool bothHandsOn() {
  return digitalRead(PIN_TOUCH) == HIGH && digitalRead(PIN_TOUCH2) == HIGH;
}

// Sounds (Josh's voice??)

// void toneSuccess() { Serial.println(">>> SUCCESS"); }
// void toneFail()    { Serial.println(">>> FAIL"); }

///////////////////////// WEB APP INTEGRATION /////////////////////////////////////////////
///////////////////////// WEB APP INTEGRATION /////////////////////////
// Message format matches the app team's test server (server.cjs) and control.js

WebSocketsServer WebSockets(WS_PORT);
const bool APP_DEBUG = true;    // prints app messages to the Serial Monitor

// Same state names the test server sends in status_update
const char* appGameState() {
  switch (state) {
    case ST_IDLE:       return "ST_IDLE";
    case ST_WAIT_START: return "ST_WAIT_START";
    case ST_IN_REP:     return "ST_IN_REP";
    case ST_FEEDBACK:   return "ST_FEEDBACK";
    case ST_GAME_OVER:  return "ST_GAME_OVER";
  }
  return "ST_IDLE";
}

String appAddress() {
  IPAddress ip = WIFI_USE_AP ? WiFi.softAPIP() : WiFi.localIP();
  return "ws://" + ip.toString() + ":" + String(WS_PORT);
}

// Send to every connected app
void sendJson(JsonDocument& doc) {
  doc["version"] = 1;
  String out;
  serializeJson(doc, out);
  WebSockets.broadcastTXT(out);
  if (APP_DEBUG) { Serial.print("APP <-- "); Serial.println(out); }
}

// Send to one app only (used for replies)
void sendJsonTo(uint8_t client, JsonDocument& doc) {
  doc["version"] = 1;
  String out;
  serializeJson(doc, out);
  WebSockets.sendTXT(client, out);
  if (APP_DEBUG) { Serial.print("APP <-- "); Serial.println(out); }
}

// Reply to a start_game / stop_game request
void sendMessageResult(uint8_t client, const String& requestId, bool success, const char* message) {
  JsonDocument doc;
  doc["type"]       = "sent_message_result";
  doc["request_id"] = requestId;
  doc["success"]    = success;
  doc["message"]    = message;
  sendJsonTo(client, doc);
}

void sendGameStarted() {
  JsonDocument doc;
  doc["type"]    = "game_started";
  doc["game_id"] = "liftit";
  doc["score"]   = score;
  sendJson(doc);
}

void sendExerciseUpdate() {
  JsonDocument doc;
  doc["type"]        = "exercise_update";
  doc["exercise_id"] = roundNum;
  doc["curl_type"]   = curlAction(target);
  doc["text"]        = curlPrompt(target);
  doc["time_limit"]  = REP_TIMEOUT_MS;
  sendJson(doc);
}

void sendUserResult(bool success, unsigned long responseTime) {
  JsonDocument doc;
  doc["type"]          = "user_result";
  doc["exercise_id"]   = roundNum;
  doc["success"]       = success;
  doc["response_time"] = responseTime;   // milliseconds
  sendJson(doc);
}

void sendScoreUpdate() {
  JsonDocument doc;
  doc["type"]  = "score_update";
  doc["score"] = score;
  sendJson(doc);
}

void sendGameOver(const char* reason) {
  JsonDocument doc;
  doc["type"]        = "game_over";
  doc["final_score"] = score;
  doc["reason"]      = reason;   // "user_stopped" or "max_misses"
  sendJson(doc);
}

void fillStatus(JsonDocument& doc) {
  doc["type"]       = "status_update";
  doc["game_state"] = appGameState();
  doc["score"]      = score;
  if (state != ST_IDLE && state != ST_GAME_OVER && target != CURL_NONE) {
    doc["exercise"]["exercise_id"] = roundNum;
    doc["exercise"]["curl_type"]   = curlAction(target);
    doc["exercise"]["text"]        = curlPrompt(target);
    doc["exercise"]["time_limit"]  = REP_TIMEOUT_MS;
  } else {
    doc["exercise"] = nullptr;
  }
}

void onWsEvent(uint8_t client, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      Serial.println("App connected");
      JsonDocument doc;
      fillStatus(doc);
      sendJsonTo(client, doc);
      break;
    }

    case WStype_DISCONNECTED:
      Serial.println("App disconnected");
      break;

    case WStype_TEXT: {
      JsonDocument doc;
      if (deserializeJson(doc, payload, length)) {
        Serial.println("App sent invalid JSON");
        return;
      }
      String msgType   = doc["type"] | "";
      String requestId = doc["request_id"] | "";
      if (APP_DEBUG) { Serial.print("APP --> "); Serial.println(msgType); }

      bool gameRunning = (state != ST_IDLE && state != ST_GAME_OVER);

      if (msgType == "start_game") {
        if (gameRunning) {
          sendMessageResult(client, requestId, false, "Game already running.");
        } else {
          appStartRequested = true;
          sendMessageResult(client, requestId, true, "Game started.");
        }
      }
      else if (msgType == "stop_game" || msgType == "end_game") {
        if (gameRunning) {
          appEndRequested = true;
          sendMessageResult(client, requestId, true, "Game stopped");
        } else {
          sendMessageResult(client, requestId, false, "No game running.");
        }
      }
      else if (msgType == "get_status") {
        JsonDocument status;
        fillStatus(status);
        sendJsonTo(client, status);
      }
      else {
        sendMessageResult(client, requestId, false, "Unknown message type.");
      }
      break;
    }

    default:
      break;
  }
}

void appBegin() {
  if (WIFI_USE_AP) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
      delay(250);
    }
  }
  WebSockets.begin();
  WebSockets.onEvent(onWsEvent);
  Serial.print("App URL: ");
  Serial.println(appAddress());
}

void appLoop() {
  WebSockets.loop();
}