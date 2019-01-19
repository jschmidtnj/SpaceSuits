#include <Arduino.h>

#define DBG_OUTPUT_PORT Serial
#define BT_BAUD_RATE 9600
#define RX_PIN 13 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 12 // tx pin on bluetooth module connected to this pin on Arduino
#define DBG_BAUD_RATE 115200

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
HardwareSerial btSerial(1);

void setup() {
  // Open serial communications and wait for port to open:
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  while (!DBG_OUTPUT_PORT) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  DBG_OUTPUT_PORT.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  btSerial.begin(BT_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  btSerial.println("Hello, world?");
}

void loop() { // run over and over
  if (btSerial.available()) {
    DBG_OUTPUT_PORT.write(btSerial.read());
  }
  if (DBG_OUTPUT_PORT.available()) {
    btSerial.write(DBG_OUTPUT_PORT.read());
  }
}