#include <WiFi.h>
#include <WebServer.h>
#include <SparkFun_APDS9960.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> jsonDocument;

// Buffer to store the received image data
std::vector<uint8_t> imageBuffer;

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

const char* serverName1 = "http://192.168.4.2:8080/esp32-th";
const char* serverName2 = "http://192.168.4.2:8080/esp32-mb";
const char* serverName3 = "http://192.168.4.2:5000/upload";

unsigned long timerDelay1 = 5000;
unsigned long timerDelay2 = 6000;

unsigned long lastTime1 = 0;
unsigned long lastTime2 = 0;

const char* ssid = "ESP32_Access_Point";
const char* password = "password123";

WebServer server(80);

// Variables to store received data
int receivedTemperature;
int receivedHumidity;
int receivedBrightness;

unsigned long previousMillis = 0; // Stores the last time display was updated
const long interval = 2000;       // Interval of 2 seconds
bool showFirstSet = true;         // Boolean to toggle between display sets


// Handler function for receiving data from esp32 with humidity, temp and brightness
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


// To receive image data via POST from esp32-cam
void handleImageUpload() {
  if (server.uri() == "/upload" && server.method() == HTTP_POST) {
    if (server.hasArg("plain")) {
      Serial.println("Error: Expecting binary data, not plain text");
      server.send(400, "application/json", "{\"status\":\"Error\"}");
      return;
    }

    // Allocate buffer to hold incoming binary data
    HTTPUpload& upload = server.upload();
    
    if (upload.status == UPLOAD_FILE_START) {
      // Clear the image buffer when starting a new upload
      imageBuffer.clear();
      Serial.println("Starting image upload...");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // Append received data to the buffer
      imageBuffer.insert(imageBuffer.end(), upload.buf, upload.buf + upload.currentSize);
      Serial.printf("Received %d bytes\n", upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      // Complete upload; the entire image is now in imageBuffer
      Serial.printf("Upload completed, total size: %d bytes\n", imageBuffer.size());
      
      // Send success response
      server.send(200, "application/json", "{\"status\":\"Success\"}");

      // Process image data as needed, e.g., save to SPIFFS, display, etc.
    }
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

  // Start web server and define route for temp, humidity and brightness
  server.on("/post-data", HTTP_POST, handlePostRequest);
  // Define route for receiving POST request with image data from esp32-cam
  server.on("/upload", HTTP_POST, handleImageUpload);
  
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


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   // Update the time
    showFirstSet = !showFirstSet;     // Toggle between first and second set

    lcd.clear(); // Clear the display before showing new data

    if (showFirstSet) {
    lcd.setCursor(0, 0); // Set the cursor to the first row, first column
    lcd.print("Humidity: ");  // Print the label
    lcd.setCursor(9, 0); // Set the cursor position to start after "Humidity: "
    lcd.print("        "); // Clear any previous humidity value (6 spaces)
    lcd.setCursor(9, 0); // Set the cursor back to where the value should start
    lcd.print(receivedHumidity);  // Display the humidity value

    lcd.setCursor(0, 1); // Set the cursor to the first row, first column
    lcd.print("Temperature: ");  // Print the label
    lcd.setCursor(12, 1); // Set the cursor position to start after "Humidity: "
    lcd.print("        "); // Clear any previous humidity value (6 spaces)
    lcd.setCursor(12, 1); // Set the cursor back to where the value should start
    lcd.print(receivedTemperature);  // Display the humidity value

    } else {

    lcd.setCursor(0, 0); // Set the cursor to the first row, first column
    lcd.print("Brightness: ");  // Print the label
    lcd.setCursor(11, 0); // Set the cursor position to start after "Humidity: "
    lcd.print("        "); // Clear any previous humidity value (6 spaces)
    lcd.setCursor(11, 0); // Set the cursor back to where the value should start
    lcd.print(receivedBrightness);  // Display the humidity value

    lcd.setCursor(0, 1); // Set the cursor to the first row, first column
    lcd.print("Motion: ");  // Print the label
    lcd.setCursor(7, 1); // Set the cursor position to start after "Humidity: "
    lcd.print("        "); // Clear any previous humidity value (6 spaces)
    lcd.setCursor(7, 1); // Set the cursor back to where the value should start
    lcd.print(motion);  // Display the humidity value
    }
  }

  
  // Send via HTTP POST request for light and presence to server
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

  // Sending an HTTP POST request for temperature and humidity on behalf of esp32_TH to server
  if ((millis() - lastTime1) > timerDelay1) {
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      // Domain name or IP address with port
      http.begin(client, serverName1);
      http.addHeader("Content-Type", "application/json");
      String jsonPayload = "{\"Temperature\": \"" + String(receivedTemperature) + 
                    "\", \"Humidity\": \"" + String(receivedHumidity) + "\"}";
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
    lastTime1 = millis();
  }

  // Send image to server
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient client;
    http.begin(client, serverName3);
    
    // Prepare multipart/form-data for sending image
    String boundary = "SmartHaus";
    String header = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--" + boundary + "--\r\n";

    // Convert String to uint8_t* (byte array)
    uint8_t* headerBytes = (uint8_t*)header.c_str();
    uint8_t* tailBytes = (uint8_t*)tail.c_str();
    
    // Calculate total length
    uint32_t totalLen = header.length() + imageBuffer.size() + tail.length();

    // Add headers
    http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
    http.addHeader("Content-Length", String(totalLen));
    
    // Send the POST request header
    int httpResponseCode = http.sendRequest("POST", headerBytes, header.length());
    
    if (httpResponseCode == HTTP_CODE_OK) {
        // Send the image data in chunks
        Serial.println("Sending image to server...");
        
        // Send image data
        http.sendRequest("", imageBuffer.data(), imageBuffer.size());  // Send image buffer
        
        // Send the tail (boundary end)
        http.sendRequest("", tailBytes, tail.length());  // Send boundary tail
    } else {
        Serial.println("Failed to send image. Response Code: " + String(httpResponseCode));
    }
    
    // Close the HTTP client
    http.end();
}


  
  delay(5000);
  
}