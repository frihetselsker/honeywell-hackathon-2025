#include <AceRoutine.h>

#include "ui.h"
#include "misc.h"
#include "constants.h"
#include "comm.h"
#include <SoftwareSerial.h>
using namespace ace_routine;

enum GasState
{
  BadGasState,
  MediumGasState,
  GoodGasState
};

// State variables
static GasState gasState = GoodGasState;

// Globals to store the latest sensor values
static int gasValue = 0;
static int noiseValue = 0;
static float tempValue = 0.0;
static float humValue = 0.0;

COROUTINE(stateTask)
{
  COROUTINE_LOOP()
  {
    GasState prev = gasState;
    int gv = gasValue; // snapshot for consistent logging
    if (gv > 200)
    {
      gasState = BadGasState;
    }
    else if (gv > 100)
    {
      gasState = MediumGasState;
    }
    else
    {
      gasState = GoodGasState;
    }

    COROUTINE_DELAY(50); // update 2 Hz
  }
}

COROUTINE(rgbTask)
{
  COROUTINE_LOOP()
  {
    switch (gasState)
    {
    case BadGasState:
      // Serial.println("Danger! Gas level HIGH!");
      setRGB(255, 0, 0);
      break;

    case MediumGasState:
      // Serial.println("Warning: Medium gas level.");
      setRGB(255, 255, 0);
      break;

    case GoodGasState:
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
    Serial.print("Task Comm addr gasValue = 0x");
    Serial.println((uintptr_t)&gasValue, HEX);
    // Wait for response with timeout
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
  setRGB(255, 0, 0); // Set RGB to red
  Serial.begin(9600);
  Serial.println("B: ready");
  singAlert();
  showWelcomeMessage();
  showPrompt();
  CoroutineScheduler::setup();
}

void loop()
{
  // if (mySerial.available() > 0) {
  //   String msg = mySerial.readStringUntil('\n');
  //   msg.trim(); // remove any \r
  //   Serial.print("B: got <- ");
  //   Serial.println(msg);

  //   // reply back (with newline!)
  //   mySerial.println("ACK from B");
  //   Serial.println("B: sent -> ACK from B");
  // }
  // delay(1000);
  // read_potentiometer();
  // checkPassword();
  CoroutineScheduler::loop();

  // Serial.println(readButton());
  // delay(1000);
}
