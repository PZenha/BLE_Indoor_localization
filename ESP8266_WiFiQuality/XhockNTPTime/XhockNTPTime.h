#ifndef XhockNTPTime_h
#define XhockNTPTime_h

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "XhockPrint.h"

extern unsigned int localPort;
extern time_t t;
extern WiFiUDP udp;
extern int once;
extern boolean flag_NTP;
extern int NTPfail;
extern unsigned int localPort;
extern int difTimes;
extern int timeDif;
extern int TotalDelay;
extern boolean NTPlisten;


void NTP_Time();
void ReqNtpTime();
void NTPisWaiting();
void timeDiference();

void sendNTPpacket(IPAddress& address);

#endif