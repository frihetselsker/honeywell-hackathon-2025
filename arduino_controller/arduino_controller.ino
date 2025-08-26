#include "ui.h"
#include "misc.h"
#include "constants.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13); // RX=12, TX=13

void setup() {
  initUI();
  initMisc();
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.setTimeout(50);
  Serial.println("B: ready");
}

void loop() {
  if (mySerial.available() > 0) {
    String msg = mySerial.readStringUntil('\n');
    msg.trim(); // remove any \r
    Serial.print("B: got <- ");
    Serial.println(msg);

    // reply back (with newline!)
    mySerial.println("ACK from B");
    Serial.println("B: sent -> ACK from B");
  }
  read_potentiometer();
  checkPassword();
}
