#include "globals.h"
#include "mqtt.h"
#include <Wire.h>

static const char TAG[] = __FILE__;

#if (USE_MQTT)
PubSubClient MqttClient(wifiClient);

//const char *mqtt_server = "192.168.1.100"; // Raspberry
const char *mqtt_server = "85.209.49.65"; // Netcup
const char *mqtt_topic = "mrflexi/device/";
const char *mqtt_topic_mosi = "/mosi";
const char *mqtt_topic_miso = "/miso";
const char *mqtt_topic_irq = "/miso/irq";
const char *mqtt_topic_traincontroll = "/TrainControll/";

long lastMsgAlive = 0;
long lastMsgDist = 0;


void doConcat(const char *a, const char *b, const char *c, char *out)
{
  strcpy(out, a);
  strcat(out, b);
  strcat(out, c);
}

void mqtt_loop()
{

  // MQTT Connection
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!MqttClient.connected())
    {
      Serial.println("MQTT Client not connected ");
      reconnect();
    }
    MqttClient.loop();
  }
  else
  {
    //ESP_LOGE(TAG, "Wifi not connected ");
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{


  String message = "";

  //Serial.println("MQTT message topic %s", topic);

  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  // Convert Payload to a JSON object
  StaticJsonDocument<500> doc;
  deserializeJson(doc, message);
  serializeJsonPretty(doc, Serial);

  // Check if there is a incomming command

  const char *action_in = doc["command"]["action"];
  const char *value = doc["command"]["value"];

  String action = String(action_in);
  //Serial.println(" action: %s  value: %s", action.c_str(), value);

  if (action == "sleep_time")
  {
    dataBuffer.settings.sleep_time = atoi(value);
    //Serial.println("MQTT: sleep time %2d", dataBuffer.settings.sleep_time);
  }

#if (USE_PWM_SERVO)
    if (action == "servo")
  {
    const char *number = doc["command"]["number"];
    const char *position = doc["command"]["position"];
    
    uint8_t servo_number = atoi(number);
    uint8_t servo_position = atoi(position);
    Serial.println("MQTT: move servo &d to %3d degree", servo_number , servo_position);
  }  
#endif


}

void reconnect()
{

  char topic_in[40];
  // build MQTT topic e.g.  mrflexi/device/soil_moisture-01/data
  doConcat(mqtt_topic, DEVICE_NAME, mqtt_topic_mosi, topic_in);

  int i = 0;

  if (WiFi.status() == WL_CONNECTED)
  {
    // Loop until we're reconnected
    while (!MqttClient.connected())
    {
     Serial.println("Attempting MQTT connection...");
      // Attempt to connect
      if (MqttClient.connect(DEVICE_NAME))
      {
        
        MqttClient.publish(mqtt_topic, "connected");
        MqttClient.subscribe(topic_in);
        Serial.println("Subscribed to topic");
        //Serial.println("Subscribed to topic %s", topic_in);
      }
      else
      {
        Serial.println( "failed, rc=");
        Serial.print(MqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(500);
      }
      i++;
    }
  }
  else
    Serial.println("No Wifi connection");
}

void setup_mqtt()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    MqttClient.setServer(mqtt_server, 1883);
    MqttClient.setCallback(callback);
    MqttClient.setBufferSize(500);
    MqttClient.setSocketTimeout(120);

    if (!MqttClient.connected())
    {
      reconnect();
    }
    Serial.println("MQTT connected");
    MqttClient.publish(mqtt_topic, "ESP32 is alive...");

  }
}

void mqtt_send()
{
  char topic_out[50];

  // build MQTT topic e.g.  mrflexi/device/TBEAM-01/data
  doConcat(mqtt_topic, DEVICE_NAME, mqtt_topic_miso, topic_out);

  if (MqttClient.connected())
  {
    Serial.println("MQTT send: "); Serial.println(topic_out);
    //Serial.println("Payload: %s", dataBuffer.to_json().c_str());
    MqttClient.publish(topic_out, dataBuffer.to_json().c_str());
  }
  else
  {
    Serial.println("Mqtt not connected");
  }
}


#endif

#define SSD1306_PRIMARY_ADDRESS (0x3D)
#define SSD1306_SECONDARY_ADDRESS (0x3C)
#define PCA9685_PRIMARY_ADDRESS (0x40)
#define BME_PRIMARY_ADDRESS (0x77)
#define BME_SECONDARY_ADDRESS (0x76)
#define AXP192_PRIMARY_ADDRESS (0x34)
#define MCP_24AA02E64_PRIMARY_ADDRESS (0x50)
#define QUECTEL_GPS_PRIMARY_ADDRESS (0x10)
#define ADXL345 (0x53)
#define IP5306_ADDR (0X75)


int i2c_scan(void) {

    Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      }
  }
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
