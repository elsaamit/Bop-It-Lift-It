/*
  integrated_sensors.ino
  Sensor + screen helpers for the Lift-It game.
  

  Functions the main file can use:
    imuBegin()  --->  start the BNO055 (call in setup after Wire.begin)
    imuGravity()   --->      current gravity direction as a Vec3
    imuHandleRatio() --->    0 = handle level, 1 = handle vertical (hammer grip)
    bothHandsOn()  --->     true when both touch sensors are touched
    displayBegin() --->   start the OLED (call in setup after Wire1.begin)
    showMessage(), showWaitStart(), showInRep(), showFeedback(), showGameOver()
    toneSuccess(), toneFail()
    printDebug()  --->      live values to Serial Monitor
*/

#include <configurations.h>
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

/////////////////// // OLED (Wire1) ** FOR TESTING PURPOSES WITHOUT WEB APP**  /////////////////////////////////////////
// Adafruit_SSD1306 oled(128, 64, &Wire1, -1);
// const uint8_t OLED_ADDR = 0x3C;   // try 0x3D if the screen isn't found
// const char DEG = (char)247;

/*
bool displayBegin() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found! Check wiring / address.");
    return false;
  }
  oled.cp437(true);
  oled.setTextColor(SSD1306_WHITE);
  oled.clearDisplay();
  oled.display();
  Wire1.setClock(400000);   // smoother screen; delete if you get freezes
  return true;
}

void header(const char* title) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(title);
  oled.drawFastHLine(0, 10, 128, SSD1306_WHITE);
}

void showMessage(const char* title, const char* text) {
  header(title);
  oled.setCursor(0, 16);
  oled.print(text);
  oled.display();
}

void showWaitStart(const char* curl, int round, int score, int misses) {
  header("GET READY");
  oled.setTextSize(2);
  oled.setCursor(0, 16);
  oled.print(curl);
  oled.setTextSize(1);
  oled.setCursor(0, 38); oled.print("Hold still at bottom");
  oled.setCursor(0, 54);
  oled.print("R:"); oled.print(round);
  oled.print("  Pts:"); oled.print(score);
  oled.print("  X:"); oled.print(misses);
  oled.display();
}

void showInRep(const char* curl, float angle, bool reachedTop, unsigned long secLeft) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(curl); oled.print(" CURL");

  char buf[10];
  snprintf(buf, sizeof(buf), "%.0f", angle);
  oled.setTextSize(3);
  oled.setCursor(0, 14);
  oled.print(buf);
  oled.setTextSize(1);
  oled.print(DEG);

  oled.setCursor(0, 44);
  oled.print(reachedTop ? "Now lower it down" : "Curl up!");

  oled.setCursor(0, 56);
  oled.print("Time left: "); oled.print(secLeft); oled.print("s");
  oled.display();
}

void showFeedback(const String& line, int score, int misses) {
  header("RESULT");
  oled.setTextSize(2);
  oled.setCursor(0, 18);
  oled.print(line);
  oled.setTextSize(1);
  oled.setCursor(0, 54);
  oled.print("Score:"); oled.print(score);
  oled.print("  Misses:"); oled.print(misses);
  oled.display();
}

void showGameOver(int score) {
  header("GAME OVER");
  oled.setTextSize(2);
  oled.setCursor(0, 18);
  oled.print("Score: "); oled.print(score);
  oled.setTextSize(1);
  oled.setCursor(0, 54);
  oled.print("Press to play again");
  oled.display();
}


// Debug output (Serial Monitor, 115200 baud)
const bool DEBUG = true;

void printDebug(const char* stateText, float repAngle) {
  static unsigned long lastDebug = 0;
  if (!DEBUG || millis() - lastDebug < 200) return;
  lastDebug = millis();

  Vec3 g = imuGravity();
  uint8_t sys = 0, gc = 0, ac = 0, mc = 0;
  if (BNO){
    BNO->getCalibration(&sys, &gc, &ac, &mc);
  } 
}
*/

///////////////////////// WEB APP INTEGRATION /////////////////////////////////////////////
WebSocketServer webSocket(WS_PORT);
const bool APP_DEBUG = true;    // this allows prints app messages to serial monitor

//the app's PlayView.vue checks for different states: 'idle', 'playing', and 'gameOver'
const char* appGameState() {
  if (state == ST_IDLE) {
    return "idle";
  }
  if (state == ST_GAME_OVER) {
    return "gameOver";
  }
  return "playing";
}

String appAddress() {
  IPAddress ip = WIFI_USE_AP ? WiFi.softAPIP() : WiFi.localIP();
  return "ws://" + ip.toString() + ":" + String(WS_PORT);
}

void sendToJSON(JsonDocument& doc) {
  String out;
  serializeJson(doc, out);
  webSocket.broadcastTXT(out);
  if (APP_DEBUG) {
    Serial.print("APP <--");
    Serial.print(out);
  }
}

void sendGameStarted() {
  JsonDocument doc;
  doc["version"] = 1;
  doc["type"] = "game_started";
  sendJson(doc);
}

void sendCommandUpdate() {
  JsonDocument doc;
  doc["version"] = 1;
  doc["type"] = "command_update";
  doc["command_id"] = roundNum;
  doc["action"] = curlAction(target);
  doc["text"] = curlPrompt(target);
  doc["time_limit"] = REP_TIMEOUT_MS;
  sendJson(doc);
}

void sendUserResult(bool correct, CurlType detected) {
  JsonDocument doc;
  doc["version"] = 1;
  doc["type"] = "user_result";
  doc["last_user_result"] = correct;
  doc["detected"] = curlAction(detected);
  sendJson(doc);
}

void sendScoreUpdate() {
  JsonDocument doc;
  doc["version"] = 1;
  doc["type"]    = "score_update";
  doc["score"]   = score;
  sendJson(doc);
}

// full process of the game for the app (double check this)
void fillStatus(JsonDocument& doc) {
  doc["version"]    = 1;
  doc["type"]       = "status_update";
  doc["game_state"] = appGameState();
  doc["score"]      = score;
  if (state != ST_IDLE && state != ST_GAME_OVER && target != CURL_NONE) {
    doc["command"]["command_id"] = roundNum;
    doc["command"]["action"]     = curlAction(target);
    doc["command"]["text"]       = curlPrompt(target);
    doc["command"]["time_limit"] = REP_TIMEOUT_MS;
  } else {
    doc["command"] = nullptr;
  }
}

void sendGameOver() {
  JsonDocument doc;
  doc["version"] = 1;
  doc["type"] = "game_over";
  doc["final_score"]= score;
  sendJson(doc);
// communication btwn control.js and PlayView checks
  JsonDocument status;
  fillStatus(status);
  sendJson(status);

}

void onWsEvent(uint8_t client, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      Serial.println("App connected");
      JsonDocument doc;
      fillStatus(doc);
      String out;
      serializeJson(doc, out);
      webSocket.sendTXT(client, out);
      break;
    }
    
    case WStype_DISCONNECTED:
      Serial.println("App diconnected");
      break;
  
    case WStype_TEXT: {   /// debugging case
      JsonDocument doc;
      if (deserializeJson(doc, payload, length)) {
        Serial.println("App sent invalid JSON");
        return;
      }
      const char* msgType = doc["type"] | "";
      if (APP_DEBUG){
        Serial.print("APP -->");
        Serial.println(msgType);
      }
      if (strcmp(msgType, "start_game") == 0) {
        appStartRequested = true;
      }
      else if (strcmp(msgType, "end_game") == 0) {
        appEndRequested = true;
      }
      break;
    }
    // defualt 
    default:
      break;

  }
}


void appBegin() {
  if (WIFI_USE_AP) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
  } else {
    Wifi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && milis() - t0 < 15000){
      delay(250);
    }
  } 

  webSocket.begin();
  webSocket.onEvent(onWsEvent);
  Serial.print("App URL: ");
  Serial.println(appAddress());
}

void appLoop() {
  webSocket.loop();
}




