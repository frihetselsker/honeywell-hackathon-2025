#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>


#include <Keypad.h>

// ==== KEYPAD SETUP: 3x3 (digits only) ====
const byte rows = 3;
const byte cols = 3;

// use only the first 3 row pins and first 3 column pins
byte row_pins[rows] = {9, 8, 7}; 
byte col_pins[cols] = {5, 4, 3};   

char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};


Keypad k = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);

// ==== LCD SETUP ====
LiquidCrystal_I2C lcd(0x27, 16, 2);   // address may be 0x27 or 0x3F

const char correctPIN[] = "1818";
char enteredPIN[5];   // 4 chars + '\0'
byte pinIndex = 0;

int potentiometer_pin = A0;

SoftwareSerial mySerial(12, 13); // RX=12, TX=13

// --- Helpers ---
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
  for (byte i = 0; i < pinIndex; i++) lcd.print('*');
  for (byte i = pinIndex; i < 4; i++) lcd.print(' ');
  lcd.setCursor(6 + pinIndex, 1);
}

void resetEntry() {
  pinIndex = 0;
  showStars();
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


void printCentered(const char* text, uint8_t row) {
  int len = strlen(text);
  int col = (16 - len) / 2;   // center for 16 chars per row
  if (col < 0) col = 0;       // in case text is longer than 16
  lcd.setCursor(col, row);
  lcd.print(text);
}


int read_potentiometer() {
  int potent_reading = analogRead(potentiometer_pin);

  Serial.println(potent_reading);

  return potent_reading;
}

void setup() {
  lcd.init();
  lcd.backlight();
  showPrompt();
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.setTimeout(50);
  Serial.println("B: ready");
}

void loop() {
  if (mySerial.available() > 0) {
    String msg = mySerial.readStringUntil('\n');
    msg.trim(); // remove any \r
    Serial.print("B: got <- ");
    Serial.println(msg);

    // reply back (with newline!)
    mySerial.println("ACK from B");
    Serial.println("B: sent -> ACK from B");
  }

  read_potentiometer();

  char key = k.getKey();
  if (!key) return;

  // '*' clears the PIN entry
  if (key == '*') {
    resetEntry();
    return;
  }

  // '#' works like backspace
  if (key == '#') {
    if (pinIndex > 0) pinIndex--;
    showStars();
    return;
  }

  // Accept digits 0-9 and A-D
  if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) {
    if (pinIndex < 4) {
      enteredPIN[pinIndex] = key;
      pinIndex++;
      showStars();

      if (pinIndex == 4) {
        enteredPIN[4] = '\0';
        bool ok = (strcmp(enteredPIN, correctPIN) == 0);
        successScreen(ok);
        resetEntry();
      }
    }
  }
}
