#ifndef SENSORS_H
#define SENSORS_H

void initSensors();
int readGas();
int readNoise();
float readTemperature();
float readHumidity();
int readDistance();

#endif
