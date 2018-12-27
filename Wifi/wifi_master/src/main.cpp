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
//#include "SoftwareSerial.h" // optional softwareserial library
//#include "EDB.h" // Extended database library - see https://github.com/jwhiddon/EDB?utm_source=platformio&utm_medium=piohome

#define DBG_OUTPUT_PORT Serial
#define debug_mode true
#define bt_mode false // false = send data after every requst, true = send data in intervals
#define redirect_all_to_host true
#define DBG_BAUD_RATE 115200
#define BT_BAUD_RATE 9600
#define RX_PIN 13 // rx pin on bluetooth module connected to this pin on Arduino
#define TX_PIN 12 // tx pin on bluetooth module connected to this pin on Arduino
#define ttl 300 // ttl for dns
#define channel_num 1 // channel number for softAP
#define max_connection 10 // max connections to AP
const char *ssid = "SPACESUITWIFI";
const char *password = "N@sASu!t";
const char *host = "spacesuit.local";
IPAddress Ip(192, 168, 1, 1); // ip address for website
const int pin_CS_SDcard = 15;
AsyncWebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;
static const String devices[] = {"glove1", "glove2", "imu"};

static const unsigned long WIFI_REFRESH_INTERVAL = 10000; // ms
static unsigned long wifiLastRefreshTime = 0;
static const unsigned long BLUETOOTH_REFRESH_INTERVAL = 1000; //ms
static unsigned long bluetoothLastRefreshTime = 0;

static const unsigned long BLINK_INTERVAL = 1000; //ms
static unsigned long lastBlink = 0;
bool blinkState = false; // false = off

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// tx pin on bluetooth module connected to rx pin on Arduino, and vice-versa
//SoftwareSerial btSerial(RX_PIN, TX_PIN, false, 256);
//not using softwareserial anymore because it had errors with data receive.
HardwareSerial btSerial(1);

// JSON data - does not work with async
//DynamicJsonBuffer jsonDataBuffer;
//JsonObject& jsonData = jsonDataBuffer.createObject();

void returnOK(AsyncWebServerRequest *request) {request->send(200, "text/plain", "");}

void returnFail(AsyncWebServerRequest *request, String msg) {request->send(500, "text/plain", msg + "\r\n");}

bool loadFromSdCard(AsyncWebServerRequest *request) {
  // making this async does not work well yet
  String path = request->url();
  String dataType = "text/plain";
  struct fileBlk {
    File dataFile;
  };
  fileBlk *fileObj = new fileBlk;

  if (path.endsWith("/")) path += "index.htm";
  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".json")) dataType = "text/json";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".svg")) dataType = "image/svg+xml";
  else if (path.endsWith(".eot")) dataType = "font/eot";
  else if (path.endsWith(".woff")) dataType = "font/woff";
  else if (path.endsWith(".woff2")) dataType = "font/woff2";
  else if (path.endsWith(".ttf")) dataType = "font/ttf";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  else if (path.endsWith(".gz")) dataType = "application/x-gzip";
  else dataType = "text/plain";
  
  fileObj->dataFile  = SD.open(path.c_str());
  if(fileObj->dataFile.isDirectory()){
    path += "/index.htm";
    dataType = "text/html";
    fileObj->dataFile = SD.open(path.c_str());
  }

  if (!fileObj->dataFile){
    delete fileObj;
    return false;
  }

  if (request->hasParam("download")) dataType = "application/octet-stream";

  // Here is the context problem.  If there are multiple downloads active, 
  // we don't have the File handles. So we only allow one active download request
  // at a time and keep the file handle in static.  I'm open to a solution.

  request->_tempObject = (void*)fileObj;
  request->send(dataType, fileObj->dataFile.size(), [request](uint8_t *buffer, size_t maxlen, size_t index) -> size_t {
    fileBlk *fileObj = (fileBlk*)request->_tempObject;
    size_t thisSize = fileObj->dataFile.read(buffer, maxlen);
    if((index + thisSize) >= fileObj->dataFile.size()){
      fileObj->dataFile.close();
      request->_tempObject = NULL;
      delete fileObj;
    }
    return thisSize;
  });
  return true;
}

void handleSDUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  struct uploadRequest {
    uploadRequest* next;
    AsyncWebServerRequest *request;
    File uploadFile;
    uint32_t fileSize;
    uploadRequest(){next = NULL; request = NULL; fileSize = 0;}
  };
  static uploadRequest uploadRequestHead;
  uploadRequest* thisUploadRequest = NULL;

  if (!index){
    if(SD.exists((char *)filename.c_str())) SD.remove((char *)filename.c_str());
    thisUploadRequest = new uploadRequest;
    thisUploadRequest->request = request;
    thisUploadRequest->next = uploadRequestHead.next;
    uploadRequestHead.next = thisUploadRequest;
    thisUploadRequest->uploadFile = SD.open(filename.c_str(), FILE_WRITE);
    if (debug_mode) {
      DBG_OUTPUT_PORT.print("Upload: START, filename: ");
      DBG_OUTPUT_PORT.println(filename);
    }
    
  }
  else{
    thisUploadRequest = uploadRequestHead.next;
    while(thisUploadRequest->request != request) thisUploadRequest = thisUploadRequest->next;
  }
  
  if(thisUploadRequest->uploadFile){
    for(size_t i=0; i<len; i++){
      thisUploadRequest->uploadFile.write(data[i]);
    }
    thisUploadRequest->fileSize += len;
  }
  
  if(final){
    thisUploadRequest->uploadFile.close();
    if (debug_mode) {
      DBG_OUTPUT_PORT.print("Upload: END, Size: ");
      DBG_OUTPUT_PORT.println(thisUploadRequest->fileSize);
    }
    
    uploadRequest* linkUploadRequest = &uploadRequestHead;
    while(linkUploadRequest->next != thisUploadRequest) linkUploadRequest = linkUploadRequest->next;
    linkUploadRequest->next = thisUploadRequest->next;
    delete thisUploadRequest;
  }
}

void deleteRecursive(String path){
  File file = SD.open((char *)path.c_str());
  if(!file.isDirectory()){
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }
  file.rewindDirectory();
  File entry;
  while(entry = file.openNextFile()) {
    String entryPath = path + "/" +entry.name();
    if(entry.isDirectory()){
      entry.close();
      deleteRecursive(entryPath);
    } else {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
  }
  SD.rmdir((char *)path.c_str());
  file.close();
}

void handleDelete(AsyncWebServerRequest *request){
  if( ! request->params()) returnFail(request, "No Path");
  String path = request->arg("path");
  if(path == "/" || !SD.exists((char *)path.c_str())) {
    returnFail(request, "Bad Path");
  }
  deleteRecursive(path);
  returnOK(request);
}

void handleCreate(AsyncWebServerRequest *request){
  if(! request->params()) returnFail(request, "No Path");
  String path = request->arg("path");
  if(path == "/" || SD.exists((char *)path.c_str())) {
    returnFail(request, "Bad Path");
    return;
  }

  if(path.indexOf('.') > 0){
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if(file){
      file.write(0);
      file.close();
    }
  } else {
    SD.mkdir((char *)path.c_str());
  }
  returnOK(request);
}

void printDirectory(AsyncWebServerRequest *request) {
  if( ! request->hasParam("dir")) return returnFail(request, "BAD ARGS");
  String path = request->arg("dir");
  if(path != "/" && !SD.exists((char *)path.c_str())) return returnFail(request, "BAD PATH");
  File dir = SD.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory()){
    dir.close();
    return returnFail(request, "NOT DIR");
  }
  DynamicJsonBuffer jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();
  dir.rewindDirectory();
  File entry;
  while(entry = dir.openNextFile()){
    JsonObject& object = jsonBuffer.createObject();
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

void handleNotFound(AsyncWebServerRequest *request){
  String path = request->url();
  if(path.endsWith("/")) path += "index.htm";
  for (String device: devices) {
    if (path.endsWith(device)) return;
  }
  if (path.endsWith("hello")) return;
  if (loadFromSdCard(request)){
    return;
  }
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

void PrintStations() {
  wifi_sta_list_t stationList;
 
  esp_wifi_ap_get_sta_list(&stationList);

  char headerChar[50];
  char buffer [5];
  String stationNumStr = itoa(stationList.num, buffer, 10);
  String headerStr = "Num Connect: " + stationNumStr;
  headerStr.toCharArray(headerChar, 50);
  if (debug_mode)
    DBG_OUTPUT_PORT.println(headerStr);

  u8x8.drawString(0, 0, headerChar);
 
  for(int i = 0; i < stationList.num; i++) {
 
    wifi_sta_info_t station = stationList.sta[i];

    String mac = "";
 
    for(int j = 0; j< 6; j++){
      mac += (String)station.mac[j];
      if(j<5) {
        mac += ":";
      }
    }
    if (debug_mode)
      DBG_OUTPUT_PORT.println(mac);
    char macChar[25];
    mac.substring(1, 16).toCharArray(macChar, 25);
    u8x8.drawString(0, i + 1, macChar);
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println("-----------------");
}

bool handleTest(AsyncWebServerRequest *request, uint8_t *datas) {

  if (debug_mode)
    DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char*)datas);
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& _test = jsonBuffer.parseObject((const char*)datas); 
  if (!_test.success()) return 0;

  if (!_test.containsKey("command")) return 0;
  String _command = _test["command"];
  if (debug_mode) {
    DBG_OUTPUT_PORT.println(_command);
    DBG_OUTPUT_PORT.println("hello post request");
  }
  request->send(200, "text/plain", "Hello World Post");
  
  return 1;
}

/*
void SendData(void) {
  String data = "";
  jsonData.printTo(data);
  btSerial.println(data);
}
*/

bool handleDataPut(AsyncWebServerRequest *request, uint8_t *datas) {
  if (debug_mode)
    DBG_OUTPUT_PORT.printf("[REQUEST]\t%s\r\n", (const char*)datas);
  int paramsNr = request->params();
  if (debug_mode) {
    DBG_OUTPUT_PORT.print(paramsNr);
    DBG_OUTPUT_PORT.println(" queries");
  }
  String dataStr;
  for(int i=0;i < paramsNr;i++){
    AsyncWebParameter* p = request->getParam(i);
    String name = p->name();
    if (debug_mode) {
      DBG_OUTPUT_PORT.print("Param name: ");
      DBG_OUTPUT_PORT.println(name);
    }
    String val = p->value();
    if (debug_mode) {
      DBG_OUTPUT_PORT.print("Param value: ");
      DBG_OUTPUT_PORT.println(val);
    }
    if (name == "data")
      dataStr = val;
    if (debug_mode)
      DBG_OUTPUT_PORT.println("------");
  }
  const char* querychar = dataStr.c_str();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((const char*)datas); 
  if (!data.success()) return 0;

  if (!data.containsKey("id")) return 0;
  String id = data["id"];
  if (debug_mode) {
    DBG_OUTPUT_PORT.println("data from " + id);
  }
  //JsonObject& nested = jsonBuffer.parseObject((const char*)datas);
  //jsonData[id] = nested;
  // send data through bluetooth immediately
  //if (!bt_mode)
  //  SendData();
  btSerial.println((const char*)datas);
  return 1;
}

void handleCommand(String command) {
  if (debug_mode)
    DBG_OUTPUT_PORT.println("received command: " + command);
}

void removeChar(char *s, int c){
  int j, n = strlen(s); 
  for (int i=j=0; i<n; i++) 
      if (s[i] != c) 
        s[j++] = s[i];
  s[j] = '\0';
} 

void setup(void) {

  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  DBG_OUTPUT_PORT.setDebugOutput(debug_mode);

  if (! SD.begin(pin_CS_SDcard)){
    if (debug_mode)
      DBG_OUTPUT_PORT.println("#SD initiatization failed. Retrying.");
    while(!SD.begin(pin_CS_SDcard)){
      delay(250); 
    } 
  }
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#SD Initialized.");

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

  if (debug_mode) {
    DBG_OUTPUT_PORT.print("#IP address: ");
    DBG_OUTPUT_PORT.println(IP);
  }

  if (redirect_all_to_host) {
    //dnsServer.start(DNS_PORT, "*", IP);
    dnsServer.setTTL(ttl);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNS_PORT, host, IP);
  }

  server.on("/list", HTTP_GET, printDirectory);
  for (String device: devices) {
    String path = "/" + device;
    const char *pathChar = path.c_str();
    server.on(pathChar, HTTP_PUT, [](AsyncWebServerRequest *request){
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      if (!handleDataPut(request, data))
          request->send(200, "text/plain", "false");
        else
          request->send(200, "text/plain", "true");
    });
  }
  // extra features for debugging
  if (debug_mode) {
    server.on("/edit", HTTP_DELETE, handleDelete);
    server.on("/edit", HTTP_PUT, handleCreate);
    server.on("/edit", HTTP_POST, returnOK, handleSDUpload);
  }
  // test1
  if (debug_mode) {
    server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
      DBG_OUTPUT_PORT.println("#hello get request");
      request->send(200, "text/plain", "Hello World Get");
    });
  }
  // test2
  if (debug_mode) {
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      String url = request->url();
      if (request->url() == "/hello") {
        if (!handleTest(request, data))
          request->send(200, "text/plain", "false");
        else
          request->send(200, "text/plain", "true");
      }
      return;
    });
  }
  server.onNotFound(handleNotFound);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#finished creating server");
  server.begin();
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#HTTP server started");

  // Bluetooth
  btSerial.begin(BT_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#The bluetooth device started, now you can pair it.");

  // set built-in led to output
  pinMode(BUILTIN_LED, OUTPUT);
  if (debug_mode)
    DBG_OUTPUT_PORT.println("#The LED is now enabled.");
}

void loop(void) {

  if (redirect_all_to_host)
    dnsServer.processNextRequest();
	
	if(millis() - wifiLastRefreshTime >= WIFI_REFRESH_INTERVAL) {
		wifiLastRefreshTime += WIFI_REFRESH_INTERVAL;
    PrintStations();
	}

  // send data after interval
  if (bt_mode) {
    if(millis() - bluetoothLastRefreshTime >= BLUETOOTH_REFRESH_INTERVAL) {
      bluetoothLastRefreshTime += BLUETOOTH_REFRESH_INTERVAL;
      //SendData();
    }
  }

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  String command;
  bool foundcommand = false;
  while (btSerial.available()) {
    delay(10); // delay to make it work well
    char c = btSerial.read(); //Conduct a serial read
    command += c; //build the string.
    if (!foundcommand)
      foundcommand = true;
  }
  if (foundcommand) {
    char *commandchar = new char[command.length() + 1];
    strcpy(commandchar, command.c_str());
    removeChar(commandchar, '\n');
    removeChar(commandchar, '\r');
    String commandstr = commandchar;
    delete commandchar;
    handleCommand(commandstr);
  }

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (DBG_OUTPUT_PORT.available())
    btSerial.write(DBG_OUTPUT_PORT.read());
  
  if(millis() - lastBlink >= BLINK_INTERVAL) {
		lastBlink += BLINK_INTERVAL;
    blinkState = !blinkState;
    digitalWrite(LED_BUILTIN, blinkState);
	}
}