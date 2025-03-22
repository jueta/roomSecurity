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
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include <FS.h>
#include "soc/rtc_cntl_reg.h"

#define FLASH_GPIO 4
#define PIR_GPIO 14

// Replace with your network credentials
const char *ssid = "SNOWWHITE BIG";
const char *password = "MRJ2023!";

boolean takeNewPhoto = false;

// Variable to store the PIR sensor state
int pirState = LOW;
volatile unsigned long pir_timer;

volatile unsigned long loop_timer;

void setup()
{
  // Initialize the PIR sensor pin as an input
  pinMode(PIR_GPIO, INPUT);
  // Initialize the flahslight pin as an output
  pinMode(FLASH_GPIO, OUTPUT);

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);

  // WIFI config
  WiFi.mode(WIFI_STA);
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

  // SPIFFS config
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else
  {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }


  // OV2640 camera config
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

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  sendPhoto(); 
}

void loop()
{

  if ((millis() - loop_timer) > 1000)
  {
    if (takeNewPhoto)
    {
      sendPhoto(); 
    }
    takeNewPhoto = false;

    // Read the state of the PIR sensor
    pirState = digitalRead(PIR_GPIO);

    // If motion is detected, turn on the LED
    if (pirState == HIGH)
    {
      // digitalWrite(FLASH_GPIO, HIGH);
      Serial.println("Motion detected!");
      if ((millis() - pir_timer) > 10000)
      {
        sendPhoto(); 
        pir_timer = millis();
      }
    }
    else
    {
      // If no motion is detected, turn off the LED
      // digitalWrite(FLASH_GPIO, LOW);
      Serial.println("No motion.");
    }

    loop_timer = millis();
  }
}

