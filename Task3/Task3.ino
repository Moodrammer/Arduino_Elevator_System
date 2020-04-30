void setup() {
  Serial.begin(9600);
  for(int i = 10; i <= 12; i++) pinMode(i, OUTPUT);
}

void loop() {
  //BCD 10 11 12   encoder 7 8 9 
   digitalWrite(12, digitalRead(9));
   digitalWrite(11, digitalRead(8));
   digitalWrite(10, digitalRead(7));
}
