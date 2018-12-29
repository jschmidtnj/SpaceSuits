# Wifi Master

try this for hc-06 config: [here](https://mcuoneclipse.com/2013/06/19/using-the-hc-06-bluetooth-module/).  

potentially use both put requests and web sockets. put requests for the fast data and web sockets for the 2-way communication.  

Master code for Arduino communicating with Hololens via Bluetooth. Deployed on ESP32: [this](http://a.co/d/21BAwOl) exact board. Utilized led and oled screen included on the board. SD card breakout board used for getting static files (asyncronously). HC-05 board used because platformio does not allow for setting enabling concurrent wifi and bluetooth connections on ESP-32 board. Software serial avoided because it caused too many errors when sending data from Hololens or other connected device to the board. Hardware Serial used instead.

See [this](http://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables), [this](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html), and [this](https://desire.giesecke.tk/index.php/2018/01/30/change-partition-size/) for partition changing for bluetooth.  

See [this](https://github.com/esp8266/Arduino/issues/570) for increasing max connections to access point. Ended up changing config settings and it worked anyway (up to 10).

## Trying ESP32-IDF Compiler

### Reasoning

Now I have another problem! There's a setting for enabling bluetooth and wifi at the same time, which doesn't exist for platformio. Thankfully others have had this problem, see [here](https://community.platformio.org/t/menuconfig-options-for-esp32-arduino/3871). I'm going to try [this](https://hackaday.io/project/43374-esp32-idf-deploying-the-development-platform/details) solution and see how it goes, tomorrow. But right now if both bluetooth and wifi are on it doesn't work...

### What I learned from trying this

TLDR: Didn't work. Use PlatformIO if you have a bunch of dependencies and don't try bluetooth and wifi at the same time.  

Follow the steps below to recreate the configuration I had. I was trying to use ESP32 IDF compiler so that I could use the menuconfig. Unfortunately using ESP32-IDF is a PAIN and not worth it. Using another bluetooth module indead. The whole point was to get make menuconfig working so that I could toggle the Software Wifi/BT Coexistence option so that wifi and bluetooth could happen at the same time. But this method just doesn't work well. There were too many dependency issues, and integrating with PlatformIO didn't work, and in the end I was left where I started with nothing working correctly.  

Menuconfig options for ESP32 Arduino - General Discussion - PlatformIO Community  
https://community.platformio.org/t/menuconfig-options-for-esp32-arduino/3871/2  

Wi-Fi/BLE Contention : esp32  
https://www.reddit.com/r/esp32/comments/9bq22e/wifible_contention/  

esp32-devenv-vscode/tutorial.md at master · VirgiliaBeatrice/esp32-devenv-vscode  
https://github.com/VirgiliaBeatrice/esp32-devenv-vscode/blob/master/tutorial.md  

ESP32-IDF - Deploying the development platform | Details | Hackaday.io  
https://hackaday.io/project/43374-esp32-idf-deploying-the-development-platform/details  

Remove these directories from the Arduino Library  
* WiFiClientSecure
* HTTPClient
* HTTPUpdate
* ESP32/examples/Camera

`git submodule update --init --recursive`  
`git clone --single-branch --branch idf-update https://github.com/me-no-dev/AsyncTCP`  
`git clone https://github.com/me-no-dev/ESPAsyncWebServer`  
`git clone https://github.com/bblanchon/ArduinoJson`  
`git clone https://github.com/olikraus/u8g2`  
`git clone https://github.com/espressif/arduino-esp32`  