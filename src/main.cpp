#include <Arduino.h>
#include "globals.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h> //Ticker Library

#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

#include <SPI.h>
#include <Wire.h>

#define SENSOR_PIN A0
#define REFERENCE_RESISTANCE 10000 // 10K  NTC
#define NOMINAL_RESISTANCE 100000  // 10k Widerstand Spannungsteiler
#define NOMINAL_TEMPERATURE 25
#define B_VALUE 3950
#define SMOOTHING_FACTOR 10

#define LED D1 // Led in NodeMCU at pin D1
#define USE_DISPLAY 1

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

unsigned long lastMsg;
uint8_t cyclic_cnt = 0;

Ticker cyclic10s;
Ticker cyclic1m;

WiFiClient wifiClient;
Thermistor *thermistor = NULL;


// --------------------- Laser distance sensor -------------------------------
#include <VL53L1X.h>
VL53L1X sensor;



// ---------------------------- MPU 6050 -------------------------------
#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN D7 // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13       // (Arduino is 13, Teensy is 11, Teensy++ is 6)
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"
MPU6050 mpu;

bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion quat;        // [w, x, y, z]         quaternion container
VectorInt16 aa;      // [x, y, z]            accel sensor measurements
VectorInt16 gy;      // [x, y, z]            gyro sensor measurements
VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z]            gravity vector
float euler[3];      // [psi, theta, phi]    Euler angle container
float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = {'$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n'};

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
IRAM_ATTR void dmpDataReady()
{
  mpuInterrupt = true;
}

void getSensorValues()
{
  // Get NTC temperature
  const double celsius = thermistor->readCelsius();
  dataBuffer.data.ntc_temp1 = celsius;
  Serial.print("NTC Temperature: ");
  Serial.println(celsius);

// Get BME280
#if (USE_BME280)
  float temp = bme.readTemperature();
  if (temp < 60)
  {
    dataBuffer.data.temperature = temp;
  }
  dataBuffer.data.humidity = bme.readHumidity();

  float hum = bme.readHumidity();

  float dewPoint = 243.04 * (log(hum / 100.0) + ((17.625 * temp) / (243.04 + temp))) / (17.625 - log(hum / 100.0) - ((17.625 * temp) / (243.04 + temp)));

  Serial.print("Dew Point");
  Serial.println(dewPoint);
  dataBuffer.data.dewPoint = dewPoint;

#endif

// Get Soil Moisture
#if (USE_SOILSENSOR)
  Serial.print("ADC Value: ");
  Serial.println(analogRead(A0));
  dataBuffer.data.soil_moisture = analogRead(A0);
#endif
}

void t_cyclic10s() // Ticker called every 10 seconds
{

  getSensorValues();
  t_moveDisplay();
}

void t_cyclic1m() // Ticker called every 1 minute
{
  // Send data via MQTT
  Serial.println("Mqtt send");
  mqtt_send();
}

void setup_ticker()
{
  // Initialize Tickers
  cyclic10s.attach(10, t_cyclic10s); // Use attach_ms if you need time in
  cyclic1m.attach(15, t_cyclic1m);   // Use attach_ms if you need time in
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

void led_blink_short()
{
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
}

void led_blink_long()
{
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
}

void setup_mpu()
{
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // wait for ready
  // Serial.println(F("\nSend any character to begin DMP programming and demo: "));
  // while (Serial.available() && Serial.read()); // empty buffer
  // while (!Serial.available());                 // wait for data
  // while (Serial.available() && Serial.read()); // empty buffer again

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(51);
  mpu.setYGyroOffset(8);
  mpu.setZGyroOffset(21);
  mpu.setXAccelOffset(1150);
  mpu.setYAccelOffset(-50);
  mpu.setZAccelOffset(1060);
  // make sure it worked (returns 0 if so)
  if (devStatus == 0)
  {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    Serial.println();
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;
    Serial.print("MPU Temp:");
    Serial.println(mpu.getTemperature());

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void loop_mpu()
{
  // if programming failed, don't try to do anything
  if (!dmpReady)
    return;
  // read a packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
  { // Get the Latest packet

#ifdef OUTPUT_READABLE_QUATERNION
    // display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    Serial.print("quat\t");
    Serial.print(q.w);
    Serial.print("\t");
    Serial.print(q.x);
    Serial.print("\t");
    Serial.print(q.y);
    Serial.print("\t");
    Serial.println(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetEuler(euler, &q);
    Serial.print("euler\t");
    Serial.print(euler[0] * 180 / M_PI);
    Serial.print("\t");
    Serial.print(euler[1] * 180 / M_PI);
    Serial.print("\t");
    Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&quat, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &quat);
    mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180 / M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180 / M_PI);
    Serial.print("\t");
   
    float deg_r =  ypr[2] * 180 / M_PI;
    float deg_z =  ypr[0] * 180 / M_PI;
    Serial.print(deg_r);
    Serial.println();

    LED_showDegree(deg_z);
    servo_move_to(0,deg_z);
    /*
      mpu.dmpGetAccel(&aa, fifoBuffer);
      Serial.print("\tRaw Accl XYZ\t");
      Serial.print(aa.x);
      Serial.print("\t");
      Serial.print(aa.y);
      Serial.print("\t");
      Serial.print(aa.z);
      mpu.dmpGetGyro(&gy, fifoBuffer);
      Serial.print("\tRaw Gyro XYZ\t");
      Serial.print(gy.x);
      Serial.print("\t");
      Serial.print(gy.y);
      Serial.print("\t");
      Serial.print(gy.z);
    */
   

#endif

#ifdef OUTPUT_READABLE_REALACCEL
    // display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    Serial.print("areal\t");
    Serial.print(aaReal.x);
    Serial.print("\t");
    Serial.print(aaReal.y);
    Serial.print("\t");
    Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    Serial.print("aworld\t");
    Serial.print(aaWorld.x);
    Serial.print("\t");
    Serial.print(aaWorld.y);
    Serial.print("\t");
    Serial.println(aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
    // display quaternion values in InvenSense Teapot demo format:
    teapotPacket[2] = fifoBuffer[0];
    teapotPacket[3] = fifoBuffer[1];
    teapotPacket[4] = fifoBuffer[4];
    teapotPacket[5] = fifoBuffer[5];
    teapotPacket[6] = fifoBuffer[8];
    teapotPacket[7] = fifoBuffer[9];
    teapotPacket[8] = fifoBuffer[12];
    teapotPacket[9] = fifoBuffer[13];
    Serial.write(teapotPacket, 14);
    teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
#endif

    // blink LED to indicate activity
    blinkState = !blinkState;
    // digitalWrite(LED_PIN, blinkState);
  }
}

void setup_VL53L1X()
{
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Lidar: failed to detect and initialize sensor!");
  }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(50);
}

void setup()
{
  Serial.begin(115200);
  i2c_scan();
  Wire.setClock(400000);
#if (USE_FASTLED)
  setup_FastLed();
#endif

#if (HAS_INA3221 || HAS_INA219 || USE_BME280)
  Serial.print("-----------  Setup I2c devices   -----------");
  setup_i2c_sensors();
#endif
  setup_ntc();
  setup_mpu();

  setup_VL53L1X();

  getSensorValues();

#if (USE_DISPLAY)
  setup_display();
  showPage(PAGE_BME280);
#endif

#if (USE_PWM_SERVO)
  setup_servo_pwm();
  servo_move_to(0,45);
  servo_move_to(1,90);
#endif

  setup_wifi();
  setup_mqtt();

  setup_ticker();
}

void loop_VL53L1X()
{
  sensor.read();

  Serial.print("Lindar distance in mm: ");
  Serial.print(sensor.ranging_data.range_mm);
  Serial.print("\tstatus: ");
  Serial.print(VL53L1X::rangeStatusToString(sensor.ranging_data.range_status));
  Serial.print("\tpeak signal: ");
  Serial.print(sensor.ranging_data.peak_signal_count_rate_MCPS);
  Serial.print("\tambient: ");
  Serial.print(sensor.ranging_data.ambient_count_rate_MCPS);

  Serial.println();
}

void loop()
{
  unsigned long now = millis();
  if (now - lastMsg > 10000)
  {
    cyclic_cnt++;
    lastMsg = now;
    if (cyclic_cnt > 10)
    {
      Serial.print("going to sleep for 2 minutes");
      dataBuffer.settings.sleep_time = 2;

#if (USE_DISPLAY)
      showPage(PAGE_SLEEP);
#endif
      LED_sunset();
      startDeepSleepMinutes(2);
    }
  }
  loop_mpu();
  loop_VL53L1X();
  mqtt_loop();
}