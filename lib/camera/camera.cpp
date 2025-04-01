// -----------------------------------------------------------------
//
//  Piserver Code
//
// -----------------------------------------------------------------


// #include"camera.h"


// String serverName = "192.168.0.81";   

// String serverPath = "/upload.php"; 

// const int serverPort = 80;

// WiFiClient client;

// // Check if photo capture was successful
// bool checkPhoto( fs::FS &fs ) {
//   File f_pic = fs.open( FILE_PHOTO );
//   unsigned int pic_sz = f_pic.size();
//   return ( pic_sz > 100 );
// }

// // Capture Photo and Save it to SPIFFS
// void capturePhotoSaveSpiffs( void ) {
//   camera_fb_t * fb = NULL; // pointer
//   bool ok = 0; // Boolean indicating if the picture has been taken correctly

//   do {
//     // Take a photo with the camera
//     Serial.println("Taking a photo...");

//     fb = esp_camera_fb_get();
//     if (!fb) {
//       Serial.println("Camera capture failed");
//       return;
//     }

//     // Photo file name
//     Serial.printf("Picture file name: %s\n", FILE_PHOTO);
//     File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

//     // Insert the data in the photo file
//     if (!file) {
//       Serial.println("Failed to open file in writing mode");
//     }
//     else {
//       file.write(fb->buf, fb->len); // payload (image), payload length
//       Serial.print("The picture has been saved in ");
//       Serial.print(FILE_PHOTO);
//       Serial.print(" - Size: ");
//       Serial.print(file.size());
//       Serial.println(" bytes");
//     }
//     // Close the file
//     file.close();
//     esp_camera_fb_return(fb);

//     // check if file has been correctly saved in SPIFFS
//     ok = checkPhoto(SPIFFS);
//   } while ( !ok );
// }



// String sendPhoto() {
//   String getAll;
//   String getBody;

//   camera_fb_t * fb = NULL;
//   fb = esp_camera_fb_get();
//   if(!fb) {
//     Serial.println("Camera capture failed");
//     delay(1000);
//     ESP.restart();
//   }
  
//   Serial.println("Connecting to server: " + serverName);

//   if (client.connect(serverName.c_str(), serverPort)) {
//     Serial.println("Connection successful!");    
//     String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
//     String tail = "\r\n--RandomNerdTutorials--\r\n";

//     uint32_t imageLen = fb->len;
//     uint32_t extraLen = head.length() + tail.length();
//     uint32_t totalLen = imageLen + extraLen;
  
//     client.println("POST " + serverPath + " HTTP/1.1");
//     client.println("Host: " + serverName);
//     client.println("Content-Length: " + String(totalLen));
//     client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
//     client.println();
//     client.print(head);
  
//     uint8_t *fbBuf = fb->buf;
//     size_t fbLen = fb->len;
//     for (size_t n=0; n<fbLen; n=n+1024) {
//       if (n+1024 < fbLen) {
//         client.write(fbBuf, 1024);
//         fbBuf += 1024;
//       }
//       else if (fbLen%1024>0) {
//         size_t remainder = fbLen%1024;
//         client.write(fbBuf, remainder);
//       }
//     }   
//     client.print(tail);
    
//     esp_camera_fb_return(fb);
    
//     int timoutTimer = 10000;
//     long startTimer = millis();
//     boolean state = false;
    
//     while ((startTimer + timoutTimer) > millis()) {
//       Serial.print(".");
//       delay(100);      
//       while (client.available()) {
//         char c = client.read();
//         if (c == '\n') {
//           if (getAll.length()==0) { state=true; }
//           getAll = "";
//         }
//         else if (c != '\r') { getAll += String(c); }
//         if (state==true) { getBody += String(c); }
//         startTimer = millis();
//       }
//       if (getBody.length()>0) { break; }
//     }
//     Serial.println();
//     client.stop();
//     Serial.println(getBody);
//   }
//   else {
//     getBody = "Connection to " + serverName +  " failed.";
//     Serial.println(getBody);
//   }
//   return getBody;
// }


// -----------------------------------------------------------------
//
//  Telegram code
//
// -----------------------------------------------------------------

#include"camera.h"


// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while ( !ok );
}