#include <Arduino.h>
#include "sensors.h"
#include "constants.h"
#include <dht11.h>

static dht11 DHT11;

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
