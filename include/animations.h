#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <U8g2lib.h>
#include <Arduino.h>

// 小鸟动画状态
struct BirdAnimation {
  int wingState;                  // 0=翅膀向上, 1=平展, 2=向下
  unsigned long lastToggleTime;
  int x, y;                       // 位置
  
  BirdAnimation() : wingState(0), lastToggleTime(0), x(70), y(10) {}
  void update();
  void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
};

// 星星动画
struct StarAnimation {
  unsigned long lastToggleTime;
  int starPositions[5];
  int starPhases[5];
  
  StarAnimation();
  void update();
  void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int offsetX, int offsetY);
};

// 天气图标动画
struct WeatherIconAnimation {
  unsigned long lastToggleTime;
  int phase;
  
  WeatherIconAnimation() : lastToggleTime(0), phase(0) {}
  void update();
  void drawSunny(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y);
  void drawCloudy(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y);
  void drawRainy(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y);
};

// WiFi 信号动画
struct WifiSignalAnimation {
  unsigned long lastToggleTime;
  int phase;
  
  WifiSignalAnimation() : lastToggleTime(0), phase(0) {}
  void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y, int rssi);
};

// 心跳动画
struct HeartbeatAnimation {
  unsigned long lastToggleTime;
  int phase;
  
  HeartbeatAnimation() : lastToggleTime(0), phase(0) {}
  void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y);
};

// 屏保动画（漂浮的星星）
struct ScreenSaverAnimation {
  struct FloatingStar {
    int x, y;
    int dx, dy;
    int size;
  };
  
  FloatingStar stars[8];
  unsigned long lastUpdate;
  
  ScreenSaverAnimation();
  void update();
  void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
};

#endif
