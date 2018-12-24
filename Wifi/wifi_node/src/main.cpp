#define DBG_OUTPUT_PORT Serial
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

const char* ssid = "SPACESUITWIFI";
const char* password =  "N@sASu!t";

void(* resetFunc) (void) = 0; 

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  Serial.println("Connected to the WiFi network");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("http://192.168.1.1:80/hello"); //Specify the URL
    int httpCode = http.GET(); //Make the request

    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP get request");
      Serial.println("Resetting...");
      resetFunc(); //call reset
    }

    StaticJsonBuffer<200> jsonBuffer;
    http.addHeader("operator", "text/plain");
    // create an object
    JsonObject& payload = jsonBuffer.createObject();
    payload["hello"] = "world";
    payload["command"] = "test";
    String jsonStr;
    payload.printTo(jsonStr);
    httpCode = http.POST(jsonStr);
    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP post request");
      Serial.println("Resetting...");
      resetFunc(); //call reset
    }

    http.end(); //Free the resources
  }

  delay(10000);
}