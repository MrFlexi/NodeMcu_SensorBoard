#include "globals.h"
#include "display.h"

static const char TAG[] = __FILE__;

//-------------------------------------------------------------------------------
// 128*64 Pixel --> Center = 64*32
//-------------------------------------------------------------------------------
HAS_DISPLAY u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA); // ESP32 Thing, HW I2C with pin remapping
U8G2LOG u8g2log;
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
int PageNumber = 0;
char sbuf[32];
uint8_t page_array[13];
uint8_t max_page_counter;
uint8_t page_counter = 0;

#if (HAS_TFT_DISPLAY)
// Depend TFT_eSPI library ,See  https://github.com/Bodmer/TFT_eSPI
// goto pio->libsdeps-->usb-->TFT_eSPI-->User_Setup_Select.h and comment line 22
//                                                               uncomment line 72
TFT_eSPI tft = TFT_eSPI();
// The scrolling area must be a integral multiple of TEXT_HEIGHT
#define TEXT_HEIGHT 16    // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 0  // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TOP_FIXED_AREA 16 // Number of lines in top fixed area (lines counted from top of screen)
#define YMAX 320          // Bottom of screen area

// The initial y coordinate of the top of the scrolling area
uint16_t yStart = TOP_FIXED_AREA;
// yArea must be a integral multiple of TEXT_HEIGHT
uint16_t yArea = YMAX - TOP_FIXED_AREA - BOT_FIXED_AREA;
// The initial y coordinate of the top of the bottom text line
uint16_t yDraw = YMAX - BOT_FIXED_AREA - TEXT_HEIGHT;

// Keep track of the drawing x coordinate
uint16_t xPos = 0;

#endif

void displayRegisterPages()
{

  max_page_counter = 0;
  page_array[max_page_counter] = PAGE_TBEAM;

  max_page_counter++;
  page_array[max_page_counter] = PAGE_NTC;

#if (USE_BME280)
  max_page_counter++;
  page_array[max_page_counter] = PAGE_SENSORS;
#endif


}

void log_display(String s)
{
  Serial.println(s);
  // Serial.print("Runmode:");Serial.println(dataBuffer.data.runmode);

#if (USE_SERIAL_BT)
  SerialBT.println(s);
#endif

  // if (dataBuffer.data.runmode < 1)
  //{
  //   u8g2log.print(s);
  //   u8g2log.print("\n");
  // }

#if (HAS_TFT_DISPLAY)
  tft.println(s);
#endif
}





void setup_display(void)
{

  u8g2.begin();
  u8g2.setFont(u8g2_font_profont11_mf); // set the font for the terminal window
  u8g2.setContrast(255);
  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer); // connect to u8g2, assign buffer
  u8g2log.setLineHeightOffset(0);                               // set extra space between lines in pixel, this can be negative
  u8g2log.setRedrawMode(0);                                     // 0: Update screen with newline, 1: Update screen for every char
  u8g2.enableUTF8Print();

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

  String IP_String = "";
  String availableModules = "";

  u8g2.clearBuffer();
  u8g2.clearDisplay();
  uint8_t icon = 0;

  // Serial.println("Display page: %d", page);

  switch (page)
  {

  case PAGE_BOOT:
    // drawSymbol(30, 40, SUN);
    // drawSymbol(30, 40, ICON_SMILE);
    drawSymbol(48, 32, ICON_BOOT); // place in the center of display

    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 60, "booting...");
    break;

  case PAGE_TBEAM:
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 15);
    u8g2.setCursor(1, 45);
    u8g2.printf("Deep Sleep for: %3d", dataBuffer.settings.sleep_time);

    u8g2.setCursor(1, 60);
    u8g2.printf("BootCnt: %2d ", dataBuffer.data.bootCounter);
    break;

  case PAGE_NTC:
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 15, "NTC Sensors");
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 30);
    u8g2.printf("Temp1: %.2fC ", dataBuffer.data.ntc_temp1 );
    
    break;

   case PAGE_SENSORS:
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 15, "BME 280");
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 30);
    u8g2.printf("Temp1: %.2fC ", dataBuffer.data.temperature, dataBuffer.data.humidity);
    u8g2.setCursor(1, 45);
    u8g2.printf("CPU Temp: %.2f C ", dataBuffer.data.cpu_temperature);
    u8g2.setCursor(1, 60);
    break;

  case PAGE_SLEEP:
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(1, 15, "Sleep");
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.setCursor(1, 64);
    u8g2.printf("Sleeping for %i min", dataBuffer.settings.sleep_time);
    drawSymbol(60, 12, THUNDER);

    break;
  }
}

void t_moveDisplay(void)
{

  if (page_counter < max_page_counter)
  {
    page_counter++;
    // Serial.println("P counter: %d", page_counter);
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
