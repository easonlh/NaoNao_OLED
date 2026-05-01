#ifndef SMART_NIGHT_H
#define SMART_NIGHT_H

#include <Arduino.h>

// 智能夜灯联动状态
enum NightMode {
  NIGHT_IDLE,
  NIGHT_GOING_DARK,   // 正在进入夜晚模式
  NIGHT_GOING_BRIGHT  // 正在进入白天模式
};

// 配置
#define NIGHT_SERVO_DURATION_MS   2000  // 舵机动作时长
#define NIGHT_DARK_SERVO_SPEED    0     // 天黑舵机方向（顺时针=关窗）
#define NIGHT_BRIGHT_SERVO_SPEED  180   // 天亮舵机方向（逆时针=开窗）
#define NIGHT_BRIGHTNESS_DARK     30    // 夜间亮度
#define NIGHT_BRIGHTNESS_BRIGHT   255   // 白天亮度

class SmartNight {
public:
  SmartNight();

  void begin();
  void update();

  // 光照变化时调用
  void onLightChanged(bool isDark);

  // API 控制
  void setEnabled(bool enabled);
  bool isEnabled() const;
  NightMode getState() const;
  const char* stateName() const;

  // 配置
  void setDarkSpeed(int speed);
  void setBrightSpeed(int speed);
  void setDuration(unsigned long ms);

  // 获取当前亮度
  int getTargetBrightness() const;

private:
  bool enabled;
  NightMode state;
  unsigned long actionStart;
  unsigned long actionDuration;
  int darkSpeed;
  int brightSpeed;
  int targetBrightness;
  bool lastWasDark;

  void triggerServo(int speed, unsigned long duration);
};

extern SmartNight smartNight;

#endif  // SMART_NIGHT_H
