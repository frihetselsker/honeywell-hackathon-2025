#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// UI Elements
inline constexpr int RGB_RED_PIN   = 9;   // Example
inline constexpr int RGB_GREEN_PIN = 10;
inline constexpr int RGB_BLUE_PIN  = 11;

inline constexpr int BUTTON_PIN    = 2;
inline constexpr int POTEN_PIN     = A0;

// Pinpad
inline constexpr byte ROWS = 4;
inline constexpr byte COLS = 4;

// Buzzer
inline constexpr int BUZZER_PIN = 3;

// LCD
inline constexpr int LCD_SDA = A4;
inline constexpr int LCD_SCL = A5;

// Communication
inline constexpr long BAUD_RATE = 9600;
inline constexpr int RX_PIN = 12;
inline constexpr int TX_PIN = 13;

#endif
