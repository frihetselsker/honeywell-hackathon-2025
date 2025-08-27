#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Servo.h>

extern Servo window;  // create servo object to control a servo
extern int window_pos;

extern bool is_window_open;
extern  bool is_cooler_on;

void initActuators();
void toggleWidnow();
void toggleCooler();

#endif
