#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include <Arduino.h>

// 屏幕模式枚举
enum ScreenMode {
  MODE_CLOCK,       // 时钟模式
  MODE_DATE,        // 日期模式
  MODE_NOTIFICATION,// 通知模式
  MODE_WEATHER,     // 天气信息模式
  MODE_SYSTEM       // 系统状态模式
};

extern ScreenMode currentMode;
extern unsigned long lastModeSwitchTime;
extern unsigned long lastActivityTime;
extern bool screenSaverActive;

// 显示初始化
void initDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制时钟界面
void drawClock(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制日期界面
void drawDate(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制通知界面
void drawNotification(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制天气信息界面
void drawWeatherInfo(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制系统状态界面
void drawSystemStatus(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制屏保
void drawScreenSaver(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 绘制顶部状态栏
void drawStatusBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

// 切换屏幕模式
void switchMode(ScreenMode mode);

// 自动切换屏幕模式
void autoCycleMode();

// 检查并激活屏保
void checkScreenSaver();

// 调整亮度（根据时间段）
void adjustBrightness(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2);

#endif
