// Touch sensor test for ESP32-S3
// Prints to the Serial Monitor (115200 baud) whenever the sensor's output changes.

const int DIGITAL_PIN = 4;     // module DO (or SIG / I/O on 3-pin modules)
//const int ANALOG_PIN  = 1;     // module AO (4-pin modules only, ADC1 pin)
const int ANALOG_PIN  = 34;    // module AO (ADC1 pin on ESP32-WROOM)
const bool USE_ANALOG = false; // set to true if you wired AO to GPIO 1

int lastState = -1;
unsigned long lastAnalogPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); // give the Serial Monitor a moment to connect

  pinMode(DIGITAL_PIN, INPUT);
  analogReadResolution(12); // analog values range 0-4095

  Serial.println("Touch sensor test ready. Touch the sensor...");
}

void loop() {
  int state = digitalRead(DIGITAL_PIN);

  if (state != lastState) {
    Serial.print("Digital output: ");
    Serial.println(state == HIGH ? "HIGH" : "LOW");
    lastState = state;
  }

  if (USE_ANALOG && millis() - lastAnalogPrint >= 250) {
    Serial.print("Analog value: ");
    Serial.println(analogRead(ANALOG_PIN));
    lastAnalogPrint = millis();
  }

  delay(10);
}
