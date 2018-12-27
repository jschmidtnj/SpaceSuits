# Configure Bluetooth Module (HC-05)

Finally got this working with AT commands. Originally tried to run commands on ESP32 board, but SoftwareSerial libraries did not work. Switched to Arduino UNO board and got good results. Followed information in [this](http://www.techbitar.com/modify-the-hc-05-bluetooth-module-defaults-using-at-commands.html) webpage for AT commands and changing the information of the device.  
  
Currently there are issues with changing the baud rate and passkey of the device, which might be due to wrong AT commands. This can be fixed by changing commented out section in the `config_bt/` diretory. `test_bt/` allows for connection and testing using bluetooth terminal on Android device (see [this](https://play.google.com/store/search?q=bluetooth+terminal&c=apps) - most of them work).  

I just tried doing this automatically during each run of the wifi_master program, but it does not want to work. The program will not upload if the hc-05 device is being powered on and off. This configuration sketch is therefore necessary.