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
  analogWrite(COOLER_PIN, 0); // Full speed
}

void toggleWidnow() {
    if (is_window_open) {
        for (window_pos = 90; window_pos >= 0; window_pos--) { // goes from 0 degrees to 180 degrees
            window.write(window_pos);              // tell servo to go to position in variable 'window_pos'
            delay(15);                       // waits 15ms for the servo to reach the position
        }
    } else {
        for (window_pos = 0; window_pos <= 90; window_pos++) { // goes from 180 degrees to 0 degrees
            window.write(window_pos);              // tell servo to go to position in variable 'window_pos' 
            delay(15);                       // waits 15ms for the servo to reach the position
        }
    }
    is_window_open = !is_window_open;
}

void toggleCooler() {
    if (is_cooler_on) {
        analogWrite(COOLER_PIN, 0);
    } else {
        analogWrite(COOLER_PIN, 255);
    }
    is_cooler_on = !is_cooler_on;
}