# ESP32-WIFIMANAGER-VIA-BLUETOOTH
A simple way to connect your ESP32 iot project to your wifi.

I find it handy to get feedback of the IP address when connected to wifi. The existing wifimanagers didn't provide that at the time
so i changed wifimanager in such way that it does. Although this works most of the time, it is not completely reliable. Especially the feedback<br>
of the IP address needs wifimode changes which can cause disconnections from the user during the connection process.<br>
Moreover the whole captive portal software and webpages take a lot of programming space. 

So connecting via bluetooth is a nice alternative which has the following advantages:
- little programspace needed
- direct feedback of the IP addres
- can be used to set other system variables like an administrator password
- The bluetooth doesn't exist in normal operation.
## how does this work ##
When the ESP32 boots, it tries to connect to wifi. When this failes, the bluetooth is initialized and an endless loop is entered.
The program returns from this loop only when:
- the loop times out
- the bluetooth user terminates

The user can connect to the ESP32 using an app on a smartphone, like 'Serial Bluetooth Terminal'. The device has the name ESP32-1234567 (ESP32 + the chipID). When we type 'info', we get an overview
of the possible commands and some system information. When we type 'connect' we are prompted for the ssid and password. When connected we can
see the IP address.

### is this save ###
My understanding is that only one master can be connected at a time to the ESP32.  So when you connect a.s.p. after the bluetooth becomes active, chances that your neighbour connects first are negligibly small. And if this were to happen unexpectedly, he could not do much harm. So yes i consider this to be save.

### manual start BT ###
Consider a link in your async webserver that makes a global defined integer 'actionflag' 12<br>
when we have this in the loop:<br>
    if (actionFlag == 12) { <br>
     actionFlag=0;<br>
     digitalWrite(led_onb, LED_AAN); // the onboard led on<br>
     SerialBT.begin(getChipId(false)); //Bluetooth device name<br>
     Serial.printf("\nThe device \"%s\" started.\nYou can pair it with BT!\n", getChipId(false).c_str());<br>
     connectionLoop();<br>
    }<br>
the bluetooth is started manually.
## how to install it ##
I suggest to make a tab named BLUETOOTH in arduino IDE. Copy BLUETOOTH.ino there.
Other things to add:
#include <WiFi.h><br>
#include "BluetoothSerial.h"<br>
// blue tooth settings <br>
uint8_t securityLevel = 6; // to determine how many characters must match the IP and the routers IP<br>
char pswd[11] = "0000";  // for the login on the webinterface<br>
unsigned long previousMillis = 0;<br>
//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below<br>
//const char *pin = "9999"; // Change this to more secure PIN.<br>
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)<br>
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it<br>
#endif<br>
#if !defined(CONFIG_BT_SPP_ENABLED)<br>
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.<br>
#endif<br>
BluetoothSerial SerialBT;<br>

void setup() (<br>
  Serial.begin(115200);<br>
  start_wifi(); // start wifi and webserver<br>

The pswd and sucurityLevel part are instances of how you can use his to set extra values that are important to the security of your system. They can be skipped.   
