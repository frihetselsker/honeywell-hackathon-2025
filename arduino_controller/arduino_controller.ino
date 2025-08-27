#include <AceRoutine.h>

#include "ui.h"
#include "misc.h"
#include "constants.h"
#include "comm.h"
#include <SoftwareSerial.h>

using namespace ace_routine;

COROUTINE(commTask) {
  COROUTINE_LOOP() {
    Serial.println("At comm task");
    COROUTINE_AWAIT(link.available() > 0);
    String cmd = link.readStringUntil('\n');
    Serial.println("Got a message");
    Serial.println(cmd);
    COROUTINE_DELAY(500);
  }
}

void setup() {
  initUI();
  initComms();
  initMisc();
  Serial.begin(9600);
  Serial.println("B: ready");
  CoroutineScheduler::setup();
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
  //checkPassword();
  CoroutineScheduler::loop();
}
