#include <Arduino.h>
#include "SoftwareSerial.h"

#define DBG_OUTPUT_PORT Serial
#define RX_PIN 8 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 9 // tx pin on bluetooth module connected to this pin on Arduino
#define DBG_BAUD_RATE 115200

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
SoftwareSerial btSerial(RX_PIN, TX_PIN);

void setup() {
  // Open serial communications and wait for port to open:
  DBG_OUTPUT_PORT.begin(9600);
  while (!DBG_OUTPUT_PORT) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  DBG_OUTPUT_PORT.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  btSerial.begin(9600);
  btSerial.println("Hello, world?");
}

void loop() { // run over and over
  if (btSerial.available()) {
    Serial.write(btSerial.read());
  }
  if (Serial.available()) {
    btSerial.write(Serial.read());
  }
}