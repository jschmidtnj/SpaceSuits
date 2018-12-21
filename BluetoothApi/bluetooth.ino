void setup() {
    Serial.begin(9600); 
    delay(50);
}

int ctr = 0;
int dataReceived;
void loop() {
  if (Serial.available()){
    dataReceived = Serial.read();
    Serial.flush();
    ctr += 1;
    Serial.write(ctr); // echo everything
    delay(1000);
  }
}