//--------------------------------------------------------------------------
// U8G2 Display Setup  Definition
//--------------------------------------------------------------------------

#pragma once

#include "globals.h"
#include "databuffer.h"

#define HAS_DISPLAY U8G2_SSD1306_128X64_NONAME_F_HW_I2C 
//#define HAS_DISPLAY U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C

#define SUN	0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4
#define SLEEP 10
#define ICON_NOTES 11
#define ICON_SMILE 12
#define ICON_BOOT  13

#define PAGE_TBEAM 0
#define PAGE_SENSORS 1
#define PAGE_NTC 2

#define PAGE_SLEEP 20         // Pages > 20 are not in the picture loop
#define PAGE_BOOT 21

// assume 4x6 font, define width and height
#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 6

#include <U8g2lib.h>

extern HAS_DISPLAY u8g2;             // 
extern U8G2LOG u8g2log;             // Create a U8g2log object
extern int PageNumber; 

// allocate memory
extern uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

void log_display(String s);
void setup_display(void);
void t_moveDisplayRTOS(void *pvParameters);
void t_moveDisplay(void);

void setup_display_new();
void dp_printf(uint16_t x, uint16_t y, uint8_t font, uint8_t inv,
               const char *format, ...);

void showPage(int page);
void drawSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol);

