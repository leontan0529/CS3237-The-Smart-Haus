#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>

// Wake-up interval in microseconds (30 seconds)
#define WAKEUP_INTERVAL 30 * 1000000 // 30 seconds in microseconds

//DHT11
#define DHTPIN 16
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

//Presence detection
int motion;
#define PIR_SENSOR_PIN 13

//Light
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;

/*
const char* ssid = "ESP32_Access_Point";
const char* password = "password123";
*/

const char* ssid = "MitsubishiAttrage";
const char* password = "Toyota Vios";

//const char* serverName1 = "http://192.168.4.2:8080/esp32-th";
const char* serverName2 = "http://192.168.115.20:80/post-data";


// Timer set to 5 seconds (5000)
unsigned long timerDelay1 = 5000;
unsigned long timerDelay2 = 6000;

//unsigned long lastTime = 0;
unsigned long lastTime1 = 0;
unsigned long lastTime2 = 0;

WebServer server(80);

float temperature;
float humidity;

StaticJsonDocument<1024> jsonDocument;

char buffer[1024];

// Set your Static IP address
IPAddress local_IP(192, 168, 115, 30);
// Set your Gateway IP address
IPAddress gateway(192, 168, 115, 159);
IPAddress subnet(255, 255, 255, 0);

/*
// Set your Static IP address
IPAddress local_IP(192, 168, 4, 20);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
*/

void setup() {

  Serial.begin(9600);
  delay(1500); 
  
  WiFi.mode(WIFI_STA);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  //DHT11
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  delayMS = sensor.min_delay / 1000;

  //Presence Detection
  pinMode(PIR_SENSOR_PIN, INPUT);

  //Light
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }

  delay(1000);

  // Read sensor data
  readSensors();

  // Send data to the server
  sendData();

  // Configure the ESP32 to wake up every 30 seconds
  esp_sleep_enable_timer_wakeup(WAKEUP_INTERVAL);

  // Print wake-up reason
  printWakeupReason();

  // Go to deep sleep
  Serial.println("Going to deep sleep for 30 seconds...");
  delay(100); // Allow Serial print to complete
  esp_deep_sleep_start();
}

void readSensors() {
  // Read temperature and humidity
  sensors_event_t tempEvent;
  sensors_event_t humidityEvent;
  
  dht.temperature().getEvent(&tempEvent);
  if (isnan(tempEvent.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    temperature = tempEvent.temperature;
  }
  
  dht.humidity().getEvent(&humidityEvent);
  if (isnan(humidityEvent.relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    humidity = humidityEvent.relative_humidity;
  }

  // Read light sensor data
  if (!apds.readAmbientLight(ambient_light)) {
    Serial.println("Error reading light values");
  }

  Serial.println("Temperature: " + String(temperature));
  Serial.println("Humidity: " + String(humidity));
  Serial.println("Ambient Light: " + String(ambient_light));
}

void sendData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName2);
    http.setTimeout(5000);  // Set a 5-second timeout for the request
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String jsonPayload = "{\"Temperature\": \"" + String(temperature) +
                         "\", \"Humidity\": \"" + String(humidity) +
                         "\", \"Brightness\": \"" + String(ambient_light) + "\"}";

    int httpResponseCode = http.POST(jsonPayload);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println("Response: " + response);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Function to print wakeup reason
void printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    default:
      Serial.println("Wakeup not caused by deep sleep timer");
      break;
  }
}

void loop() {
}
