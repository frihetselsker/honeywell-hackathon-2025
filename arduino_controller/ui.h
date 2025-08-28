#ifndef UI_H
#define UI_H

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Arduino.h>

inline constexpr float VCC; // 5V

inline constexpr float D1_POWER_CONSUMPTION; // 2 mA RGB LED
inline constexpr float PIEZO1_POWER_CONSUMPTION; // Buzzer
inline constexpr float PIEZO3_POWER_CONSUMPTION; // Buzzer
inline constexpr float ARDUINO1_POWER_CONSUMPTION; // 70 mA
inline constexpr float ARDUINO2_POWER_CONSUMPTION; // 70 mA
inline constexpr float LCD_POWER_CONSUMPTION; // 20 mA
inline constexpr float GAS_POWER_CONSUMPTION; // 150 mA
inline constexpr float NOISE_SENSOR_POWER_CONSUMPTION; // 20 mA
inline constexpr float FIRE_SENSOR_POWER_CONSUMPTION; // 20 mA
inline constexpr float SERVO_POWER_CONSUMPTION; // 150 mA
inline constexpr float M1_POWER_CONSUMPTION; // MOTOR 1

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
void powerUsage()

#endif
