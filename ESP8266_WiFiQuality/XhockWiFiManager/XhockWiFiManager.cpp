#include "arduino.h"
#include "XhockWiFiManager.h"

//HTML CODE
const char HTTP_HEAD[] PROGMEM = {"<!DOCTYPE html>\n<html>\n<body>\n<h2>{v}</h2>\n<p> Select the network you wish to connect: </p>\n"};
const char HTTP_SCRIPT[] PROGMEM = {"<script>\nfunction Myfunction(l){\n\tdocument.getElementById(\"s\").value = l.innerHTML;\n   }\n</script>"};
const char HTTP_FORM[] PROGMEM = {"<form method=\"get\" action=\"/\">\n  SSID:<br>\n  <input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\">\n  <br>\n  Password:<br>\n  <input id=\"p\" name=\"p\" type=\"password\" placeholder=\"Password\">\n  <br><br>\n  <button type=\"submit\">Submit</button>\n</form> "};
const char HTTP_LIST[] PROGMEM = {"<p>List of WiFi networks: </p>"};
const char HTTP_END[] PROGMEM = {"</body>\n</html>\n"};
const char WIFI_LIST[] PROGMEM = {"<span onclick=\"Myfunction(this)\">{v}</span> <span> - {r}%</span> <br>"};

//Control flags
boolean flag_ifWifiConnected = false,New_Input = false,flag_IP = false;

//New SSID from Webserver
String New_ssid = "";
String New_pass = "";

//server on Port 80
ESP8266WebServer server(80);

int signalQuality[200], l=0;
unsigned long time_off = 0, time_off_last = 0;

//NON EEPROM SSID and PASSW
const char* ssid = "XHOCKWAR"; //"MEO-5559F1"  "MEO-06D107"    "iPhone de Pedro"  "XHOCKWARE"
const char* pass ="xhock4ever"; //     "546A780F24"  "3F11BB50DB"	  "123zenha456"		 "xhock4ever"

//ESP SSID
String APname = "ESP8266 " + String(WiFi.macAddress());
char *APssid =&APname[0u];

//ESP PASSW
char APpass[]="xhock4ever";

//SSID and PASSW stored at EEPROM
String eSSID,ePASS;

void Wifi_Manager(){

	server.handleClient();  //Recebe os pedidos HTTP do cliente
	//Wifi manager code
	if(l == 0 && flag_ifWifiConnected == false){
		Serial.println("");
		Serial.println("Scan started");
		
		
		int n = WiFi.scanNetworks();	// WiFi.scanNetworks will return the number of networks found
		Serial.println("Scan completed");
		
		if (n == 0) {
			Serial.println("no networks found");
			} else {
			Serial.print(n);
			Serial.println(" networks found");
			
			for (int i = 0; i < n; ++i) {
				signalQuality[i] = getRSSIasQuality(WiFi.RSSI(i));
				Serial.print(i + 1);
				Serial.print(": ");
				Serial.print(WiFi.SSID(i));
				Serial.print(" (");
				Serial.print(signalQuality[i]);
				Serial.print(")");
				Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
				l=1;
			}
		}
	}
	if(New_Input == true){
		int  i=0;
		char *SNew_ssid = &New_ssid[0u];
		char *SNew_pass = &New_pass[0u];
		Serial.print("Trying to connect to: ");
		Serial.println(New_ssid);
		WiFi.mode(WIFI_AP_STA);    //AP e station
		WiFi.begin(SNew_ssid,SNew_pass);   //Conectar a nova rede
		while(WiFi.status() != WL_CONNECTED && i<100){
			delay(200);
			Serial.print(".");
			i++;
		}
		if(WiFi.status() == WL_CONNECTED){  //Ligado a nova rede
			Serial.println("");
			//cleaning EEPROM
			if(New_ssid.length() > 0 && New_pass.length() > 0){
				Serial.println("Cleaning EEPROM...");
				for(i = 0;i < 96;i++){
					EEPROM.write(i,0);
				}
			}
			//Writing to EEPROM
			Serial.println("Writing SSID to EEPROM...");
			for(i = 0;i < New_ssid.length(); i++){
				EEPROM.write(i,New_ssid[i]);

			}
			Serial.println("Writing Password to EEPROM...");
			for(i = 0;i < New_pass.length(); i++){
				EEPROM.write(32+i,New_pass[i]);
			}
			EEPROM.commit();
			
			New_Input = false;
			Serial.println("");
			Serial.print("Successfully connected to: ");
			Serial.println(New_ssid);
			Serial.println("Shutting down webserver");
			server.stop();  //Desliga o servidor
			WiFi.mode(WIFI_STA);  //Apenas modo station
			flag_ifWifiConnected = true;
			}else {
			Serial.println("Failed to connect");
			New_Input = false;
			WiFi.mode(WIFI_AP);
			l=0;
		}
	}
}

void handleRoot(){
	// String page;
	String page = FPSTR(HTTP_HEAD);
	page.replace("{v}",APname);
	page += FPSTR(HTTP_SCRIPT);
	page += FPSTR(HTTP_FORM);
	page += FPSTR(HTTP_LIST);
	
	int n = WiFi.scanNetworks();
	if (n == 0) {
		page += "<p> No networks found </p>";
		} else {
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (signalQuality[j] > signalQuality[i]) {
					int tmp = signalQuality[i];
					signalQuality[i] = signalQuality[j];
					signalQuality[j] = tmp;
					
					String temp = WiFi.SSID(i);
					WiFi.SSID(i) = WiFi.SSID(j);
					WiFi.SSID(j) = temp;
				}
			}
		}
		
		for (int i = 0; i < n; i++) {
			String item = FPSTR(WIFI_LIST);
			String rssiQ;
			rssiQ = signalQuality[i];
			item.replace("{v}",WiFi.SSID(i));
			item.replace("{r}",rssiQ);
			page += item;
		}
	}
	page += FPSTR(HTTP_END);
	
	server.sendHeader("Content-Length", String(page.length()));
	server.send(200,"text/html",page);
	
	New_ssid = server.arg("s").c_str();
	New_pass = server.arg("p").c_str();
	
	if(New_ssid != "" || New_pass != ""){
		
		Serial.println(New_ssid);
		Serial.println(New_pass);
		New_Input = true;
	}
}

int getRSSIasQuality(int RSSI) {
	int quality = 0;

	if (RSSI <= -100) {
		quality = 0;
		} else if (RSSI >= -50) {
		quality = 100;
		} else {
		quality = 2 * (RSSI + 100);
	}
	return quality;
}