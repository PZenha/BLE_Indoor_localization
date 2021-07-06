#ifndef XhockWiFiManager_h
#define XhockWiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//HTML CODE
extern const char HTTP_HEAD[] PROGMEM;
extern const char HTTP_SCRIPT[] PROGMEM; 
extern const char HTTP_FORM[] PROGMEM; 
extern const char HTTP_LIST[] PROGMEM; 
extern const char HTTP_END[] PROGMEM; 
extern const char WIFI_LIST[] PROGMEM; 

//Control flags
extern boolean flag_ifWifiConnected,flag_IP,New_Input;

extern ESP8266WebServer server;

//New SSID from WebServer
extern String New_ssid;
extern String New_pass;

//Server on Port 80
//extern ESP8266WebServer server(80);

extern int signalQuality[200], l;
extern unsigned long time_off, time_off_last;

//NON EEPROM SSID and PASSW
extern const char* ssid;//"MEO-5559F1"  "MEO-06D107"    "iPhone de Pedro"  "XHOCKWARE"
extern const char* pass; //     "546A780F24"  "3F11BB50DB"	  "123zenha456"		 "xhock4ever"

//ESP SSID
extern String APname;
extern char *APssid;

//ESP PASSW
extern char APpass[];

//SSID and PASSW stored at EEPROM
extern String eSSID,ePASS;


//Functions

void Wifi_Manager();

void handleRoot();

int getRSSIasQuality(int RSSI);

#endif