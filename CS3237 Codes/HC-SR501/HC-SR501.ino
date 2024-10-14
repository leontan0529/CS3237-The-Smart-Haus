/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-motion-sensor
 */

const int PIN_TO_SENSOR = 19; // GPIO19 pin connected to OUTPUT pin of sensor
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin
const unsigned long DELAY_TIME_MS = 30000; // 30000 miliseconds ~ 30 seconds
bool delayEnabled = false;
unsigned long delayStartTime;


void setup() {
  Serial.begin(9600);            // initialize serial
  pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from OUTPUT pin of sensor
}

void loop() {
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");
    Serial.println("Turning on / activating");
    delayEnabled = false; // disable delay
    // TODO: turn on alarm, light or activate a device ... here
  }
  else
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");
    Serial.println("Motion stopped!");
    delayEnabled = true; // enable delay
    delayStartTime = millis(); // set start time
    // TODO: turn off alarm, light or deactivate a device ... here
  }

  if (delayEnabled == true && (millis() - delayStartTime) >= DELAY_TIME_MS) {
    Serial.println("Turning off / deactivating");
    delayEnabled = false; // disable delay
    // TODO: turn off alarm, light or deactivate a device ... here
  }
}
