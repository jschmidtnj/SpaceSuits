#include <Arduino.h>
#include "SoftwareSerial.h"

#define DBG_OUTPUT_PORT Serial
#define RX_PIN 8 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 9 // tx pin on bluetooth module connected to this pin on Arduino
#define KEY_PIN 10 // this pin will pull the HC-05 pin 34 (key pin or EN pin) HIGH to switch
#define DBG_BAUD_RATE 115200
#define AT_BAUD_RATE 38400
static unsigned int NEW_BAUD_RATE = 9600;
#define RESET_TO_DEFAULT false
#define NEW_NAME "SPACESUITBT"
#define NEW_SECURITY_KEY "1234"
#define MAIN_DELAY_TIME 10000 //ms
#define COMMAND_DELAY_TIME 100 //ms

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
SoftwareSerial btSerial(RX_PIN, TX_PIN);

void runCommand(String command) {
  btSerial.print(command + "\r\n");
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
  pinMode(KEY_PIN, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(KEY_PIN, HIGH);
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  DBG_OUTPUT_PORT.println("Starting...");
  btSerial.begin(38400);  // HC-05 default speed in AT command mode
  delay(MAIN_DELAY_TIME); // wait 10 seconds to get the wiring correct
  runCommand("AT");
  runCommand("AT+VERSION?");
  if (RESET_TO_DEFAULT) {
    runCommand("AT+ORGL");
  } else {
    DBG_OUTPUT_PORT.println("change device name to " + String(NEW_NAME));
    runCommand("AT+NAME=" + String(NEW_NAME));
    // below commands don't quite work yet
    //DBG_OUTPUT_PORT.println("change device security code to " + String(NEW_SECURITY_KEY));
    //runCommand("AT+PSWD=" + String(NEW_SECURITY_KEY));
    //char newBaudRate[20];
    //itoa(NEW_BAUD_RATE, newBaudRate, 30);
    //DBG_OUTPUT_PORT.println("change device baud rate to " + String(newBaudRate));
    //runCommand("AT+UART=" + String(newBaudRate) + ",1,0");
  }
  DBG_OUTPUT_PORT.println("Enter AT commands:");
}

void loop() {
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (btSerial.available())
    DBG_OUTPUT_PORT.write(btSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (DBG_OUTPUT_PORT.available())
    btSerial.write(DBG_OUTPUT_PORT.read());
}