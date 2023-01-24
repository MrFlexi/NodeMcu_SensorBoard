#include <Arduino.h>
#include "globals.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

#define SENSOR_PIN A0
#define REFERENCE_RESISTANCE 10000 // 10K  NTC
#define NOMINAL_RESISTANCE  100000  // 100k Widerstand Spannungsteiler
#define NOMINAL_TEMPERATURE 25
#define B_VALUE 3950
#define SMOOTHING_FACTOR 10

#define LED D1          // Led in NodeMCU at pin D1

unsigned long lastMsg;
uint8_t cyclic_cnt = 0;

WiFiClient wifiClient;
Thermistor *thermistor = NULL;

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

void startDeepSleepMinutes(uint32_t value)
{
  uint64_t s_time_us = value * uS_TO_S_FACTOR * 60;
  ESP.deepSleep(s_time_us);
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

void led_blink_short(){
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
}

void led_blink_long(){
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);    // LED pin as output.
  

  setup_wifi();
  setup_mqtt();
  setup_ntc();
  led_blink_short();
}

void loop()
{

  const double celsius = thermistor->readCelsius();
  dataBuffer.data.ntc_temp1 = celsius;

  unsigned long now = millis();
  if (now - lastMsg > 10000)
  {
    cyclic_cnt++;
 
    

    lastMsg = now;
    Serial.print("Looping... ");Serial.println(cyclic_cnt);

    

    // Output of information
    Serial.print("Temperature: ");
    Serial.print(celsius);
    Serial.print(" C ");
    mqtt_send();
    mqtt_loop();
    led_blink_short();
    if (cyclic_cnt > 4)
      {
      Serial.print("going to sleep for 2 minutes");
      led_blink_long();
      startDeepSleepMinutes(2);
      }
  }
  delay(100);
}