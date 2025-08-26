#include <dht11.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 13); // RX=12, TX=13

const int gasPin = A0;
const int noisePin = A1;
const int DHT11PIN = 4; 
float gasValue = 0;  // variable to store the value coming from the sensor
float noiseValue = 0;

unsigned long lastSend = 0;
const unsigned long period = 1000; // send once per second



dht11 DHT11;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.setTimeout(50);     // faster reads if newline missing
  Serial.println("A: ready");


}

void loop() {
  // put your main code here, to run repeatedly:

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

  gasValue = analogRead(gasPin);
  noiseValue = analogRead(noisePin);   
  int chk = DHT11.read(DHT11PIN);
  // turn the ledPin on
  //Serial.print("The value of the gas sensor ");
  //Serial.println(gasValue);
  //Serial.print("The value of the noise sensor ");
  //Serial.println(noiseValue);
  //Serial.print("Humidity (%): ");
  //Serial.println((float)DHT11.humidity, 2);
  //Serial.print("Temperature  (C): ");
  //Serial.println((float)DHT11.temperature, 2);
  // stop the program for <sensorValue> milliseconds:
  delay(200);

}