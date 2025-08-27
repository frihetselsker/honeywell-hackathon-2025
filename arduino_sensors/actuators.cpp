#include <Arduino.h>
#include <Servo.h>
#include "actuators.h"
#include "constants.h"

Servo window;  // create servo object to control a servo
int window_pos = 0;

static bool is_window_open = false;
static bool is_cooler_on = false;

void initActuators() {
  window.attach(SERVO_PIN);
  pinMode(COOLER_PIN, OUTPUT);
  analogWrite(COOLER_PIN, 0);
}

void toggleWindow() {
    if (is_window_open) {
        offWindow();
        is_window_open = false;
    } else {
        onWindow();
        is_window_open = true;
    }
}

void toggleCooler() {
    if (is_cooler_on) {
        offCooler();
        is_cooler_on = false;
    } else {
        onCooler();
        is_cooler_on = true;
    }
}

void onWindow() {
    if (window_pos < 90) {
        for (window_pos = window_pos; window_pos <= 90; window_pos += 10) { // goes from 0 degrees to 180 degrees
            window.write(window_pos);              // tell servo to go to position in variable 'window_pos'
            delay(5);                       // waits 5ms for the servo to reach the position
        }
    }
}

void offWindow() {
    if (window_pos > 0) {
        for (window_pos = window_pos; window_pos >= 0; window_pos -= 10) { // goes from 0 degrees to 180 degrees
            window.write(window_pos);              // tell servo to go to position in variable 'window_pos'
            delay(5);                       // waits 5ms for the servo to reach the position
        }
    }
}

void offCooler() {
    analogWrite(COOLER_PIN, 0);
}

void onCooler() {
    analogWrite(COOLER_PIN, 255);
}