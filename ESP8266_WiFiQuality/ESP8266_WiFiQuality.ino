#include <XhockWiFiManager.h>
#include <XhockPrint.h>
#include <XhockNTPTime.h>
#include <XhockNEOPIXEL.h>
#include <TimeLib.h>
#include <ESP8266Ping.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>



/////////////////////////////////////SYSTEM STATUS
#define BOOT_START 0;
#define WIFI_NOT_CONNECTED 1;
#define WIFI_CONNECTED 2;
#define IP_NOT_OK 3;
#define IP_OK 4;
#define DNS_NOT_OK 5;
#define DNS_OK 6;
#define	INTERNET_AVAILABLE 7;

#define DHTPIN 4 // Data pin
#define DHTTYPE DHT11 // DHT 11

#define VERMELHO 15
#define AZUL 2
#define VERDE 13

String ThingStatus ="";
boolean flag_LostSignal = false;

///////////////////////////////////SYSTEM STATUS

DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;
HTTPClient http;

//////////////////////////////POST
boolean flag_DNS = false;
String WiFi_apiKey = "ZJ1XNVJHBFW4H5YF"; //Server WIFI signal API key from channel 1
String Environment_apikey = "PP9CTFF1RNXG0NEU"; //Environment Channel
String Maintenance_apikey = "S7PJT3E5E9N7TA5U"; //Maintenance Channel
const char* Tserver = "api.thingspeak.com"; //Name server
unsigned long previousControl = 0;
String bodyOK ="";
//////////////////////////////POST
IPAddress IP;
/////////////////////////////CONTROL
int lastSecond = 0;
int ParMinute = 0;
/////////////////////////////CONTROL
/////////////////////////////Wifi Signal
int wifiPower = 0;
int wifiFail = 0;
/////////////////////////////Wifi Signal
/////////////////////////////HTTP
int HTTPDelay = 0;
int GETfail = 0;
////////////////////////////HTTP
////////////////////////////ping
int pingDelay = 0;
int pingFail = 0;
unsigned long int startTimeStamp = 0;
///////////////////////////ping
//////////////////////////NTP
int NTPDelay = 0;
/////////////////////////NTP
///////////////////////Environment
float humidity = 0;
float temperature = 0;
float HumidityArray[4], TempArray[4];
float TemperatureArray[4];
int HArrayPos = 0, TArrayPos = 0;
boolean flag_HumidityUpdate = false, flag_TempUpdate = false;
float HumidityMedia = 0, TemperatureMedia = 0;
boolean flag_OrganizeTemp = false, flag_OrganizeHumidity = false;
//////////////////////Environment
/////////////////////Maintenance
int statusCnt = 0;
unsigned long int InicialHeap = 0;
////////////////////Maintenance
String TimeAlive =""; //Save time since ESP is alive
boolean flag_UpdateTimeAlive = false;

void FilterStatus();
void ESP_IP();
int youbeepHeader();
void testDNS();
int ReadWifiSignal();
int sendPing();
float humidityValue();
float temperatureValue();
void sendPost();
void BuildBody();
void runOncePerSecond();
int VerifyIfGotWifi();
void HEAPcontrol();
void AliveTime();
void AZULON();
void VERMELHOON();
void LEDOFF();


//System Status < var name = sysAlive
int sysAlive = BOOT_START;
int oldStatus = sysAlive;


void setup(){
	delay(5000); //only for terminal
	
	
	Serial.begin(115200);
	EEPROM.begin(512);
	dht.begin();
	
	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(30);
	
	pinMode(AZUL,OUTPUT);
	pinMode(VERMELHO,OUTPUT);
	pinMode(VERDE,OUTPUT);
	
	VERMELHOON();
	//Reading SSID and PASSWORD stores at EEPROM
	Str_Buffer = "Reading SSID and PASSWORD from EEPROM...";
	SendToPrinter(0,"aa",0);
	eSSID = "\0";
	for(int i = 0; i < 32; i++){
		eSSID += char(EEPROM.read(i));
		//Str_Buffer = char(EEPROM.read(i));
	}
	for(int i = 32; i < 96; i++){
		ePASS += char(EEPROM.read(i));
	}
	
	
	//Ligação à rede Wifi
	WiFi.mode(WIFI_STA); //Modo station
	if(eSSID.length() > 1){  //If got any SSID stored at EEPROM
			WiFi.begin(eSSID.c_str(),ePASS.c_str());
			Str_Buffer = "Connecting to: ";
			Str_Buffer += eSSID.c_str();
			SendToPrinter(0,"aa",0);
		}else{
		WiFi.begin(ssid,pass);
		Str_Buffer = "Connecting to: "; //Rede xhockware como definição
		Str_Buffer += ssid;
		SendToPrinter(0,"aa",0);
	}
	int i = 0;
	
	//Checking Wi-Fi status
	while(WiFi.status() != WL_CONNECTED && i<50){
		delay(200);
		Serial.print(".");
		i++;
	}
	if(WiFi.status() == WL_CONNECTED){
		Serial.println("");
		Str_Buffer = "Connected to network";
		SendToPrinter(0,"aa",0);
		flag_ifWifiConnected = true;
	}
	if(i == 50){
		Serial.println("");
		Str_Buffer = "Error connecting to network";
		SendToPrinter(0,"aa",0);
		flag_ifWifiConnected = false;
	}
	
	if(flag_ifWifiConnected == false){
		leds[0] = 0xFF0000;
		FastLED.show();
		sysAlive = WIFI_NOT_CONNECTED;
		WiFi.mode(WIFI_AP);
		WiFi.softAP(APssid,APpass,1,false); //Username & password
		Str_Buffer = "AP name:";
		Str_Buffer += APssid;
		SendToPrinter(0,"aa",0);
		
		IPAddress myIP = WiFi.softAPIP();//GET IP ADRESS
		Str_Buffer = "AP IP: ";
		SendToPrinter(0,"aa",0);
		Serial.println(myIP);

		server.on("/",handleRoot);
		server.begin();			//Inicializar servidor
		Str_Buffer = "Server started";
		SendToPrinter(0,"aa",0);
		
		time_off = millis();
		time_off_last = time_off;
		
		//Wi-Fi Manager will be working for 5minutes maximum
		while( time_off - time_off_last < 300000 && flag_ifWifiConnected != true){
			LEDOFF();
			Wifi_Manager();
			time_off = millis();
			
		}
		
		//If after 5minutes without internet connection ESP will restart
		if(time_off - time_off_last >= 300000 && flag_ifWifiConnected != true){
			ESP.restart();
		}
	}
	
	if(flag_ifWifiConnected == true){
		ESP_IP();
		Str_Buffer = "Starting UDP - ";
		udp.begin(localPort);
		Str_Buffer += "Local port: ";
		Str_Buffer += String(udp.localPort());
		SendToPrinter(0,"aa",0);
		once++;
	}
	InicialHeap = ESP.getFreeHeap();
	Str_Buffer="";
	SendToPrinter(3,"Maximum HEAP: ",InicialHeap);;
}  //END OF SETUP

void loop(){
	
	//Verify system status
	systemStatus();
	
	//Verify if got NTP request
	NTPisWaiting();
	
	//Verify if filter value has been set
	FilterStatus();
	
	//Testing if we've got Wi-Fi
	VerifyIfGotWifi();
	
	//Verify available RAM
	HEAPcontrol();
	
	//Reset Internet Quality and turns led 5
	QualityReset();
	
	//Saves time since ESP is alive
	AliveTime();
	
	//Once per seconds calls functons
	if(lastSecond != second()){
		runOncePerSecond();
		lastSecond = second();
	}
	
} //End of Loop



void systemStatus(){
	
	if(oldStatus != sysAlive){
		oldStatus = sysAlive;
		Str_Buffer = "System status changed to: ";
		
		if(oldStatus != 7){
			VERMELHOON();
		}
		
		switch(oldStatus){
			case 0: Str_Buffer += "BOOT START";
			SendToPrinter(0,"aa",0);
			
			break;
			case 1: Str_Buffer += "WIFI NOT CONNECTED";
			SendToPrinter(0,"aa",0);
	
			break;
			case 2: Str_Buffer += "WIFI CONNECTED";
			SendToPrinter(0,"aa",0);

			break;
			case 3: Str_Buffer += "IP NOT OK";
			SendToPrinter(0,"aa",0);

			break;
			case 4: Str_Buffer += "IP OK";
			SendToPrinter(0,"aa",0);
			
			break;
			case 5: Str_Buffer += "DNS NOT OK";
			SendToPrinter(0,"aa",0);
		
			break;
			case 6: Str_Buffer += "DNS OK";
			SendToPrinter(0,"aa",0);
			
			break;
			case 7: Str_Buffer += "INTERNET AVAILABLE";
			SendToPrinter(0,"aa",0);
			AZULON();
			break;
			
			default: Str_Buffer += "Invalid status";
			SendToPrinter(0,"aa",0);
			break;
		}
		
	}
	
	//If we are connected to AP and got IP adress but DNS and NTP yet not true
	if(flag_ifWifiConnected == true && flag_DNS == false && flag_NTP == false){
		sysAlive = WIFI_CONNECTED;
		}else{
		if(flag_ifWifiConnected == false){
			sysAlive = WIFI_NOT_CONNECTED;
		}
	}
	
	//If connected to AP but IP not tested yet
	if(flag_ifWifiConnected == true && flag_IP == false && flag_DNS == false && flag_NTP == false){
		sysAlive = IP_NOT_OK;
		}else{
		if(flag_ifWifiConnected == true && flag_IP == true && flag_DNS == false && flag_NTP == false){
			sysAlive = IP_OK;
		}
	}
	
	
	//If we are connected to AP, got IP and DNS is working but got no time yet
	if(flag_ifWifiConnected == true && flag_IP == true && flag_DNS == true && flag_NTP == false){
		sysAlive = DNS_OK;
		}else{
		if(flag_ifWifiConnected == true && flag_IP == true && flag_DNS == false && flag_NTP == false){
			sysAlive = DNS_NOT_OK;
		}
	}
	
	//If DNS not OK suddenly
	if(flag_ifWifiConnected == true && flag_IP == true && flag_DNS == false && flag_NTP == true){
		sysAlive = DNS_NOT_OK;
	}
	
	//If we are connect to AP, got IP, DNS OK and got time
	if(flag_ifWifiConnected == true && flag_IP == true && flag_DNS == true && flag_NTP == true){
		sysAlive = INTERNET_AVAILABLE;
	}
}


void ESP_IP(){
	//Verify if we have IP
	
	Str_Buffer = "Verify if we have IP - IP address: ";
	SendToPrinter(0,"aa",0);
	Serial.print(WiFi.localIP());
	IP = WiFi.localIP();
	String s="";
	for (int i=0; i<4; i++){
		s += i  ? "." + String(IP[i]) : String(IP[i]);
	}
	if(s != "0.0.0.0"){ //GOT IP?
		flag_IP = true;
		}else {
		flag_IP = false;
	}
}

int youbeepHeader(){
	int GET_Delay = 0;
	
	unsigned long int getmillis = millis();
	boolean begintest = http.begin("http://ws.youbeep.com/");
	Str_Buffer = "YBHR";
	Str_Buffer += " - ";
	Str_Buffer += "Test HTTP to www.youbeep.com";
	Str_Buffer += " - ";
	Str_Buffer += "Codigo begin: ";
	Str_Buffer += String(begintest);  // True (1) or False (0);
	Str_Buffer += " - ";
	
		
	http.setTimeout(1000); //Time out after 1 second waiting
	
	const char* headerNames[] = {"Date"};
	http.collectHeaders(headerNames,sizeof(headerNames)/sizeof(headerNames[0]));
	
	
	int httpCode = http.GET();  // Se httpCode > 0 (Não ha erros) Se <0 (há erros)
	
	Str_Buffer += "httpCode: ";
	Str_Buffer += String(httpCode);
	Str_Buffer += " - ";
	if(httpCode > 0){
		if(http.hasHeader("Date")){
			String payload = http.getString();
			Str_Buffer += "payload: ";
			Str_Buffer += String(payload);
			String Date = http.header("Date");
			Str_Buffer += " - Date: ";
			Str_Buffer += String(Date);
			GET_Delay = millis() - getmillis;
			SendToPrinter(1," - HTTP Delay: ",GET_Delay);
			Serial.println("ms");
		}
		}else{
		Str_Buffer = "Error with GET";
		SendToPrinter(0,"aa",0);
		GETfail++;
	}
	http.end();
	return GET_Delay;
}


void testDNS(){
	IPAddress IP;
	boolean speakIP = WiFi.hostByName(Tserver,IP); //Get name server IP
	String s="";
	Str_Buffer = "TDNS - Test DNS to api.thingspeak.com -";
	for (int i=0; i<4; i++){
		s += i  ? "." + String(IP[i]) : String(IP[i]);
	}
	if(s != "0.0.0.0"){ //DNS OK ?
		Str_Buffer += "DNS is OK - IP: ";
		SendToPrinter(0,"aa",0);
		Serial.print(IP);
		flag_DNS = true;
		}else{
		Str_Buffer = "DNS NOT OK";
		SendToPrinter(0,"aa",0);
		flag_DNS = false;
	}
}

int ReadWifiSignal(){
	Str_Buffer = "Read Wifi signal";
	int WifiSignal = getRSSIasQuality(WiFi.RSSI());
	SendToPrinter(1,">Wi-Fi Signal: ",(int)WifiSignal);
	Serial.print("%");
	return WifiSignal;
}
int sendPing(){
	int avg_time = 0;
	Str_Buffer = "Testing Ping to www.google.com";
	SendToPrinter(0,"aa",0);
	
		Ping.ping("www.google.com");
		avg_time = Ping.averageTime();
		Str_Buffer = "";
		SendToPrinter(1,">Ping Delay: ",(int)avg_time);
		Serial.print("ms");
		
	if(avg_time == 0){
		pingFail++;
	}
	
	return avg_time;
}

float humidityValue(){
	
	float temp = 0;
	int maxPos = 0;
	int minPos = 0;
	
	float h = dht.readHumidity();
	
	if(h == 0){
		h = HumidityMedia;
	}
	
	//After array is complete it'll never come here again
	//5 measures during less than 2 minutes
	if(flag_HumidityUpdate == false){
		HumidityArray[HArrayPos] = h;
		
		Str_Buffer="HarrayPos = ";
		Str_Buffer+=String(HArrayPos);
		SendToPrinter(1,"atual humidity =",h);
		
		HArrayPos++;
		if(HArrayPos > 4){
			flag_HumidityUpdate = true;
		}
	}
	
	
	if(flag_HumidityUpdate == true){
		float maxH = HumidityArray[0];
		float minH = HumidityArray[0];
		
		for(int i = 0; i < 5; i++){
			if(HumidityArray[i] >= maxH){  //Search for max value
				maxH = HumidityArray[i];
				maxPos = i;		//Position of max value
			}
		}
			temp = HumidityArray[4];  //It'll switch the max value to the last pos of array
			HumidityArray[4] = maxH;
			HumidityArray[maxPos] = temp;
			
		for(int i = 0; i < 5 ; i++){
			if(HumidityArray[i] <= minH){ //Search for min value
				minH = HumidityArray[i];
				minPos = i;
			}
		}	
			
			temp = HumidityArray[0];  //It'll switch the min value to the last pos of array
			HumidityArray[0] = minH;
			HumidityArray[minPos] = temp;
			
			
			for(int i = 0; i < 5; i++){
				Serial.println("");
				Serial.print("array ");
				Serial.print(i);
				Serial.print(" is ");
				Serial.println(HumidityArray[i]);
			}
			
			flag_HumidityUpdate = false;
			HumidityMedia = (HumidityArray[1] + HumidityArray[2] + HumidityArray[3]) / 3;
			HArrayPos = 0;
			Str_Buffer="";
			SendToPrinter(1,"Humidity value: ",HumidityMedia);
			Serial.println(" %");
			
	}
	
	return HumidityMedia;
}

float temperatureValue(){
	
	float temp = 0;
	int maxPos = 0;
	int minPos = 0;
	
	float t = dht.readTemperature();
	
	if(t == 0){
		t = TemperatureMedia;
	}
	
	//After array is complete it'll never come here again
	//5 measures during less than 2 minutes
	if(flag_TempUpdate == false){
		TempArray[TArrayPos] = t;
		
		Str_Buffer="TarrayPos = ";
		Str_Buffer+=String(TArrayPos);
		SendToPrinter(1,"atual temp = ",t);
		
		TArrayPos++;
		if(TArrayPos > 4){
			flag_TempUpdate = true;
		}
	}
	
	
	if(flag_TempUpdate == true){
		float maxH = TempArray[0];
		float minH = TempArray[0];
		
		for(int i = 0; i < 5; i++){
			if(TempArray[i] >= maxH){  //Search for max value
				maxH = TempArray[i];
				maxPos = i;		//Position of max value
			}
		}
		temp = TempArray[4];  //It'll switch the max value to the last pos of array
		TempArray[4] = maxH;
		TempArray[maxPos] = temp;
		
		for(int i = 0; i < 5; i++){
			if(TempArray[i] <= minH){ //Search for min value
				minH = TempArray[i];
				minPos = i;		//Position of min value
			}
		}
		temp = TempArray[0];  //It'll switch the min value to the last pos of array
		TempArray[0] = minH;
		TempArray[minPos] = temp;
		
		for(int i = 0; i < 5; i++){
			Serial.println("");
			Serial.print("array ");
			Serial.print(i);
			Serial.print(" is ");
			Serial.println(TempArray[i]);
		}
		flag_TempUpdate = false;
		
		TemperatureMedia = (TempArray[1] + TempArray[2] + TempArray[3]) / 3;
		TArrayPos = 0;
		Str_Buffer="";
		SendToPrinter(1,"Temperature value: ",TemperatureMedia);
		Serial.println(" *C");
	}
	
	return TemperatureMedia;
}

void sendPost(String postThing, String API){
	startTimeStamp = millis();
	if(client.connect(Tserver,80)){   //POST Request to thingSpeak
		Str_Buffer = "Connected to: ";
		Str_Buffer += String(Tserver);
		SendToPrinter(0,"aa",0);
		client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY:"+API+"\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length:");
		client.print(postThing.length());
		client.print("\n\n");
		client.print(postThing);
		client.print("\n\n");
		client.stop();
		Str_Buffer = "Data sent to channel";
		SendToPrinter(0,"aa",0);
	}
	startTimeStamp = millis() - startTimeStamp;
}

String BuildBody(float data,int num,int first_field){
	String body;
	if(first_field != 1){
		body += "&";
	}
	
	if(first_field != 9){
		body += "field";
		body += String(num);
		body += "=";
		body += String(data);
	}
	
	if(first_field == 9){
		body += "status=";
		body += ThingStatus;
	}
	
	return body;
}

void runOncePerSecond(){
	
	int SecondSkipped = 0;
	
	//test if any seconds was skipped
	if(second() ==  lastSecond +1 || (second() == 0 && lastSecond == 59)){ //DO nothing
	}else{ 
		SecondSkipped++;
	}
	
	//Once per minute print time
	if(second() == 0){
		Str_Buffer = "ROPS - ";
		Str_Buffer += "Run once per minute";
		SendToPrinter(0,"aa",0);
	}
	
	
	
	//Every minute if DNS is OK Update DNS flag
	if(second() == 0 && sysAlive >= 4){
		testDNS();
	}
	
	
	//Se time not set corre de 5s em 5s
	//Se time set corre de 10min em 10min
	//Se desviado ajusta o tempo em 1s
	//Update NTP flag and
	//Update NTP delay
	
	//Se time not set corre de 5s em 5s
	if(flag_NTP == false && second() == 10 && NTPlisten == false){
		ReqNtpTime();
	}
	
	//Se time set corre de 10min em 10min && minute()%10 == 0 &&
	if(flag_NTP == true && minute()%10 == 0 && second() == 10){
		ReqNtpTime(); //Check if time is delayed or advanced and make -/+ 1 second adjustment
	}
	
	ParMinute = minute() % 2;    //If var is even or odd
	
	if(sysAlive == 7){
		

		//Read power of wifi signal if number is even and at second 10
		if(ParMinute == 0 && second() == 20){
			wifiPower = ReadWifiSignal();
		}
		
		
		//GET to youbeep.com to give Date and HTTP delay
		if(ParMinute == 0 && second() == 40){
			HTTPDelay = youbeepHeader();
		}
		
		//Send Ping to google and calculates ping delay
		if(ParMinute != 0 && second() == 20){
			pingDelay = sendPing();
		}
		
		//Read humidity and temperature values every 20 seconds
		if(second() == 0 || second()%20 == 0 ){										  
				humidity = humidityValue();			//Measure until we got 5 values at array
				temperature = temperatureValue();
		}

		//Send data to server

		//At even minute and second=20 send data to Environment channel
		if(ParMinute == 0 && second() == 30){
			Str_Buffer="Sending Data to Environment channel";
			SendToPrinter(0,"aa",0);
			String body ="";
			
					Str_Buffer="humidity to send: ";
					Str_Buffer+=String(humidity);
					Str_Buffer+=" Temperature to send: ";
					Str_Buffer+=String(temperature);
					SendToPrinter(0,"aa",0);
			
			if(humidity != 0){
				body += BuildBody(humidity,1,1);
				humidityPIXEL(humidity);
			}
			if(temperature != 0){
				body += BuildBody(temperature,2,2); 
				tempPIXEL(temperature);
			}
				body += BuildBody(startTimeStamp,3,2);
			
			
			LEDOFF();
			sendPost(body,Environment_apikey);
			AZULON();
		
		}
		
		//At even minute and second=40 send data internet quality channel
		if(ParMinute == 0 && second() == 50){
			Str_Buffer="Sending Data to Internet Quality channel";
			SendToPrinter(0,"aa",0);
			String body ="";
			
			//Send Wi-Fi signal
			if(wifiPower != 0){
				body += BuildBody(wifiPower,1,1);
				PixelWifiPower = wifiPower;
				wifiPower = 0;
			}
			body += BuildBody(wifiFail,2,2);
			
			//Send NTP delay
			if(TotalDelay != 0){
				body += BuildBody(TotalDelay,3,2);
				PixelNTPDelay = TotalDelay;
				TotalDelay = 0;						//Resets variables to 0 after sending
			}
			body += BuildBody(NTPfail,4,2);
			
			//Send HTTP delay
			if(HTTPDelay != 0){
				body += BuildBody(HTTPDelay,5,3);
				PixelHTTPDelay = HTTPDelay;
				HTTPDelay = 0;
			}
			body += BuildBody(GETfail,6,4);
			
			//Send Ping delay
			if(pingDelay != 0){
				body += BuildBody(pingDelay,7,2);
				PixelPingDelay = pingDelay;
				pingDelay = 0;
			}
			body += BuildBody(pingFail,8,2);

			
			if(flag_LostSignal == true){
				flag_LostSignal = false;
				ThingStatus = "Lost connection to Wi-Fi -> ";
				ThingStatus += String(hour());
				ThingStatus += ":";
				ThingStatus += String(minute());
				ThingStatus += ":";
				ThingStatus += String(second());
				body += BuildBody(0,0,9);  //Send status
			}
			
			LEDOFF();
			sendPost(body,WiFi_apiKey);
			AZULON();
			//Reset fails to 0
			if(GETfail >= 1){
				GETfail = 0;
				PixelGETFail++;
			}
			
			if(wifiFail >= 1){
				wifiFail = 0;
				PixelWifiFail++;;
			}
			
			if(pingFail >= 1){
				pingFail = 0;
				PixelPingFail++;
			}
			
			if(NTPfail >= 1){
				NTPfail = 0;
				PixelNTPFail++;
			}
			
			Str_Buffer = "NEO PIXEL UPDATE";
			SendToPrinter(0,"aa",0);
			QualityColor();
			
		}
		
		/*//At odd minute and second = 20 send data to server
		if(ParMinute != 0 && second() == 20){
			Str_Buffer="Sending Data to ... channel";
			SendToPrinter(0,"aa",0);
			String body ="";
			
			sendPost(body,WiFi_apiKey);
			}
		} */
		
		//At odd minute and second = 40 send data to Maintenance channel
		if(ParMinute != 0 && second() == 50){
			Str_Buffer="Sending Data to Maintenance ThingSpeak channel";
			SendToPrinter(0,"aa",0);
			if(statusCnt == 4){
				statusCnt=0;
			}
			String body ="";
			
			if(SecondSkipped != 0){
				body += BuildBody(SecondSkipped,1,2);
				SecondSkipped = 0;
			}
			
			body += BuildBody(ESP.getFreeHeap(),3,1);
			body += BuildBody(difTimes,4,2); //Clock adjustments
			if(timeDif != 0){
				body += BuildBody(timeDif,5,3);
				timeDif = 0;
			}
			body += BuildBody(FailQuality,6,4);
			body += BuildBody(SpeedQuality,7,5);
			body += BuildBody(InternetQualityColor,8,6);
			
			if(flag_LastHourQuality == true){
				body += BuildBody(LastQuality,2,7);
				flag_LastHourQuality = false;
			}
			
			if(statusCnt == 0){
				ThingStatus = "Local IP: ";
				for (int i=0; i<4; i++){
					ThingStatus += i  ? "." + String(IP[i]) : String(IP[i]);
				}
				body += BuildBody(0,0,9);
			}else
			if(statusCnt == 1){
				ThingStatus = "Flash ID: ";
				ThingStatus += String(ESP.getFlashChipId());
				body += BuildBody(0,0,9);
			}else
			if(statusCnt == 2){
				ThingStatus = "CPU ID: ";
				ThingStatus += String(ESP.getChipId());
				body += BuildBody(0,0,9);
			}else
			if(statusCnt == 3){
			ThingStatus = TimeAlive;
			body += BuildBody(0,0,9);
			}
			
			
			LEDOFF();
			sendPost(body,Maintenance_apikey);
			AZULON();
			
			statusCnt++;
			
			if(difTimes >= 1){
				difTimes = 0;
			}
		}
	}
}

//Verify if ESP still connected to Wi-Fi
int VerifyIfGotWifi(){
	
	int time_offline = 0;

	if(WiFi.status() != WL_CONNECTED){
		flag_LostSignal = true;
		Str_Buffer = "Wi-Fi offline";
		SendToPrinter(0,"aa",0);
		wifiFail++;
		flag_IP = false;
		unsigned long millis_off = millis();
		sysAlive = WIFI_NOT_CONNECTED;
		
		while (WiFi.status() != WL_CONNECTED && millis() - millis_off < 28800000){ //8 Hour offline
			delay(250);
			time_offline = (millis() - millis_off)/1000;
		}
		
		if(millis() - millis_off >= 28800000){
			ESP.restart();
		}
		
		
		if(WiFi.status() == WL_CONNECTED){
			ESP_IP(); //Test if we got IP
		}
		
		Str_Buffer = "";
		SendToPrinter(1,"Time offline = ",time_offline);
	}
	
	return time_offline;
}

void HEAPcontrol(){
	unsigned long int heap20 = InicialHeap* 0.20;
	if(ESP.getFreeHeap() <= heap20){
		ESP.restart();
	}  
}

void AliveTime(){
	if(flag_NTP == true && flag_UpdateTimeAlive == false){
		TimeAlive = "Alive since: ";
		if(hour() < 10){
			TimeAlive += "0";}
		TimeAlive += String(hour());
		TimeAlive += ":";
		if(minute() < 10){
			TimeAlive += 0;}
		TimeAlive += String(minute());
		TimeAlive += ":";
		if(second() < 10){
			TimeAlive += "0";}
		TimeAlive += String(second());
		TimeAlive += " - ";
		if(day() < 10){
			TimeAlive += "0";}
		TimeAlive += String(day());
		TimeAlive += "/";
		if(month() < 10){
			TimeAlive += "0";}
		TimeAlive += String(month());
		TimeAlive += "/";
		TimeAlive += String(year());
		flag_UpdateTimeAlive = true;
	}
}

void AZULON(){
	digitalWrite(VERDE,HIGH);
	digitalWrite(VERMELHO,HIGH);
	digitalWrite(AZUL,LOW);
}

void VERMELHOON(){
	digitalWrite(VERDE,HIGH);
	digitalWrite(VERMELHO,LOW);
	digitalWrite(AZUL,HIGH);
}

void LEDOFF(){
	digitalWrite(VERDE,HIGH);
	digitalWrite(VERMELHO,HIGH);
	digitalWrite(AZUL,HIGH);
}