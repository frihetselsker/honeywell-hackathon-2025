#include <dht11.h>

const int gasPin = A0;
const int noisePin = A1;
const int DHT11PIN = 4; 
float gasValue = 0;  // variable to store the value coming from the sensor
float noiseValue = 0;


dht11 DHT11;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("The start");
  gasValue = analogRead(gasPin);
  noiseValue = analogRead(noisePin);   
  int chk = DHT11.read(DHT11PIN);
  // turn the ledPin on
  //Serial.print("The value of the gas sensor ");
  //Serial.println(gasValue);
  Serial.print("The value of the noise sensor ");
  Serial.println(noiseValue);
  //Serial.print("Humidity (%): ");
  //Serial.println((float)DHT11.humidity, 2);
  //Serial.print("Temperature  (C): ");
  //Serial.println((float)DHT11.temperature, 2);
  // stop the program for <sensorValue> milliseconds:
  delay(200);

}