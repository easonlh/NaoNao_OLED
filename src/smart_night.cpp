#include "smart_night.h"
#include "servo_control.h"
#include "display.h"

SmartNight smartNight;

SmartNight::SmartNight()
  : enabled(true), state(NIGHT_IDLE), actionStart(0),
    actionDuration(NIGHT_SERVO_DURATION_MS),
    darkSpeed(NIGHT_DARK_SERVO_SPEED), brightSpeed(NIGHT_BRIGHT_SERVO_SPEED),
    targetBrightness(NIGHT_BRIGHTNESS_BRIGHT), lastWasDark(false) {}

void SmartNight::begin() {
  state = NIGHT_IDLE;
}

void SmartNight::update() {
  if (state == NIGHT_GOING_DARK || state == NIGHT_GOING_BRIGHT) {
    if (millis() - actionStart >= actionDuration) {
      servoCtrl.setSpeed(90);  // 停止舵机
      state = NIGHT_IDLE;
    }
  }
}

void SmartNight::onLightChanged(bool isDark) {
  if (!enabled) return;

  if (isDark && !lastWasDark) {
    // 天黑了
    state = NIGHT_GOING_DARK;
    targetBrightness = NIGHT_BRIGHTNESS_DARK;
    servoCtrl.setSpeed(darkSpeed);
    actionStart = millis();
    lastActivityTime = millis();
    screenSaverActive = false;
    lastWasDark = true;
  } else if (!isDark && lastWasDark) {
    // 天亮了
    state = NIGHT_GOING_BRIGHT;
    targetBrightness = NIGHT_BRIGHTNESS_BRIGHT;
    servoCtrl.setSpeed(brightSpeed);
    actionStart = millis();
    lastActivityTime = millis();
    screenSaverActive = false;
    lastWasDark = false;
  }
}

void SmartNight::setEnabled(bool e) { enabled = e; }
bool SmartNight::isEnabled() const { return enabled; }
NightMode SmartNight::getState() const { return state; }

const char* SmartNight::stateName() const {
  switch (state) {
    case NIGHT_GOING_DARK:   return "going_dark";
    case NIGHT_GOING_BRIGHT: return "going_bright";
    default:                 return "idle";
  }
}

void SmartNight::setDarkSpeed(int speed) { darkSpeed = speed; }
void SmartNight::setBrightSpeed(int speed) { brightSpeed = speed; }
void SmartNight::setDuration(unsigned long ms) { actionDuration = ms; }
int SmartNight::getTargetBrightness() const { return targetBrightness; }
