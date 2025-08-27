#include <SoftwareSerial.h>
#include "constants.h"
#include "sensors.h"
#include "actuators.h"
#include "buzzer.h"
#include "comm.h"
#include "melody.h"
#include <AceRoutine.h>
using namespace ace_routine;



// Static values
static int gasValue = 0;
static int noiseValue = 0;
static float tempValue = 0.0;
static float humValue = 0.0;


// COROUTINE(commTask) {
//   COROUTINE_LOOP() {
//     COROUTINE_AWAIT(link.available() > 0);

//     // TODO: Write the async reader
//   }
// }

COROUTINE(gasTask) {
  COROUTINE_LOOP() {
    COROUTINE_DELAY(300);
    Serial.print("Gas: ");
    Serial.println(readGas());
    COROUTINE_YIELD();
  }
}

COROUTINE(buzzerTask) {
  COROUTINE_LOOP() {
    // Static variables to maintain state between resumptions
    static int thisNote = 0;
    static unsigned long nextNoteTime = 0;
    static bool playingNote = false;
    
    // Check if we're finished with the melody
    if (thisNote >= melodyLength) {
      thisNote = 0; // Reset to beginning
      COROUTINE_DELAY(2000); // Wait before repeating
      return;
    }
    
    // Print current note for debugging
    Serial.print("Note: ");
    Serial.println(thisNote/2); // Divide by 2 to show actual note count
    
    // Get the pitch and duration
    int pitch = pgm_read_word_near(melody + thisNote);
    int divider = pgm_read_word_near(melody + thisNote + 1);
    
    // Calculate duration
    int noteDuration;
    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / abs(divider);
      noteDuration *= 1.5; // dotted notes
    } else {
      noteDuration = 0;
    }
    
    // Start playing the note
    if (pitch != REST && noteDuration > 0) {
      tone(BUZZER_PIN, pitch, noteDuration * 0.9);
    }
    Serial.println("Buzzer reached");
    
    // Use COROUTINE_DELAY for non-blocking wait
    COROUTINE_DELAY(noteDuration);
    
    // Stop the tone and prepare for next note
    noTone(BUZZER_PIN);
    
    // Move to next note (step by 2 for pitch,duration pairs)
    thisNote += 2;
    
    // Small delay between notes
    COROUTINE_DELAY(50);
  }
}

COROUTINE(noiseTask) {
  COROUTINE_LOOP() {
    noiseValue = readNoise();
    COROUTINE_DELAY(300);
    Serial.print("Noise: ");
    Serial.println(noiseValue);
    COROUTINE_YIELD();
  }
}

COROUTINE(DHTTask) {
  COROUTINE_LOOP() {
    tempValue = readTemperature();
    humValue = readHumidity();
    COROUTINE_DELAY(300);
    Serial.print("Temeperature: ");
    Serial.print(tempValue, 2);
    Serial.print("Humidity: ");
    Serial.println(humValue, 2);
    COROUTINE_YIELD();
  }
}


// put your setup code here, to run once:

void setup() {
  Serial.begin(9600);
  Serial.println("A: ready");
  initActuators();
  initBuzzer();
  CoroutineScheduler::setup();
}

// put your main code here, to run repeatedly:

void loop() {
   CoroutineScheduler::loop();
}