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

const char* ssid = "ESP32_Access_Point";
const char* password = "password123";

const char* serverName = "http://192.168.4.2:8080/mb";

// Timer set to 5 seconds (5000)
unsigned long timerDelay = 5000;
unsigned long lastTime = 0;

WebServer server(80);

float temperature;
float humidity;

StaticJsonDocument<1024> jsonDocument;

char buffer[1024];

// Set your Static IP address
IPAddress local_IP(192, 168, 4, 20);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

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
}

void loop() {

  //DHT11
  delay(delayMS);
  sensors_event_t tempEvent;
  sensors_event_t humidityEvent;
  
  dht.temperature().getEvent(&tempEvent);
  if (isnan(tempEvent.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    temperature = tempEvent.temperature;
  }
  
  dht.humidity().getEvent(&humidityEvent);
  if (isnan(humidityEvent.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    humidity = humidityEvent.relative_humidity;
  }

  //Presence Detection
  int sensorValue = digitalRead(PIR_SENSOR_PIN);
  if (sensorValue == HIGH) {
    motion = 1;
  } else {
    motion = 0;
  }

  // Read the light levels
  if (!apds.readAmbientLight(ambient_light)) {
    Serial.println("Error reading light values");
  }

  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(motion);
  Serial.println(ambient_light);

  delay(1000);

  // Send an HTTP POST request every 5 seconds
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      // Domain name or IP address with port
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      String jsonPayload = "{\"Temperature\":\"" + String(temperature) + 
                    "\", \"Humidity\":\"" + String(humidity) + 
                    "\", \"Motion\":\"" + String(motion) + 
                    "\", \"Light\":\"" + String(ambient_light) + "\"}";
      int httpResponseCode = http.POST(jsonPayload);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Response: " + response);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
