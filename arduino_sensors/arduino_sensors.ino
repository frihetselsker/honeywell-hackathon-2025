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
static int distance = 0;

static bool is_buzzer_on = false;


COROUTINE(gasTask) {
  COROUTINE_LOOP() {
    gasValue = readGas();
    COROUTINE_DELAY(300);
    Serial.print("Gas: ");
    Serial.println(gasValue);
    COROUTINE_YIELD();
  }
}


COROUTINE(distanceTask) {
  COROUTINE_LOOP() {
    COROUTINE_DELAY(300);
    Serial.print("Distance: ");
    Serial.println(readDistance());
    COROUTINE_YIELD();
  }
}



COROUTINE(buzzerTask) {
  COROUTINE_LOOP() {
    // Static variables to maintain state between resumptions
    static int thisNote = 0;
    static unsigned long nextNoteTime = 0;
    static bool playingNote = false;
    if (is_buzzer_on) {
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
      } else {
        // If buzzer is off, ensure no tone is playing and reset state
        noTone(BUZZER_PIN);
        thisNote = 0; // Reset to beginning for next time
        COROUTINE_DELAY(100); // Avoid busy-waiting
      }
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

COROUTINE(commTask) {
  COROUTINE_LOOP() {
    // Check if there's a request from the controller
     COROUTINE_AWAIT(link.available() > 0);
      String cmd = link.readStringUntil('\n');
      cmd.trim();
      Serial.print("Received command: ");
      Serial.println(cmd);
      
      if (cmd == "REQ") {
        // Send sensor data in response to request
        Serial.println("Sending sensor data...");
        link.print("GAS:");
        link.print(gasValue);
        link.print(",NOISE:"); 
        link.print(noiseValue);
        link.print(",TEMP:"); 
        link.print(tempValue, 2); 
        link.print(",HUM:"); 
        link.println(humValue, 2);
      }
    }
    
    // Small delay to avoid CPU hogging
    COROUTINE_YIELD();
  }

// put your setup code here, to run once:

void setup() {
  Serial.begin(9600);
  Serial.println("A: ready");
  initActuators();
  initComms();
  initSensors();
  initBuzzer();
  CoroutineScheduler::setup();
}

// put your main code here, to run repeatedly:

void loop() {
   CoroutineScheduler::loop();
}