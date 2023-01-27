#include <Arduino.h>
#include "globals.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h> //Ticker Library

#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

#define SENSOR_PIN A0
#define REFERENCE_RESISTANCE 100000  // 100k Widerstand Spannungsteiler
#define NOMINAL_RESISTANCE    10000  // 10K  NTC 
#define NOMINAL_TEMPERATURE 25
#define B_VALUE 3950
#define SMOOTHING_FACTOR 5

#define LED D1 // Led in NodeMCU at pin D1
#define USE_DISPLAY 1

#define DEEP_SLEEP_MINUTES 2

unsigned long lastMsg;
uint8_t cyclic1m_cnt = 0;

Ticker cyclic1s;
Ticker cyclic10s;
Ticker cyclic1m;

WiFiClient wifiClient;
Thermistor *thermistor = NULL;

void startDeepSleepMinutes(uint32_t value)
{
  uint64_t s_time_us = value * uS_TO_S_FACTOR * 60;
  ESP.deepSleep(s_time_us);
}

void t_cyclic1s() // Ticker called every 10 seconds
{
  // Get temperature
  const double celsius = thermistor->readCelsius();
  dataBuffer.data.ntc_temp1 = celsius;
  Serial.print("Temp: ");
  Serial.println(dataBuffer.data.ntc_temp1);
}

void t_cyclic10s() // Ticker called every 10 seconds
{

  dislay_update();
}

void t_cyclic1m() // Ticker called every 1 minute
{
  // Send data via MQTT
  Serial.print("Mqtt send");
  mqtt_send();

      cyclic1m_cnt++;
    if (cyclic1m_cnt > 5)
    {
      Serial.print("going to sleep for 2 minutes");
      dislay_sleep();
      startDeepSleepMinutes(dataBuffer.settings.sleep_time);
    }
}

void setup_ticker()
{
  // Initialize Tickers
  cyclic1s.attach(1, t_cyclic1s); // Use attach_ms if you need time in
  cyclic10s.attach(10, t_cyclic10s); // Use attach_ms if you need time in
  cyclic1m.attach(60, t_cyclic1m);   // Use attach_ms if you need time in
}

// the setup function runs once when you press reset or power the board
void setup_ntc()
{
  Thermistor *originThermistor = new NTC_Thermistor(
      SENSOR_PIN,
      REFERENCE_RESISTANCE,
      NOMINAL_RESISTANCE,
      NOMINAL_TEMPERATURE,
      B_VALUE);
  thermistor = new SmoothThermistor(originThermistor, SMOOTHING_FACTOR);
}



void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  #if (USE_DISPLAY)
  setup_display();
  display_boot();
#endif
  dataBuffer.settings.sleep_time = DEEP_SLEEP_MINUTES;
  pinMode(LED, OUTPUT); // LED pin as output.
  setup_wifi();
  setup_mqtt();
  setup_ntc();

  setup_ticker();


}

void loop()
{
  unsigned long now = millis();
  if (now - lastMsg > 10000)
  {

  }
  mqtt_loop();
}