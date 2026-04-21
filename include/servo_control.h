#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>
#include <Arduino.h>

// SG90 180°舵机配置（卖家例程 PWM 参数：pulsewidth = angle * 12 + 500）
#define SERVO_PIN 18
#define SERVO_MIN_PULSE 500   // 0° 脉冲宽度 (us)
#define SERVO_MAX_PULSE 2400  // 180° 脉冲宽度 (us)

class ServoController {
private:
  Servo servo;
  int currentAngle;  // 0-180 度

public:
  ServoController();

  void begin();
  void setAngle(int angle);  // 0-180 度
  void setAngleUs(int microseconds);  // 直接写入微秒值
  void setAngleFloat(float angle);    // 浮点角度
  int getAngle();
  bool isAttached();
};

extern ServoController servoCtrl;

#endif
