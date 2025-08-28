#include <SoftwareSerial.h>
#include "constants.h"
#include "sensors.h"
#include "actuators.h"
#include "buzzer.h"
#include "comm.h"
#include "melody.h"
#include <AceRoutine.h>
using namespace ace_routine;

static bool is_buzzer_on = false;

// Static values
static int gasValue = 0;
static int noiseValue = 0;
static float tempValue = 0.0;
static float humValue = 0.0;
static int distanceValue = 0;


COROUTINE(gasTask) {
  COROUTINE_LOOP() {
    gasValue = readGas();
    COROUTINE_DELAY(200);
    Serial.print("Gas: ");
    Serial.println(gasValue);
    COROUTINE_YIELD();
  }
}


COROUTINE(distanceTask) {
  COROUTINE_LOOP() {
    distanceValue = readDistance();
    COROUTINE_DELAY(200);
    Serial.print("Distance: ");
    Serial.println(distanceValue);
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
        COROUTINE_DELAY(25);
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
    static unsigned long lastTriggerTime = 0;   // Time of last loud noise
    const unsigned long holdTime = 5000;        // Keep buzzer on for 5s after trigger

    noiseValue = readNoise();
    COROUTINE_DELAY(300);

    Serial.print("Noise: ");
    Serial.println(noiseValue);

    if (noiseValue > 142) {
      lastTriggerTime = millis();   // update trigger moment
      is_buzzer_on = true;
    } else {
      // check if we are still inside the hold window
      if (millis() - lastTriggerTime < holdTime) {
        is_buzzer_on = true;
      } else {
        is_buzzer_on = false;
      }
    }

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

COROUTINE(coolerTask) {
  COROUTINE_LOOP() {
    if (gasValue > 100 || tempValue > 26.0 || humValue > 70.0) {
      onCooler();
    } else {
      offCooler();
    }
    COROUTINE_DELAY(200);
    COROUTINE_YIELD();
  }
}

COROUTINE(windowTask) {
  COROUTINE_LOOP() {
    // Hysteresis thresholds
    const int gasOpenThreshold = 100;
    const int gasCloseThreshold = 90;
    const float humOpenThreshold = 70.0;
    const float humCloseThreshold = 65.0;
    static unsigned long lastChangeTime = 0;
    const unsigned long debounceTime = 2000; // 2 seconds

    // Track why the window was opened: 0 = closed, 1 = gas, 2 = humidity
    static int windowOpenReason = 0;

    Serial.println("At window");
    Serial.println(is_window_open);

    if (!is_window_open) {
      // Only open if either gas or humidity exceeds open thresholds for debounceTime
      if ((gasValue > gasOpenThreshold || humValue > humOpenThreshold) && (millis() - lastChangeTime > debounceTime)) {
        if (gasValue > gasOpenThreshold) {
          windowOpenReason = 1;
        } else if (humValue > humOpenThreshold) {
          windowOpenReason = 2;
        }
        onWindow();
        lastChangeTime = millis();
      }
    } else {
      // Only close if the reason for opening is resolved for debounceTime
      if (windowOpenReason == 1 && gasValue < gasCloseThreshold && (millis() - lastChangeTime > debounceTime)) {
        offWindow();
        windowOpenReason = 0;
        lastChangeTime = millis();
      } else if (windowOpenReason == 2 && humValue < humCloseThreshold && (millis() - lastChangeTime > debounceTime)) {
        offWindow();
        windowOpenReason = 0;
        lastChangeTime = millis();
      }
    }

    COROUTINE_DELAY(200);
    COROUTINE_YIELD();
  }
}

COROUTINE(commTask) {
  COROUTINE_LOOP() {
    // Check if there's a request from the controller
     COROUTINE_AWAIT(link.available() > 0);
      String cmd = link.readStringUntil('\n');
      cmd.trim();
      
      if (cmd == "REQ") {
        // Send sensor data in response to request
        link.print("GAS:");
        link.print(gasValue);
        link.print(",NOISE:"); 
        link.print(noiseValue);
        link.print(",TEMP:"); 
        link.print(tempValue, 2); 
        link.print(",HUM:"); 
        link.print(humValue, 2);
        link.print(",DIST:");
        link.print(distanceValue);
        link.print(",WINDOW:");
        link.print(is_window_open);
        link.print(",COOLER:");
        link.print(is_cooler_on);
        link.print(",BUZZER:");
        link.println(is_buzzer_on);
      } else if (cmd == "TOGGLE_WINDOW") {
        toggleWindow();
      } else if (cmd == "TOGGLE_COOLER") {
        toggleCooler();
      } else if (cmd == "TOGGLE_BUZZER") {
        is_buzzer_on = !is_buzzer_on;
      } else {
        Serial.print("Unknown command: ");
        Serial.println(cmd);
      }
    }
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

  toggleWindow();
  delay(2000);
  toggleWindow();
  
  Serial.print(",WINDOW:");
  Serial.print(is_window_open);
  Serial.print(",COOLER:");
  Serial.print(is_cooler_on);
  Serial.print(",BUZZER:");
  Serial.print(is_buzzer_on);
  CoroutineScheduler::setup();
}

// put your main code here, to run repeatedly:

void loop() {
   CoroutineScheduler::loop();
}