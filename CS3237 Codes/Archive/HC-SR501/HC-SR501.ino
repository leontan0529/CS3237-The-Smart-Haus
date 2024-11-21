// Define the pin for the HC-SR501 motion sensor
#define PIR_SENSOR_PIN 13 // Adjust according to the GPIO pin you're using

void setup() {
  // Initialize the serial communication
  Serial.begin(115200);
  
  // Set PIR sensor pin as input
  pinMode(PIR_SENSOR_PIN, INPUT);
  
  Serial.println("PIR Motion Sensor Test - Continuous Updates");
}

void loop() {
  // Read PIR sensor value
  int sensorValue = digitalRead(PIR_SENSOR_PIN);
  
  // Print continuous updates based on the sensor state
  if (sensorValue == HIGH) {
    Serial.println("Motion detected!");
  } else {
    Serial.println("No motion detected.");
  }

  // Delay for readability in the serial monitor
  delay(1000); // Adjust delay time as needed (1000ms = 1 second)
}
