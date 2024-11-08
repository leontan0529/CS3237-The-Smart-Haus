#include <WiFi.h>
#include <WebServer.h>
#include <SparkFun_APDS9960.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> jsonDocument;

//Presence detection
int motion;
#define PIR_SENSOR_PIN 13

//Light
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

const char* serverName2 = "http://192.168.4.2:8080/esp32-mb";

unsigned long timerDelay2 = 6000;
unsigned long lastTime2 = 0;

const char* ssid = "ESP32_Access_Point";
const char* password = "password123";
WebServer server(80);

// Variables to store received data
int receivedTemperature;
int receivedHumidity;
int receivedBrightness;


void handlePostRequest() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("Received POST data:");
    Serial.println(body);

    // Parse JSON data
    StaticJsonDocument<200> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, body);
    if (error) {
      Serial.print("JSON deserialization failed: ");
      Serial.println(error.c_str());
      server.send(400, "application/json", "{\"status\":\"Error\"}");
      return;
    }

    // Extract data
    receivedTemperature = jsonDocument["Temperature"].as<int>();
    receivedHumidity = jsonDocument["Humidity"].as<int>();
    receivedBrightness = jsonDocument["Brightness"].as<int>();

    
    Serial.print("Temperature: ");
    Serial.println(receivedTemperature);
    Serial.print("Humidity: ");
    Serial.println(receivedHumidity);
    Serial.print("Brightness: ");
    Serial.println(receivedBrightness);

    server.send(200, "application/json", "{\"status\":\"Success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"Error\"}");
  }
}


void handleRoot() {
  IPAddress clientIP = server.client().remoteIP();  // Get client IP address
  Serial.print("Connected device IP: ");
  Serial.println(clientIP);
  server.send(200, "text/plain", "Hello from ESP32 Access Point!");  // Simple response
}

void setup() {
  Serial.begin(9600);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

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
  
  // Set up the ESP32 as an access point
  WiFi.softAP(ssid, password);
  
  // Display the IP address of the access point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  // Start web server and define route
  server.on("/post-data", HTTP_POST, handlePostRequest);
  server.begin();
  Serial.println("Web server started");

  delay(1000);
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Show number of connected clients
  int numStations = WiFi.softAPgetStationNum();
  Serial.print("Number of connected devices: ");
  Serial.println(numStations);

  //Presence Detection
  int sensorValue = digitalRead(PIR_SENSOR_PIN);
  if (sensorValue == HIGH) {
    motion = 1;
  } else {
    motion = 0;
  }

  Serial.println(receivedTemperature);
  Serial.println(receivedHumidity);
  Serial.println(receivedBrightness);
  Serial.println(motion);


  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Humidity = " + receivedHumidity);
  lcd.setCursor(0,1);
  lcd.print("Temperature = " + receivedTemperature);
  delay(1000);

  
  // Send an HTTP POST request every 6 seconds for light and presence
  if ((millis() - lastTime2) > timerDelay2) {
      WiFiClient client;
      HTTPClient http;
      // Domain name or IP address with port
      http.begin(client, serverName2);
      http.addHeader("Content-Type", "application/json");
      String jsonPayload = "{\"Motion\": \"" + String(motion) + 
                    "\", \"Brightness\": \"" + String(receivedBrightness) + "\"}";

      int httpResponseCode = http.POST(jsonPayload);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Response: " + response);
      http.end();
    lastTime2 = millis();
  }
  
  delay(6000); // Update every 6 seconds
  
}
