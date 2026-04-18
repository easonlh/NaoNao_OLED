#include "display.h"
#include "config.h"
#include <time.h>
#include <WiFi.h>

// 外部函数声明（在 main.cpp 中定义）
extern int getMsgCount();
extern int getCurrentMsgIndex();

// 全局变量
ScreenMode currentMode = MODE_CLOCK;
unsigned long lastModeSwitchTime = 0;
unsigned long lastActivityTime = 0;
bool screenSaverActive = false;
static int currentBrightness = BRIGHTNESS_DAY;

// 外部变量声明
extern char displayMessage[];
extern int scrollPos;
extern unsigned long lastScrollTime;

void initDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setContrast(BRIGHTNESS_DAY);
}

void drawStatusBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawHLine(0, 11, 128);
  
  // WiFi 状态
  if (WiFi.status() == WL_CONNECTED) {
    u8g2.setCursor(2, 9);
    u8g2.print("WiFi");
    
    // 信号强度指示
    long rssi = WiFi.RSSI();
    int bars = 0;
    if (rssi > -50) bars = 4;
    else if (rssi > -60) bars = 3;
    else if (rssi > -70) bars = 2;
    else if (rssi > -80) bars = 1;
    
    for (int i = 0; i < bars; i++) {
      u8g2.drawBox(30 + i * 3, 9 - (i + 1) * 2, 2, (i + 1) * 2);
    }
  } else {
    u8g2.setCursor(2, 9);
    u8g2.print("No WiFi");
  }
  
  // 时间显示（小）
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    u8g2.setCursor(80, 9);
    u8g2.print(timeStr);
  }
  
  // 消息计数
  extern int getMsgCount();
  int msgCount = getMsgCount();
  if (msgCount > 0) {
    u8g2.setCursor(55, 9);
    u8g2.print("Msg:");
    u8g2.print(msgCount);
  }
}

void drawClock(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(10, 35);
    u8g2.print("Time Syncing...");
    return;
  }

  char timeStr[20];
  sprintf(timeStr, "%02d:%02d:%02d",
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);

  // 大号时间显示
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(5, 38);
  u8g2.print(timeStr);
  
  // 秒点闪烁
  u8g2.setFont(u8g2_font_logisoso32_tf);
  if (timeinfo.tm_sec % 2 == 0) {
    u8g2.setCursor(68, 38);
    u8g2.print(":");
  }

  // 星期
  const char* weekdays[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(30, 58);
  u8g2.print(weekdays[timeinfo.tm_wday]);
  
  drawStatusBar(u8g2);
}

void drawDate(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(10, 35);
    u8g2.print("Date N/A");
    return;
  }

  const char* weekdays[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
  const char* months[] = {"1月", "2月", "3月", "4月", "5月", "6月", 
                          "7月", "8月", "9月", "10月", "11月", "12月"};

  // 大日期显示
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(15, 28);
  u8g2.print(timeinfo.tm_year + 1900);
  u8g2.print("年");
  u8g2.print(months[timeinfo.tm_mon]);
  u8g2.print(timeinfo.tm_mday);
  u8g2.print("日");
  
  // 星期
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(40, 52);
  u8g2.print(weekdays[timeinfo.tm_wday]);
  
  // 时间（小）
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", 
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(35, 63);
  u8g2.print(timeStr);
  
  drawStatusBar(u8g2);
}

void drawNotification(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  extern char displayMessage[];
  extern int scrollPos;
  extern unsigned long lastScrollTime;

  const int DISPLAY_WIDTH = 126;

  // 通知标签
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(2, 15);
  u8g2.print("最新通知:");
  u8g2.drawHLine(0, 18, 128);

  // 多行通知文字（支持自动换行）
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  int textWidth = u8g2.getUTF8Width(displayMessage);
  int lineCount = (textWidth / DISPLAY_WIDTH) + 1;

  if (lineCount <= 3) {
    // 文字较短时自动换行显示
    u8g2.drawUTF8(5, 35, displayMessage);
  } else {
    // 文字过长时启用滚动模式
    if (scrollPos == 0 || scrollPos > DISPLAY_WIDTH) {
      scrollPos = DISPLAY_WIDTH; // 从右边开始
    }

    if (millis() - lastScrollTime >= SCROLL_SPEED) {
      scrollPos -= 2;
      if (scrollPos < -textWidth) {
        scrollPos = DISPLAY_WIDTH;
      }
      lastScrollTime = millis();
    }

    u8g2.setCursor(scrollPos, 35);
    u8g2.print(displayMessage);
  }

  // 消息队列指示器
  int msgCount = getMsgCount();
  int currentIdx = getCurrentMsgIndex();

  if (msgCount > 1) {
    for (int i = 0; i < min(msgCount, 10); i++) {
      int dotX = 55 + i * 6;
      if (i == currentIdx) {
        u8g2.drawBox(dotX, 56, 4, 4);
      } else {
        u8g2.drawCircle(dotX + 2, 58, 2, U8G2_DRAW_ALL);
      }
    }
  }

  drawStatusBar(u8g2);
}

void drawWeatherInfo(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  // 天气占位显示（可通过 HTTP API 更新）
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(10, 25);
  u8g2.print("天气: 晴");
  
  u8g2.setCursor(10, 45);
  u8g2.print("温度: 25°C");
  
  // 简单太阳图标
  int cx = 100, cy = 30, r = 8;
  u8g2.drawCircle(cx, cy, r, U8G2_DRAW_ALL);
  // 太阳光线
  for (int i = 0; i < 8; i++) {
    float angle = i * PI / 4;
    int x1 = cx + (r + 2) * cos(angle);
    int y1 = cy + (r + 2) * sin(angle);
    int x2 = cx + (r + 5) * cos(angle);
    int y2 = cy + (r + 5) * sin(angle);
    u8g2.drawLine(x1, y1, x2, y2);
  }
  
  drawStatusBar(u8g2);
}

void drawSystemStatus(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.setFont(u8g2_font_6x12_tr);
  
  int y = 20;
  u8g2.setCursor(5, y);
  u8g2.print("IP:");
  u8g2.print(WiFi.localIP().toString());
  
  y += 14;
  u8g2.setCursor(5, y);
  u8g2.print("RSSI:");
  u8g2.print(WiFi.RSSI());
  u8g2.print(" dBm");
  
  y += 14;
  u8g2.setCursor(5, y);
  u8g2.print("Uptime:");
  unsigned long uptimeSec = millis() / 1000;
  unsigned long hours = uptimeSec / 3600;
  unsigned long mins = (uptimeSec % 3600) / 60;
  u8g2.print(hours);
  u8g2.print("h");
  u8g2.print(mins);
  u8g2.print("m");
  
  y += 14;
  u8g2.setCursor(5, y);
  u8g2.print("Free Heap:");
  u8g2.print(ESP.getFreeHeap() / 1024);
  u8g2.print(" KB");
  
  drawStatusBar(u8g2);
}

void drawScreenSaver(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  // 简单的屏保：显示一个小时钟
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.setCursor(30, 40);
    u8g2.print(timeStr);
  }
}

void switchMode(ScreenMode mode) {
  currentMode = mode;
  lastModeSwitchTime = millis();
  lastActivityTime = millis();
  screenSaverActive = false;
}

void autoCycleMode() {
  if (millis() - lastActivityTime > SCREEN_CYCLE_INTERVAL) {
    int nextMode = (currentMode + 1) % 5;
    switchMode((ScreenMode)nextMode);
    lastActivityTime = millis();
  }
}

void checkScreenSaver() {
  if (millis() - lastActivityTime > SCREENSAVER_TIMEOUT) {
    screenSaverActive = true;
  }
}

void adjustBrightness(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int hour = timeinfo.tm_hour;
    int targetBrightness;
    
    if (hour >= NIGHT_START_HOUR || hour < DAY_START_HOUR) {
      targetBrightness = BRIGHTNESS_NIGHT;
    } else {
      targetBrightness = BRIGHTNESS_DAY;
    }
    
    if (targetBrightness != currentBrightness) {
      currentBrightness = targetBrightness;
      u8g2.setContrast(currentBrightness);
    }
  }
}
