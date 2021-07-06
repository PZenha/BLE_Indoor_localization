#include <HTTPClient.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>


int ScansNum = 0;
float mills = 0;
int seconds = 0;
//BLE
int scanTime = 3; //In seconds

String MyUUID = "4c000215bcbe1be07afac0ade8119f7b98c4b8c1"; //	 4c000215f7826da64fa24e988024bc5b71e0893edfee4771b3
String ThisUUID ="";
double MajMin = 0.0;
String major="";
String minor ="";
int BeaconPos = 0;
int atualPos = 0;
double MissingMajMin = 0.0;
int MBP = 0; //Missing Beacon Position
int HigherRSSI = 0;
//////

//Wi-Fi
const char* SSID = "XHOCKWARE";	//XHOCKWARE  "MEO-5559F1"
const char* PASSWORD ="xhock4ever"; //xhock4ever  "546A780F24"
//////

//192.168.100.44 

HTTPClient http;

class FoundiBeacon{
	boolean TableFULL;
	public:
	boolean BeaconExist;
	int iBeaconTable[8][3] = {{0}};
	int TotaliBeaconsFound[10][3] = {{0}};
	int LastScanBeacon[10][2] = {{0}};
	void set(unsigned int,unsigned int,int);
	void confirmExist(unsigned int,unsigned int);
	void UpdateRSSI(unsigned int,unsigned int,int);
	int confirmMissingPos();
	double deleteBeacon(int);
	boolean MyBeaconHistory(unsigned int,unsigned int);
	int GetBeaconArrayPosition(unsigned int,unsigned int);
}iBeacon;

//Fuctions
void BLEScanner();
void ConnectWiFi();
void FillCurrentBeaconArray();
void ClearCurrentBeaconArray();
void sendData();

void FoundiBeacon::set(unsigned int maj,unsigned int min,int rssi){
	
	Serial.print("Beacon already Exist? -> ");
	Serial.print(iBeacon.BeaconExist);
	Serial.println("");
	if(iBeacon.BeaconExist == false){
		TableFULL = true;
		for(int i = 0; i < 8; i++){
			if(iBeacon.iBeaconTable[i][0] == 0 && iBeacon.iBeaconTable[i][1] == 0 && iBeacon.iBeaconTable[i][2] == 0){
				iBeacon.iBeaconTable[i][0] = maj;
				iBeacon.iBeaconTable[i][1] = min;
				iBeacon.iBeaconTable[i][2] = rssi;
				HigherRSSI = rssi;
				TableFULL = false;
				Serial.print("Set at: ");
				Serial.print(i);
				Serial.print(", Maj:min: ");
				Serial.print(iBeacon.iBeaconTable[i][0]);
				Serial.print(",");
				Serial.print(iBeacon.iBeaconTable[i][1]);
				Serial.print(" - ");
				Serial.println(iBeacon.iBeaconTable[i][2]);
				i = 8;
			}
		}
		if(TableFULL == true){
			Serial.println("No more space to save iBeacon...");
		}
	}
}

void FoundiBeacon::confirmExist(unsigned int maj, unsigned int min){
	iBeacon.BeaconExist = false;
	for(int i = 0; i < 8; i++){
		if(maj == iBeacon.iBeaconTable[i][0] && min == iBeacon.iBeaconTable[i][1]){
			iBeacon.BeaconExist = true;
		}
	}
	
}

void FoundiBeacon::UpdateRSSI(unsigned int maj,unsigned int min,int rssi){
	if(iBeacon.BeaconExist == true){
		for(int i = 0; i < 8; i++){
			if(maj == iBeacon.iBeaconTable[i][0] && min == iBeacon.iBeaconTable[i][1]){
				HigherRSSI = iBeacon.iBeaconTable[i][2];
				if(rssi > iBeacon.iBeaconTable[i][2]){
					iBeacon.iBeaconTable[i][2] = rssi;
					HigherRSSI = iBeacon.iBeaconTable[i][2];
					i = 8;
				}
			}
		}
	}
}

int FoundiBeacon::confirmMissingPos(){
	int k = 0;

		boolean Missing = false;
		k = MBP;
		for(int j = 0; j < 10; j++){
			if(iBeacon.TotaliBeaconsFound[MBP][0] != iBeacon.LastScanBeacon[j][0] && iBeacon.TotaliBeaconsFound[MBP][1] != iBeacon.LastScanBeacon[j][1] && j == 9 && iBeacon.TotaliBeaconsFound[MBP][0] != 0){
				iBeacon.TotaliBeaconsFound[MBP][2]--;
				if(iBeacon.TotaliBeaconsFound[MBP][2] == 0){
					Missing = true;
				}
				j=10;
			}else{
				if(iBeacon.TotaliBeaconsFound[MBP][0] == iBeacon.LastScanBeacon[j][0] && iBeacon.TotaliBeaconsFound[MBP][1] == iBeacon.LastScanBeacon[j][1]){
					j=10;
				}
			}
		}
		if(Missing == true){
			Serial.print("Missing: ");
			Serial.print(iBeacon.TotaliBeaconsFound[k][0]);
			Serial.print(",");
			Serial.println(iBeacon.TotaliBeaconsFound[k][1]);
			
			for(int i = 0; i < 8; i++){
				if(iBeacon.iBeaconTable[i][0] == iBeacon.TotaliBeaconsFound[k][0] && iBeacon.iBeaconTable[i][1] == iBeacon.TotaliBeaconsFound[k][1]){
					k = i;
					i = 8;
					iBeacon.TotaliBeaconsFound[k][0] = 0;
					iBeacon.TotaliBeaconsFound[k][1] = 0;
					iBeacon.TotaliBeaconsFound[k][2] = 0;
				}
			}
			
			return k;
		}
	
	return 1000;
}

double FoundiBeacon::deleteBeacon(int pos){
	
	double MissingBeacon = 0.0;
	
	float Fmajor = (float)iBeacon.iBeaconTable[pos][0];
	float Fminor = (float)iBeacon.iBeaconTable[pos][1];
	
	MissingBeacon = (Fmajor + Fminor/100000) * -1;
	
	iBeacon.iBeaconTable[pos][0] = 0;
	iBeacon.iBeaconTable[pos][1] = 0;
	iBeacon.iBeaconTable[pos][2] = 0;
	
	Serial.println(MissingBeacon,7);
	return MissingBeacon;
}

boolean FoundiBeacon::MyBeaconHistory(unsigned int maj,unsigned int min){
	
	boolean FirstTimeBeacon = true;
	
	for(int i = 0; i < 10; i++){
		if(iBeacon.TotaliBeaconsFound[i][0] == maj && iBeacon.TotaliBeaconsFound[i][1] == min){
			iBeacon.TotaliBeaconsFound[i][2]++;
			FirstTimeBeacon = false;
		}
		if(iBeacon.TotaliBeaconsFound[i][2] > 2){
			iBeacon.TotaliBeaconsFound[i][2] = 2;
		}
	}
	
	return FirstTimeBeacon;
}

int FoundiBeacon::GetBeaconArrayPosition(unsigned int maj,unsigned int min){
	int pos;
	for(int i = 0; i < 10; i++){
		if(iBeacon.TotaliBeaconsFound[i][0] == maj && iBeacon.TotaliBeaconsFound[i][1] == min){
			pos = i;
		}
	}
	return pos;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		int iBeaconRSSI;
		if(advertisedDevice.haveManufacturerData() == true){  // Returns the manufacturer data of the found beacon
			ThisUUID = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
			if(ThisUUID.substring(0,40) == MyUUID){  //If my iBeacon is found than print his info
				seconds = (millis()/1000);
				mills = (millis()%1000);
				String iBeaconDataSerial =";";
				iBeaconDataSerial += String(seconds);
				iBeaconDataSerial += ",";
				iBeaconDataSerial += String(mills);
				iBeaconDataSerial += ";Xhockware iBeacon;";
				iBeaconDataSerial += String(ThisUUID);
				iBeaconDataSerial += ";Major;";
				//Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
				
				//Serial.printf("Rssi: %d \n", (int)advertisedDevice.getRSSI());
			
				major =  ThisUUID.charAt(40); 
				major += ThisUUID.charAt(41); 
				major += ThisUUID.charAt(42);
				major += ThisUUID.charAt(43);
			
				unsigned int IntMajor = (int) strtol(&major[0],NULL,16);
				iBeaconDataSerial += String(IntMajor);
				iBeaconDataSerial += ";Minor;";
				
				minor =  ThisUUID.charAt(44);
				minor += ThisUUID.charAt(45);
				minor += ThisUUID.charAt(46);
				minor += ThisUUID.charAt(47);
			
				unsigned int IntMinor = (int) strtol(&minor[0],NULL,16);
				iBeaconDataSerial += String(IntMinor);
				iBeaconRSSI = (int)advertisedDevice.getRSSI(); 
				iBeaconDataSerial += ";RSSI;";
				iBeaconDataSerial += String(iBeaconRSSI);
				iBeaconDataSerial += ";TXpwr;";
				iBeaconDataSerial += String(advertisedDevice.getTXPower());
				
				FillCurrentBeaconArray(IntMajor,IntMinor); //Fill an array with major and minor from last scan
			
				boolean FirstTimeBeacon = iBeacon.MyBeaconHistory(IntMajor,IntMinor); //Will increment iBeacon
				
				if(FirstTimeBeacon == true){
					iBeacon.TotaliBeaconsFound[BeaconPos][0] = IntMajor;
					iBeacon.TotaliBeaconsFound[BeaconPos][1] = IntMinor;
					iBeacon.TotaliBeaconsFound[BeaconPos][2]++;
					iBeaconDataSerial += ";Score;";
					iBeaconDataSerial += String(iBeacon.TotaliBeaconsFound[BeaconPos][2]);
					iBeaconDataSerial += ";Higher RSSI;";
					iBeaconDataSerial += String(iBeaconRSSI);
					BeaconPos++;
				}
				
		/*		float FMajor = (float)IntMajor;
				float FMinor = (float)IntMinor;
				MajMin = FMajor + FMinor/100000; 			
				
				Serial.print(" -> ");
				Serial.println(MajMin,7); */
					
				if(FirstTimeBeacon == false){
					int pos = iBeacon.GetBeaconArrayPosition(IntMajor,IntMinor);
					iBeaconDataSerial += ";Score;";
					iBeaconDataSerial += String(iBeacon.TotaliBeaconsFound[pos][2]);
					if(iBeacon.TotaliBeaconsFound[pos][2] == 2){
						iBeacon.confirmExist(IntMajor,IntMinor); //Verify if Beacon already exist and update 
						iBeacon.set(IntMajor,IntMinor,iBeaconRSSI); //if not, put Beacon on table
						iBeacon.UpdateRSSI(IntMajor,IntMinor,iBeaconRSSI);
						iBeaconDataSerial += ";Higher RSSI;";
						iBeaconDataSerial += String(HigherRSSI);
					}
				}
				Serial.println(iBeaconDataSerial); //Print all Data from found iBeacon
			}
		}
	}
};



void setup() {
	
	Serial.begin(115200);
		
}

void loop() {
	
		BLEScanner();
	
	if(ScansNum == 5){
		btStop();
		sendData();
		ESP.restart();
	}
	
}

void BLEScanner(){
	Serial.println("Scanning...");
	BLEDevice::init("");
	BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
	BLEScanResults foundDevices = pBLEScan->start(scanTime);
	Serial.print("Devices found: ");
	Serial.println(foundDevices.getCount());
	Serial.println("Scan done!");
	ScansNum++;
	Serial.println("");
		for(MBP = 0; MBP < 8; MBP++){
			int pos = iBeacon.confirmMissingPos();
			if(pos != 1000){
				MissingMajMin =	iBeacon.deleteBeacon(pos);
			} 
		}
	MBP = 0; //Reset Missing Beacon Position 
	
	Serial.println("iBeaconTable");
	Serial.println("MAJOR | Minor | RSSI");
	for(int i = 0; i < 8; i++){
		Serial.print(iBeacon.iBeaconTable[i][0]);
		Serial.print(":");
		Serial.print(iBeacon.iBeaconTable[i][1]);
		Serial.print("  ");
		Serial.println(iBeacon.iBeaconTable[i][2]);
	}
	
	Serial.println("");
	Serial.println("Total iBeacons found");
	Serial.println("MAJOR | Minor | Score");
	for(int i = 0; i < 10; i++){
		Serial.print(iBeacon.TotaliBeaconsFound[i][0]);
		Serial.print(":");
		Serial.print(iBeacon.TotaliBeaconsFound[i][1]);
		Serial.print("|");
		Serial.println(iBeacon.TotaliBeaconsFound[i][2]);
	}
	
	Serial.println("");
	Serial.println("Last Scan iBeacons");
	Serial.println("MAJOR | Minor");
	for(int i = 0; i < 10; i++){
		Serial.print(iBeacon.LastScanBeacon[i][0]);
		Serial.print(":");
		Serial.println(iBeacon.LastScanBeacon[i][1]);
	}
	
	ClearCurrentBeaconArray(); //Will clear the iBeacons found last scan
		
	}

void ConnectWiFi(){
	unsigned int now = millis();
	
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID,PASSWORD);
	
	
	Serial.print("Connecting to ");
	Serial.println(SSID);
	
	while(WiFi.status() != WL_CONNECTED){
	}
	
	Serial.println("");
	Serial.println("Connected to Wi-Fi...");
	}
	
	
void FillCurrentBeaconArray(unsigned int major, unsigned int minor){
	iBeacon.LastScanBeacon[atualPos][0] = major;
	iBeacon.LastScanBeacon[atualPos][1] = minor;
	atualPos++;
}
	
void ClearCurrentBeaconArray(){
	for (int i = 0; i < 10; i++){
		iBeacon.LastScanBeacon[i][0] = 0;
		iBeacon.LastScanBeacon[i][1] = 0;
	}
	
	atualPos = 0; 
}

void sendData(){
	int startMillis = millis();
	String post="";
	int httpResponseCode = 0;
	
	
	ConnectWiFi();
	
	for(int i = 0; i < 8; i++){
		if(iBeacon.iBeaconTable[i][0] != 0){
			post = "major=";
			post += iBeacon.iBeaconTable[i][0];
			post +="&minor=";
			post += iBeacon.iBeaconTable[i][1];
			post +="&rssi=";
			post += iBeacon.iBeaconTable[i][2];
			post += "&mac=";
			post += WiFi.macAddress();
			http.begin("http://192.168.100.44:8080/");
			http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	
			httpResponseCode = http.POST(post);
	
			if(httpResponseCode>0){
		
				String response = http.getString();                       //Get the response to the request
		
				Serial.println(httpResponseCode);   //Print return code
				Serial.println(response);           //Print request answer
		
				}else{
		
				Serial.print("Error on sending POST: ");
				Serial.println(httpResponseCode);
		
			}	
		http.end();  //Free resources
		}
	}
	int stopMillis = millis() - startMillis;
	Serial.print("Millis after scan: ");
	Serial.println(stopMillis);
}
