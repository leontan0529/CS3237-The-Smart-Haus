#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_Access_Point";
const char* password = "password123";
WebServer server(80);

void handleRoot() {
  IPAddress clientIP = server.client().remoteIP();  // Get client IP address
  Serial.print("Connected device IP: ");
  Serial.println(clientIP);
  server.send(200, "text/plain", "Hello from ESP32 Access Point!");  // Simple response
}

void setup() {
  Serial.begin(9600);
  
  // Set up the ESP32 as an access point
  WiFi.softAP(ssid, password);
  
  // Display the IP address of the access point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  // Start web server and define route
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Show number of connected clients
  int numStations = WiFi.softAPgetStationNum();
  Serial.print("Number of connected devices: ");
  Serial.println(numStations);

  delay(5000); // Update every 5 seconds
}
