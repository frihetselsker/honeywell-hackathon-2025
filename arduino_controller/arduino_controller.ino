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

enum MenuState {
  Menu,
  ViewData,
  ViewTime,
  ControlCooler,
  ControlWindow,
  ControlBuzzer
};

const char* menuItems[] = {
  "View Data",
  "Control Cooler",
  "Control Window",
  "Control Buzzer"
};
const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);
int selectedMenuIndex = 0;
int menuPage = 0;
bool menuConfirmed = false;

// State variables
static State state = GoodState;
static MenuState menuState = Menu;

// Globals to store the latest sensor values
static int gasValue = 0;
static int noiseValue = 0;
static float tempValue = 0.0;
static float humValue = 0.0;

static int distanceValue = 0;
static bool isWindowOpen = false;
static bool isCoolerOn = false;
static bool isBuzzerOn = false;
static int potValue = 0;

COROUTINE(potTask)
{
  COROUTINE_LOOP()
  {
    potValue = readPotentiometer();
    // Serial.println(potValue);
    COROUTINE_DELAY(10); // update 100 Hz
  }
}

COROUTINE(menuTask) {
  COROUTINE_LOOP()
  {
    if (menuState == Menu) {
      // Read potentiometer and map to menu index
      int pot = potValue;
      int range = 1024 / menuItemCount; // For 4 items, range = 256
      int newIndex = pot / range;
      if (newIndex >= menuItemCount) newIndex = menuItemCount - 1;

      // Only update if changed
      if (newIndex != selectedMenuIndex) {
        selectedMenuIndex = newIndex;
        menuPage = selectedMenuIndex / 2; // 2 items per page
        lcd.clear();
        for (int i = 0; i < 2; i++) {
          int itemIdx = menuPage * 2 + i;
          if (itemIdx < menuItemCount) {
            lcd.setCursor(0, i);
            if (itemIdx == selectedMenuIndex) {
              lcd.print(">");
            } else {
              lcd.print(" ");
            }
            lcd.print(menuItems[itemIdx]);
          }
        }
      }

      // Check for button press to select and immediately transition to the selected state
      if (readButton() == 1) {
        switch (selectedMenuIndex) {
          case 0: menuState = ViewData; break;
          case 1: menuState = ControlCooler; break;
          case 2: menuState = ControlWindow; break;
          case 3: menuState = ControlBuzzer; break;
        }
        lcd.clear();
        COROUTINE_DELAY(200); // Debounce
      }
      
      COROUTINE_DELAY(5); // Polling rate
    } else if (menuState == ViewData) {
      // Display sensor data
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(tempValue);
      lcd.setCursor(0, 1);
      lcd.print("Humid: ");
      lcd.print(humValue);
      COROUTINE_DELAY(4000); // Show for 2 seconds
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Gas: ");
      lcd.print(gasValue);
      lcd.setCursor(0, 1);
      lcd.print("Noise: ");
      lcd.print(noiseValue);
      COROUTINE_DELAY(4000); // Show for 2 seconds
      lcd.clear();
      lcd.setCursor(0, 0);
      printCentered("Distance: ", 0);
      printCentered(String(distanceValue), 1);
      COROUTINE_DELAY(4000); // Show for 2 seconds
  lcd.clear();
  menuState = Menu; // Return to menu
  selectedMenuIndex = -1; // Force redraw when returning from submenu
  COROUTINE_YIELD();
    } else if (menuState == ControlCooler) {
      // Control cooler
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cooler is ");
      lcd.print(isCoolerOn ? "ON " : "OFF");
      lcd.setCursor(0, 1);
      lcd.print("Press to toggle");
      // Wait for button press to toggle
      while (readButton() != 1) {
        COROUTINE_YIELD();
      }
      COROUTINE_DELAY(200); // Debounce

      // Ask for confirmation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Toggle cooler?");
      lcd.setCursor(0, 1);
      lcd.print(">Yes   No");
      bool confirmYes = true;
      while (true) {
        int pot = potValue;
        confirmYes = (pot < 512);
        lcd.setCursor(0, 1);
        if (confirmYes) {
          lcd.print(">Yes   No");
        } else {
          lcd.print(" Yes  >No");
        }
        if (readButton() == 1 && pot < 512) {
          isCoolerOn = !isCoolerOn;
          Serial.println("TOGGLE_COOLER");
          link.println("TOGGLE_COOLER");
          COROUTINE_DELAY(200); // Debounce
          break;
        } else if (readButton() == 1 && pot >= 512){
          COROUTINE_DELAY(200); // Debounce
          break;
        }
        COROUTINE_YIELD();
      }
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Done");
      COROUTINE_DELAY(1000);
  menuState = Menu;
  selectedMenuIndex = -1; // Force menu redraw when returning from submenu
  COROUTINE_YIELD();
    } else if (menuState == ControlWindow) {
      // Control window
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Window is ");
      lcd.print(isWindowOpen ? "OPEN" : "CLOSED");
      lcd.setCursor(0, 1);
      lcd.print("Press to toggle");
      // Wait for button press to toggle
      while (readButton() != 1) {
        COROUTINE_YIELD();
      }
      COROUTINE_DELAY(200); // Debounce

      // Ask for confirmation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Toggle window?");
      lcd.setCursor(0, 1);
      lcd.print(">Yes   No");
      bool confirmYes = true;
      while (true) {
        int pot = potValue;
        confirmYes = (pot < 512);
        lcd.setCursor(0, 1);
        if (confirmYes) {
          lcd.print(">Yes   No");
        } else {
          lcd.print(" Yes  >No");
        }
        if (readButton() == 1 && pot < 512) {
          isWindowOpen = !isWindowOpen;
          Serial.println("TOGGLE_WINDOW");
          link.println("TOGGLE_WINDOW");
          COROUTINE_DELAY(200); // Debounce
          break;
        } else if (readButton() == 1 && pot >= 512){
          COROUTINE_DELAY(200); // Debounce
          break;
        }
        COROUTINE_YIELD();
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Done");
      COROUTINE_DELAY(1000);
  menuState = Menu;
  selectedMenuIndex = -1; // Force menu redraw when returning from submenu
  COROUTINE_YIELD();
    } else if (menuState == ControlBuzzer){
      // Control buzzer
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Buzzer is ");
      lcd.print(isBuzzerOn ? "ON " : "OFF");
      lcd.setCursor(0, 1);
      lcd.print("Press to toggle");
      // Wait for button press to toggle
      while (readButton() != 1) {
        COROUTINE_YIELD();
      }
      COROUTINE_DELAY(200); // Debounce

      // Ask for confirmation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Toggle buzzer?");
      lcd.setCursor(0, 1);
      lcd.print(">Yes   No");
      bool confirmYes = true;
      while (true) {
        int pot = potValue;
        confirmYes = (pot < 512);
        lcd.setCursor(0, 1);
        if (confirmYes) {
          lcd.print(">Yes   No");
        } else {
          lcd.print(" Yes  >No");
        }
        delay(200);
        if (readButton() == 1 && pot < 512) {
          isBuzzerOn = !isBuzzerOn;
          Serial.println("TOGGLE_BUZZER");
          link.println("TOGGLE_BUZZER");
          COROUTINE_DELAY(200); // Debounce
          break;
        } else if (readButton() == 1 && pot >= 512){
          COROUTINE_DELAY(200); // Debounce
          break;
        }
        COROUTINE_YIELD();
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Done");
      COROUTINE_DELAY(1000);
  menuState = Menu;
  selectedMenuIndex = -1; // Force menu redraw when returning from submenu
  COROUTINE_YIELD();
    }
  }
}

COROUTINE(stateTask)
{
  COROUTINE_LOOP()
  {
    Serial.println("Checking state...");
    if (gasValue > 200 || noiseValue > 150 || tempValue > 35.0 || humValue > 80.0)
    {
      state = BadState;
    }
    else if (gasValue > 100 || noiseValue > 100 || tempValue > 30.0 || humValue > 70.0)
    {
      state = MediumState;
    }
    else
    {
      state = GoodState;
    }

    COROUTINE_DELAY(2000); // update 2 Hz
  }
}

COROUTINE(rgbTask)
{
  COROUTINE_LOOP()
  {
    switch (state)
    {
    case BadState:
      setRGB(255, 0, 0);
      break;

    case MediumState:
      setRGB(255, 255, 0);
      break;

    case GoodState:
      setRGB(0, 255, 0);
      break;
    }

    COROUTINE_DELAY(50);
  }
}

// COROUTINE(commTask)
// {
//   COROUTINE_LOOP()
//   {
//     // Request sensor data
//     Serial.println("Requesting sensor data...");
//     link.println("REQ");
//     unsigned long startTime = millis();
//     while (millis() - startTime < 1000)
//     { // 1 second timeout
//       COROUTINE_YIELD();
//       if (link.available() > 0)
//       {
//         String response = link.readStringUntil('\n');
//         response.trim();
//         Serial.print("Received: ");
//         Serial.println(response);

//         // Parse the extended response
//         int g = 0, n = 0, dist = 0;
//         float t = 0.0, h = 0.0;
//         int window = 0, cooler = 0, buzzer = 0;

//         // Example: GAS:123,NOISE:45,TEMP:23.45,HUM:56.78,DIST:100,WINDOW:1,COOLER:0,BUZZER:1
//         int parsed = sscanf(response.c_str(),
//           "GAS:%d,NOISE:%d,TEMP:%f,HUM:%f,DIST:%d,WINDOW:%d,COOLER:%d,BUZZER:%d",
//           &g, &n, &t, &h, &dist, &window, &cooler, &buzzer);

//         if (parsed == 8) {
//           gasValue = g;
//           noiseValue = n;
//           tempValue = t;
//           humValue = h;
//           distanceValue = dist;
//           isWindowOpen = (window != 0);
//           isCoolerOn = (cooler != 0);
//           isBuzzerOn = (buzzer != 0);

//           Serial.println("Successfully parsed extended sensor data");
//         } else {
//           Serial.println("Failed to parse sensor data");
//         }
//       }
//     }
//   }

//   // Wait before next request
//   COROUTINE_DELAY(2000);
// }

void setup()
{
  initUI();
  initComms();
  initMisc();
  setRGB(255, 255, 255); // Set RGB to white
  Serial.begin(9600);
  Serial.println("B: ready");
  singAlert();
  selectedMenuIndex = -1;
  //delay(2000);
  //showPrompt();
  CoroutineScheduler::setup();
}

void loop(){
  //delay(1000);
  //Serial.println(readButton());
  //checkPassword();
  CoroutineScheduler::loop();

  // Serial.println(readButton());
  
}
