#include "light_sensor.h"

LightSensor::LightSensor() : lastRawValue(0), lastState(LIGHT_UNKNOWN),
                              lastDarkTransition(0), wasDark(false) {}

void LightSensor::begin() {
  pinMode(LIGHT_SENSOR_A0, INPUT);
  pinMode(LIGHT_SENSOR_D0, INPUT);
  lastRawValue = analogRead(LIGHT_SENSOR_A0);
  lastState = getStateFromValue(lastRawValue);
  wasDark = lastState <= LIGHT_DIM;
  Serial.printf("Light sensor: A0=GPIO%d, D0=GPIO%d, initial=%d (%s)\n",
                LIGHT_SENSOR_A0, LIGHT_SENSOR_D0, lastRawValue, stateName());
}

void LightSensor::update() {
  int raw = analogRead(LIGHT_SENSOR_A0);
  lastRawValue = raw;

  LightState newState = getStateFromValue(raw);

  // 记录边缘变化
  bool nowDark = newState <= LIGHT_DIM;
  if (!wasDark && nowDark) {
    justBecameDark = true;
    lastDarkTransition = millis();
  }
  if (wasDark && !nowDark) {
    justBecameBright = true;
    lastDarkTransition = millis();
  }

  lastState = newState;
  wasDark = nowDark;
}

LightState LightSensor::getStateFromValue(int raw) {
  if (raw < 300) return LIGHT_DARK;
  if (raw < LIGHT_DARK_THRESHOLD) return LIGHT_DIM;
  if (raw < LIGHT_BRIGHT_THRESHOLD) return LIGHT_NORMAL;
  return LIGHT_BRIGHT;
}

int LightSensor::getRawValue() { return lastRawValue; }
LightState LightSensor::getState() { return lastState; }
bool LightSensor::isDark() { return lastState <= LIGHT_DIM; }
bool LightSensor::isBright() { return lastState >= LIGHT_BRIGHT; }

const char* LightSensor::stateName() {
  switch (lastState) {
    case LIGHT_DARK:    return "dark";
    case LIGHT_DIM:     return "dim";
    case LIGHT_NORMAL:  return "normal";
    case LIGHT_BRIGHT:  return "bright";
    default:            return "unknown";
  }
}

bool LightSensor::justGotDark() {
  bool v = justBecameDark;
  justBecameDark = false;
  return v;
}

bool LightSensor::justGotBright() {
  bool v = justBecameBright;
  justBecameBright = false;
  return v;
}

float LightSensor::getBrightnessFactor() {
  float factor = map(lastRawValue, 300, 3000, 10, 100) / 100.0;
  if (factor < 0.1) factor = 0.1;
  if (factor > 1.0) factor = 1.0;
  return factor;
}

LightSensor lightSensor;
