#include <Arduino.h>
#include "sensors.h"
#include "constants.h"
#include <dht11.h>

static dht11 DHT11;

void initSensors() {
  pinMode(TRIG_PIN, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the ECHO_PIN as an Input
}

int readGas() {
  return analogRead(GAS_PIN);
}

int readNoise() {
  return analogRead(NOISE_PIN);
}

float readTemperature() {
  int chk = DHT11.read(DHT_PIN);
  return (float)DHT11.temperature;
}

float readHumidity() {
  int chk = DHT11.read(DHT_PIN);
  return (float)DHT11.humidity;
}

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}
