#include "buzzer.h"
#include "constants.h"
#include "melody.h"  // contains the melody[] array and tempo

static int notes;
static int wholenote;

void initBuzzer() {

  // melody is pairs of (pitch, divider)
  notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // duration of a whole note in ms
  wholenote = (60000 * 4) / tempo;
}

void playMelody() {
  int divider = 0;
  int noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
    // read divider (duration)
    divider = pgm_read_word_near(melody + thisNote + 1);

    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / abs(divider);
      noteDuration *= 1.5; // dotted notes
    }

    // play pitch
    tone(BUZZER_PIN, pgm_read_word_near(melody + thisNote), noteDuration * 0.9);

    // wait duration
    delay(noteDuration);

    // stop before next note
    noTone(BUZZER_PIN);
  }
}