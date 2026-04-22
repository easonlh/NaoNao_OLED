#include "servo_control.h"

ServoController::ServoController() : currentSpeed(90) {}

void ServoController::begin() {
  servo.setPeriodHertz(50);  // SG90 标准 50Hz
  servo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servo.writeMicroseconds(1500);  // 发送精确 1500us 停止信号
  currentSpeed = 90;
  Serial.println("Servo initialized on GPIO18 (stopped)");
}

void ServoController::setSpeed(int speed) {
  if (speed < 0) speed = 0;
  if (speed > 180) speed = 180;
  currentSpeed = speed;
  if (speed == 90) {
    servo.writeMicroseconds(1500);  // 停止位用精确 1500us
  } else {
    servo.write(speed);
  }
  Serial.printf("Servo speed set to: %d\n", speed);
}

void ServoController::setSpeedUs(int us) {
  if (us < 500) us = 500;
  if (us > 2400) us = 2400;
  servo.writeMicroseconds(us);
  currentSpeed = map(us, 500, 2400, 0, 180);
  Serial.printf("Servo PWM set to: %d us\n", us);
}

int ServoController::getSpeed() {
  return currentSpeed;
}

bool ServoController::isAttached() {
  return servo.attached();
}

ServoController servoCtrl;
