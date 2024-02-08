// ************************************************************************************
// *                        START wifi
// ************************************************************************************
void start_wifi() {
   int event = 0;
  // WiFi.softAPdisconnect(true);
   WiFi.mode(WIFI_STA);
   Serial.println("starting wifi ");
   delay(1000);
   WiFi.setHostname(getChipId(false).c_str()); 

   // 10 attempts to connect with the last used credentials
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print("*");
     WiFi.begin();
     event += 1;
     if (event==10) {break;}
  }
  // if connection failed we enter the connection process  
  if (event > 9) {
    Serial.println("\nWARNING connection failed");
    digitalWrite(led_onb, LED_ON); // the onboard led on
    SerialBT.begin(getChipId(false)); //Bluetooth device name
    Serial.printf("\nThe device %s started.\nYou can pair it with BT!", getChipId(false).c_str());
    // now we wait for the bt input
    connectionLoop(); // this always results in a reboot to test the connection and make the BT go away
      } else {
     Serial.print("\nconnection success, ip = ");
     SerialBT.disconnect(); // this is not really needed
     }
   Serial.println("# connection attempts = " + String(event));  
   event=0; // we continue to the rest
   // start_server(); // start the webserver
}

// ************************************************************************************
// *                wifi connection process
// ************************************************************************************
void connectionLoop() {
  Serial.println("entering the connection loop");
 // this is an infinitive loop that imes out after 5 minuts 
 // we stay in this loop until:
 //  - it times out. When that happens we reboot. This takes care for automatic 
 //    reconnection after grid or router failures
 //  - handleBT = true, also reboot
  previousMillis = millis(); // for the time-out
  while (millis() < previousMillis + 300*1000UL) { // 5 minutes = 300

    if(SerialBT.available()) {
        if(handleBT()) break; // handleBT is true on user confirm exit
    }
  } 
  // if we are here we are connected or the loop timed out
  // in both cases we reboot
  Serial.println("leaving the connectionLoop");
  SerialBT.println("\nrebooting");
  ESP.restart();
}

bool handleBT() {
      char sid[33] = ""; 
      char pas[64] = "";       
      int InByteCounter = 0;
      byte SerialInByte;
      char InputBuffer_BT[50];
      InputBuffer_BT[0] = '\0'; // make it like empty       
      delay(200); // wait untill maybe more data available
      Serial.println("serialBT data available: " + String(SerialBT.available()) );
  
      while(SerialBT.available()) {
            SerialInByte = SerialBT.read(); 
            
            if(isprint(SerialInByte)) {
              if(InByteCounter < 50) InputBuffer_BT[InByteCounter++] = SerialInByte;
            }    
            if(SerialInByte=='\n') {  // new line character                                            
              InputBuffer_BT[InByteCounter]=0;
              break; // serieel data is complete
            }
       }  
     Serial.println("InputBuffer_BT =" + String(InputBuffer_BT) );
     Serial.println("length InputBuffer_BT = " + String(strlen(InputBuffer_BT)) );
     
     // evaluate the incomming data

     if (strncasecmp(InputBuffer_BT,"INFO",4) == 0) {
         if( !btAuth ) {
              SerialBT.println(F("\n ** you have no permission! **"));
         return false; 
         scroll(4);
         SerialBT.println("*** AVAILABLE COMMANDS ***");
         SerialBT.println("CONNECT (connect to the wifi)");
         SerialBT.println("ADMINPW (set admin password)");  
         SerialBT.println("SECURITY (set security level)"); 
         SerialBT.println("WIPEWIFI (remove wifi settings)");
         SerialBT.println("EXIT  (reboot the device)");
         SerialBT.println("\n*** SOME SYSTEM INFO ***");
         SerialBT.print("IP address: "); SerialBT.println(WiFi.localIP());
         SerialBT.println("admin password : " + String(pswd)); //for safety we could comment this
         SerialBT.println("securityLevel : " + String(securityLevel));
         return false;
     } else 

        // **************   AUTHENTICATE   *****************************
     if (strncasecmp(InputBuffer_BT,"AUTH",4) == 0) {
        char tpwd[60];
        char chipid[10];
        getChipId(true).toCharArray(chipid, getChipId(true).length()+1);
        Serial.println( "\nchipid = " + String(chipid) );
        SerialBT.println(F("\nplease enter your admin password"));
        String temp = readSerial();
        temp.toCharArray(tpwd, temp.length()+1);
        if(strcmp(pswd, tpwd) == 0 || strcmp(chipid, tpwd) == 0) {
        SerialBT.println(F("\npasswd oke"));
        btAuth = true; 
        } else {
        SerialBT.println(F("\nwrong passwd")); 
        }
        
        return false;
     } else
        
        // **************   EXIT   *****************************
     if (strncasecmp(InputBuffer_BT,"EXIT",4) == 0) {
        SerialBT.println(F("\nare you sure to exit Y/N ?"));
        if( confirm_bt() ) return true;
        return false;
     } else

    // **************  remove wifi settings   *****************************
    if (strncasecmp(InputBuffer_BT,"WIPEWIFI", 8) == 0) {
        SerialBT.println(F("\nare you sure to wipe the wifi Y/N ?)"));
        if( confirm_bt() ) { 
            flush_wifi();
            SerialBT.println(F("\nwiped wifi settings)"));
         }
        return false;
    } else         

    // **************  connect to wifi   *****************************          
    if (strncasecmp(InputBuffer_BT,"CONNECT",7) == 0) {
      WiFi.mode(WIFI_OFF);
      SerialBT.println(F("\nplease enter your wifi name (ssid)"));
      String temp = readSerial();
      temp.toCharArray(sid, temp.length()+1);
      SerialBT.println(F("\nplease enter your wifi password"));
      temp = readSerial();
      Serial.println("readstring");
      temp.toCharArray(pas, temp.length()+1);

      SerialBT.println(F("\nconnect with these credentials Y/N ?"));
      if( confirm_bt() ) {
           WiFi.mode(WIFI_STA);
           SerialBT.println("trying to connect ");
           WiFi.begin(sid, pas);// Start Wifi with new values.
           if (connect2Wifi() == WL_CONNECTED) {
               SerialBT.println(F("\nyoupyyyyy, connected!"));
               digitalWrite(led_onb, LED_OFF);
               ledblink(3,500);
               SerialBT.println(F("do you want to exit Y/N ?"));
               if( confirm_bt() ) return true;
               return false;
            } else {
               SerialBT.println(F("could not connect, try again"));
               digitalWrite(led_onb, LED_ON); // 
               return false;
            }
        } 
        return false;
    } else

     // **************   edit security level  *****************************
     //char pswd[11] = "0000"; globally defined
    if (strncasecmp(InputBuffer_BT,"SECURITY",8) == 0) {
       Serial.println("found SECURITY");
       SerialBT.println("\nplease enter a value 0-9");
       while(SerialBT.available() == 0) { } // wait for input
       securityLevel = SerialBT.parseInt();
       if(securityLevel < 0 || securityLevel > 9) {
         SerialBT.println("\n invalid value 0-9");
       } else {
         SerialBT.println("\nsaving securityLevel");
         // wifiConfigsave(); // define your own save to SPIFFS function
         SerialBT.println("do you want to exit Y/N ?");
         if( confirm_bt() ) return true; // return from handle_BT              
       }
       return false;
    } else

     // **************   edit admin password   *****************************
     if (strncasecmp(InputBuffer_BT,"ADMINPW",7) == 0) {
         Serial.println("found ADMINPW");
         SerialBT.println(F("\please enter your admin password"));
         String temp = readSerial();
         // save the passwd
         SerialBT.printf("\npasswd = \"%s\", save it Y/N ?" , String(pswd));
         if( confirm_bt() ) {
            temp.toCharArray(pswd, temp.length()+1);
            SerialBT.println("\nsaving the password");
            // wifiConfigsave(); // define your save to SPIFFS function
         } 
         return false;
      }
           
      // if we are here, there was input but not recognized
      SerialBT.println(F(" INVALID COMMAND , TYPE INFO or AUTH" ));
      // the buffercontent is not making sense so we empty the buffer
      while(SerialBT.available()) { SerialBT.read(); }     
      return false; // return to the connectionLoop
}

String readSerial() {
    while(SerialBT.available() == 0) { } 
    String rec = SerialBT.readString();
    SerialBT.println("\nyou entered " + rec);
    Serial.println("\nentered " + rec + " len=" + String(rec.length()) );
    return rec;
}
// ********************  user input Y or N ******************************
bool confirm_bt() {
     if( !btAuth ) {
     SerialBT.println(F("\n ** you have no permission! **"));
     return false; 
     }
     while(SerialBT.available() == 0) { }
          int tmp = SerialBT.read();
          Serial.println("serialBT.read = " + String(tmp));
          if(tmp == 'Y' || tmp == 'y') {
              return true;
          } else {
              SerialBT.println(F("\nabort"));
              return false;
          }
}

int connect2Wifi() {  
    Serial.println("try connect with the new credentials");
    
    int connRes = WiFi.waitForConnectResult();
    SerialBT.println("Connection result is : " + String(connRes));
    SerialBT.println(WiFi.localIP());
    checkFixed(); // set static ip if configured
  
    return connRes;
}

void flush_wifi() {
     if( diagNose != 0 ) consoleOut("erasing the wifi credentials");
     WiFi.disconnect();
     if( diagNose != 0 ) consoleOut(F("wifi disconnected"));
     //now we try to overwrite the wifi creentials     
     const char* wfn = "dummy";
     const char* pw = "dummy";
     WiFi.begin(wfn, pw);
     Serial.println(F("\nConnecting to dummy network"));
     int teller = 0;
     while(WiFi.status() != WL_CONNECTED){
        Serial.print(F("wipe wifi credentials\n"));
        delay(100);         
        teller ++;
        if (teller > 2) break;
    }
    // if we are here, the wifi should be wiped 
}

String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-" + String(chipId);
}
// flash the led i times, tempo is determined by wait (  
void ledblink(int i, int wait) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_ON);
    delay(wait);
    digitalWrite(led_onb, LED_OFF);
    delay(wait);
   }
}
