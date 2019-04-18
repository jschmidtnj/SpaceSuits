#define FS_NO_GLOBALS
#include <Arduino.h>
#include "WiFi.h"
#include "FS.h"
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "SPI.h"
#include "ArduinoJson.h"
#include "SD.h"
#include "esp_wifi.h"
#include "U8x8lib.h"
#include "HTTPClient.h"
//#include "SoftwareSerial.h" // optional softwareserial library
//#include "EDB.h" // Extended database library - see https://github.com/jwhiddon/EDB?utm_source=platformio&utm_medium=piohome

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
#define bluetooth_on true
#define website_on false
#define bt_requests true
#define bt_mode false          // false = send data after every requst, true = send data in intervals
#define create_local_host true // creates DNS for host and can be used to redirect all to host
#define DBG_BAUD_RATE 115200
#define ttl 300             // ttl for dns
#define channel_num 1       // channel number for softAP
#define max_connection 10   // max connections to AP
#define websocket false     // web socket on = true, off = false. has relatively high latancy (fast for first 30 requests and then slows down)
#define websocketslow false // turn on if web socket requests >= 500 ms apart
#define BT_BAUD_RATE 9600
#define RX_PIN 13                    // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 12                    // tx pin on bluetooth module connected to this pin on Arduino
#define bluetooth_device_config true // true = configure hc-06 on start
#define NEW_BT_NAME "SPACESUITBT"
#define NEW_BT_SECURITY_KEY "1234"
#define AT_COMMAND_DELAY_TIME 1000 //ms
#define INITIAL_BT_DELAY 1000      //ms
const char *NEW_BT_BAUD_RATE = "9600";
const char *ssid = "SPACESUITWIFI";
const char *password = "N@sASu!t";
const char *host = "spacesuit.com";
IPAddress Ip(192, 168, 1, 1); // ip address for website
const int pin_CS_SDcard = 15;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // starting web socket
const byte DNS_PORT = 53;
DNSServer dnsServer;
static const String devices[] = {"glove1", "glove2", "imu"};
// to add another device add to devices[], create a JsonObject below, and modify the setup() function accordingly.
// JSON data
DynamicJsonBuffer jsonBuffer;
JsonObject &glove1 = jsonBuffer.createObject();
JsonObject &imu = jsonBuffer.createObject();
JsonObject &jsonDataNodes = jsonBuffer.createObject();

// data for hololens and tasks
JsonObject &hololensData = jsonBuffer.createObject();
JsonArray &tasksArray = jsonBuffer.createArray();
JsonObject &inertialState = jsonBuffer.createObject();
JsonObject &suitTelemetry = jsonBuffer.createObject();
JsonObject &suitData = jsonBuffer.createObject();
static unsigned int warning = 0;
static unsigned int glove1data = 1;
static String tasksObjectStr = "";

static const unsigned long WIFI_REFRESH_INTERVAL = 10000; // ms
static unsigned long wifiLastRefreshTime = 0;
static const unsigned long BLUETOOTH_REFRESH_INTERVAL = 500; //ms // 100 ms works well
static unsigned long bluetoothLastRefreshTime = 0;

static const unsigned long BLINK_INTERVAL = 1000; //ms
static unsigned long lastBlink = 0;
static bool blinkState = false; // false = off

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);

// Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
//SoftwareSerial btSerial(RX_PIN, TX_PIN, false, 256);
//not using softwareserial anymore because it had errors with data receive.
HardwareSerial btSerial(1);

void returnOK(AsyncWebServerRequest *request) { request->send(200, "text/plain", ""); }

void returnFail(AsyncWebServerRequest *request, String msg) { request->send(500, "text/plain", msg + "\r\n"); }

bool loadFromSdCard(AsyncWebServerRequest *request)
{
  // making this async does not work well yet
  String path = request->url();
  String dataType = "text/plain";
  struct fileBlk
  {
    File dataFile;
  };
  fileBlk *fileObj = new fileBlk;

  if (path.endsWith("/"))
    path += "index.htm";
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".json"))
    dataType = "text/json";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".svg"))
    dataType = "image/svg+xml";
  else if (path.endsWith(".eot"))
    dataType = "font/eot";
  else if (path.endsWith(".woff"))
    dataType = "font/woff";
  else if (path.endsWith(".woff2"))
    dataType = "font/woff2";
  else if (path.endsWith(".ttf"))
    dataType = "font/ttf";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";
  else if (path.endsWith(".gz"))
    dataType = "application/x-gzip";
  else
    dataType = "text/plain";

  fileObj->dataFile = SD.open(path.c_str());
  if (fileObj->dataFile.isDirectory())
  {
    path += "/index.htm";
    dataType = "text/html";
    fileObj->dataFile = SD.open(path.c_str());
  }

  if (!fileObj->dataFile)
  {
    delete fileObj;
    return false;
  }

  if (request->hasParam("download"))
    dataType = "application/octet-stream";

  // Here is the context problem.  If there are multiple downloads active,
  // we don't have the File handles. So we only allow one active download request
  // at a time and keep the file handle in static.  I'm open to a solution.

  request->_tempObject = (void *)fileObj;
  request->send(dataType, fileObj->dataFile.size(), [request](uint8_t *buffer, size_t maxlen, size_t index) -> size_t {
    fileBlk *fileObj = (fileBlk *)request->_tempObject;
    size_t thisSize = fileObj->dataFile.read(buffer, maxlen);
    if ((index + thisSize) >= fileObj->dataFile.size())
    {
      fileObj->dataFile.close();
      request->_tempObject = NULL;
      delete fileObj;
    }
    return thisSize;
  });
  return true;
}

void handleSDUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  struct uploadRequest
  {
    uploadRequest *next;
    AsyncWebServerRequest *request;
    File uploadFile;
    uint32_t fileSize;
    uploadRequest()
    {
      next = NULL;
      request = NULL;
      fileSize = 0;
    }
  };
  static uploadRequest uploadRequestHead;
  uploadRequest *thisUploadRequest = NULL;

  if (!index)
  {
    if (SD.exists((char *)filename.c_str()))
      SD.remove((char *)filename.c_str());
    thisUploadRequest = new uploadRequest;
    thisUploadRequest->request = request;
    thisUploadRequest->next = uploadRequestHead.next;
    uploadRequestHead.next = thisUploadRequest;
    thisUploadRequest->uploadFile = SD.open(filename.c_str(), FILE_WRITE);
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.print("Upload: START, filename: ");
      DBG_OUTPUT_PORT.println(filename);
    }
  }
  else
  {
    thisUploadRequest = uploadRequestHead.next;
    while (thisUploadRequest->request != request)
      thisUploadRequest = thisUploadRequest->next;
  }

  if (thisUploadRequest->uploadFile)
  {
    for (size_t i = 0; i < len; i++)
    {
      thisUploadRequest->uploadFile.write(data[i]);
    }
    thisUploadRequest->fileSize += len;
  }

  if (final)
  {
    thisUploadRequest->uploadFile.close();
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.print("Upload: END, Size: ");
      DBG_OUTPUT_PORT.println(thisUploadRequest->fileSize);
    }

    uploadRequest *linkUploadRequest = &uploadRequestHead;
    while (linkUploadRequest->next != thisUploadRequest)
      linkUploadRequest = linkUploadRequest->next;
    linkUploadRequest->next = thisUploadRequest->next;
    delete thisUploadRequest;
  }
}

void deleteRecursive(String path)
{
  File file = SD.open((char *)path.c_str());
  if (!file.isDirectory())
  {
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }
  file.rewindDirectory();
  File entry;
  while (entry = file.openNextFile())
  {
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory())
    {
      entry.close();
      deleteRecursive(entryPath);
    }
    else
    {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
  }
  SD.rmdir((char *)path.c_str());
  file.close();
}

void handleDelete(AsyncWebServerRequest *request)
{
  if (!request->params())
    returnFail(request, "No Path");
  String path = request->arg("path");
  if (path == "/" || !SD.exists((char *)path.c_str()))
  {
    returnFail(request, "Bad Path");
  }
  deleteRecursive(path);
  returnOK(request);
}

void handleCreate(AsyncWebServerRequest *request)
{
  if (!request->params())
    returnFail(request, "No Path");
  String path = request->arg("path");
  if (path == "/" || SD.exists((char *)path.c_str()))
  {
    returnFail(request, "Bad Path");
    return;
  }

  if (path.indexOf('.') > 0)
  {
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if (file)
    {
      file.write(0);
      file.close();
    }
  }
  else
  {
    SD.mkdir((char *)path.c_str());
  }
  returnOK(request);
}

void printDirectory(AsyncWebServerRequest *request)
{
  if (!request->hasParam("dir"))
    return returnFail(request, "BAD ARGS");
  String path = request->arg("dir");
  if (path != "/" && !SD.exists((char *)path.c_str()))
    return returnFail(request, "BAD PATH");
  File dir = SD.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory())
  {
    dir.close();
    return returnFail(request, "NOT DIR");
  }
  JsonArray &array = jsonBuffer.createArray();
  dir.rewindDirectory();
  File entry;
  while (entry = dir.openNextFile())
  {
    JsonObject &object = jsonBuffer.createObject();
    object["type"] = (entry.isDirectory()) ? "dir" : "file";
    object["name"] = entry.name();
    array.add(object);
    entry.close();
  }
  String response = "";
  array.printTo(response);
  request->send(200, "application/json", response);
  dir.close();
}

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
  if (website_on && loadFromSdCard(request))
  {
    return;
  }
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

  char headerChar[50] = {0};
  char buffer[5] = {0};
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

bool handleTest(AsyncWebServerRequest *request, uint8_t *datas)
{
  if (debug_mode)
  {
    // DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char *)datas);
  }

  JsonObject &_test = jsonBuffer.parseObject((const char *)datas);
  if (!_test.success())
    return 0;

  if (!_test.containsKey("command"))
    return 0;
  String _command = _test["command"];
  if (debug_mode)
  {
    DBG_OUTPUT_PORT.println(_command);
    DBG_OUTPUT_PORT.println("hello post request");
  }
  request->send(200, "text/plain", "Hello World Post");

  return 1;
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

void sendDataBT()
{
  String data = "";
  jsonDataNodes.printTo(data);
  btSerial.println(data);
}

bool handleDataPut(AsyncWebServerRequest *request, uint8_t *datas)
{
  if (debug_mode)
  {
    DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char *)datas);
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
  if (id == devices[2])
  {
    inertialState["accelx"] = data["data"]["accelx"];
    inertialState["accely"] = data["data"]["accely"];
    inertialState["accelz"] = data["data"]["accelz"];
    inertialState["roll"] = data["data"]["roll"];
    inertialState["pitch"] = data["data"]["pitch"];
    inertialState["yaw"] = data["data"]["yaw"];
  }
  else if (id == devices[0])
  {
    hololensData["glove"] = data["data"];
  }
  // send data through bluetooth immediately
  if (!bt_mode && !bt_requests)
    sendDataBT();
  // uncomment this to print data instead of sendDataBT()
  // btSerial.println((const char*)datas);
  return 1;
}

void sendToAllWs(String message)
{
  const char *dataChar = message.c_str();
  ws.printfAll(dataChar);
}

void handleWs(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  // currently a bug preventing messages to be sent faster than 500ms intervals to clients
  if (type == WS_EVT_CONNECT)
  {
    //client connected
    if (debug_mode)
      DBG_OUTPUT_PORT.printf("ws[%s][%u] connect\n", server->url(), client->id());
    if (websocketslow)
      client->printf("{\"status\": 200, \"message\": \"Connected to client %u\"}", client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    //client disconnected
    if (debug_mode)
      DBG_OUTPUT_PORT.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    //error was received from the other end
    if (debug_mode)
      DBG_OUTPUT_PORT.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    //pong message was received (in response to a ping request maybe)
    if (debug_mode)
      DBG_OUTPUT_PORT.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    //data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      if (debug_mode)
        DBG_OUTPUT_PORT.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
      if (info->opcode == WS_TEXT)
      {
        data[len] = 0;
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("%s\n", (char *)data);
      }
      else
      {
        for (size_t i = 0; i < info->len; i++)
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.printf("%02x ", data[i]);
        }
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("\n");
      }
      if (websocketslow)
      {
        if (info->opcode == WS_TEXT)
          client->text("{\"status\": 200, \"message\": \"received text message\"}");
        else
          client->binary("{\"status\": 200, \"message\": \"received binary message\"}");
      }
    }
    else
    {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          if (debug_mode)
            DBG_OUTPUT_PORT.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      if (debug_mode)
        DBG_OUTPUT_PORT.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
      if (info->message_opcode == WS_TEXT)
      {
        data[len] = 0;
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("%s\n", (char *)data);
      }
      else
      {
        for (size_t i = 0; i < len; i++)
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.printf("%02x ", data[i]);
        }
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("\n");
      }

      if ((info->index + len) == info->len)
      {
        if (debug_mode)
          DBG_OUTPUT_PORT.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final)
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (websocketslow)
          {
            if (info->message_opcode == WS_TEXT)
              client->text("{\"status\": 200, \"message\": \"received text message\"}");
            else
              client->binary("{\"status\": 200, \"message\": \"received binary message\"}");
          }
        }
      }
    }
  }
}

void sendToSpecificIp(String message, String ip)
{
  HTTPClient http;
  String urlfull = "http://" + ip + ":80/command";
  http.addHeader("operator", "text/plain");
  http.begin(urlfull); //Specify the URL
  // put request
  int httpCode = http.PUT(message);
  if (httpCode > 0)
  { //Check for the returning code
    String payload = http.getString();
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.println(httpCode);
      DBG_OUTPUT_PORT.println(payload);
    }
  }
  else
  {
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.println("Error on HTTP put request");
    }
  }
  http.end(); //Free the resources
}

void testIPGet(String ip)
{
  HTTPClient http;
  String urlfull = "http://" + ip + "/test";
  DBG_OUTPUT_PORT.println(urlfull);
  http.addHeader("operator", "text/plain");
  http.begin(urlfull); //Specify the URL
  // put request
  int httpCode = http.GET();
  if (httpCode > 0)
  { //Check for the returning code
    String payload = http.getString();
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.println(httpCode);
      DBG_OUTPUT_PORT.println(payload);
    }
  }
  else
  {
    if (debug_mode)
    {
      DBG_OUTPUT_PORT.println("Error on HTTP get request");
      DBG_OUTPUT_PORT.println("Resetting...");
    }
    //resetFunc(); //call reset
  }
  http.end(); //Free the resources
}

void sendToAllHttp(String message)
{
  for (auto kv : jsonDataNodes)
  {
    JsonObject &tempObj = jsonDataNodes[kv.key];
    if (tempObj.containsKey("ip"))
      sendToSpecificIp(message, tempObj["ip"]);
  }
}

void refreshTaskArray(JsonObject &tasksObject)
{
  // clear task array
  for (int i = 0; i < tasksArray.measureLength(); i++)
  {
    tasksArray.remove(i);
  }
  // then add new tasks
  for (auto majorkeyvalues : tasksObject)
  {
    for (auto subkeyvalues : majorkeyvalues.value.as<JsonObject>())
    {
      String jsonStr = "";
      subkeyvalues.value.as<JsonObject>().printTo(jsonStr);
      JsonObject &newObj = jsonBuffer.parse(jsonStr);
      newObj["majorkey"] = majorkeyvalues.key;
      newObj["subkey"] = subkeyvalues.key;
      tasksArray.add<JsonObject>(newObj);
    }
  }
  String val = "";
  tasksArray.printTo(val);
  DBG_OUTPUT_PORT.println(val);
  String data = "";
  hololensData.printTo(data);
  DBG_OUTPUT_PORT.println(data);
}

void sendDataHololensBT()
{
  String data = "";
  hololensData.printTo(data);
  String dataLen = String(data.length());
  if (dataLen.length() > 4)
  {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("json object is too long for 4 digits len");
    return;
  }
  else
  {
    for (int i = 0; i < 4 - dataLen.length(); i++)
    {
      dataLen = "0" + dataLen;
    }
  }
  btSerial.println(dataLen);
  btSerial.println(data);
  /*hololensData["glove"] = 0; // reset glove data
  if (debug_mode)
    DBG_OUTPUT_PORT.println("reset glove 1");*/
}

void handleCommand(String command)
{
  if (debug_mode)
    DBG_OUTPUT_PORT.println("received command: " + command);
  JsonObject &data = jsonBuffer.parseObject(command);
  if (data.success())
  {
    if (data.containsKey("id"))
    {
      if (data["id"] == "settasks")
      {
        if (data.containsKey("tasks"))
        {
          JsonObject &taskObj = jsonBuffer.parseObject((const char *)data["tasks"]);
          if (!taskObj.success())
            if (debug_mode)
              DBG_OUTPUT_PORT.println("did not get valid json in task data");
            else
            {
              tasksObjectStr = data["tasks"].as<String>();
              refreshTaskArray(taskObj);
              if (debug_mode)
                DBG_OUTPUT_PORT.println("updated tasks from hololens data");
            }
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not receive task data");
        }
      }
      else if (data["id"] == "suitTelem")
      {
        if (data.containsKey("data"))
        {
          JsonArray &telemetryArray = jsonBuffer.parseArray((const char *)data["data"]);
          if (!telemetryArray.success())
            if (debug_mode)
              DBG_OUTPUT_PORT.println("did not get valid json array in suit telemetry");
            else
            {
              for (auto kv : telemetryArray[0].as<JsonObject>())
              {
                suitTelemetry[kv.key] = kv.value;
              }
              if (debug_mode)
                DBG_OUTPUT_PORT.println("updated tasks from hololens data");
            }
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not receive telemetry data");
        }
      }
      else if (data["id"] == "getdata")
      {
        if (bluetooth_on)
        {
          sendDataHololensBT();
          if (debug_mode)
            DBG_OUTPUT_PORT.println("sent response to getdata request");
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not send getdata response because bluetooth disabled");
        }
      }
      else if (data["id"] == "resetglove1")
      {
        hololensData["glove"] = 0; // reset glove data
        if (bluetooth_on)
        {
          sendDataHololensBT();
          if (debug_mode)
            DBG_OUTPUT_PORT.println("sent reset glove 1 data");
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not send reset glove 1 data because bluetooth disconnected");
        }
      }
      else if (data["id"] == "status")
      {
        // return status request message
        if (bluetooth_on)
        {
          sendDataBT();
          if (debug_mode)
            DBG_OUTPUT_PORT.println("sent status response");
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not send status response because bluetooth disabled");
        }
      }
      else if (data["id"] == "all")
      {
        sendToAllHttp(command);
        if (websocket || debug_mode)
          sendToAllWs(command);
      }
      else if (data["id"] != "all")
      {
        const char *ip = "";
        for (auto kv : jsonDataNodes)
        {
          JsonObject &tempObj = jsonDataNodes[kv.key];
          if (tempObj.containsKey("id") && tempObj["id"] == data["id"] && tempObj.containsKey("ip"))
          {
            ip = tempObj["ip"];
            break;
          }
        }
        if (ip[0] != '\0')
        {
          sendToSpecificIp(command, ip);
          //testIPGet(ip);
        }
        else
        {
          if (debug_mode)
            DBG_OUTPUT_PORT.println("did not find ip address");
        }
      }
      else
      {
        if (debug_mode)
          DBG_OUTPUT_PORT.println("did not receive valid request");
      }
    }
    else
    {
      if (debug_mode)
        DBG_OUTPUT_PORT.println("did not receive valid id");
    }
  }
  else
  {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("received invalid json command");
    if (bluetooth_on)
      sendDataHololensBT();
  }
}

String setTaskData(AsyncWebServerRequest *request, uint8_t *datas)
{
  char* chardata = (char*) datas;
  if (debug_mode)
  {
    DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\n", chardata);
    DBG_OUTPUT_PORT.printf("size of char: %u", strlen(chardata));
  }
  DynamicJsonBuffer taskDataBuffer(strlen(chardata) + 1);
  JsonObject &data = taskDataBuffer.parseObject(chardata);
  JsonObject &resp = taskDataBuffer.createObject();
  String respStr = "";
  if (!data.success())
  {
    String errormsg = "invalid json in put task";
    if (debug_mode)
      DBG_OUTPUT_PORT.println(errormsg);
    resp["message"] = errormsg;
  }
  else
  {
    // maybe do some validation here
    String dataStr = "";
    data.printTo(dataStr);
    char tasksChar[dataStr.length() + 1] = {0};
    dataStr.toCharArray(tasksChar, dataStr.length() + 1);
    JsonObject &tasksObject = taskDataBuffer.parseObject(tasksChar);
    if (!tasksObject.success())
    {
      String errormsg = "invalid json in put task data";
      if (debug_mode)
        DBG_OUTPUT_PORT.println(errormsg);
      resp["message"] = errormsg;
    }
    else
    {
      tasksObjectStr = dataStr;
      refreshTaskArray(tasksObject);
      String successmsg = "set task data";
      if (debug_mode)
        DBG_OUTPUT_PORT.println(successmsg);
      resp["message"] = successmsg;
    }
  }
  resp.printTo(respStr);
  return respStr;
}

void removeChar(char *s, int c)
{
  int j, n = strlen(s);
  for (int i = j = 0; i < n; i++)
    if (s[i] != c)
      s[j++] = s[i];
  s[j] = '\0';
}

void runATCommand(String command)
{
  btSerial.print(command);
  bool cont = false;
  while (!cont)
  {
    if (btSerial.available())
    {
      DBG_OUTPUT_PORT.write(btSerial.read());
      cont = true;
    }
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println();
  delay(AT_COMMAND_DELAY_TIME);
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
  jsonDataNodes["imu"] = imu;
  // hololens data is for the hololens
  hololensData["tasks"] = tasksArray;
  hololensData["inertialState"] = inertialState;
  hololensData["suitTelemetry"] = suitTelemetry;
  hololensData["warning"] = warning;
  hololensData["glove"] = 0;
  hololensData["tasksstr"] = tasksObjectStr;
  // suit data is for the websites
  suitData["inertialState"] = inertialState;
  suitData["suitTelemetry"] = suitTelemetry;
  suitData["warning"] = warning;
  suitData["glove"] = glove1data;
  // default values
  // inertial
  inertialState["accelx"] = 0;
  inertialState["accely"] = 0;
  inertialState["accelz"] = 0;
  inertialState["roll"] = 0;
  inertialState["pitch"] = 0;
  inertialState["yaw"] = 0;
  // telemetry
  suitTelemetry["createDate"] = "0";
  suitTelemetry["heart_bpm"] = 0;
  suitTelemetry["p_sub"] = 0.0;
  suitTelemetry["t_sub"] = 0;
  suitTelemetry["v_fan"] = 0;
  suitTelemetry["p_o2"] = 0;
  suitTelemetry["rate_02"] = 0.0;
  suitTelemetry["cap_battery"] = 0;
  suitTelemetry["p_h2o_g"] = 0;
  suitTelemetry["p_h2o_1"] = 0;
  suitTelemetry["p_sop"] = 0;
  suitTelemetry["rate_sop"] = 0;
  suitTelemetry["t_battery"] = "0";
  suitTelemetry["t_oxygen"] = "0";
  suitTelemetry["t_water"] = "0";

  if (debug_mode)
    DBG_OUTPUT_PORT.println("initialized global variables");

  if (website_on)
  {
    if (!SD.begin(pin_CS_SDcard))
    {
      if (debug_mode)
        DBG_OUTPUT_PORT.println("#SD initiatization failed. Retrying.");
      while (!SD.begin(pin_CS_SDcard))
      {
        delay(250);
      }
    }
    else
    {
      if (debug_mode)
        DBG_OUTPUT_PORT.println("#SD Initialized.");
    }
  }
  else
  {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#SD not initialized.");
  }

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
  // list directories
  if (website_on)
    server.on("/list", HTTP_GET, printDirectory);
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
          String hololensStr;
          hololensData.printTo(hololensStr);
          DBG_OUTPUT_PORT.println(hololensStr);
        }
        request->send(200, "text/plain", "true");
        } });
  }
  // add web sockets
  if (website_on && (websocket || debug_mode))
  {
    ws.onEvent(handleWs);
    server.addHandler(&ws);
  }
  // extra features for debugging
  if (website_on && debug_mode)
  {
    server.on("/edit", HTTP_DELETE, handleDelete);
    server.on("/edit", HTTP_PUT, handleCreate);
    server.on("/edit", HTTP_POST, returnOK, handleSDUpload);
  }
  // test1
  if (debug_mode)
  {
    server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request) {
      DBG_OUTPUT_PORT.println("#hello get request");
      request->send(200, "text/plain", "Hello World Get");
    });
  }

  // test2
  if (debug_mode)
  {
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String url = request->url();
      if (request->url() == "/hello")
      {
        if (!handleTest(request, data))
          request->send(200, "text/plain", "false");
        else
          request->send(200, "text/plain", "true");
      }
      return;
    });
  }
  server.on("/gettaskdata", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#tasks get request");
    // String data = "";
    // tasksArray.printTo(data);
    request->send(200, "application/json", tasksObjectStr);
  });
  server.on("/settaskdata", HTTP_PUT, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#set task request");
    request->send(200, "application/json", setTaskData(request, data)); });
  server.on("/getsuitdata", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#suit data get request");
    String data = "";
    suitData.printTo(data);
    request->send(200, "application/json", data);
  });
  server.on("/getstatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#sensor data get request");
    String data = "";
    jsonDataNodes.printTo(data);
    request->send(200, "application/json", data);
  });
  server.on("/gethololens", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#hololens data get request");
    String data = "";
    hololensData.printTo(data);
    request->send(200, "application/json", data);
  });
  //handle not found
  server.onNotFound(handleNotFound);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#finished creating server");
  server.begin();
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#HTTP server started");

  // Bluetooth
  if (bluetooth_on)
  {
    btSerial.begin(BT_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    if (bluetooth_device_config)
    {
      delay(INITIAL_BT_DELAY);
      StaticJsonBuffer<200> btjsonBuffer;
      JsonObject &baudRates = btjsonBuffer.createObject();
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
      if (baudRates.containsKey(NEW_BT_BAUD_RATE))
      {
        String theBaudRate = baudRates[NEW_BT_BAUD_RATE];
        runATCommand("AT+" + theBaudRate);
        if (debug_mode)
          DBG_OUTPUT_PORT.println("#change bt device baud rate to " + String(NEW_BT_BAUD_RATE));
      }
      else
      {
        if (debug_mode)
          DBG_OUTPUT_PORT.println("#bt baud rate " + String(NEW_BT_BAUD_RATE) + " not available");
      }
      if (debug_mode)
        DBG_OUTPUT_PORT.println("change bt device name to " + String(NEW_BT_NAME));
      runATCommand("AT+NAME" + String(NEW_BT_NAME));
      if (debug_mode)
        DBG_OUTPUT_PORT.println("#change bt device security code to " + String(NEW_BT_SECURITY_KEY));
      runATCommand("AT+PIN" + String(NEW_BT_SECURITY_KEY));
    }
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#The bluetooth device started, now you can pair it.");
  }
  else
  {
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#The bluetooth device was not enabled.");
  }

  // set built-in led to output
  pinMode(BUILTIN_LED, OUTPUT);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#The LED is now enabled.");

  // print initial stations (0)
  PrintStations();
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#printed initial stations.");
  // tasksObjectStr = "{}";
  // JsonObject &tasksObject = jsonBuffer.parseObject(tasksObjectStr);
  // refreshTaskArray(tasksObject);
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

  if (bluetooth_on)
  {
    // send data after interval
    if (bt_mode && !bt_requests)
    {
      if (millis() - bluetoothLastRefreshTime >= BLUETOOTH_REFRESH_INTERVAL)
      {
        bluetoothLastRefreshTime += BLUETOOTH_REFRESH_INTERVAL;
        sendDataBT();
      }
    }

    // Keep reading from bluetooth module and send to Arduino Serial Monitor
    String command;
    bool foundcommand = false;
    while (btSerial.available())
    {
      delay(20);                // delay to make it work well
      char c = btSerial.read(); // Conduct a serial read
      command += c;             // build the string.
      if (!foundcommand)
        foundcommand = true;
    }
    if (foundcommand)
    {
      char *commandchar = new char[command.length() + 1];
      strcpy(commandchar, command.c_str());
      removeChar(commandchar, '\n');
      removeChar(commandchar, '\r');
      //removeChar(commandchar, ' ');
      String commandstr = commandchar;
      delete commandchar;
      handleCommand(commandstr);
    }

    // Keep reading from Arduino Serial Monitor and send to HC-05
    if (DBG_OUTPUT_PORT.available())
      btSerial.write(DBG_OUTPUT_PORT.read());
  }

  if (millis() - lastBlink >= BLINK_INTERVAL)
  {
    lastBlink += BLINK_INTERVAL;
    blinkState = !blinkState;
    digitalWrite(LED_BUILTIN, blinkState);
  }
}