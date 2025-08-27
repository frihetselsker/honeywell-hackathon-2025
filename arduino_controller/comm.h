#ifndef COMM_H
#define COMM_H

#include <SoftwareSerial.h>

extern SoftwareSerial link;

void initComms();
void requestSensorData();
bool parseIncomingData(int &gas, int &noise, float &temp, float &hum);
void sendCommand(const char* cmd);

#endif
