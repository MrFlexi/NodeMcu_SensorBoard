#include "globals.h"
#include "display.h"
#include <Wire.h>

static const char TAG[] = __FILE__;

//-------------------------------------------------------------------------------
// 128*64 Pixel --> Center = 64*32
//-------------------------------------------------------------------------------
HAS_DISPLAY u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA );
 
U8G2LOG u8g2log;
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
int PageNumber = 0;
char sbuf[32];
uint8_t page_array[13];
uint8_t max_page_counter;
uint8_t page_counter = 0;


void displayRegisterPages()
{

  max_page_counter = 0;
  page_array[max_page_counter] = PAGE_SOILSENSOR;

  max_page_counter++;
  page_array[max_page_counter] = PAGE_NTC;

#if (USE_BME280)
  max_page_counter++;
  page_array[max_page_counter] = PAGE_BME280;
#endif


}


void setup_display(void)
{

  u8g2.begin();
  u8g2.setFont(u8g2_font_profont11_mf); // set the font for the terminal window
  u8g2.setContrast(255);
  //u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer); // connect to u8g2, assign buffer
  //u8g2log.setLineHeightOffset(0);                               // set extra space between lines in pixel, this can be negative
  //u8g2log.setRedrawMode(0);                                     // 0: Update screen with newline, 1: Update screen for every char
  //u8g2.enableUTF8Print();

  displayRegisterPages();
}

void drawSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic

  switch (symbol)
  {
  case SUN:
    u8g2.setFont(u8g2_font_open_iconic_weather_8x_t);
    u8g2.drawGlyph(x, y, 69);
    break;
  case SUN_CLOUD:
    u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
    u8g2.drawGlyph(x, y, 65);
    break;
  case CLOUD:
    u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
    u8g2.drawGlyph(x, y, 64);
    break;
  case RAIN:
    u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
    u8g2.drawGlyph(x, y, 67);
    break;
  case THUNDER:
    u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
    u8g2.drawGlyph(x, y, 67);
    break;
  case SLEEP:
    u8g2.setFont(u8g2_font_open_iconic_all_4x_t);
    u8g2.drawGlyph(x, y, 72);
    break;
  case ICON_NOTES:
    u8g2.setFont(u8g2_font_open_iconic_all_4x_t); // 32x32 Pixel
    u8g2.drawGlyph(x, y, 225);
    break;
  case ICON_BOOT:
    u8g2.setFont(u8g2_font_open_iconic_all_4x_t); // 32x32 Pixel
    u8g2.drawGlyph(x, y, 145);
    break;
  case ICON_SMILE:
    u8g2.setFont(u8g2_font_emoticons21_tr);
    u8g2.drawGlyph(x, y, 17);
    break;
  }
}

void showPage(int page)
{

  String availableModules = "";

  u8g2.clearBuffer();
  u8g2.clearDisplay();
  
  Serial.print("Display page: "); Serial.println(page);

  switch (page)
  {

  case PAGE_BOOT:
    // drawSymbol(30, 40, SUN);
    // drawSymbol(30, 40, ICON_SMILE);
    drawSymbol(48, 64, SUN) ; // place in the center of display

    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 15, "booting...");
    break;

  case PAGE_SYSTEM:
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 15);
    u8g2.setCursor(1, 45);
    u8g2.printf("Deep Sleep for: %3d", dataBuffer.settings.sleep_time);

    u8g2.setCursor(1, 60);
    u8g2.printf("BootCnt: %2d ", dataBuffer.data.bootCounter);
    break;

    case PAGE_SOILSENSOR:
    u8g2.setFont(u8g2_font_courB12_tr);
    u8g2.drawStr(1, 17, "Soilsensor");
    u8g2.setCursor(1, 35);
    u8g2.printf("Moist: %.1f  ", dataBuffer.data.soil_moisture );
    
    break;

  case PAGE_NTC:
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 15, "NTC Sensors");
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 30);
    u8g2.printf("Temp: %.1fC ", dataBuffer.data.ntc_temp1 );
    
    break;

   case PAGE_BME280:
    u8g2.setFont(u8g2_font_courB14_tr);
    u8g2.setCursor(1, 17);
    u8g2.printf("Temp %.1f C", dataBuffer.data.temperature);
    u8g2.setCursor(1, 35);
    u8g2.printf("Hum  %.1f%%",  dataBuffer.data.humidity);
    u8g2.setCursor(1, 52);
    u8g2.printf("DewP %.1f C",  dataBuffer.data.dewPoint);
    break;

  case PAGE_SLEEP:
    u8g2.setFont(u8g2_font_courB14_tr);
    u8g2.setCursor(1, 17);
    u8g2.printf("Temp %.1f C", dataBuffer.data.temperature);
    u8g2.setCursor(1, 35);
    u8g2.printf("Hum  %.1f%%",  dataBuffer.data.humidity);
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 64);
    u8g2.printf("sleeping for %i min", dataBuffer.settings.sleep_time);
    break;
  }

  u8g2.sendBuffer();
}

void t_moveDisplay(void)
{
  if (page_counter < max_page_counter)
  {
    page_counter++;
    //Serial.println("P counter: %d", page_counter);
  }
  else
  {
    page_counter = 0;
    Serial.println("P counter set to 0");
  }
  PageNumber = page_array[page_counter];

  // Refresh Display
  showPage(PageNumber);
}
