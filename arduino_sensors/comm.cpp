#include "comm.h"
#include "constants.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial link(RX_PIN, TX_PIN);

// Change pins and baud if needed
void initComms() {
  link.begin(BAUD_RATE);
}

// Push sensor data to channel and send via SoftwareSerial
void sendSensorData(int gas, int noise, float temp, float hum) {
    Serial.println("Sending a message");
    link.print("GAS:"); 
    link.print(gas);
    link.print(",NOISE:"); link.print(noise);
    link.print(",TEMP:"); link.print(temp, 2);
    link.print(",HUM:"); link.println(hum, 2);
}


// TODO: Use Coroutine class for rewriting this function
void checkForCommands() {
  if (link.available()) {
    String cmd = link.readStringUntil('\n');
    cmd.trim();
    if (cmd == "OPEN_WINDOW") {
      // call actuator function
      // openWindow();
    } else if (cmd == "START_COOLER") {
      // startCooler();
    }
    // extend as needed
  }
}
