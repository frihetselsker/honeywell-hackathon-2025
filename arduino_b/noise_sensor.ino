const int sensorPin = A0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int raw = analogRead(sensorPin);   
  Serial.println(raw);
  delay(10);                       
}
