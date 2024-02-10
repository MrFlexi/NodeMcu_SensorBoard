//--------------------------------------------------------------------------
// U8G2 Display Setup  Definition
//--------------------------------------------------------------------------

#ifndef _FLED_H
#define _FLED_H

#include <Wire.h>
#include <Arduino.h>

#include "FastLED.h"

#define FASTLED_DATA_PIN D8
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    12
#define BRIGHTNESS  10



void setup_FastLed(void); 
void LED_boot(void);
void LED_sunset(void);
void LED_sunrise(void);
void LED_HeatColor(uint8_t heatIndex);

void LED_showSleepCounter(void);
void LED_deepSleep(void);
void LED_showDegree(int i);
void LED_wakeup(void);
void LED_on(uint8_t value);
void LED_off();

#endif