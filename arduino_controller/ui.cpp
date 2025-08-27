#include "ui.h"
#include "constants.h"
#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

const char correct_PIN[] = "1818";
char entered_PIN[5];   // 4 chars + '\0'
byte pinndex = 0;


//!!! WE NEED TO FIX IT !!!
// ==== KEYPAD SETUP: 4x4 (digits only) ====
const byte rows = 4;
const byte cols = 4;

// use only the first 3 row pins and first 3 column pins
byte row_pins[rows] = {2, 3, 4, 5}; 
byte col_pins[cols] = {7, 8, A1, A2};   

char keys[rows][cols] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad k = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);

void initUI() {
  lcd.init();
  lcd.backlight();
  showPrompt();
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
}

void showWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart System");
  lcd.setCursor(0, 1);
  lcd.print("Controller Ready");
}

void clearLCD() {
    lcd.clear();
}

// This function should be modified
void showSensorData(int gas, int noise, float temp, float hum) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C H:");
  lcd.print(hum);
  
  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gas);
  lcd.print(" N:");
  lcd.print(noise);
}

void setRGB(int r, int g, int b) {
  analogWrite(RGB_RED_PIN,   r);
  analogWrite(RGB_GREEN_PIN, g);
  analogWrite(RGB_BLUE_PIN,  b);
}


void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void printCentered(const char* text, unsigned int row) {
  int len = strlen(text);
  int col = (16 - len) / 2;   // center for 16 chars per row
  if (col < 0) col = 0;       // in case text is longer than 16
  lcd.setCursor(col, row);
  lcd.print(text);
}

void successScreen(bool ok) {
  lcd.clear();
  if (ok) {
    printCentered("Welcome,come in!", 0);
  } else {
    // split long message across 2 rows
    printCentered("Incorrect", 0);
    printCentered("password!", 1);
  }
  delay(1500);   // show result longer
  showPrompt();
}

void showPrompt() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Enter PIN");
  lcd.setCursor(0, 1);
  lcd.print("                "); // clear line 2
  lcd.setCursor(6, 1);           // start stars here
}

void showStars() {
  lcd.setCursor(6, 1);
  for (byte i = 0; i < pinndex; i++) lcd.print('*');
  for (byte i = pinndex; i < 4; i++) lcd.print(' ');
  lcd.setCursor(6 + pinndex, 1);
}

void resetEntry() {
  pinndex = 0;
  showStars();
}

void checkPassword() {
  char key = k.getKey();
  if (!key) return;

  // '*' clears the PIN entry
  if (key == '*') {
    Serial.println(key);
    resetEntry();
    return;
  }

  // '#' works like backspace
  if (key == '#') {
    Serial.println(key);
    if (pinndex > 0) pinndex--;
    showStars();
    return;
  }

  // Accept digits 0-9 and A-D
  if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) {
    Serial.println(key);
    if (pinndex < 4) {
      entered_PIN[pinndex] = key;
      pinndex++;
      showStars();

      if (pinndex == 4) {
        entered_PIN[4] = '\0';
        bool ok = (strcmp(entered_PIN, correct_PIN) == 0);
        successScreen(ok);
        resetEntry();
      }
    }
  }
}
