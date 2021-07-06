#ifndef XhockNEOPIXEL_h
#define XhockNEOPIXEL_h

#include <FastLED.h>

#define NUM_LEDS 11
#define DATA_PIN 2

FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
FastLED.setBrightness(30);

extern CRGB leds[NUM_LEDS];

//Functions

void QualityReset();

void QualityColor();

void humidityPIXEL(float humi);

void tempPIXEL(float temp);

#endif