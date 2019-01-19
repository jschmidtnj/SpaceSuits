#include <Arduino.h>
#include "ArduinoJson.h"

#define DBG_OUTPUT_PORT Serial
#define BT_BAUD_RATE 9600
#define RX_PIN 13 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 12 // tx pin on bluetooth module connected to this pin on Arduino
#define DBG_BAUD_RATE 115200
#define NEW_NAME "SPACESUITBLUE"
#define NEW_SECURITY_KEY "0000"
#define COMMAND_DELAY_TIME 1000 //ms

const char* NEW_BAUD_RATE = "9600";

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
HardwareSerial btSerial(1);

void runCommand(String command) {
  btSerial.print(command);
  bool cont = false;
  while (!cont) {
    if (btSerial.available()) {
      DBG_OUTPUT_PORT.write(btSerial.read());
      cont = true;
    }
  }
  DBG_OUTPUT_PORT.println();
  delay(COMMAND_DELAY_TIME);
}

void setup() {
  // Open serial communications and wait for port to open:
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  while (!DBG_OUTPUT_PORT) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  btSerial.begin(BT_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

  delay(COMMAND_DELAY_TIME);

  StaticJsonBuffer<200> btjsonBuffer;
  JsonObject& baudRates = btjsonBuffer.createObject();

  baudRates["1200"] = "BAUD1";
  baudRates["2400"] = "BAUD2";
  baudRates["4800"] = "BAUD3";
  baudRates["9600"] = "BAUD4";
  baudRates["19200"] = "BAUD5";
  baudRates["38400"] = "BAUD6";
  baudRates["57600"] = "BAUD7";
  baudRates["115200"] = "BAUD8";
  baudRates["230400"] = "BAUD9";
  baudRates["460800"] = "BAUDA";
  baudRates["921600"] = "BAUDB";
  baudRates["1382400"] = "BAUDC";

  if (baudRates.containsKey(NEW_BAUD_RATE)) {
    String theBaudRate = baudRates[NEW_BAUD_RATE];
    runCommand("AT+" + theBaudRate);
    DBG_OUTPUT_PORT.println("change device baud rate to " + String(NEW_BAUD_RATE));
  } else {
    DBG_OUTPUT_PORT.println("baud rate " + String(NEW_BAUD_RATE) + " not available");
  }
  DBG_OUTPUT_PORT.println("change device name to " + String(NEW_NAME));
  runCommand("AT+NAME" + String(NEW_NAME));
  DBG_OUTPUT_PORT.println("change device security code to " + String(NEW_SECURITY_KEY));
  runCommand("AT+PIN" + String(NEW_SECURITY_KEY));

  DBG_OUTPUT_PORT.println("Enter AT commands:");
}

void loop() { // run over and over
  if (btSerial.available()) {
    DBG_OUTPUT_PORT.write(btSerial.read());
  }
  if (DBG_OUTPUT_PORT.available()) {
    btSerial.write(DBG_OUTPUT_PORT.read());
  }
}