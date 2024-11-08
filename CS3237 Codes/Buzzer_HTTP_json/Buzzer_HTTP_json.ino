#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Wire.h>

#define DOOR_SENSOR_PIN  23
int buzzer = 5; //Define buzzer receiver pin
int doorState = LOW;

//true is arm, false is disarm
bool alarm_status = false;

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

  alarm_status = jsonDocument["alarm_status"];

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
  addJsonObject("alarm_status", alarm_status);
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
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
}

void loop() {
  server.handleClient();

  doorState = digitalRead(DOOR_SENSOR_PIN); // read state

   if (doorState == HIGH) {
    Serial.println("The door is open, turns on Piezo Buzzer");
    alarm_status = 1;
    digitalWrite(buzzer, LOW);
  }

  if (!alarm_status) {
      digitalWrite(buzzer, HIGH); //Stop the sound
  }

  Serial.print("Door state: ");
  Serial.println(doorState);
  Serial.print("Alarm status: ");
  Serial.println(alarm_status);


//active low, so opposite

/*
if (!alarm_status) {
      digitalWrite(buzzer, HIGH); //Stop the sound
    } else {
      if (doorState == LOW) {
    Serial.println("The door is closed, turns off Piezo Buzzer");
    digitalWrite(BUZZER_PIN, HIGH);
    }

  if (doorState == LOW) {
    Serial.println("The door is closed, turns off Piezo Buzzer");
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    Serial.println("The door is open, turns on Piezo Buzzer");
    digitalWrite(BUZZER_PIN, LOW);
  }
*/

}