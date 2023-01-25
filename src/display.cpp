#include "globals.h"
#include "display.h"

OBDISP obd;
ONE_BIT_DISPLAY *dp = NULL;

void dp_clear(void) {
  dp->fillScreen(MY_DISPLAY_BGCOLOR);
  dp->display();
  dp->setCursor(0, 0);
}

void setup_display() {
int rc;
// The I2C SDA/SCL pins set to -1 means to use the default Wire library
// If pins were specified, they would be bit-banged in software
// This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs

rc = obdI2CInit(&obd, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz
  if (rc != OLED_NOT_FOUND)
  {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};
    obdFill(&obd, OBD_WHITE, 1);
    obdWriteString(&obd, 0,0,0,msgs[rc], FONT_8x8, OBD_BLACK, 1);
    obdSetBackBuffer(&obd, ucBackBuffer);
    delay(2000);
  }

dp = new ONE_BIT_DISPLAY;
dp->setI2CPins(SDA_PIN, SCL_PIN, RESET_PIN);
  dp->setBitBang(false);
  dp->I2Cbegin(MY_OLED, OLED_ADDR, OLED_FREQUENCY);
  dp->allocBuffer(); // render all outputs to lib internal backbuffer
  dp->setTextWrap(false);
  dp_clear();
  dp->printf("NTC 1: v%f.2\r\n", dataBuffer.data.ntc_temp1);


}

void loop_display() {
  // put your main code here, to run repeatedly:
int i, x, y;
char szTemp[32];
unsigned long ms;

  obdFill(&obd, OBD_WHITE, 1);
  obdWriteString(&obd, 0,28,0,(char *)"OLED Demo", FONT_8x8, OBD_BLACK, 1);
  obdWriteString(&obd, 0,0,8,(char *)"Written by Larry Bank", FONT_6x8, OBD_WHITE, 1);
  obdWriteString(&obd, 0,0,24,(char *)"**Demo**", FONT_16x32, OBD_BLACK, 1);
  delay(2000);
  
 // Pixel and line functions won't work without a back buffer
#ifdef USE_BACKBUFFER
  obdFill(&obd, OBD_WHITE, 1);
  obdWriteString(&obd, 0,0,0,(char *)"Backbuffer Test", FONT_8x8,OBD_BLACK,1);
  obdWriteString(&obd, 0,0,8,(char *)"3000 Random dots", FONT_8x8,OBD_BLACK,1);
  delay(2000);
  obdFill(&obd, OBD_WHITE,1);
  ms = millis();
  for (i=0; i<3000; i++)
  {
    x = random(OLED_WIDTH);
    y = random(OLED_HEIGHT);
    obdSetPixel(&obd, x, y, 1, 1);
  }
  ms = millis() - ms;
  sprintf(szTemp, "%dms", (int)ms);
  obdWriteString(&obd, 0,0,0,szTemp, FONT_8x8, OBD_BLACK, 1);
  obdWriteString(&obd, 0,0,8,(char *)"Without backbuffer", FONT_6x8,OBD_BLACK,1);
  delay(2000);
  obdFill(&obd, OBD_WHITE,1);
  ms = millis();
  for (i=0; i<3000; i++)
  {
    x = random(OLED_WIDTH);
    y = random(OLED_HEIGHT);
    obdSetPixel(&obd, x, y, 1, 0);
  }
  obdDumpBuffer(&obd, NULL);
  ms = millis() - ms;
  sprintf(szTemp, "%dms", (int)ms);
  obdWriteString(&obd, 0,0,0,szTemp, FONT_8x8, OBD_BLACK, 1);
  obdWriteString(&obd, 0,0,8,(char *)"With backbuffer", FONT_6x8,OBD_BLACK,1);
  delay(2000);
  obdFill(&obd, OBD_WHITE, 1);
  obdWriteString(&obd, 0,0,0,(char *)"Backbuffer Test", FONT_8x8,OBD_BLACK,1);
  obdWriteString(&obd, 0,0,8,(char *)"96 lines", FONT_8x8,OBD_BLACK,1);
  delay(2000);
  ms = millis();
  for (x=0; x<OLED_WIDTH-1; x+=2)
  {
    obdDrawLine(&obd, x, 0, OLED_WIDTH-x, OLED_HEIGHT-1, OBD_BLACK, 1);
  }
  for (y=0; y<OLED_HEIGHT-1; y+=2)
  {
    obdDrawLine(&obd, OLED_WIDTH-1,y, 0,OLED_HEIGHT-1-y, OBD_BLACK, 1);
  }
  ms = millis() - ms;
  sprintf(szTemp, "%dms", (int)ms);
  obdWriteString(&obd, 0,0,0,szTemp, FONT_8x8, OBD_BLACK, 1);
  obdWriteString(&obd, 0,0,8,(char *)"Without backbuffer", FONT_6x8,OBD_BLACK,1);
  delay(2000);
  obdFill(&obd, OBD_WHITE,1);
  ms = millis();
  for (x=0; x<OLED_WIDTH-1; x+=2)
  {
    obdDrawLine(&obd, x, 0, OLED_WIDTH-1-x, OLED_HEIGHT-1, OBD_BLACK, 0);
  }
  for (y=0; y<OLED_HEIGHT-1; y+=2)
  {
    obdDrawLine(&obd, OLED_WIDTH-1,y, 0,OLED_HEIGHT-1-y, OBD_BLACK, 0);
  }
  obdDumpBuffer(&obd, ucBackBuffer);
  ms = millis() - ms;
  sprintf(szTemp, "%dms", (int)ms);
  obdWriteString(&obd, 0,0,0,szTemp, FONT_8x8, OBD_BLACK, 1);
  obdWriteString(&obd, 0,0,8,(char *)"With backbuffer", FONT_6x8,OBD_BLACK,1);
  delay(2000);
#endif
} /* loop() */