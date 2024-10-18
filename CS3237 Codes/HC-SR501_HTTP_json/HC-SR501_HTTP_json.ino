#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

int motion;

#define PIR_SENSOR_PIN 13 // Adjust according to the GPIO pin you're using

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
  //jsonDocument.clear(); // Clear json buffer
  addJsonObject("Motion Status", motion);

  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void setupApi() {
  server.on("/getValues", getValues);
  //server.on("/setStatus", HTTP_POST, handlePost);
 
  // start server
  server.begin();
}

void setup() {

  Serial.begin(115200);
  delay(1500); 

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

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

  pinMode(PIR_SENSOR_PIN, INPUT);
  
  Serial.println("PIR Motion Sensor Test - Continuous Updates");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  // Read PIR sensor value
  int sensorValue = digitalRead(PIR_SENSOR_PIN);
  
  // Print continuous updates based on the sensor state
  if (sensorValue == HIGH) {
    motion = 1;
  } else {
    motion = 0;
  }

  Serial.println(motion);

  // Delay for readability in the serial monitor
  delay(1000); // Adjust delay time as needed (1000ms = 1 second)
}