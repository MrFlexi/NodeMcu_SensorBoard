#include "globals.h"
#include "display.h"

OBDISP obd;
ONE_BIT_DISPLAY *dp = NULL;

void dp_clear(void)
{
  dp->fillScreen(OBD_WHITE);
  dp->display();
  dp->setCursor(0, 0);
}

void setup_display()
{
  // The I2C SDA/SCL pins set to -1 means to use the default Wire library
  // If pins were specified, they would be bit-banged in software
  // This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs

  dp = new ONE_BIT_DISPLAY;
  dp->setI2CPins(SDA_PIN, SCL_PIN, RESET_PIN);
  dp->setBitBang(false);
  dp->I2Cbegin(MY_OLED);
  dp->allocBuffer(); // render all outputs to lib internal backbuffer
  dp->fillScreen(OBD_WHITE);
  dp->setTextWrap(false);
  dp->setScroll(true);
  dp->setTextColor(OBD_BLACK);
  dp->setFont(FONT_12x16);
  dp->printf("NTC Temp\r\n");
  dp->display();
}

void dislay_sleep()
{
  dp->setFont(FONT_NORMAL);
  dp->setCursor(0, 56);
  dp->printf("Sleep %d\r\n", dataBuffer.settings.sleep_time);
  dp->display();
  //delay(3000);
  // dp->setPower(false);
}

void dislay_update()
{
  dp->fillScreen(OBD_WHITE);
  dp->display();

  dp->setFont(FONT_16x32);
  dp->setCursor(16, 24);
  dp->printf("%01.1f C\r\n", dataBuffer.data.ntc_temp1);
  dp->display();
}

void display_boot()
{
  unsigned long ms;
  ms = millis();
  dp->printf("booting...\r\n");

} /* loop() */