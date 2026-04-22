#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include <Arduino.h>

// 屏幕模式枚举
enum ScreenMode {
  MODE_CLOCK,        // 0 - 时钟模式
  MODE_DATE,         // 1 - 日期模式
  MODE_NOTIFICATION, // 2 - 通知模式
  MODE_WEATHER,      // 3 - 天气模式（真实数据）
  MODE_SYSTEM,       // 4 - 系统状态模式
  MODE_COUNTDOWN,    // 5 - 倒计时模式
  MODE_PRICE,        // 6 - 加密货币价格模式
  MODE_GITHUB,       // 7 - GitHub Star 模式
  MODE_MQTT_MONITOR, // 8 - MQTT 消息模式
  MODE_LIGHT_SENSOR  // 9 - 光敏传感器模式
};

extern ScreenMode currentMode;
extern unsigned long lastModeSwitchTime;
extern unsigned long lastActivityTime;
extern bool screenSaverActive;

// 显示初始化
void initDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制函数
void drawClock(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawDate(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawNotification(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawWeatherInfo(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawSystemStatus(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawCountdownTimer(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawPriceDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawGitHubStars(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawMqttMonitor(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawLightSensor(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);
void drawScreenSaver(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 状态栏
void drawStatusBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 模式切换
void switchMode(ScreenMode mode);
void autoCycleMode();
ScreenMode getNextEnabledMode(ScreenMode current);
void checkScreenSaver();
void adjustBrightness(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

#endif
