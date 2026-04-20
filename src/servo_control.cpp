#include "servo_control.h"

ServoController::ServoController() : currentSpeed(90) {}

void ServoController::begin() {
  servo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servo.setPeriodHertz(50);  // SG90 标准 50Hz
  servo.write(90);  // 初始停止
  Serial.println("Servo initialized on GPIO18");
}

void ServoController::setSpeed(int speed) {
  if (speed < 0) speed = 0;
  if (speed > 180) speed = 180;
  currentSpeed = speed;
  servo.write(speed);
  Serial.printf("Servo speed set to: %d\n", speed);
}

void ServoController::setSpeedUs(int microseconds) {
  // 直接写入微秒值，绕过角度映射
  // 连续旋转舵机典型值: 1500=停止, 1000=全速反转, 2000=全速正转
  if (microseconds < 500) microseconds = 500;
  if (microseconds > 2500) microseconds = 2500;
  servo.writeMicroseconds(microseconds);
  currentSpeed = map(microseconds, 500, 2500, 0, 180);
  Serial.printf("Servo PWM set to: %d us\n", microseconds);
}

void ServoController::stop() {
  currentSpeed = 90;
  servo.write(90);
  Serial.println("Servo stopped");
}

int ServoController::getSpeed() {
  return currentSpeed;
}

bool ServoController::isAttached() {
  return servo.attached();
}

ServoController servoCtrl;
