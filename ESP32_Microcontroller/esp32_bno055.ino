
/*
  ESP32 + BNO055 Motion Sensor Reader
  -----------------------------------
  Wiring (BNO055 board  ->  ESP32):
    VIN  -> 3V3
    GND  -> GND
    SDA  -> GPIO 21
    SCL  -> GPIO 22
    (leave the other pins unconnected)

  Library needed (Tools -> Manage Libraries):
    "Adafruit BNO055" by Adafruit  (click "Install All" to get its helpers too)

  Open Serial Monitor at 115200 baud to read numbers,
  or Tools -> Serial Plotter to see live graphs.
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

const int SDA_PIN = 21;
const int SCL_PIN = 22;

// The BNO055 can live at address 0x28 or 0x29 depending on the board.
// We'll try both automatically.
Adafruit_BNO055 bno28 = Adafruit_BNO055(55, 0x28, &Wire);
Adafruit_BNO055 bno29 = Adafruit_BNO055(55, 0x29, &Wire);
Adafruit_BNO055* bno = nullptr;

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (bno28.begin()) {
    bno = &bno28;
    Serial.println("BNO055 found at address 0x28!");
  } else if (bno29.begin()) {
    bno = &bno29;
    Serial.println("BNO055 found at address 0x29!");
  } else {
    Serial.println("BNO055 not found! Check wiring.");
    while (true) {
      delay(1000);   // stop here so you can fix the wiring
    }
  }

  delay(1000);

  // Adafruit boards have a small timing crystal that makes readings more stable.
  // If your board is a generic one and readings act strangely, change true to false.
  bno->setExtCrystalUse(true);
}

void loop() {
  // Raw acceleration in m/s^2 (includes gravity: about 9.8 pointing down)
  imu::Vector<3> accel = bno->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  // Acceleration with gravity removed: only your actual movement (0 when still)
  imu::Vector<3> linear = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // Orientation in degrees, calculated by the sensor itself
  imu::Vector<3> euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);

  // Calibration levels: 0 = not calibrated, 3 = fully calibrated
  uint8_t sys, gyroCal, accelCal, magCal;
  bno->getCalibration(&sys, &gyroCal, &accelCal, &magCal);

  // "Name:value" format works in both Serial Monitor and Serial Plotter
  Serial.print("AccelX:");    Serial.print(accel.x(), 2);
  Serial.print(",AccelY:");   Serial.print(accel.y(), 2);
  Serial.print(",AccelZ:");   Serial.print(accel.z(), 2);

  Serial.print(",MoveX:");    Serial.print(linear.x(), 2);
  Serial.print(",MoveY:");    Serial.print(linear.y(), 2);
  Serial.print(",MoveZ:");    Serial.print(linear.z(), 2);

  Serial.print(",Heading:");  Serial.print(euler.x(), 1);
  Serial.print(",Roll:");     Serial.print(euler.y(), 1);
  Serial.print(",Pitch:");    Serial.print(euler.z(), 1);

  Serial.print(",Cal_Sys:");  Serial.print(sys);
  Serial.print(",Cal_Acc:");  Serial.print(accelCal);
  Serial.print(",Cal_Gyro:"); Serial.print(gyroCal);
  Serial.print(",Cal_Mag:");  Serial.println(magCal);

  delay(100);   // 10 readings per second
}
