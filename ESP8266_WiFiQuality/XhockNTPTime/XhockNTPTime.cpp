#include "arduino.h"
#include "XhockNTPTime.h"

////////////////////////////// NTP UDP
unsigned long int trx;
unsigned long int t_local;

unsigned int localPort = 2390;
int NTPserver=0;
int once = 0;
int NTPfail = 0;
int timeDif = 0;
int difTimes = 0;
int TotalDelay = 0;
boolean flag_NTP= false, NTPlisten = false, flag_firstUpdate = true,flag_NTPfail = false;

unsigned long StartNtpStamp = 0;

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "0.pt.pool.ntp.org";
const char* ntpServerName1 = "pool.ntp.org";
const char* ntpServerName2 = "1.europe.pool.ntp.org";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

WiFiUDP udp;

///////////////////////////////NTP UDP



void ReqNtpTime(){
	
	if(NTPserver == 0){
		WiFi.hostByName(ntpServerName, timeServerIP);
		Str_Buffer ="Test to NTP service - Sendind UDP packet to ";
		Str_Buffer += ntpServerName;
		SendToPrinter(0,"aa",0);
	}
	if(NTPserver == 1){
		WiFi.hostByName(ntpServerName1, timeServerIP);
		Str_Buffer ="Test to NTP service - Sendind UDP packet to ";
		Str_Buffer += ntpServerName1;
		SendToPrinter(0,"aa",0);
	}
	if(NTPserver == 2){
		WiFi.hostByName(ntpServerName2, timeServerIP);
		Str_Buffer ="Test to NTP service - Sendind UDP packet to ";
		Str_Buffer += ntpServerName2;
		SendToPrinter(0,"aa",0);
	}
	
	// discard any previously received packets
	sendNTPpacket(timeServerIP); // envia o pedido
	NTPlisten = true;
	StartNtpStamp = millis();
	
} // end of ReqNTPtime


void NTPisWaiting(){
	
	if(NTPlisten == true){
		NTP_Time();
		if(flag_NTP == true && flag_firstUpdate == true){
			setTime(trx); //Update ESP time
			NTPlisten = false;
			flag_firstUpdate = false;
		}
		
		if(flag_NTP == true && flag_firstUpdate == false && flag_NTPfail == false){
			NTPlisten = false;
			timeDiference();
		}
	}
}

void timeDiference(){
	
	if(trx >= now()){
		timeDif = trx%1000 - now()%1000;
	}else
	if(trx < now()){
		timeDif = (now()%1000 - trx%1000)* -1;
	}	
	
	timeDif = timeDif + (TotalDelay / 2);
	
	if(timeDif >= 1000){
		difTimes++;
		setTime(trx);
		Str_Buffer = "";
		SendToPrinter(2,"Number of clock adjustments: ",(int)difTimes);
	}else
	if(timeDif <= -1000){
			difTimes++;
			setTime(trx);
			Str_Buffer = "";
			SendToPrinter(2,"Number of clock adjustments: ",(int)difTimes);
	}
	
	Str_Buffer = "";
	SendToPrinter(1,"Time Diference(ms): ",(int)timeDif);
}


void NTP_Time(){
	unsigned long epoch;
	
	if((millis() - StartNtpStamp) < 600){
	
		int cb = udp.parsePacket();
		if (cb >= 48) {
			
			Str_Buffer = "";
			TotalDelay = millis() - StartNtpStamp;
			SendToPrinter(1,">NTP Delay: ",TotalDelay);
			Serial.print("ms");
			
			NTPlisten = false;
			flag_NTP = true;
			flag_NTPfail = false;
			
			Str_Buffer += " - packet received, ";
			SendToPrinter(2,"length= ",cb);
			// We've received a packet, read the data from it
			udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

			//the timestamp starts at byte 40 of the received packet and is four bytes,
			// or two words, long. First, esxtract the two words:

			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			// combine the four bytes (two words) into a long integer
			// this is NTP time (seconds since Jan 1 1900):
			unsigned long secsSince1900 = highWord << 16 | lowWord;

			// now convert NTP time into everyday time:
			// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
			const unsigned long seventyYears = 2208988800UL;
			// subtract seventy years:
			epoch = secsSince1900 - seventyYears;
			// print Unix time:

			// print the hour, minute and second:

			trx=epoch;
			t_local = now();
			
			Serial.print(" The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
			Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
			Serial.print(':');
			if ( ((epoch % 3600) / 60) < 10 ) {
				// In the first 10 minutes of each hour, we'll want a leading '0'
				Serial.print("0");
			}
			Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
			Serial.print(":");
			if ( (epoch % 60) < 10 ) {
				// In the first 10 seconds of each minute, we'll want a leading '0'
				Serial.print('0');
			}
			Serial.print(epoch % 60); // print the second
		}else{
			flag_NTPfail = true;
		}
		
	}else{
			flag_NTPfail = true;
			NTPfail++;
			TotalDelay = 0;
			NTPlisten = false;
			Str_Buffer="NTP packet timed out";
			SendToPrinter(0,"aa",0);
			
			if(NTPserver != 2){
				NTPserver++;
			}else{
				NTPserver=0;
			}
			
	}
}

void sendNTPpacket(IPAddress& address){
	//Serial.println("sending NTP packet...");
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();
	StartNtpStamp = millis();
}


