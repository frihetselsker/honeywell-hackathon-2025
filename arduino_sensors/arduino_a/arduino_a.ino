#include <Servo.h>
#include <dht11.h>
#include <SoftwareSerial.h>
#include "constants.h"

/// Constants

unsigned long lastSend = 0;
const unsigned long period = 1000; // send once per second

/// Static variables

SoftwareSerial mySerial(12, 13); // RX=12, TX=13
Servo myservo;  // create servo object to control a servo
dht11 DHT11;

int pos = 0;    // variable to store the servo position


// put your setup code here, to run once:

void setup() {
  Serial.begin(9600);
  mySerial.begin(BAUD_RATE);
  mySerial.setTimeout(50);     // faster reads if newline missing
  Serial.println("A: ready");
  myservo.attach(SERVO_PIN);
  pinMode(COOLER_PIN, OUTPUT);
  analogWrite(COOLER_PIN, 255); // Full speed

}

// put your main code here, to run repeatedly:

void loop() {

  // send a line every second
  if (millis() - lastSend >= period) {
    lastSend = millis();
    mySerial.println("PING from A");   // NOTE: println adds '\n'
    Serial.println("A: sent -> PING from A");
  }

  // read any reply from B
  if (mySerial.available() > 0) {
    String msg = mySerial.readStringUntil('\n');
    msg.trim(); // remove any \r
    Serial.print("A: got <- ");
    Serial.println(msg);
  }

  float gasValue = analogRead(GAS_PIN);
  float noiseValue = analogRead(NOISE_PIN);   
  int chk = DHT11.read(DHT_PIN);
  Serial.print("The value of the gas sensor ");
  Serial.println(gasValue);
  Serial.print("The value of the noise sensor ");
  Serial.println(noiseValue);
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  Serial.print("Temperature  (C): ");
  Serial.println((float)DHT11.temperature, 2);
  delay(200);

  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position
  }

  analogWrite(COOLER_PIN, 255); // Full speed      
}