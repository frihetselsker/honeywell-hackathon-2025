#include <AceRoutine.h>

#include "ui.h"
#include "misc.h"
#include "constants.h"
#include "comm.h"
#include <SoftwareSerial.h>

using namespace ace_routine;
SoftwareSerial link(12, 13); // RX=12, TX=13

COROUTINE(commTask) {
  COROUTINE_LOOP() {
    COROUTINE_AWAIT(link.available() > 0);
    String cmd = link.readStringUntil('\n');
    Serial.println("Got a message");
    Serial.println(cmd);
    COROUTINE_DELAY(100);
  }
}

void setup() {
  initUI();
  initComms();
  initMisc();
  Serial.begin(9600);
  link.begin(9600);
  link.setTimeout(50);
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
  //mySerial.println("ACK from B");
  //delay(1000);
  //read_potentiometer();
  //checkPassword();
  CoroutineScheduler::loop();
}
