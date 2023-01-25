//--------------------------------------------------------------------------
// U8G2 Display Setup  Definition
//--------------------------------------------------------------------------

#pragma once

#include "globals.h"
#include "databuffer.h"

#include <OneBitDisplay.h>
extern ONE_BIT_DISPLAY *dp;

// if your system doesn't have enough RAM for a back buffer, comment out
// this line (e.g. ATtiny85)
#define USE_BACKBUFFER

static uint8_t ucBackBuffer[1024];


// Use -1 for the Wire library default pins
// or specify the pin numbers to use with the Wire library or bit banging on any GPIO pins
// These are the pin numbers for the M5Stack Atom Grove port I2C (reversed SDA/SCL for straight through wiring)
#define SDA_PIN -1
#define SCL_PIN -1
//#define SDA_PIN 32
//#define SCL_PIN 26
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let OneBitDisplay figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 1

// Change these if you're using a different OLED display
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_FREQUENCY 400000L

#define MY_DISPLAY_FGCOLOR 1 // OLED_WHITE
#define MY_DISPLAY_BGCOLOR 0 // OLED_BLACK

//#define MY_OLED OLED_64x32
//#define OLED_WIDTH 64
//#define OLED_HEIGHT 32




void setup_display();
