#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <HTTPClient.h>

#define DOOR_SENSOR_PIN  23
int buzzer = 5; // Define buzzer receiver pin
int doorState = LOW;
int previousDoorState = LOW; // Track previous state to detect changes

// true is arm, false is disarm
bool alarm_status = false;

// Web server running on port 80
WebServer server(80);

// Define server IP and port for notification
//const char* serverName = "http://192.168.33.223:8080/door_open"; // Change IP to your Flask server

StaticJsonDocument<1024> jsonDocument;

char buffer[1024];

/*
void handlePost() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Bad Request - No Data");
    return;
  }

  String body = server.arg("plain");

  // Check if received data is "1" or "0" and set alarm_status accordingly
  if (body == "1") {
    alarm_status = true;
    server.send(200, "text/plain", "Alarm armed");
  } else if (body == "0") {
    alarm_status = false;
    server.send(200, "text/plain", "Alarm disarmed");
  } else {
    // Invalid data
    server.send(400, "text/plain", "Bad Request - Invalid Data");
  }
}
*/

void handleControl() {
  // Print a message to the Serial Monitor
  Serial.println("Received control command!");

  // Check for 'state' parameter in the URL
  if (server.hasArg("state")) {
    String state = server.arg("state");

    if (state == "on") {
      Serial.println("Received 'On' command from Grafana!");
      alarm_status = 1;
    } else if (state == "off") {
      Serial.println("Received 'Off' command from Grafana!");
      alarm_status = 0;
    } else {
      Serial.println("Unknown command received.");
    }
    // Send a response back to the client
    server.send(200, "text/plain", "Command received: " + state);
  } else {
    server.send(400, "text/plain", "Missing 'state' parameter");
  }
}

void createJson(char *name, int value) {  
  jsonDocument.clear();
  jsonDocument["name"] = name;
  jsonDocument["value"] = value;
  serializeJson(jsonDocument, buffer);  
}

void addJsonObject(char *name, int value) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
}

void getValues() {
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("alarm_status", alarm_status);
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void setupApi() {
  server.on("/getValues", getValues);
  //server.on("/setStatus", HTTP_POST, handleControl);
  server.on("/setStatus", handleControl);
 
  // start server
  server.begin();
}

// Set your Static IP address
IPAddress local_IP(192, 168, 4, 10);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  delay(1500); 

  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
  }

  const char* ssid = "ESP32_Access_Point";
  const char* password = "password123";


  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.print(ssid);

  setupApi();

  pinMode(buzzer, OUTPUT); // Set the output mode
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // Set ESP32 pin to input pull-up mode
}

void sendDoorOpenNotification() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "text/plain"); // Set header to plain text
    int httpResponseCode = http.POST("Door opened"); // Send POST request with "Door opened" message

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // End the HTTP connection
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() { 
  
  server.handleClient();

  doorState = digitalRead(DOOR_SENSOR_PIN); // read state

   if (doorState == HIGH) {
    Serial.println("The door is open, turns on Piezo Buzzer");
    alarm_status = 1;
    digitalWrite(buzzer, LOW);
    sendDoorOpenNotification(); // Send notification when door opens
  }

  if (!alarm_status) {
      digitalWrite(buzzer, HIGH); //Stop the sound
  }

  Serial.print("Door state: ");
  Serial.println(doorState);
  Serial.print("Alarm status: ");
  Serial.println(alarm_status);

  previousDoorState = doorState; // Update previous state
}
