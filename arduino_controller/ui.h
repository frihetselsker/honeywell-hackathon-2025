#ifndef UI_H
#define UI_H

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Arduino.h>

extern LiquidCrystal_I2C lcd;
extern Keypad k;

extern const char correct_PIN[];
extern char entered_PIN[5];   // 4 chars + '\0'
extern byte pinndex;

// UI-related functions
void initUI();
void showWelcomeMessage();
void showSensorData(int gas, int noise, float temp, float hum);
void setRGB(int r, int g, int b);
void clearLCD();

// Password-related functions
void showPrompt();
void showStars();
void resetEntry();
void printCentered(String text, unsigned int row);
void checkPassword();
void successScreen(bool ok);

#endif
