/*********
 * 
 *  AUTHOR: @jueta
 * 
 * This code sets up an ESP32-CAM module to connect to a Wi-Fi network and host a web server. 
 * The server allows a user to capture a photo and view the last captured photo through a web interface. 
 * It also uses a PIR sensor to detect motion and automatically capture a photo if motion is detected. 
 * The captured photos are saved in the SPIFFS filesystem of the ESP32.
 * 
*********/

#include <Arduino.h>
#include "camera.h"


// Replace with your network credentials
const char* ssid = "SNOWWHITE BIG";
const char* password = "MRJ2023!";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean takeNewPhoto = false;


const int pirPin = 14; // PIR sensor output pin connected to digital pin 2
// Variable to store the PIR sensor state
int pirState = LOW;
volatile unsigned long timer;


void setup() {
  // Initialize the PIR sensor pin as an input
  pinMode(pirPin, INPUT);
  // Initialize the flahslight pin as an output
  // pinMode(lahslightPin, OUTPUT);
  // Begin serial communication at a baud rate of 9600
  Serial.begin(9600);

  // // Connect to Wi-Fi
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }


  
  // if (!SPIFFS.begin(true)) {
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   ESP.restart();
  // }
  // else {
  //   delay(500);
  //   Serial.println("SPIFFS mounted successfully");
  // }

  // // Print ESP32 Local IP Address
  // Serial.print("IP Address: http://");
  // Serial.println(WiFi.localIP());

  // // Turn-off the 'brownout detector'
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // // OV2640 camera module
  // camera_config_t config;
  // config.ledc_channel = LEDC_CHANNEL_0;
  // config.ledc_timer = LEDC_TIMER_0;
  // config.pin_d0 = Y2_GPIO_NUM;
  // config.pin_d1 = Y3_GPIO_NUM;
  // config.pin_d2 = Y4_GPIO_NUM;
  // config.pin_d3 = Y5_GPIO_NUM;
  // config.pin_d4 = Y6_GPIO_NUM;
  // config.pin_d5 = Y7_GPIO_NUM;
  // config.pin_d6 = Y8_GPIO_NUM;
  // config.pin_d7 = Y9_GPIO_NUM;
  // config.pin_xclk = XCLK_GPIO_NUM;
  // config.pin_pclk = PCLK_GPIO_NUM;
  // config.pin_vsync = VSYNC_GPIO_NUM;
  // config.pin_href = HREF_GPIO_NUM;
  // config.pin_sscb_sda = SIOD_GPIO_NUM;
  // config.pin_sscb_scl = SIOC_GPIO_NUM;
  // config.pin_pwdn = PWDN_GPIO_NUM;
  // config.pin_reset = RESET_GPIO_NUM;
  // config.xclk_freq_hz = 20000000;
  // config.pixel_format = PIXFORMAT_JPEG;

  // if (psramFound()) {
  //   config.frame_size = FRAMESIZE_UXGA;
  //   config.jpeg_quality = 10;
  //   config.fb_count = 2;
  // } else {
  //   config.frame_size = FRAMESIZE_SVGA;
  //   config.jpeg_quality = 12;
  //   config.fb_count = 1;
  // }
  // // Camera init
  // esp_err_t err = esp_camera_init(&config);
  // if (err != ESP_OK) {
  //   Serial.printf("Camera init failed with error 0x%x", err);
  //   ESP.restart();
  // }

  // // Route for root / web page
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
  //   request->send_P(200, "text/html", index_html);
  // });

  // server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
  //   takeNewPhoto = true;
  //   request->send_P(200, "text/plain", "Taking Photo");
  // });

  // server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
  //   request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  // });

  // // Start server
  // server.begin();

}




void loop() {
  
  if (takeNewPhoto) {
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  }

  // Read the state of the PIR sensor
  pirState = digitalRead(pirPin);

  // If motion is detected, turn on the LED
  if (pirState == HIGH) {
    // digitalWrite(flashPin, HIGH);
    Serial.println("Motion detected!");
    if ((millis() - timer) > 10000){
        // capturePhotoSaveSpiffs();
        timer = millis();
      }
  } else {
    // If no motion is detected, turn off the LED
    // digitalWrite(flashPin, LOW);
    Serial.println("No motion.");
  }

  delay(1000);
  
  }


