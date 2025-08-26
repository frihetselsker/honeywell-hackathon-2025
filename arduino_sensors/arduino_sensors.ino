#include <SoftwareSerial.h>
#include "constants.h"
#include "sensors.h"
#include "actuators.h"
#include "buzzer.h"

/// Constants

unsigned long lastSend = 0;
const unsigned long period = 1000; // send once per second

/// Static variables

SoftwareSerial mySerial(12, 13); // RX=12, TX=13


int pos = 0;    // variable to store the servo position


// put your setup code here, to run once:

void setup() {
  Serial.begin(9600);
  mySerial.begin(BAUD_RATE);
  mySerial.setTimeout(50);     // faster reads if newline missing
  Serial.println("A: ready");
  initActuators();
}

// put your main code here, to run repeatedly:

void loop() {

  // send a line every second
  if (millis() - lastSend >= period) {
    lastSend = millis();
    mySerial.println("PING from A");   // NOTE: println adds '\n'
    Serial.println("A: sent -> PING from A");
  }

  // read any reply from B
  if (mySerial.available() > 0) {
    String msg = mySerial.readStringUntil('\n');
    msg.trim(); // remove any \r
    Serial.print("A: got <- ");
    Serial.println(msg);
  }

  float gasValue = readGas();
  float noiseValue = readNoise();   
  Serial.print("The value of the gas sensor ");
  Serial.println(gasValue);
  Serial.print("The value of the noise sensor ");
  Serial.println(noiseValue);
  Serial.print("Humidity (%): ");
  Serial.println(readHumidity(), 2);
  Serial.print("Temperature  (C): ");
  Serial.println(readTemperature(), 2);
  delay(200);    
}