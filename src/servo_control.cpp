#include "servo_control.h"

ServoController::ServoController() : currentAngle(0) {}

void ServoController::begin() {
  servo.setPeriodHertz(50);  // SG90 标准 50Hz
  servo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servo.write(0);  // 初始 0°
  currentAngle = 0;
  Serial.println("Servo initialized on GPIO18");
}

void ServoController::setAngle(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  currentAngle = angle;
  servo.write(angle);
  Serial.printf("Servo angle set to: %d deg\n", angle);
}

void ServoController::setAngleFloat(float angle) {
  // 连续平滑控制，支持小数角度
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  currentAngle = (int)angle;
  servo.write(angle);
  Serial.printf("Servo angle set to: %.1f deg\n", angle);
}

void ServoController::setAngleUs(int microseconds) {
  // 直接写入微秒值，按卖家公式：pulsewidth = angle * 12 + 500
  if (microseconds < 500) microseconds = 500;
  if (microseconds > 2400) microseconds = 2400;
  servo.writeMicroseconds(microseconds);
  currentAngle = map(microseconds, 500, 2400, 0, 180);
  Serial.printf("Servo PWM set to: %d us (~%d deg)\n", microseconds, currentAngle);
}

int ServoController::getAngle() {
  return currentAngle;
}

bool ServoController::isAttached() {
  return servo.attached();
}

ServoController servoCtrl;
