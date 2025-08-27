#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// Sensor pins
inline constexpr int GAS_PIN   = A0;
inline constexpr int NOISE_PIN = A1;
inline constexpr int DHT_PIN   = 4;

// Actuators
inline constexpr int SERVO_PIN   = 5;
inline constexpr int COOLER_PIN  = 6;
inline constexpr int BUZZER_PIN  = 3;

// Serial communication
inline constexpr long BAUD_RATE = 9600;
inline constexpr int RX_PIN = 12;
inline constexpr int TX_PIN = 13;

#endif
