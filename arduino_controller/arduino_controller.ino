#include <AceRoutine.h>

#include "ui.h"
#include "misc.h"
#include "constants.h"
#include "comm.h"
#include <SoftwareSerial.h>

using namespace ace_routine;


// Globals to store the latest sensor values
int gasValue = 0;
int noiseValue = 0;
float tempValue = 0.0;
float humValue = 0.0;

// COROUTINE(commTask) {
//   COROUTINE_LOOP() {
//     // Request sensor data
//     Serial.println("Requesting sensor data...");
//     link.println("REQ");
    
//     // Wait for response with timeout
//     unsigned long startTime = millis();
//     while (millis() - startTime < 1000) { // 1 second timeout
//       COROUTINE_YIELD();
//       if (link.available() > 0) {
//         String response = link.readStringUntil('\n');
//         response.trim();
//         Serial.print("Received: ");
//         Serial.println(response);
        
//         // Parse the response
//         int g, n;
//         float t, h;
//         if (sscanf(response.c_str(), "GAS:%d,NOISE:%d,TEMP:%f,HUM:%f", &g, &n, &t, &h) == 4) {
//           gasValue = g;
//           noiseValue = n;
//           tempValue = t;
//           humValue = h;
//           Serial.println("Successfully parsed sensor data");
//           break;
//         }
//       }
//     }
    
//     // Wait before next request
//     COROUTINE_DELAY(2000);
//   }
// }

void setup() {
  initUI();
  initComms();
  initMisc();
  setRGB(255, 255, 255);  // Set RGB to red
  Serial.begin(9600);
  Serial.println("B: ready");
  singAlert();
  showWelcomeMessage();
  showPrompt();
  // CoroutineScheduler::setup();
}

void loop() {
  // if (mySerial.available() > 0) {
  //   String msg = mySerial.readStringUntil('\n');
  //   msg.trim(); // remove any \r
  //   Serial.print("B: got <- ");
  //   Serial.println(msg);

  //   // reply back (with newline!)
  //   mySerial.println("ACK from B");
  //   Serial.println("B: sent -> ACK from B");
  // }
  //delay(1000);
  //read_potentiometer();
  checkPassword();
  // CoroutineScheduler::loop();

  // Serial.println(readButton());
  // delay(1000);
}
