#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <DNSServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
/*
* D0 = MISO = 19
* D1 = MOSI = 23
* CLK = SCK = 18
* CS = 2
*/

const char *ssid = "MyESP32AP";
const char *password = "testpassword";
IPAddress Ip(192, 168, 1, 1); // ip address for website
 
AsyncWebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;
 
void setup(){
  Serial.begin(115200);
  if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
 
  WiFi.mode(WIFI_AP);
  Serial.println("Wait 100 ms for AP_START...");
  delay(100);
  WiFi.softAP(ssid, password);

  Serial.println("Set softAPConfig");
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
 
  IPAddress IP = WiFi.softAPIP();

  Serial.print("IP address: ");
  Serial.println(IP);

  dnsServer.start(DNS_PORT, "*", IP);

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });
 
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Test123");
  });
 
  server.begin();
}

void loop(){
  dnsServer.processNextRequest();
}
