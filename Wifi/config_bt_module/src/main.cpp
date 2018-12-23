#define DBG_OUTPUT_PORT Serial
#include <Arduino.h>
#include "SoftwareSerial.h"

#define RX_PIN 13 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 12// tx pin on bluetooth module connected to this pin on Arduino
#define KEY_PIN 14 // this pin will pull the HC-05 pin 34 (key pin or EN pin) HIGH to switch
#define DBG_BAUD_RATE 115200
#define AT_BAUD_RATE 38400
static unsigned int NEW_BAUD_RATE = 9600;
#define RESET_TO_DEFAULT false
#define NEW_NAME "SPACESUIT1"
#define NEW_SECURITY_KEY "1234"

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
SoftwareSerial btSerial(RX_PIN, TX_PIN, false, 256);

void setup() {
  pinMode(KEY_PIN, OUTPUT);
  digitalWrite(KEY_PIN, HIGH);
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  btSerial.begin(AT_BAUD_RATE);
  delay(10000); // wait 10 seconds to get the wiring correct
  /*
  DBG_OUTPUT_PORT.print("HC-05 version: ");
  btSerial.print("AT+VERSION?\r\n");
  if (RESET_TO_DEFAULT) {
    DBG_OUTPUT_PORT.println("Reset to original settings");
    btSerial.print("AT+ORGL\r\n");
  } else {
    DBG_OUTPUT_PORT.println("change device name to " + String(NEW_NAME));
    btSerial.print("AT+NAME=" + String(NEW_NAME) + "\r\n");
    delay(100);
    DBG_OUTPUT_PORT.println("change device security code to " + String(NEW_SECURITY_KEY));
    btSerial.print("AT+PSWD=" + String(NEW_SECURITY_KEY) + "\r\n");
    delay(100);
    char newBaudRate[15];
    itoa(NEW_BAUD_RATE, newBaudRate, 25);
    DBG_OUTPUT_PORT.println("change device baud rate to " + String(newBaudRate));
    btSerial.print("AT+UART=" + String(newBaudRate) + ",1,0\r\n");
    delay(100);
  }
  */
}

void loop() {
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (btSerial.available())
    Serial.write(btSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    btSerial.write(Serial.read());
}