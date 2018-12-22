# Wifi Master

master code for Arduino communicating with Hololens via Bluetooth.

See [this](http://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables), [this](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html), and [this](https://desire.giesecke.tk/index.php/2018/01/30/change-partition-size/) for partition changing for bluetooth.

Now I have another problem! There's a setting for enabling bluetooth and wifi at the same time, which doesn't exist for platformio. Thankfully others have had this problem, see [here](https://community.platformio.org/t/menuconfig-options-for-esp32-arduino/3871). I'm going to try [this](https://hackaday.io/project/43374-esp32-idf-deploying-the-development-platform/details) solution and see how it goes, tomorrow. But right now if both bluetooth and wifi are on it doesn't work...