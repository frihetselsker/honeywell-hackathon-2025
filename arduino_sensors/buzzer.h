#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

extern int wholenote;
extern bool is_buzzer_on;

void initBuzzer();
void playMelody();

#endif
