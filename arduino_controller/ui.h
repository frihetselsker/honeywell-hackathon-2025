#ifndef UI_H
#define UI_H

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Arduino.h>

// Power consumption constants (declare as extern, define in .cpp)
extern const float VCC; // 5V

extern const float D1_POWER_CONSUMPTION; // 2 mA RGB LED
extern const float PIEZO1_POWER_CONSUMPTION; // Buzzer
extern const float PIEZO3_POWER_CONSUMPTION; // Buzzer
extern const float ARDUINO1_POWER_CONSUMPTION; // 70 mA
extern const float ARDUINO2_POWER_CONSUMPTION; // 70 mA
extern const float LCD_POWER_CONSUMPTION; // 20 mA
extern const float GAS_POWER_CONSUMPTION; // 150 mA
extern const float NOISE_SENSOR_POWER_CONSUMPTION; // 20 mA
extern const float FIRE_SENSOR_POWER_CONSUMPTION; // 20 mA
extern const float SERVO_POWER_CONSUMPTION; // 150 mA
extern const float M1_POWER_CONSUMPTION; // MOTOR 1

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
void powerUsage(); // <-- fixed missing semicolon

#endif
