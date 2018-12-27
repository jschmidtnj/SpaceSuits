#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
const char* ssid = "SPACESUITWIFI";
const char* password =  "N@sASu!t";
String url = "http://192.168.1.1:80";
String id = "device_name";
#define SEND_DELAY 100 //ms

void (*resetFunc) (void) = 0;

void setup() {
  DBG_OUTPUT_PORT.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (debug_mode)
      DBG_OUTPUT_PORT.println("Connecting to WiFi..");
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println("Connected to the WiFi network");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    String urlfull = url + "/" + id;

    StaticJsonBuffer<200> jsonBuffer;
    http.addHeader("operator", "text/plain");
    // add data
    JsonObject& data = jsonBuffer.createObject();
    data["id"] = id;
    data["data"] = "12345";
    String jsonStr;
    data.printTo(jsonStr);
    http.begin(urlfull); //Specify the URL
    // put request
    int httpCode = http.PUT(jsonStr);
    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      if (debug_mode) {
        DBG_OUTPUT_PORT.println(httpCode);
        DBG_OUTPUT_PORT.println(payload);
      }
    } else {
      if (debug_mode) {
        DBG_OUTPUT_PORT.println("Error on HTTP put request");
        DBG_OUTPUT_PORT.println("Resetting...");
      }
      resetFunc(); //call reset
    }
    http.end(); //Free the resources
    delay(SEND_DELAY);
  }
}