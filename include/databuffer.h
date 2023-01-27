//--------------------------------------------------------------------------
// U8G2 Display Setup  Definition
//--------------------------------------------------------------------------

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

typedef struct
{

  float temperature;        // temperature signal
  float humidity;           // humidity signal
  float pressure;           // pressure signal
  float cpu_temperature;    // raw temperature signal
  double ntc_temp1;         // raw temperature signal
  float raw_temperature;    // raw temperature signal
  float raw_humidity;       // raw humidity signal
  uint8_t aliveCounter;     // aliveCounter
  uint8_t sleepCounter;     // aliveCounter
  uint16_t bootCounter;
  uint32_t freeheap;        // free memory
  uint16_t potentiometer_a;   //
  uint16_t adc0;              // Voltage in mVolt
  bool potentiometer_a_changed;
  uint8_t servo1;
  uint8_t servo2;
  double sun_azimuth;
  double sun_elevation;
  float panel_voltage = 0;
  float panel_current = 0;
  float bus_voltage = 0;
  float bus_current = 0;
  float bat_voltage = 0;
  float bat_charge_current = 0;
  float bat_discharge_current = 0;
  float soil_moisture = 0;
  uint16_t hcsr04_distance = 0;     // in cm
  bool distance_changed;
  tm timeinfo;
  bool ble_device_connected = false;
} deviceStatus_t;


typedef struct
{
  uint8_t sleep_time; 
} deviceSettings_t;

class DataBuffer
{
  public:
    DataBuffer();
    void set( deviceStatus_t input );
    void get();
    String to_json();
    String to_json_web();
    deviceStatus_t data ;
    deviceSettings_t settings;
    const char* getError() const { return _error; }
  private:
    char* _error;   
};

extern DataBuffer dataBuffer;