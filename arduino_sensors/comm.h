#ifndef COMM_H
#define COMM_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "constants.h"

extern SoftwareSerial link;

// Public interface
void initComms();
void sendSensorData(int gas, int noise, float temp, float hum);
void checkForCommands();  // read commands from Uno B

#endif
