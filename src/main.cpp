// /*********
//  *
//  *  AUTHOR: @jueta
//  *
//  * This code sets up an ESP32-CAM module to connect to a Wi-Fi network and host a web server.
//  * The server allows a user to capture a photo and view the last captured photo through a web interface.
//  * It also uses a PIR sensor to detect motion and automatically capture a photo if motion is detected.
//  * The captured photos are saved in the SPIFFS filesystem of the ESP32.
//  *
//  *********/

// #include <Arduino.h>
// #include "camera.h"
// #include "web.h"

// #define FLASH_GPIO 4
// #define PIR_GPIO 14

// // Replace with your network credentials
// const char *ssid = "SNOWWHITE BIG";
// const char *password = "MRJ2023!";

// // Create AsyncWebServer object on port 80
// AsyncWebServer server(80);

// boolean takeNewPhoto = false;

// // Variable to store the PIR sensor state
// int pirState = LOW;
// volatile unsigned long pir_timer;

// volatile unsigned long loop_timer;

// void setup()
// {
//   // Initialize the PIR sensor pin as an input
//   pinMode(PIR_GPIO, INPUT);
//   // Initialize the flahslight pin as an output
//   pinMode(FLASH_GPIO, OUTPUT);
//   // Begin serial communication at a baud rate of 115200
//   Serial.begin(115200);

//   // Connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }

//   // Photo will be saved on SPIFFS
//   if (!SPIFFS.begin(true))
//   {
//     Serial.println("An Error has occurred while mounting SPIFFS");
//     ESP.restart();
//   }
//   else
//   {
//     delay(500);
//     Serial.println("SPIFFS mounted successfully");
//   }

//   // Print ESP32 Local IP Address
//   Serial.print("IP Address: http://");
//   Serial.println(WiFi.localIP());

//   // Turn-off the 'brownout detector'
//   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

//   // OV2640 camera module
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sscb_sda = SIOD_GPIO_NUM;
//   config.pin_sscb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_JPEG;

//   if (psramFound())
//   {
//     config.frame_size = FRAMESIZE_UXGA;
//     config.jpeg_quality = 10;
//     config.fb_count = 2;
//   }
//   else
//   {
//     config.frame_size = FRAMESIZE_SVGA;
//     config.jpeg_quality = 12;
//     config.fb_count = 1;
//   }
//   // Camera init
//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK)
//   {
//     Serial.printf("Camera init failed with error 0x%x", err);
//     ESP.restart();
//   }

//   // Route for root / web page
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
//             { request->send_P(200, "text/html", index_html); });

//   server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     takeNewPhoto = true;
//     request->send_P(200, "text/plain", "Taking Photo"); });

//   server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest *request)
//             { request->send(SPIFFS, FILE_PHOTO, "image/jpg", false); });

//   // Start server
//   server.begin();
// }

// void loop()
// {

//   if ((millis() - loop_timer) > 1000)
//   {
//     if (takeNewPhoto)
//     {
//       capturePhotoSaveSpiffs();
//     }
//     takeNewPhoto = false;

//     // Read the state of the PIR sensor
//     pirState = digitalRead(PIR_GPIO);

//     // If motion is detected, turn on the LED
//     if (pirState == HIGH)
//     {
//       digitalWrite(FLASH_GPIO, HIGH);
//       Serial.println("Motion detected!");
//       if ((millis() - pir_timer) > 10000)
//       {
//         capturePhotoSaveSpiffs();
//         pir_timer = millis();
//       }
//     }
//     else
//     {
//       // If no motion is detected, turn off the LED
//       digitalWrite(FLASH_GPIO, LOW);
//       Serial.println("No motion.");
//     }

//     loop_timer = millis();
//   }
// }





/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-shield-pcb-telegram/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>

// Replace with your network credentials
const char *ssid = "SNOWWHITE BIG";
const char *password = "MRJ2023!";

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
String chatId = "7013307530";

// Initialize Telegram BOT
String BOTtoken = "7412812872:AAFnQcuuTvOvQ84Bo_N2I3W-cBbSHvgQEac";

bool sendPhoto = false;

WiFiClientSecure clientTCP;

UniversalTelegramBot bot(BOTtoken, clientTCP);

//CAMERA_MODEL_AI_THINKER
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

#define FLASH_LED_PIN 4
bool flashState = LOW;

// Motion Sensor
bool motionDetected = false;


int botRequestDelay = 1000;   // mean time between scan messages
long lastTimeBotRan;     // last time messages' scan has been done

void handleNewMessages(int numNewMessages);
String sendPhotoTelegram();


// Indicates when motion is detected
static void IRAM_ATTR detectsMovement(void * arg){
  //Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
}

void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);
  
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
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

  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  // Drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  // PIR Motion Sensor mode INPUT_PULLUP
  //err = gpio_install_isr_service(0); 
  err = gpio_isr_handler_add(GPIO_NUM_13, &detectsMovement, (void *) 13);  
  if (err != ESP_OK){
    Serial.printf("handler add failed with error 0x%x \r\n", err); 
  }
  err = gpio_set_intr_type(GPIO_NUM_13, GPIO_INTR_POSEDGE);
  if (err != ESP_OK){
    Serial.printf("set intr type failed with error 0x%x \r\n", err);
  }
}

void loop(){
  if (sendPhoto){
    Serial.println("Preparing photo");
    sendPhotoTelegram(); 
    sendPhoto = false; 
  }

  if(motionDetected){
    bot.sendMessage(chatId, "Motion detected!!", "");
    Serial.println("Motion Detected");
    sendPhotoTelegram();
    motionDetected = false;
  }
  
  if (millis() > lastTimeBotRan + botRequestDelay){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages){
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

String sendPhotoTelegram(){
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));

  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
}

void handleNewMessages(int numNewMessages){
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++){
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != chatId){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String fromName = bot.messages[i].from_name;

    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo  request");
    }
    if (text == "/readings"){
      String readings = "DEU BOM!";
      bot.sendMessage(chatId, readings, "");
    }
    if (text == "/start"){
      String welcome = "Welcome to the ESP32-CAM Telegram bot.\n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggle flash LED\n";
      welcome += "/readings : request sensor readings\n\n";
      welcome += "You'll receive a photo whenever motion is detected.\n";
      bot.sendMessage(chatId, welcome, "Markdown");
    }
  }
}