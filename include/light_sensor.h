#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

// 光敏电阻引脚（ESP32 ADC 仅支持 GPIO32-39）
#define LIGHT_SENSOR_A0 34  // 模拟输出
#define LIGHT_SENSOR_D0 4   // 数字输出（阈值触发）

// 光照阈值（ADC 0-4095，需根据实际校准）
#define LIGHT_DARK_THRESHOLD 800    // 低于此值判定为暗
#define LIGHT_BRIGHT_THRESHOLD 2000 // 高于此值判定为亮

enum LightState {
  LIGHT_UNKNOWN = 0,
  LIGHT_DARK,     // 暗
  LIGHT_DIM,      // 弱光
  LIGHT_NORMAL,   // 正常
  LIGHT_BRIGHT    // 强光
};

class LightSensor {
private:
  int lastRawValue;
  LightState lastState;
  unsigned long lastDarkTransition;
  bool wasDark;
  bool justBecameDark = false;
  bool justBecameBright = false;

  LightState getStateFromValue(int raw);

public:
  LightSensor();

  void begin();
  void update();

  int getRawValue();          // ADC 原始值 0-4095
  LightState getState();      // 光照状态
  bool isDark();              // 是否暗
  bool isBright();            // 是否亮
  const char* stateName();    // 状态名称

  bool justGotDark();         // 刚刚变暗（一次触发）
  bool justGotBright();       // 刚刚变亮（一次触发）

  // 自动亮度因子（0.0-1.0）
  float getBrightnessFactor();
};

extern LightSensor lightSensor;

#endif
