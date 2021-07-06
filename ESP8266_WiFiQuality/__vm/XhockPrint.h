#ifndef XhockPrint_h
#define XhockPrint_h

#include <TimeLib.h>
extern String Str_Buffer;

void FilterStatus();

void SendToPrinter(int level, char VarName[] , float var);

void printTime();
#endif