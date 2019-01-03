#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
#define DBG_BAUD_RATE 115200
#define SEND_DELAY 5000 //ms // 50 ms works well
const char* ssid = "SPACESUITWIFI";
const char* password =  "N@sASu!t";
String url = "http://192.168.1.1:80";
String id = "imu";

AsyncWebServer server(80);

void (*resetFunc) (void) = 0;

bool handleDataPut(AsyncWebServerRequest *request, uint8_t *datas) {
  if (debug_mode)
    DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char*)datas);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((const char*)datas); 
  if (!data.success()) return 0;

  if (!data.containsKey("command")) return 0;
  String command = data["command"];
  if (debug_mode) {
    DBG_OUTPUT_PORT.println("command: " + command);
  }
  return 1;
}

void handleNotFound(AsyncWebServerRequest *request){
  String message = "\nNo Handler\r\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET)?"GET":"POST";
  message += "\nParameters: ";
  message += request->params();
  message += "\n";
  for (uint8_t i=0; i<request->params(); i++){
    AsyncWebParameter* p = request->getParam(i);
    String name = p->name().c_str();
    String val = p->value().c_str();
    message += (name + " : " + val + "\r\n");
  }
  request->send(404, "text/plain", message);
  if (debug_mode)
    DBG_OUTPUT_PORT.print(message);
}

void setup() {
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  delay(500);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (debug_mode)
      DBG_OUTPUT_PORT.println("Connecting to WiFi..");
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println("Connected to the WiFi network");

  server.on("/command", HTTP_PUT, [](AsyncWebServerRequest *request){
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (!handleDataPut(request, data))
        request->send(200, "text/plain", "false");
      else
        request->send(200, "text/plain", "true");
  });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    DBG_OUTPUT_PORT.println("#hello get request");
    request->send(200, "text/plain", "Hello World Get");
  });
  server.onNotFound(handleNotFound);
  server.begin();
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
    data["ip"] = WiFi.localIP().toString();
    data["data"] = "imudata";
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