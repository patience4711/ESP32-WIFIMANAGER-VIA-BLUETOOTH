# ESP32-WIFIMANAGER-VIA-BLUETOOTH
A simple way to connect your ESP32 iot project to your wifi.

I find it handy to get feedback of the IP address when connected to wifi. The existing wifimanagers didn't do that at the time<br>
so i changed wifimanager in such way that it does. Although this works most of the time, it is not completely reliable. Especially the feedback<br>
of the IP address needs wifimode changes which can cause disconnections from the user during the connection process.<br>
Moreover the whole captive portal software and webpages take a lot of programming space. 

So connecting via bluetooth is a nice alternative which has the following advantages:
- less programspace needed
- direct feedback of the IP addres
- can be used to set other system variables like an administrator password
- The bluetooth doesn't exist in normal operation.
## how does this work ##
When the ESP32 boots, it tries to connect to wifi. When this failes, the bluetooth is initialized and an endless loop is entered.
The program returns from this loop only when:
- the loop times out
- the bluetooth user terminates

The user can connect to the ESP32 using an app on a smartphone, like 'Serial Bluetooth Terminal'. The device has the name ESP32-1234567 (ESP32 + the chipID). When we type 'info', we get an overview<br>
of the possible commands and some system information. When we type 'connect' we are prompted for the ssid and password. When connected we can<br>
see the IP address.

## how to install it ##
I suggest to make a tab named BLUETOOTH in arduino IDE. Copy BLUETOOTH.ino there.
Other things to add:
#include <WiFi.h>
#include "BluetoothSerial.h"
// blue tooth settings 
uint8_t securityLevel = 6; // to determine how many characters must match the IP and the routers IP
char pswd[11] = "0000";  // for the login on the webinterface
unsigned long previousMillis = 0;
#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "9999"; // Change this to more secure PIN.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif
BluetoothSerial SerialBT;

void setup() (
  Serial.begin(115200);
  start_wifi(); // start wifi and webserver
