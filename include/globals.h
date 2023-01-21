
#pragma once

#include <Arduino.h>
#include "databuffer.h"

#define WDT_TIMEOUT 10           // Watchdog time out x seconds
#define uS_TO_S_FACTOR 1000000UL //* Conversion factor for micro seconds to seconds */
#define SEALEVELPRESSURE_HPA (1013.25)

//--------------------------------------------------------------------------
// GPIO
//--------------------------------------------------------------------------
// 36                           --> ADC Channel 0 --> Poti, Soil Moist Sensor
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Device Settings
//--------------------------------------------------------------------------

// Define Applications
#define DEVICE_NAME  "nodemcu-01"
#define USE_MQTT 1

#if (USE_WEBSERVER || USE_CAYENNE || USE_MQTT || USE_WIFI)
#include <ESP8266WiFi.h>
extern WiFiClient wifiClient;
#endif


//--------------------------------------------------------------------------
// Wifi Settings
//--------------------------------------------------------------------------
const char ssid[] = "MrFlexi";
const char wifiPassword[] = "Linde-123";

#if (HAS_INA3221 || HAS_INA219 || USE_BME280)
#include "i2c_sensors.h"
#endif

#if (USE_MQTT)
#include "mqtt.h"
#endif


#if (USE_POTI || USE_SOIL_MOISTURE)
#include "poti.h"
#endif


#if (USE_DISTANCE_SENSOR_HCSR04)        // Ultrasonic distance sensor
#include "sensor_hcsr04.h"
#endif

#if (USE_SUN_POSITION)
#include "Helios.h"
#endif

#if (USE_PWM_SERVO)
#include "servo.h"
#endif