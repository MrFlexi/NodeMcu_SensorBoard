#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#if (USE_MQTT)
#include <PubSubClient.h>


void mqtt_loop();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void mqtt_send();
void setup_mqtt();
void i2c_scan();

#endif
