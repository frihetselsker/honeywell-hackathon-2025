#include <AceRoutine.h>

#include "ui.h"
#include "misc.h"
#include "constants.h"
#include "comm.h"
#include <SoftwareSerial.h>
using namespace ace_routine;

enum State
{
  GoodState,
  MediumState,
  BadState
};

// State variables
static State state = GoodState;

// Globals to store the latest sensor values
static int gasValue = 0;
static int noiseValue = 0;
static float tempValue = 0.0;
static float humValue = 0.0;

COROUTINE(stateTask)
{
  COROUTINE_LOOP()
  {
    State prev = state;
    int gv = gasValue; // snapshot for consistent logging
    if (gv > 200)
    {
      state = BadState;
    }
    else if (gv > 100)
    {
      state = MediumState;
    }
    else
    {
      state = GoodState;
    }

    COROUTINE_DELAY(50); // update 2 Hz
  }
}

COROUTINE(rgbTask)
{
  COROUTINE_LOOP()
  {
    switch (state)
    {
    case BadState:
      // Serial.println("Danger! Gas level HIGH!");
      setRGB(255, 0, 0);
      break;

    case MediumState:
      // Serial.println("Warning: Medium gas level.");
      setRGB(255, 255, 0);
      break;

    case GoodState:
      // Serial.println("Gas levels normal.");
      setRGB(0, 255, 0);
      break;
    }

    COROUTINE_DELAY(50);
  }
}

COROUTINE(commTask)
{
  COROUTINE_LOOP()
  {
    // Request sensor data
    Serial.println("Requesting sensor data...");
    link.println("REQ");
    unsigned long startTime = millis();
    while (millis() - startTime < 1000)
    { // 1 second timeout
      COROUTINE_YIELD();
      if (link.available() > 0)
      {
        String response = link.readStringUntil('\n');
        response.trim();
        Serial.print("Received: ");
        Serial.println(response);

        // Parse the response
        int g, n;
        char tbuf[16], hbuf[16];

        if (sscanf(response.c_str(),
                   "GAS:%d,NOISE:%d,TEMP:%15[^,],HUM:%15s",
                   &g, &n, tbuf, hbuf) == 4)
        {
          float t = atof(tbuf);
          float h = atof(hbuf);

          gasValue = g;
          noiseValue = n;
          tempValue = t;
          humValue = h;

          Serial.println("Successfully parsed sensor data");
        }
      }
    }
  }

  // Wait before next request
  COROUTINE_DELAY(2000);
}

void setup()
{
  initUI();
  initComms();
  initMisc();
  setRGB(255, 255, 255); // Set RGB to white
  Serial.begin(9600);
  Serial.println("B: ready");
  singAlert();
  delay(2000);
  showPrompt();
  CoroutineScheduler::setup();
}

void loop(){
  int pot = readPotentiometer();
  Serial.println(pot);
  delay(1000);
  checkPassword();
  CoroutineScheduler::loop();

  // Serial.println(readButton());
  
}
