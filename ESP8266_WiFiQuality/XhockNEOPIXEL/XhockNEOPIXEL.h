#ifndef XhockNEOPIXEL_h
#define XhockNEOPIXEL_h

#include <FastLED.h>

#define NUM_LEDS 11
#define DATA_PIN 2

extern CRGB leds[NUM_LEDS];

extern int PixelPingDelay, PixelNTPDelay, PixelHTTPDelay, PixelWifiPower;
extern int PixelGETFail, PixelWifiFail,PixelPingFail,PixelNTPFail;
extern int InternetQualityColor;
extern int FailQuality,SpeedQuality,LastQuality;
extern boolean flag_LastHourQuality;



//Functions

void QualityReset();

void QualityColor();

void humidityPIXEL(float humi);

void tempPIXEL(float temp);

#endif