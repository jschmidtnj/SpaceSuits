#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "ArduinoJson.h"
#include "esp_wifi.h"
#include "U8x8lib.h"
#include "HTTPClient.h"
//#include "SoftwareSerial.h" // optional softwareserial library
//#include "EDB.h" // Extended database library - see https://github.com/jwhiddon/EDB?utm_source=platformio&utm_medium=piohome

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
#define create_local_host true // creates DNS for host and can be used to redirect all to host
#define DBG_BAUD_RATE 115200
#define ttl 300           // ttl for dns
#define channel_num 1     // channel number for softAP
#define max_connection 10 // max connections to AP
const char *ssid = "SPACESUITWIFI";
const char *password = "N@sASu!t";
const char *host = "spacesuit.com";
IPAddress Ip(192, 168, 1, 1); // ip address for website
AsyncWebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;
static const String devices[] = {"glove1"};
// to add another device add to devices[], create a JsonObject below, and modify the setup() function accordingly.
// JSON data
DynamicJsonBuffer jsonBuffer;
JsonObject &glove1 = jsonBuffer.createObject();
JsonObject &jsonDataNodes = jsonBuffer.createObject();

static const unsigned long WIFI_REFRESH_INTERVAL = 10000; // ms
static unsigned long wifiLastRefreshTime = 0;

static const unsigned long BLINK_INTERVAL = 1000; //ms
static unsigned long lastBlink = 0;
static bool blinkState = false; // false = off

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);

void handleNotFound(AsyncWebServerRequest *request)
{
  String path = request->url();
  if (path.endsWith("/"))
    path += "index.htm";
  for (String device : devices)
  {
    if (path.endsWith(device))
      return;
  }
  if (path.endsWith("hello"))
    return;
  String message = "\nNo Handler\r\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nParameters: ";
  message += request->params();
  message += "\n";
  for (uint8_t i = 0; i < request->params(); i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    String name = p->name().c_str();
    String val = p->value().c_str();
    message += (name + " : " + val + "\r\n");
  }
  request->send(404, "text/plain", message);
  if (debug_mode)
    DBG_OUTPUT_PORT.print(message);
}

void PrintStations()
{
  wifi_sta_list_t stationList;

  esp_wifi_ap_get_sta_list(&stationList);

  char headerChar[50];
  char buffer[5];
  String stationNumStr = itoa(stationList.num, buffer, 10);
  String headerStr = "Num Connect: " + stationNumStr;
  headerStr.toCharArray(headerChar, 50);
  if (debug_mode)
    DBG_OUTPUT_PORT.println(headerStr);

  u8x8.drawString(0, 0, headerChar);

  for (int i = 0; i < stationList.num; i++)
  {

    wifi_sta_info_t station = stationList.sta[i];

    String mac = "";

    for (int j = 0; j < 6; j++)
    {
      mac += (String)station.mac[j];
      if (j < 5)
      {
        mac += ":";
      }
    }
    if (debug_mode)
      DBG_OUTPUT_PORT.println(mac);
    char macChar[25];
    mac.substring(1, 16).toCharArray(macChar, 25);
    u8x8.drawString(0, i + 1, macChar);
  }
  for (int i = 0; i < 7 - stationList.num; i++)
  {
    u8x8.drawString(0, i + 1 + stationList.num, "                ");
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println("-----------------");
}

void handleQuery(AsyncWebServerRequest *request)
{
  int paramsNr = request->params();
  if (debug_mode)
  {
    DBG_OUTPUT_PORT.print(paramsNr);
    DBG_OUTPUT_PORT.println(" queries");
  }
  String dataStr;
  for (int i = 0; i < paramsNr; i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    String name = p->name();
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.print("Param name: ");
      DBG_OUTPUT_PORT.println(name);
    }
    String val = p->value();
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.print("Param value: ");
      DBG_OUTPUT_PORT.println(val);
    }
    if (name == "data")
      dataStr = val;
    if (debug_mode)
      DBG_OUTPUT_PORT.println("------");
  }
  //const char* querychar = dataStr.c_str();
  // do not delete querychar (results in error)
}

bool handleDataPut(AsyncWebServerRequest *request, uint8_t *datas)
{
  if (debug_mode)
  {
    // DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char *)datas);
  }
  if (debug_mode)
    handleQuery(request);
  JsonObject &data = jsonBuffer.parseObject((const char *)datas);
  if (!data.success())
    return 0;

  if (!data.containsKey("id"))
    return 0;
  String id = data["id"];
  if (debug_mode)
  {
    DBG_OUTPUT_PORT.println("data from " + id);
  }
  for (auto kv : jsonDataNodes)
  {
    if (id == kv.key)
    {
      jsonDataNodes[kv.key] = data;
    }
  }
  if (id == devices[0])
  {
    // handle request data here
    switch ((int) data["data"])
    {
    case 1:
      // handle a 1
      break;
    default:
      break;
    }
  }
  return 1;
}

void setup()
{

  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  DBG_OUTPUT_PORT.setDebugOutput(debug_mode);
  while (!DBG_OUTPUT_PORT)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (debug_mode)
    DBG_OUTPUT_PORT.println("started debug mode");

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Origin, Content-Type, X-Auth-Token");

  // jsondata nodes comes straight from the nodes
  jsonDataNodes["glove1"] = glove1;

  if (debug_mode)
    DBG_OUTPUT_PORT.println("initialized global variables");

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  if (debug_mode)
    DBG_OUTPUT_PORT.println("#oled initialized");

  WiFi.mode(WIFI_AP);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#Wait 100 ms for AP_START...");
  delay(100);
  // const char *ssid, const char *passphrase = (const char *)__null, int channel = 1, int ssid_hidden = 0, int max_connection = 4)
  WiFi.softAP(ssid, password, channel_num, 0, max_connection);

  if (debug_mode)
    DBG_OUTPUT_PORT.println("#Set softAPConfig");
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);

  IPAddress IP = WiFi.softAPIP();

  if (debug_mode)
  {
    DBG_OUTPUT_PORT.print("#IP address: ");
    DBG_OUTPUT_PORT.println(IP);
  }

  if (create_local_host)
  {
    //dnsServer.start(DNS_PORT, "*", IP);
    dnsServer.setTTL(ttl);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNS_PORT, host, IP);
  }

  // Server requests
  // create put requests
  for (String device : devices)
  {
    String path = "/" + device;
    const char *pathChar = path.c_str();
    server.on(pathChar, HTTP_PUT, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (!handleDataPut(request, data))
      {
        request->send(500, "text/plain", "false");
      }
      else
      {
        if (debug_mode)
        {
          String jsonDataNodesStr;
          jsonDataNodes.printTo(jsonDataNodesStr);
          DBG_OUTPUT_PORT.println(jsonDataNodesStr);
        }
        request->send(200, "text/plain", "true");
        } });
  }
  // test1
  if (debug_mode)
  {
    server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request) {
      DBG_OUTPUT_PORT.println("#hello get request");
      request->send(200, "text/plain", "Hello World Get");
    });
  }
  server.on("/getstatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#sensor data get request");
    String data = "";
    jsonDataNodes.printTo(data);
    request->send(200, "application/json", data);
  });
  //handle not found
  server.onNotFound(handleNotFound);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#finished creating server");
  server.begin();
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#HTTP server started");

  // set built-in led to output
  pinMode(BUILTIN_LED, OUTPUT);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#The LED is now enabled.");

  // print initial stations (0)
  PrintStations();
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#printed initial stations.");
}

void loop()
{

  if (create_local_host)
    dnsServer.processNextRequest();

  if (millis() - wifiLastRefreshTime >= WIFI_REFRESH_INTERVAL)
  {
    wifiLastRefreshTime += WIFI_REFRESH_INTERVAL;
    PrintStations();
  }

  if (millis() - lastBlink >= BLINK_INTERVAL)
  {
    lastBlink += BLINK_INTERVAL;
    blinkState = !blinkState;
    digitalWrite(LED_BUILTIN, blinkState);
  }
}