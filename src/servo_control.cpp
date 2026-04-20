#include "servo_control.h"

ServoController::ServoController() : currentSpeed(90) {}

void ServoController::begin() {
  servo.setPeriodHertz(50);  // SG90 标准 50Hz
  servo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
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
