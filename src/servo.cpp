#include <Arduino.h>
#include "globals.h"
#include "ServoEasing.hpp"

static const char TAG[] = "";

#define servo1_pin 0
#define servo2_pin 1
#define servo3_pin 2

#if (USE_PWM_SERVO)
ServoEasing Servo1(PCA9685_DEFAULT_ADDRESS, &Wire); // Servo 1
ServoEasing Servo2(PCA9685_DEFAULT_ADDRESS, &Wire); // Servo 2
ServoEasing Servo3(PCA9685_DEFAULT_ADDRESS, &Wire); // Leistungstransitor Solarpanel



void wireReadRegister(uint8_t reg)
{
  uint16_t value16;
  uint8_t value;
  Wire.beginTransmission(PCA9685_DEFAULT_ADDRESS);
  Wire.write(reg); // Register
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(PCA9685_DEFAULT_ADDRESS, (uint8_t)1);
  // value = ((Wire.read() << 8) | Wire.read());
  value = Wire.read();
  Serial.print("Reg:");
  Serial.print(reg, HEX);
  Serial.print("  Val:");
  Serial.println(value, BIN);
}

void readRegisters()
{
  Serial.println("Chip:");
  wireReadRegister(PCA9685_MODE1_REGISTER);
  Serial.println();
  Serial.println("Servo1:");
  wireReadRegister((PCA9685_FIRST_PWM_REGISTER + 2) + 4 * servo1_pin);
  wireReadRegister((PCA9685_FIRST_PWM_REGISTER + 3) + 4 * servo1_pin);
  Serial.println();
  Serial.println();

  Serial.println("Servo2:");
  wireReadRegister((PCA9685_FIRST_PWM_REGISTER + 2) + 4 * servo2_pin);
  wireReadRegister((PCA9685_FIRST_PWM_REGISTER + 3) + 4 * servo2_pin);
  Serial.println();
}

void setup_servo_pwm()
{
  uint8_t servo1_pos;
  uint8_t servo2_pos;
  // Servo1.I2CWriteByte(PCA9685_MODE1_REGISTER, _BV(PCA9685_MODE_1_SLEEP)); // go to sleep
  struct tm timeinfo;
  int speed = 30;

  Serial.println("Attaching Servos");
  Servo1.attach(servo1_pin, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE, -90, 90);
  Servo1.setEasingType(EASE_CUBIC_IN_OUT);

  Servo2.attach(servo2_pin);
  Servo2.setEasingType(EASE_CUBIC_IN_OUT);

  Servo3.attach(servo3_pin);
  Servo3.setEasingType(EASE_CUBIC_IN_OUT);
}

void servo_move_to(uint8_t servo_number, int8_t servo_position)
{
  //setup_servo_pwm();
  Serial.print("Turning Servo to "); Serial.println(servo_position);
  ServoEasing::ServoEasingArray[servo_number]->easeTo(servo_position, 200);      //blocking
  //ServoEasing::ServoEasingArray[servo_number]->startEaseTo(servo_position, 100, START_UPDATE_BY_INTERRUPT);   // non blocking
}



#endif