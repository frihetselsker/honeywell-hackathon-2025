#ifndef UI_H
#define UI_H

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
void printCentered(const char* text, unsigned int row);
void checkPassword();
void successScreen(bool ok);

#endif
