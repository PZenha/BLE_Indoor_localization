#include "arduino.h"
#include "XhockNEOPIXEL.h"

#include <TimeLib.h>



///////////////////NEO PIXEL

CRGB leds[NUM_LEDS];
int lastHour = 0;
int FailQuality = 60;
int SpeedQuality = 40;
int PixelGETFail = 0, PixelWifiFail = 0,PixelPingFail = 0,PixelNTPFail = 0;
int PixelPingDelay = 0, PixelNTPDelay = 0, PixelHTTPDelay = 0, PixelWifiPower = 0;
int GETFailControl = 0, WifiFailControl = 0, PingFailControl = 0, NTPFailControl = 0;
int PingDelayControl = 0, NTPDelayControl = 0, HTTPDelayControl = 0, WifiPowerControl = 0;
int InternetQualityColor = 0, LastQuality = 0;
int FirstHour = 0, FirstMinute = 0;
boolean flag_QualityTime = false,flag_LastHourQuality = false;
//////////////////NEO PIXEL


void QualityReset(){
	
	
	// if already past an hour or the last hour was at 23 it will enter the if loop if were midnight
	if((hour() > FirstHour && FirstMinute == minute()) || (FirstHour == 23 && hour() == 0 && FirstMinute == minute())){
		
		flag_LastHourQuality = true;
		
		FirstHour = hour();
		
		//leds[5] = leds[4];
		LastQuality = InternetQualityColor;
		
		
		FailQuality = 60;
		SpeedQuality = 40;
		InternetQualityColor = 100;
		
		FastLED.show();
		
		//Resets all pixel variables
		PixelGETFail = 0; 
		PixelWifiFail = 0;
		PixelPingFail = 0;
		PixelNTPFail = 0;
		GETFailControl = 0; 
		WifiFailControl = 0; 
		PingFailControl = 0; 
		NTPFailControl = 0;
		PingDelayControl = 0; 
		NTPDelayControl = 0; 
		HTTPDelayControl = 0; 
		WifiPowerControl = 0;
	}
}

void QualityColor(){
	
	if(flag_QualityTime == false){
		FirstHour = hour();
		FirstMinute = minute();
		flag_QualityTime = true;
	}
	
	//#################################################
	// Fails counter
	//HTTP fail counter
	if(PixelGETFail == 1 && GETFailControl == 0){
		FailQuality--;
		GETFailControl++;
	}else
	if(PixelGETFail == 2 && GETFailControl == 1){
		FailQuality = FailQuality - 3;
		GETFailControl++;
	}else
	if(PixelGETFail == 4 && GETFailControl == 2){
		FailQuality = FailQuality - 5;
		GETFailControl++;
	}else
	if(PixelGETFail == 5 && GETFailControl == 3){
		FailQuality = FailQuality - 6;
		GETFailControl++;
	}
	
	//NTP fail counter
	if(PixelNTPFail == 1 && NTPFailControl == 0){
		FailQuality--;
		NTPFailControl++;
	}else
	if(PixelNTPFail == 2 && NTPFailControl == 1){
		FailQuality = FailQuality - 3;
		NTPFailControl++;
	}else
	if(PixelNTPFail == 4 && NTPFailControl == 2){
		FailQuality = FailQuality - 5;
		NTPFailControl++;
	}else
	if(PixelNTPFail == 5 && NTPFailControl == 3){
		FailQuality = FailQuality - 6;
		NTPFailControl++;
	}
	
	//Ping fail counter
	if(PixelPingFail == 1 && PingFailControl == 0){
		FailQuality--;
		PingFailControl++;
	}else
	if(PixelPingFail == 2 && PingFailControl == 1){
		FailQuality = FailQuality - 3;
		PingFailControl++;
	}else
	if(PixelPingFail == 4 && PingFailControl == 2){
		FailQuality = FailQuality - 5;
		PingFailControl++;
	}else
	if(PixelPingFail == 5 && PingFailControl == 3){
		FailQuality = FailQuality - 6;
		PingFailControl++;
	}
	
	//Wi-fi fail counter
	if(PixelWifiFail == 1 && WifiFailControl == 0){
		FailQuality--;
		WifiFailControl++;
	}else
	if(PixelWifiFail == 2 && WifiFailControl == 1){
		FailQuality = FailQuality - 3;
		WifiFailControl++;
	}else
	if(PixelWifiFail == 4 && WifiFailControl == 2){
		FailQuality = FailQuality - 5;
		WifiFailControl++;
	}else
	if(PixelWifiFail == 5 && WifiFailControl == 3){
		FailQuality = FailQuality - 6;
		WifiFailControl++;
	}
	//###########################################################################3

	//Wi-Fi Velocity
	
	// Ping Delay
	if(PixelPingDelay > 50 && PingDelayControl == 0){
		SpeedQuality--;
		PingDelayControl++;
	}else
	if(PixelPingDelay > 100 && PingDelayControl == 1){
		SpeedQuality = SpeedQuality - 2;
		PingDelayControl++;
	}else
	if(PixelPingDelay > 150 && PingDelayControl == 2){
		SpeedQuality = SpeedQuality - 3;
		PingDelayControl++;
	}else
	if(PixelPingDelay > 400 && PingDelayControl == 3){
		SpeedQuality = SpeedQuality - 4;
		PingDelayControl++;
	}
	
	//NTP Delay
	if(PixelNTPDelay > 50 && NTPDelayControl == 0){
		SpeedQuality--;
		NTPDelayControl++;
	}else
	if(PixelNTPDelay > 100 && NTPDelayControl == 1){
		SpeedQuality = SpeedQuality - 2;
		NTPDelayControl++;
	}else
	if(PixelNTPDelay > 150 && NTPDelayControl == 2){
		SpeedQuality = SpeedQuality - 3;
		NTPDelayControl++;
	}else
	if(PixelNTPDelay > 400 && PixelNTPDelay == 3){
		SpeedQuality = SpeedQuality - 4;
		NTPDelayControl++;
	}
	
	//HTTP Delay
	if(PixelHTTPDelay > 50 && HTTPDelayControl == 0){
		SpeedQuality--;
		HTTPDelayControl++;
	}else
	if(PixelHTTPDelay > 100 && HTTPDelayControl == 1){
		SpeedQuality = SpeedQuality - 2;
		HTTPDelayControl++;
	}else
	if(PixelHTTPDelay > 150 && HTTPDelayControl == 2){
		SpeedQuality = SpeedQuality - 3;
		HTTPDelayControl++;
	}else
	if(PixelHTTPDelay > 400 && PixelHTTPDelay == 3){
		SpeedQuality = SpeedQuality - 4;
		HTTPDelayControl++;
	}
	
	//Wifi Power
	if(PixelWifiPower < 75 && WifiPowerControl == 0){
		SpeedQuality--;
		WifiPowerControl++;
	}else
	if(PixelWifiPower < 60 && WifiPowerControl == 1){
		SpeedQuality = SpeedQuality - 2;
		WifiPowerControl++;
	}else
	if(PixelWifiPower < 40 && WifiPowerControl == 2){
		SpeedQuality = SpeedQuality - 3;
		WifiPowerControl++;
	}else
	if(PixelWifiPower < 20 && WifiPowerControl == 3){
		SpeedQuality = SpeedQuality - 4;
		WifiPowerControl++;
	}
	InternetQualityColor = FailQuality + SpeedQuality;
	//##############################################################################
	// Set colors
	/*
	//Fails color set
	if(FailQuality <= 60 && FailQuality >= 50){
		leds[3] = 0x0000FF; //Azul
	}else
	if(FailQuality <= 49 && FailQuality >= 40){
		leds[3] = 0x00FF00; //Verde
	}else
	if(FailQuality <= 39 && FailQuality >= 30){
		leds[3] = 0x99FF99; //Verde claro
	}else
	if(FailQuality <= 29 && FailQuality >= 10){
		leds[3] = 0xFFFF00; //Amarelo
	}else
	if(FailQuality <= 9 && FailQuality >= 5){
		leds[3] = 0xFF9100; // Laranja
	}else
	if(FailQuality <= 4 && FailQuality >= 1){
		leds[3] = 0xFF0000; //Vermelho
	}else
	if(FailQuality == 0){
		leds[3] = 0x990000; //Vermelho escuro
	}
	
	//Internet quality color set
	
	
	if(InternetQualityColor <= 100 && InternetQualityColor >= 90){
		leds[4] = 0x0000FF; //Azul
	}else
	if(InternetQualityColor <= 89 && InternetQualityColor >= 70){
		leds[4] = 0x00FF00; //Verde
	}else
	if(InternetQualityColor <= 69 && InternetQualityColor >= 40){
		leds[4]= 0x99FF99; //Verde claro
	}else
	if(InternetQualityColor <= 39 && InternetQualityColor >= 30){
		leds[4] = 0xFFFF00; //Amarelo
	}else
	if(InternetQualityColor <= 29 && InternetQualityColor >= 20){
		leds[4] = 0xFF9100; // Laranja
	}else
	if(InternetQualityColor <= 19 && InternetQualityColor >= 10){
		leds[4] = 0xFF0000; //Vermelho
	}else
	if(InternetQualityColor <= 9 && InternetQualityColor >= 0){
		leds[4] = 0x990000; //Vermelho escuro
	}
	
	FastLED.show(); */
	//####################################################################
}

void humidityPIXEL(float humi){
	if(humi >= 80.0 && humi <= 100.0){
		leds[1]=0x0000FF; //Azul escuro
	}else
	if(humi >= 60.0 && humi < 80.0){
		leds[1]=0x00EFFF; //Azul claro
	}else
	if(humi >= 40.0 && humi < 60.0){
		leds[1]=0x22FF00; //Verde
	}else
	if(humi >= 30.0 && humi < 40.0){
		leds[1]=0xFFEF00; //Amarelo
	}else
	if(humi >= 20.0 && humi < 30.0){
		leds[1]=0xFF9900; //Laranja
	}else
	if(humi >= 0.0 && humi < 20.0){
		leds[1]=0x000000; //Preto
	}
	FastLED.show();
}

void tempPIXEL(float temp){
	if(temp >= 0 && temp <= 5){
		leds[2]=0x0000FF; //Azul escuro
	}else
	if(temp >= 6 && temp <= 11){
		leds[2]=0x00EFFF; //Azul claro
	}else
	if(temp >= 12 && temp <= 18){
		leds[2]=0x66FF66; //Verde claro
	}else
	if(temp >= 19 && temp <= 22){
		leds[2]=0x22FF00; //Verde
	}else
	if(temp >= 23 && temp <= 26){
		leds[2]=0xFFEF00; //Amarelo
	}else
	if(temp >= 27 && temp <= 29){
		leds[2]=0xFF9900; //Laranja
	}else
	if(temp >= 30){
		leds[2]=0xFF0000; //Vermelho
	}
	FastLED.show();
}