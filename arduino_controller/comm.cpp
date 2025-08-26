#include "comm.h"
#include "constants.h"
#include <SoftwareSerial.h>

static SoftwareSerial link(RX_PIN, TX_PIN); // RX, TX pins (adjust as needed)

void initComms() {
  link.begin(BAUD_RATE);
}

void requestSensorData() {
  link.println("REQ");
}

bool parseIncomingData(int &gas, int &noise, float &temp, float &hum) {
  // TODO: Write the async reader
    if (link.available()) {
    String msg = link.readStringUntil('\n');
    // Expected format: GAS:123,NOISE:456,TEMP:25.3,HUM:40.1
    int g, n;
    float t, h;
    if (sscanf(msg.c_str(), "GAS:%d,NOISE:%d,TEMP:%f,HUM:%f", &g, &n, &t, &h) == 4) {
      gas = g;
      noise = n;
      temp = t;
      hum = h;
      return true;
    }
  }
  return false;
}

void sendCommand(const char* cmd) {
  link.println(cmd);
}
