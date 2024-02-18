#include "globals.h"
#include "FastLed.h"



#if (USE_FASTLED)

CRGB leds[NUM_LEDS];

uint8_t val_old;
uint8_t val_poti_old;

uint8_t poti_scale[12] = {8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7};



void task_LED_loop(void *parameter)
{
  DataBuffer *locdataBuffer;
  locdataBuffer = (DataBuffer *)parameter;

  int i = 0;
  int val_poti_old = 0;

  while (1)
  {

    #if (FASTLED_SHOW_POTI)
    int val = map(locdataBuffer->data.potentiometer_a, 0, 4096, 0, 11);

    i = poti_scale[val];

    if (val_poti_old != i)
    {
      FastLED.clear();
      leds[i] = CRGB::Green;
      FastLED.show();
      val_poti_old = i;      
    }
    #endif
  }
}


void setup_FastLed()
{
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, FASTLED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

 FastLED.clear();
 FastLED.show();
 leds[4] = CRGB::Blue;
 leds[10] = CRGB::Blue;
 FastLED.show();

  }

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16; //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}

void LED_HeatColor(uint8_t heatIndex)
{
    // HeatColors_p is a gradient palette built in to FastLED
    // that fades from black to red, orange, yellow, white
    // feel free to use another palette or define your own custom one
    CRGB color = ColorFromPalette(HeatColors_p, heatIndex);

    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    //ESP_LOGI(TAG, "Heat Color: %i",heatIndex );
}

void LED_sunset()
{

  for (uint8_t heatIndex = 255; heatIndex > 0; heatIndex--)
  {
    // HeatColors_p is a gradient palette built in to FastLED
    // that fades from black to red, orange, yellow, white
    // feel free to use another palette or define your own custom one
    CRGB color = ColorFromPalette(HeatColors_p, heatIndex);

    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    delay(10);
  }
  FastLED.clear();

}

void LED_sunrise()
{

  for (uint8_t heatIndex = 0; heatIndex < 255; heatIndex++)
  {
    // HeatColors_p is a gradient palette built in to FastLED
    // that fades from black to red, orange, yellow, white
    // feel free to use another palette or define your own custom one
    CRGB color = ColorFromPalette(LavaColors_p, heatIndex);

    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    delay(5);
  }
  
}

void LED_on(uint8_t value)
{
if ( value > FastLED.getBrightness() )
{
  for (uint8_t heatIndex = FastLED.getBrightness(); heatIndex < value; heatIndex++)
  {
    // HeatColors_p is a gradient palette built in to FastLED
    // that fades from black to red, orange, yellow, white
    // feel free to use another palette or define your own custom one
    CRGB color = ColorFromPalette(LavaColors_p, heatIndex);

    FastLED.setBrightness(heatIndex);

    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(1);
  }
}
else
{
  for (uint8_t heatIndex = FastLED.getBrightness(); heatIndex > value; heatIndex--)
  {
  
    CRGB color = ColorFromPalette(LavaColors_p, heatIndex);

    FastLED.setBrightness(heatIndex);
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(1);
  }
}

  fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
}

void LED_off()
{

  for (uint8_t heatIndex = 255; heatIndex >= 0; heatIndex--)
  {
    // HeatColors_p is a gradient palette built in to FastLED
    // that fades from black to red, orange, yellow, white
    // feel free to use another palette or define your own custom one
    CRGB color = ColorFromPalette(LavaColors_p, heatIndex);

    FastLED.setBrightness(heatIndex);

    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(1);
  }
  FastLED.clear();
}

void LED_poti()
{
  int i = 0;

  int val = map(dataBuffer.data.potentiometer_a, 0, 4096, 0, 11);
  i = poti_scale[val];

  if (val_poti_old != i)
  {
    FastLED.clear();
    leds[i] = CRGB::Green;
    FastLED.show();
    val_poti_old = i;
  }
}

void LED_bootcount()
{
  int i = 0;
  FastLED.clear();

  if (dataBuffer.data.bootCounter <= NUM_LEDS)
    i = dataBuffer.data.bootCounter;
  else
    i = NUM_LEDS;

  for (int l = 0; l < i; l++)
  {
    leds[l] = CRGB::Yellow;
  }
  FastLED.show();
}

void LED_deepSleep()
{
  FastLED.clear();
}

void LED_showDegree(int i)
{
  int val = map(i, -90, 90, 0, 6);
  Serial.println(val);
  int led1 = val+1;
  int led2 = led1+6;
  if (led2 >=12) 
  {
    led2=led2-12;
  }
  FastLED.clear();
  leds[led1] = CRGB::Red;
  leds[led2] = CRGB::Red;
  FastLED.show();
}

void LED_boot()
{

  for (int i = 0; i < 2000; i++)
  {
    pride();
    FastLED.show();
  }
}



void LED_wakeup()
{
    LED_sunrise();
}

#endif


