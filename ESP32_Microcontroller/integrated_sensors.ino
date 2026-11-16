/*
  integrated_sensors.ino
  Sensor + screen helpers for the Lift-It game.
  
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
// Message format matches the app team's test server (server.cjs) and control.js

WebSocketsClient WebSockets;
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
  // IPAddress ip = WIFI_USE_AP ? WiFi.softAPIP() : WiFi.localIP();
  // return "ws://" + ip.toString() + ":" + String(WS_PORT);
  return "ws://" + String(WS_HOST) + ":" + String(WS_PORT);
}

// Send to every connected app
void sendJson(JsonDocument& doc) {
  doc["version"] = 1;
  String out;
  serializeJson(doc, out);
  WebSockets.sendTXT(out);
  if (APP_DEBUG) { Serial.print("APP <-- "); Serial.println(out); }
}

// // Send to one app only (used for replies)
// void sendJsonTo(uint8_t client, JsonDocument& doc) {
//   doc["version"] = 1;
//   String out;
//   serializeJson(doc, out);
//   WebSockets.sendTXT(client, out);
//   if (APP_DEBUG) { Serial.print("APP <-- "); Serial.println(out); }
// }

// Reply to a start_game / stop_game request
void sendMessageResult(const String& requestId, bool success, const char* message) {
  JsonDocument doc;
  doc["type"] = "sent_message_result";
  doc["request_id"] = requestId;
  doc["success"] = success;
  doc["message"] = message;
  sendJson(doc);
}

void sendGameStarted() {
  JsonDocument doc;
  doc["type"] = "game_started";
  doc["game_id"] = "liftit";
  doc["score"] = score;
  sendJson(doc);
}

void sendExerciseUpdate() {
  JsonDocument doc;
  doc["type"] = "exercise_update";
  doc["exercise_id"] = roundNum;
  doc["curl_type"] = curlAction(target);
  doc["text"] = curlPrompt(target);
  doc["time_limit"] = REP_TIMEOUT_MS;
  sendJson(doc);
}

void sendUserResult(bool success, unsigned long responseTime) {
  JsonDocument doc;
  doc["type"] = "user_result";
  doc["exercise_id"] = roundNum;
  doc["success"] = success;
  doc["response_time"] = responseTime;   // milliseconds
  sendJson(doc);
}

void sendScoreUpdate() {
  JsonDocument doc;
  doc["type"] = "score_update";
  doc["score"] = score;
  sendJson(doc);
}

void sendGameOver(const char* reason) {
  JsonDocument doc;
  doc["type"] = "game_over";
  doc["final_score"] = score;
  doc["reason"] = reason;   // "user_stopped" or "max_misses"
  sendJson(doc);
}

void sendStatus() {
  JsonDocument doc;
  doc["type"] = "status_update";
  doc["game_state"] = appGameState();
  doc["score"] = score;
  if (state != ST_IDLE && state != ST_GAME_OVER && target != CURL_NONE) {
    doc["exercise"]["exercise_id"] = roundNum;
    doc["exercise"]["curl_type"] = curlAction(target);
    doc["exercise"]["text"] = curlPrompt(target);
    doc["exercise"]["time_limit"] = REP_TIMEOUT_MS;
  } else {
    doc["exercise"] = nullptr;
  }
  sendJson(doc);
}

void onWsEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      Serial.println("App connected");
      JsonDocument hello; // send a hello message to the app
      hello["type"] = "hello";
      hello["role"] = "esp32";
      sendJson(hello);
      sendStatus();
      break;
    }

    case WStype_DISCONNECTED:
      Serial.print("Can't reach server at: ");
      Serial.print(appAddress());
      Serial.print(" | WiFi: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String("NOT connected")); 
      // Serial.println("App disconnected");
      break;

    case WStype_TEXT: {
      JsonDocument doc;
      if (deserializeJson(doc, payload, length)) {
        Serial.println("App sent invalid JSON");
        return;
      }
      String msgType   = doc["type"] | "";
      String requestId = doc["request_id"] | "";
      if (APP_DEBUG) {
        Serial.print("APP --> "); Serial.println(msgType); 
      }

      bool gameRunning = (state != ST_IDLE && state != ST_GAME_OVER);

      if (msgType == "start_game") {
        if (gameRunning) {
          sendMessageResult(requestId, false, "Game already running.");
        } else {
          appStartRequested = true;
          sendMessageResult(requestId, true, "Game started.");
        }
      }
      else if (msgType == "stop_game" || msgType == "end_game") {
        if (gameRunning) {
          appEndRequested = true;
          sendMessageResult(requestId, true, "Game stopped");
        } else {
          sendMessageResult(requestId, false, "No game running.");
        }
      }
      else if (msgType == "get_status") {
        sendStatus();
      }
      else {
        sendMessageResult(requestId, false, "Unknown message type.");
      }
      break;
    }

    default:
      break;
  }
}
// wifi latency is too large and slow
// use websocket/hotspot
void appBegin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to Wifi \"");
  Serial.print(WIFI_SSID);
  Serial.print("\"");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED){
    Serial.print("WiFi connected, ESP32 IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected yet");
  }

  WebSockets.begin(WS_HOST, WS_PORT, "/");
  WebSockets.onEvent(onWsEvent);
  WebSockets.setReconnectInterval(200); // retry every 2 seconds if the server isnt up
  Serial.print("Server: ");
  Serial.println(appAddress());

  // if (WIFI_USE_AP) {
  //   Serial.println("Setting Wi-Fi mode: "); Serial.flush();
  //   WiFi.mode(WIFI_AP);
  //   Serial.println("Starting hotspot..."); Serial.flush();
  //   bool ok = WiFi.softAP(WIFI_SSID, WIFI_PASS);
  //   Serial.println("Hotspot started: "); Serial.println(ok ? "yes" : "NO!");
  //   WiFi.softAP(WIFI_SSID, WIFI_PASS);
  // } else {
  //   WiFi.mode(WIFI_STA);
  //   WiFi.begin(WIFI_SSID, WIFI_PASS);
  //   unsigned long t0 = millis();
  //   while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
  //     delay(250);
  //   }
  // }
  // WebSockets.begin();
  // Serial.println("Websocket server started"); Serial.flush();
  // WebSockets.onEvent(onWsEvent);
  // Serial.print("App URL: ");
  // Serial.println(appAddress());
}

void appLoop() {
  WebSockets.loop();
}