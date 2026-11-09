
#include <configurations.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
// #include <ArduinoJson.h> 
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

// debouncing function that outputs a single "pressed" for every press
class Button {
  public:
    explicit Button(int pin) : pin(pin) {}
    void begin() {
      pinMode(pin, INPUT_PULLUP); 
    }
    bool pressed() {
      bool reading = digitalRead(pin);
      if (reading != lastReading) {
        lastReading = reading;
        changedAt = millis();
      }
      if (millis() - changedAt > 30 && reading != stable) {
        stable = reading;
        return stable == LOW; // low - pressed with pull-up
      }
      return false;
    }
  private:
    int pin;
    bool stable = HIGH, lastReading = HIGH;
    unsigned long changedAt = 0;
};
Button tareBtn(PIN_TEAR);


// Global
GameState state = ST_IDLE;
CurlType target = CURL_NONE, lastDetected = CURL_NONE;
bool lastCorrect = false;
int score = 0, misses = 0, roundNum = 0;
unsigned long stateEnteredAt = 0;
unsigned long lastDraw = 0;
bool appStartRequested = false;         // set when the app's Start button is pressed
bool appEndRequested = false;
String feedbackLine = "";

// Socket Server Info
// SocketServer();

// void broadcastStatus(){
  // for now serial, later send this to the app over WebSocket
  // Serial.print("State: ");  Serial.print(stateName(state));
  // Serial.print(" | Target: "); Serial.print(curlName(target));
  // Serial.print(" | Score: ");  Serial.print(score);
  // Serial.print(" | Misses: "); Serial.println(misses);
// };

// Rep detection/counting/verification
// when start is pressed in the app, the gravity direction is marked
// during the curl we measure how far the dumbell has rotated from the start/tared values. One rep
// is "top reached, then back to the start" movement --> this is no matter how the user holds it.

struct Rep {
  Vec3 startG = {0,0,0};
  unsigned long startedAt = 0;
  float angle = 0, maxAngle = 0, maxHandle = 0;
  int samples = 0, twoHandSamples = 0;
  bool reachTop = false;
};
Rep rep;

void startRep() {
  rep = Rep();
  rep.startG = imuGravity();
  rep.startedAt = millis();
}

CurlType completedRep(){
  float twoHandFrac = rep.samples ? (float)rep.twoHandSamples / rep.samples : 0;
  if (twoHandFrac >= PREACHER_TOUCH_FRACTION) return CURL_PREACHER;
  // bicept curl (palms up) - the handle stays level, rotating around its axis)
  // hammer curl (palms are perpendicular to the floor) - the handle turns vertical motion up
  if (rep.maxHandle >= HAMMER_RATIO){
    return CURL_HAMMER;
  }
  return CURL_BICEP;
}

// curl_none when rep is in progress, or the detected type when movement is finished
CurlType updateRep(){
  rep.angle = angleDeg(imuGravity(), rep.startG);
  rep.maxAngle = fmaxf(rep.maxAngle, rep.angle);
  rep.maxHandle = fmaxf(rep.maxHandle, imuHandleRatio());
  rep.samples++;
  if (bothHandsOn()) {
    rep.twoHandSamples++;
  }
  if (rep.angle > TOP_ANGLE_DEG){
    rep.reachTop = true;
  }
  if (rep.reachTop && rep.angle < BOTTOM_ANGLE_DEG){
    return completedRep();
  }
  return CURL_NONE; // if not finished
}

// GAME LOGIC

void setState(GameState s) {
  state = s;
  stateEnteredAt = millis();
  lastDraw = 0; // initialize draw, redraw the screen right away in the new state
  // broadcastStatus();   // not definded yet
}

// void tiltStartRound() {
//   targetIndex = random(NUM_TARGETS);  // a random target for each rep?
//   tiltState = TARING;
// }

void resetGame() {
  score = misses = roundNum = 0;
  target = lastDetected = CURL_NONE;
  lastCorrect = false;
  setState(ST_IDLE);
}

void nextMove() {
  roundNum++;
  CurlType previous = target;
  do {
    target = (CurlType)random(CURL_BICEP, CURL_PREACHER + 1);
  } while (target == previous);   // eliminate same move twice in a row
  lastDetected = CURL_NONE;
  // toneCue();   -- used for proceeding to next move
  setState(ST_WAIT_START);
  sendCommandUpdate();
}

void startNewGame() {
  score = misses = roundNum = 0;
  target = lastDetected = CURL_NONE;
  lastCorrect = false;
  sendGameStarted();
  sendScoreUpdate();
  nextMove();
}

void goGameOver() {
  setState(ST_GAME_OVER);
  sendGameOver();
}

void finishRep(CurlType detected) {
  lastDetected = detected;
  lastCorrect = (detected == target);
  if (lastCorrect) { 
    score++;
    // toneSuccess(); // audio to verify success
    feedbackLine = "Correct! +1";
  }else {
    misses++;
    // toneFail();
    feedbackLine = (detected == CURL_NONE) ? "Too slow!" : String("Missed: ") + curlName(detected);
  }
  setState(ST_FEEDBACK);
  sendUserResult(lastCorrect, detected);
  sendScoreUpdate();
}

void setup(){
  tareBtn.begin();  // setting up the tare pin
  Serial.begin(115200);
  delay(500);

  Wire.begin(PIN_SDA, PIN_SCL);      // BNO055 9-axis
  Wire1.begin(PIN_OLED_SDA, PIN_OLED_SCL);  // OLED

  tareBtn.begin();
  pinMode(PIN_TOUCH, INPUT);
  pinMode(PIN_TOUCH2, INPUT);

  displayBegin();
  showMessage("LIFT-IT", "Starting sensors...");
  if (!imuBegin()){
    showMessage("ERROR", "Check wiring");
    while (true) delay(1000);
  }
  // imuBegin();                       // integrated_sensors.ino
  appBegin();
  String addr = appAddress();
  showMessage("APP Address: ", addr.c_str());
  delay(2500);
  resetGame();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = millis();
  appLoop(); // logic for game
  if (appEndRequested) {
    appEndRequested = false;
    if (state != ST_IDLE && state != ST_GAME_OVER) {
      goGameOver();
    }
  }

  bool draw = (now - lastDraw >= DRAW_MS);
  if (draw){
    lastDraw = now;
  } 
  // if (tareBtn.pressed()) {
  //   // logic needed
  //   //pressed = true;
  // }

  printDebug(stateName(state), rep.angle);

  switch (state) {
    
    case ST_IDLE:
    if (draw) showMessage("LIFT-IT", "Press button to start");
    // until the app is connected, the tare button acts as START
    if (appStartRequested || tareBtn.pressed()){
      appStartRequested = false;
      startNewGame();
    }
    break;

    case ST_WAIT_START:
      if (draw) showWaitStart(curlName(target), roundNum, score, misses);
      // tare on button press, or automatically after the settle time
      if (tareBtn.pressed() || now - stateEnteredAt > SETTLE_MS){
        startRep();
        setState(ST_IN_REP);
      }
      break;

    case ST_IN_REP: {
      static unsigned long lastSample = 0;
      if (now - lastSample >= SAMPLE_MS) {
        lastSample = now;
        CurlType detected = updateRep();
        if (detected != CURL_NONE) {
          finishRep(detected);
          break;
        }
        if (now - stateEnteredAt > REP_TIMEOUT_MS){
          finishRep(CURL_NONE);
          break;
        }
      }

      if (draw) {
        unsigned long elapsed = now - stateEnteredAt;
        unsigned long secLeft = elapsed < REP_TIMEOUT_MS ? (REP_TIMEOUT_MS - elapsed) / 1000 : 0;
        showInRep(curlName(target), rep.angle, rep.reachTop, secLeft);
      }
      break;
    }

      case ST_FEEDBACK:
        if (draw) {
          showFeedback(feedbackLine, score, misses);
        }
        if (now - stateEnteredAt > FEEDBACK_MS ) {
          if (misses >= MAX_MISSES){
            goGameOVer();
          }
          else {
            startNewGame();
          }
        }
        break;  // only runs after the timer ends

        case ST_GAME_OVER:
          if (draw) {
            showGameOver(score);
          }
          if (appStartRequested || tareBtn.pressed()) {
            appStartRequested = false;
            startNewGame();
          }
          break;
      }
    }
