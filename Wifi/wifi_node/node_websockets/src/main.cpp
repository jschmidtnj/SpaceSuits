#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "WebSocketsClient.h"
#include "ArduinoJson.h"
#include "ESP8266WiFiMulti.h"

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
#define requestMode false // true = websockets, false = http reuqests
#define websocket_reconnect_interval 100 //ms
const char* ssid = "SPACESUITWIFI";
const char* password =  "N@sASu!t";
const char* websocketPath = "/ws";
String url = "192.168.1.1";
String id = "glove1";

static const unsigned long SEND_DELAY_INTERVAL = 100; //ms // 50 ms works well
static unsigned long lastSend = 0;

static const unsigned long WS_SEND_DELAY_INTERVAL = 100; //ms
static unsigned long lastSendWS = 0;

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

void (*resetFunc) (void) = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	if(type == WStype_DISCONNECTED) {
    if (debug_mode)
			DBG_OUTPUT_PORT.printf("[WSc] Disconnected!\n");
  } else if (type == WStype_CONNECTED) {
		if (debug_mode)
    	DBG_OUTPUT_PORT.printf("[WSc] Connected to url: %s\n", payload);
    // send message to server when Connected
    webSocket.sendTXT("Connected");
	} else if (type == WStype_TEXT) {
    if (debug_mode)
			DBG_OUTPUT_PORT.printf("[WSc] get text: %s\n", payload);
    DynamicJsonBuffer jsonBufferInput;
    JsonObject& inputDataJson = jsonBufferInput.parseObject(payload);
    bool gotJson = false;
    if (inputDataJson.success())
      gotJson = true;
    if (gotJson) {
      if (debug_mode)
        DBG_OUTPUT_PORT.println("valid json");
    } else {
      if (debug_mode)
        DBG_OUTPUT_PORT.println("invalid json");
    }
    // send message to server
    // webSocket.sendTXT("message");
  } else if (type == WStype_BIN) {
		if (debug_mode)
			DBG_OUTPUT_PORT.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);
    // send data to server
    // webSocket.sendBIN(payload, length);
  } else {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("unhandled type");
  }
}

void wsSetup() {
  // server address, port and URL
	webSocket.begin(url, 80, websocketPath);

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try again if connection has failed
	webSocket.setReconnectInterval(websocket_reconnect_interval);
  for (int i = 0; i < 100; i++) {
    webSocket.loop();
    delay(10);
  }
}

void setup() {
  DBG_OUTPUT_PORT.begin(115200);
  delay(500);
  //WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  while (WiFi.status() != WL_CONNECTED && WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    if (debug_mode)
      DBG_OUTPUT_PORT.println("Connecting to WiFi..");
  }
  if (debug_mode) {
    DBG_OUTPUT_PORT.println("Connected to the WiFi network");
    DBG_OUTPUT_PORT.println("IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.localIP());
  }
  if (requestMode)
    wsSetup();
}


void putRequest(JsonObject& data) {
  HTTPClient http;
  String urlfull = "http://" + url + ":80/" + id;
  http.addHeader("operator", "text/plain");
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
}

void webSocketSend(JsonObject& data) {
  String jsonStr;
  data.printTo(jsonStr);
  webSocket.sendTXT(jsonStr);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("sent message via websocket: " + jsonStr);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && WiFiMulti.run() == WL_CONNECTED) {
    if (requestMode) {
      int numres = 0;
      int numtries = 1;
      for (int i = 0; i < numtries; i++) {
        numres += webSocket.sendPing();
        if (numtries > 1)
          delay(10);
      }
      if (numres < numtries) {
        if (debug_mode) {
          DBG_OUTPUT_PORT.println("Lost Wifi connection");
          DBG_OUTPUT_PORT.println("Resetting...");
        }
        resetFunc();
      }
      webSocket.loop();
      if (millis() - lastSendWS >= WS_SEND_DELAY_INTERVAL) {
        lastSendWS += WS_SEND_DELAY_INTERVAL;
        StaticJsonBuffer<200> jsonBuffer;
        // add data
        JsonObject& data = jsonBuffer.createObject();
        data["id"] = id;
        String ipaddr = WiFi.localIP().toString();
        data["ip"] = ipaddr;
        webSocketSend(data);
      }
    } else {
      if(millis() - lastSend >= SEND_DELAY_INTERVAL) {
        lastSend += SEND_DELAY_INTERVAL;
        StaticJsonBuffer<200> jsonBuffer;
        // add data
        JsonObject& data = jsonBuffer.createObject();
        data["id"] = id;
        data["data"] = "glovedata";
        putRequest(data);
      }
    }
  } else {
    if (debug_mode) {
      DBG_OUTPUT_PORT.println("Lost Wifi connection");
      DBG_OUTPUT_PORT.println("Resetting...");
    }
    resetFunc();
  }
}