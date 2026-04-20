#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>
#include <Arduino.h>

// SG90 360度舵机配置
#define SERVO_PIN 18
#define SERVO_MIN_PULSE 500   // 最小脉冲宽度 (us)
#define SERVO_MAX_PULSE 2400  // 最大脉冲宽度 (us)

class ServoController {
private:
  Servo servo;
  int currentSpeed;  // 0-180, 90=停止

public:
  ServoController();

  void begin();
  void setSpeed(int speed);  // 0-180, 90=停止
  void stop();
  int getSpeed();
  bool isAttached();
};

extern ServoController servoCtrl;

#endif
