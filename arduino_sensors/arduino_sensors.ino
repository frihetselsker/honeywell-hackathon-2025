#include <SoftwareSerial.h>
#include "constants.h"
#include "sensors.h"
#include "actuators.h"
#include "buzzer.h"
#include "comm.h"
#include <AceRoutine.h>
using namespace ace_routine;

// COROUTINE(commTask) {
//   COROUTINE_LOOP() {
//     COROUTINE_AWAIT(link.available() > 0);

//     // TODO: Write the async reader
//   }
// }

COROUTINE(gasTask) {
  COROUTINE_LOOP() {
    int gas_value = readGas();
    COROUTINE_DELAY(300);
    Serial.print("Gas: ");
    Serial.println(gas_value);
    COROUTINE_YIELD();
  }
}

COROUTINE(noiseTask) {
  COROUTINE_LOOP() {
    int noise_value = readNoise();
    COROUTINE_DELAY(300);
    Serial.print("Noise: ");
    Serial.println(noise_value);
    COROUTINE_YIELD();
  }
}

COROUTINE(DHTTask) {
  COROUTINE_LOOP() {
    float temp_value = readTemperature();
    float hum_value = readHumidity();
    COROUTINE_DELAY(300);
    Serial.print("Temeperature: ");
    Serial.print(temp_value, 2);
    Serial.print("Humidity: ");
    Serial.println(hum_value, 2);
    COROUTINE_YIELD();
  }
}


// put your setup code here, to run once:

void setup() {
  Serial.begin(9600);
  Serial.println("A: ready");
  initActuators();

  CoroutineScheduler::setup();
}

// put your main code here, to run repeatedly:

void loop() {
   CoroutineScheduler::loop();
}