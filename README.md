# ESP32-WIFIMANAGER-VIA-BLUETOOTH
simple way to connect your iot project to your wifi

I find it important to get feedback of the IP address when connected to wifi. The existing wifimanagers don't do that<br>
so i changed wifimanager in such way that it does. Although this works mostly it is not completely reliable. Especially the feedback<br>
of the IP address needs wifimode changes which can cause disconnections from the user during the connection process.<br>
Moreover the whole captive portal software and webpages take a lot of programming space. 

So connecting via bluetooth is a nice alternative which has the following advantages:
- less programspace needed
- direct feedback of the IP addres
- can be used to set other system variables like an administrator password
## how does this work ##
When the ESP32 boots, it tries to connect to wifi. When this failes, the bluetooth is initialized and an endless loop is entered.
The program returns from this loop only when:
- the loop times out
- the bluetooth user terminates

The tuser can connect to the ESP32 using an app on a smartphone, like 
