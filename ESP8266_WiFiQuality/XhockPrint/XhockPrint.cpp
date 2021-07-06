#include "arduino.h"
#include "XhockPrint.h"

String Str_Buffer ="";  //Buffer to print
int Filter = 99;  //Select debug level


void FilterStatus(){
	String strFilter = "";
	boolean start = false;
	if(Serial.available() > 0){
		char filterData = Serial.read();
		strFilter += filterData;
		
		if(filterData == '0' || filterData == '1' || filterData == '2' || filterData == '3' || filterData == '4' || filterData == '5' || filterData == '6' || filterData == '7' || filterData == '8' || filterData == '9'){
			start = true;
		}
		
		int filterExist = strFilter.lastIndexOf('filter ');
		
		if(strFilter.charAt(filterExist + 1) >= 0 && start == true){
			Filter = (int)strFilter.charAt(filterExist + 1) - '0';
			Serial.println("");
			Serial.print("New filter set to: ");
			Serial.println(Filter);
		}
	}
}

void SendToPrinter(int level, char VarName[] , float var){
	if(Filter < level){return;}
	printTime();
	Serial.print(Str_Buffer);
	if(level > 0){
		Serial.print(" - ");
		Serial.print(VarName);
		Serial.print(var);
	}
}

void printTime(){
	Serial.println(""); Serial.println("");
	if(hour() < 10){
		Serial.print("0");
	}
	Serial.print(hour());	Serial.print(":");
	if(minute() < 10){
		Serial.print("0");
	}
	Serial.print(minute());	Serial.print(":");
	if(second() < 10){
		Serial.print("0");
	}
	Serial.print(second());
	Serial.print(" - ");
}