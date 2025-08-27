#include "misc.h"
#include "constants.h"
#include <Wire.h>
#include <Arduino.h>

void initMisc() {
    pinMode(BUTTON_PIN, INPUT); // Button with internal pull-up
    pinMode(BUZZER_PIN, OUTPUT);
}
bool readButton() {
    return digitalRead(BUTTON_PIN);
}

// Success melody
void singSuccess() {
  tone(BUZZER_PIN, 1000, 200);  // Play 1000Hz for 200ms
  delay(250);
  tone(BUZZER_PIN, 1500, 200);  // Play 1500Hz for 200ms
  delay(250);
  tone(BUZZER_PIN, 2000, 200);  // Play 2000Hz for 200ms
  delay(250);
  noTone(BUZZER_PIN);           // Stop the buzzer
}

// Failure melody
void singFailure() {
  tone(BUZZER_PIN, 400, 400);   // Low tone
  delay(450);
  tone(BUZZER_PIN, 300, 400);   // Lower tone
  delay(450);
  noTone(BUZZER_PIN);
}

void singAlert() {
  // Simple alert: three short beeps
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN
, 1000, 150); // 1000 Hz for 150 ms
    delay(200);                 // Short pause between beeps
  }
  noTone(BUZZER_PIN);            // Stop buzzer
}

int readPotentiometer() {
  int potent_reading = analogRead(POTEN_PIN);

  Serial.println(potent_reading);

  return potent_reading;
}