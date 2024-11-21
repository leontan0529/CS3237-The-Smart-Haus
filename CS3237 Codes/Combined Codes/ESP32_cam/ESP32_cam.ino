#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

#define LED_FLASH_PIN 4  // GPIO pin for the LED (on some ESP32-CAMs, it's GPIO 4)

const char* ssid = "MitsubishiAttrage";
const char* password = "Toyota Vios";

String serverName = "192.168.115.8";

String serverPath = "/upload";

const int serverPort = 9091;

bool camera_status = 0;
bool photo_taken = 0;

WiFiClient client;

/*
// Set your Static IP address
IPAddress local_IP(192, 168, 4, 30);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
*/

// Set your Static IP address
IPAddress local_IP(192, 168, 115, 40);
IPAddress gateway(192, 168,115, 159);
IPAddress subnet(255, 255, 255, 0);

// Web server running on port 80
WebServer server(80);

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int timerInterval = 30000;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent



void handlePost() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Bad Request - No Data");
    return;
  }

  String body = server.arg("plain");

  // Check if received data is "1" or "0" and set camera_status accordingly
  if (body == "1") {
    camera_status = true;
    server.send(200, "text/plain", "Photo taken");
  } else if (body == "0") {
    camera_status = false;
    server.send(200, "text/plain", "no photo taken");
  } else {
    // Invalid data
    server.send(400, "text/plain", "Bad Request - Invalid Data");
  }
}

void setupApi() {
  server.on("/setStatus", HTTP_POST, handlePost);
 
  // start server
  server.begin();
}

String sendPhoto() {
  String getAll;
  String getBody;

  // Turn on the flash LED
  digitalWrite(LED_FLASH_PIN, HIGH);
  delay(100);  // Brief delay to allow the LED to brighten

  // Capture the first image
  camera_fb_t * fb = esp_camera_fb_get();
  digitalWrite(LED_FLASH_PIN, LOW);  // Turn off the LED flash immediately after capture

  if (!fb) {
    Serial.println("First camera capture failed");
  } else {
    // Discard the first image
    esp_camera_fb_return(fb);  // Return the frame buffer memory
    Serial.println("First image discarded");
  }

  // Now capture the second image
  fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Second camera capture failed");
  } else {
    // Process the second image (send it, save it, etc.)
    Serial.println("Second image captured");

    // Process the second image here (send to server, save, etc.)
    // For example, send it over HTTP or save it to SD card
  }
  /*
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }
  */
  
  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");    
    String head = "--SmartHaus\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--SmartHaus--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=SmartHaus");
    client.println();
    client.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(tail);
    
    esp_camera_fb_return(fb);
    
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);      
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}

void setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(9600);

  pinMode(LED_FLASH_PIN, OUTPUT);
  digitalWrite(LED_FLASH_PIN, LOW);  // Make sure the LED is initially off

  WiFi.mode(WIFI_STA);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
  }

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 30;  //0-63 lower number means higher quality
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 30;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  setupApi();
  //sendPhoto(); 
}

void loop() {
  server.handleClient();

  /*
  //unsigned long currentMillis = millis();
  if (camera_status == 1) {
    sendPhoto();
    //previousMillis = currentMillis;
    camera_status = 0;
  }
  */

  // Check if there's a transition from 0 to 1
  // Trigger sendPhoto() only if the door just opened (camera_status goes from 0 to 1)
  if (camera_status == 1 && photo_taken == 0) {
    sendPhoto(); // Capture and send a photo when door opens
    photo_taken = 1;
    delay(50);
  } else if (camera_status == 0) {
    photo_taken = 0;
  }

  // Wait for a second before reading again
  delay(1000);
}
