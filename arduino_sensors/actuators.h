#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Servo.h>

extern Servo window;  // create servo object to control a servo
extern int window_pos;

extern bool is_window_open;
extern  bool is_cooler_on;

void initActuators();
void toggleWindow();
void toggleCooler();
void onWindow();
void offWindow();
void onCooler();
void offCooler();

#endif
