#include <Servo.h>

#include <dht11.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 13); // RX=12, TX=13

const int gasPin = A0;
const int noisePin = A1;
const int DHT11PIN = 4; 
float gasValue = 0;  // variable to store the value coming from the sensor
float noiseValue = 0;

int motorPin = 6; // Motor control pin (PWM)

unsigned long lastSend = 0;
const unsigned long period = 1000; // send once per second

Servo myservo;  // create servo object to control a servo


int pos = 0;    // variable to store the servo position



dht11 DHT11;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.setTimeout(50);     // faster reads if newline missing
  Serial.println("A: ready");
  myservo.attach(5);

  pinMode(motorPin, OUTPUT);
  analogWrite(motorPin, 255); // Full speed

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

  // for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees

  //   // in steps of 1 degree

  //   myservo.write(pos);              // tell servo to go to position in variable 'pos'

  //   delay(15);                       // waits 15ms for the servo to reach the position

  // }

  // for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees

  //   myservo.write(pos);              // tell servo to go to position in variable 'pos'

  //   delay(15);                       // waits 15ms for the servo to reach the position

  // }

  analogWrite(motorPin, 255); // Full speed
  // delay(2000);               // Run 2 seconds
  // analogWrite(motorPin, 0);   // Stop
  // delay(2000);          

}