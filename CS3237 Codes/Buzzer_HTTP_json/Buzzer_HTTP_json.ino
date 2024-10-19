#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Wire.h>

int buzzer = 5; //Define buzzer receiver pin

bool buzzer_status = false;

// Web server running on port 80
WebServer server(80);

StaticJsonDocument<1024> jsonDocument;

char buffer[1024];

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  buzzer_status = jsonDocument["buzzer_status"];

  // Respond to the client
  server.send(200, "application/json", "{}");
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
  addJsonObject("buzzer_status", buzzer_status);
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void setupApi() {
  server.on("/getValues", getValues);
  server.on("/setStatus", HTTP_POST, handlePost);
 
  // start server
  server.begin();
}

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  delay(1500); 

  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("MitsubishiAttrage","Toyota Vios"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Connected...yeey :)");
  }

  setupApi();

  pinMode(buzzer, OUTPUT);//Set the output mode
}

void loop() {
  server.handleClient();

//active low, so opposite
  if (!buzzer_status) {
    digitalWrite(buzzer, HIGH); //sound production
  } else {
    digitalWrite(buzzer, LOW); //Stop the sound
  }
  
  delay(1000);
}
