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
- after grid failure it connects automatically when the router is up.
## how does this work ##
When the ESP32 boots, it tries to connect to wifi. If this has failed, the bluetooth is initialized and an endless loop is entered.<br>
This is visible because the blue onboard led is on.<br>
The program returns (with a reboot) from this loop only when:
- the loop times out
- the bluetooth user terminates

The user can connect to the ESP32 bluetooth using an app on a smartphone, like 'Serial Bluetooth Terminal'. 
### Please note: you should edit the newlinesettings in the terminal (disable cr and ln). <br>

The device has the name ESP32-123456 (ESP32 + the chipID). When we type 'info', we get an overview of the possible commands and some system information. When we type 'connect' we are prompted for the ssid and password. When connected we can
see the IP address.

### pswd and security level ###
These values are set via the bluetooth connection so that only someone that has physical access to our ESP32 can edit them.<br>
The pswd is used to authenticate visitors of (parts of) the webinterface. It is also used for the login on the BT terminal.

I like to make my iot even more secure by preventing visiters from outside my network. We can filter this by comparing the visitor's IP to that of our router.<br>
The securityLevel decides how many characters must match, so if our router is 192.168.0.1 than 192.168.0 is a match when securityLevel = 9.<br>
coding example: <br>
```
// find out if the request comes from inside the network 
bool checkRemote(String url) { 
//check if client ip contains the first 'securityLevel' characters of the router f.i. 192.168.0.1 
    if(securityLevel == 0) return false; // disable filtering
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
} 
```
### is this save ? ###
I am still looking for a way to prevent unauthorised persons to connect to the BT. This should work with a pin but so far i could not get it to work, the examples of the librabry do not support this.
So as a workaround, the user can only issue commands when authenticated with the pswd.<br>
This has a drawback; if you forgot the admin passwd you have to recover it. So i made it possible to authenticate with the chipid also, this is displayed in the serial output at boot.<br>
If we write this down to our ESP we have it always at hand.<br>
So someone accidentely connected to our BT can't issue any commands and will be timed out after 5 minutes. I think this is secure enough.  
### manual start BT ###
Consider a link in your async webserver that makes a global defined integer 'actionflag' 12<br>
When we'd have this in the loop:<br>
```
if (actionFlag == 12) { 
   actionFlag=0;
   digitalWrite(led_onb, LED_ON); // the onboard led on
   SerialBT.begin(getChipId(false)); //Bluetooth device name
   Serial.printf("\nThe device \"%s\" started.\nYou can pair it with BT!\n", getChipId(false).c_str());
   connectionLoop();
}
```
the bluetooth is started manually.
## how to install it ##
I suggest to make a tab named BLUETOOTH in arduino IDE. Copy BLUETOOTH.ino there.
Other things to add:
```
#include <WiFi.h>
#include "BluetoothSerial.h"
#define LED_ON     HIGH   
#define LED_OFF    LOW
#define led_onb      2  // onboard led was 2

// blue tooth settings 
uint8_t securityLevel = 6; // to determine how many characters must match the IP and the routers IP
char pswd[11] = "0000";  // for the login on the webinterface
unsigned long previousMillis = 0;
bool btAuth = false;
//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
//const char *pin = "9999"; // Change this to more secure PIN.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif
BluetoothSerial SerialBT;

void setup() (
  Serial.begin(115200);
  delay(200);
  Serial.println("chipid = " + getChipId(true)); // note this as fallback for the pswd
  start_wifi(); // start wifi and webserver
  etc
```
The pswd and sucurityLevel part are instances of how you can use his to set extra values that are important to the security of your system. They can be skipped.   
