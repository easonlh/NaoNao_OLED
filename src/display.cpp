#include "display.h"
#include "config.h"
#include "animations.h"
#include "message_queue.h"
#include "weather_client.h"
#include "price_client.h"
#include "github_client.h"
#include "countdown_timer.h"
#include "mqtt_client_wrapper.h"
#include "light_sensor.h"
#include <time.h>
#include <WiFi.h>

// 外部函数声明
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
extern WeatherIconAnimation weatherIcon;

void initDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setContrast(BRIGHTNESS_DAY);
}

void drawStatusBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawHLine(0, 11, 128);

  if (WiFi.status() == WL_CONNECTED) {
    u8g2.setCursor(2, 9);
    u8g2.print("WiFi");
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

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    u8g2.setCursor(80, 9);
    u8g2.print(timeStr);
  }

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
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(5, 38);
  u8g2.print(timeStr);

  if (timeinfo.tm_sec % 2 == 0) {
    u8g2.setCursor(68, 38);
    u8g2.print(":");
  }

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

  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(15, 28);
  u8g2.print(timeinfo.tm_year + 1900);
  u8g2.print("年");
  u8g2.print(months[timeinfo.tm_mon]);
  u8g2.print(timeinfo.tm_mday);
  u8g2.print("日");

  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(40, 52);
  u8g2.print(weekdays[timeinfo.tm_wday]);

  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d",
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(35, 63);
  u8g2.print(timeStr);

  drawStatusBar(u8g2);
}

void drawNotification(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  const int DISPLAY_WIDTH = 126;

  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(2, 15);
  u8g2.print("最新通知:");
  u8g2.drawHLine(0, 18, 128);

  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  int textWidth = u8g2.getUTF8Width(displayMessage);
  int lineCount = (textWidth / DISPLAY_WIDTH) + 1;

  if (lineCount <= 3) {
    u8g2.drawUTF8(5, 35, displayMessage);
  } else {
    if (scrollPos == 0 || scrollPos > DISPLAY_WIDTH) {
      scrollPos = DISPLAY_WIDTH;
    }
    if (millis() - lastScrollTime >= SCROLL_SPEED) {
      scrollPos -= 2;
      if (scrollPos < -textWidth) scrollPos = DISPLAY_WIDTH;
      lastScrollTime = millis();
    }
    u8g2.setCursor(scrollPos, 35);
    u8g2.print(displayMessage);
  }

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
  WeatherData& weather = weatherClient.getData();

  if (!weather.valid && strlen(CFG_WEATHER_API_KEY) == 0) {
    // API key not configured, show placeholder
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 25);
    u8g2.print("天气: 未配置 API");
    u8g2.setCursor(10, 45);
    u8g2.print("请在 .env 设置");
    return;
  }

  if (!weather.valid) {
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 30);
    u8g2.print("天气加载中...");
    drawStatusBar(u8g2);
    return;
  }

  // Temperature large
  char tempStr[16];
  snprintf(tempStr, sizeof(tempStr), "%.1f", weather.temperature);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(5, 40);
  u8g2.print(tempStr);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.print("C");

  // City and description
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(5, 15);
  u8g2.print(weather.city);

  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(5, 55);
  u8g2.print("Humidity: ");
  u8g2.print(weather.humidity);
  u8g2.print("%");

  // Weather icon
  int code = weather.weatherCode;
  if (code == 800) {
    weatherIcon.drawSunny(u8g2, 105, 30);
  } else if (code >= 801 && code <= 803) {
    weatherIcon.drawCloudy(u8g2, 100, 30);
  } else if (code >= 200 && code <= 699) {
    weatherIcon.drawRainy(u8g2, 100, 30);
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

void drawCountdownTimer(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  TimerState state = countdownTimer.getState();

  // Title
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(35, 14);
  if (state == TIMER_IDLE) {
    u8g2.print("倒计时");
  } else if (state == TIMER_RUNNING) {
    u8g2.print("倒计时中");
  } else if (state == TIMER_PAUSED) {
    u8g2.print("已暂停");
  } else if (state == TIMER_FINISHED) {
    u8g2.print("时间到!");
  }

  // Time display
  unsigned long remainingSec = countdownTimer.getRemainingSec();
  unsigned long mins = remainingSec / 60;
  unsigned long secs = remainingSec % 60;

  char timeStr[16];
  snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu", mins, secs);

  if (state == TIMER_FINISHED) {
    // Flash "DONE!"
    if (millis() / 500 % 2 == 0) {
      u8g2.setFont(u8g2_font_logisoso32_tf);
      u8g2.setCursor(10, 45);
      u8g2.print("DONE!");
    }
  } else {
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.setCursor(5, 45);
    u8g2.print(timeStr);
  }

  // Progress bar
  unsigned long totalSec = countdownTimer.getTotalSec();
  if (totalSec > 0) {
    int barWidth = (remainingSec * 120) / totalSec;
    u8g2.drawFrame(4, 56, 120, 6);
    u8g2.drawBox(5, 57, max(barWidth - 1, 0), 4);
  }

  drawStatusBar(u8g2);
}

void drawPriceDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  PriceData& price = priceClient.getData();

  if (!price.valid) {
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(20, 30);
    u8g2.print("价格加载中...");
    drawStatusBar(u8g2);
    return;
  }

  // Symbol
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(45, 16);
  u8g2.print(price.symbol);

  // Price - abbreviate if large
  char priceStr[24];
  if (price.priceUsd >= 10000) {
    snprintf(priceStr, sizeof(priceStr), "$%.1fK", price.priceUsd / 1000);
  } else {
    snprintf(priceStr, sizeof(priceStr), "$%.2f", price.priceUsd);
  }

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(15, 40);
  u8g2.print(priceStr);

  // 24h change
  u8g2.setFont(u8g2_font_ncenB10_tr);
  if (price.change24h >= 0) {
    u8g2.setCursor(30, 56);
    u8g2.print("+");
    u8g2.print(price.change24h, 1);
    u8g2.print("%");
  } else {
    u8g2.setCursor(30, 56);
    u8g2.print(price.change24h, 1);
    u8g2.print("%");
  }

  drawStatusBar(u8g2);
}

void drawGitHubStars(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  GitHubData& gh = githubClient.getData();

  if (!gh.valid) {
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(20, 30);
    u8g2.print("加载中...");
    drawStatusBar(u8g2);
    return;
  }

  // Repo name (truncate if too long)
  u8g2.setFont(u8g2_font_6x12_tr);
  int nameLen = strlen(gh.repoName);
  if (nameLen > 18) {
    char shortName[20];
    strncpy(shortName, gh.repoName, 15);
    shortName[15] = '.';
    shortName[16] = '.';
    shortName[17] = '.';
    shortName[18] = '\0';
    u8g2.setCursor(5, 16);
    u8g2.print(shortName);
  } else {
    u8g2.setCursor(5, 16);
    u8g2.print(gh.repoName);
  }

  // Star count
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(5, 48);
  u8g2.print(gh.starCount);

  // Language tag
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(5, 58);
  u8g2.print(gh.language);

  // Star icon
  int cx = 105, cy = 35;
  for (int i = 0; i < 5; i++) {
    float angle = (i * 72 - 90) * PI / 180;
    float outerX = cx + 10 * cos(angle);
    float outerY = cy + 10 * sin(angle);
    float innerAngle = ((i * 72) + 36 - 90) * PI / 180;
    float innerX = cx + 4 * cos(innerAngle);
    float innerY = cy + 4 * sin(innerAngle);
    u8g2.drawLine(cx, cy, (int)outerX, (int)outerY);
    u8g2.drawLine((int)outerX, (int)outerY, (int)innerX, (int)innerY);
    u8g2.drawLine((int)innerX, (int)innerY, cx, cy);
  }

  drawStatusBar(u8g2);
}

void drawMqttMonitor(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(2, 16);
  u8g2.print("MQTT");

  // Connection status
  u8g2.setFont(u8g2_font_6x12_tr);
  if (mqttWrapper.isConnected()) {
    u8g2.setCursor(45, 16);
    u8g2.print("Connected");
  } else {
    u8g2.setCursor(45, 16);
    u8g2.print("Disconnected");
  }

  // Subscribe topic
  u8g2.setCursor(2, 28);
  u8g2.print("Sub: ");
  u8g2.print(CFG_MQTT_SUBSCRIBE_TOPIC);

  // Last message
  Message* currentMsg = msgQueue.getCurrentMessage();
  if (currentMsg && msgQueue.getCount() > 0) {
    u8g2.setCursor(2, 42);
    u8g2.print("Msg: ");
    u8g2.setFont(u8g2_font_6x12_tr);
    // Truncate if too long
    int len = strlen(currentMsg->text);
    if (len > 14) {
      char shortMsg[16];
      strncpy(shortMsg, currentMsg->text, 11);
      shortMsg[11] = '.';
      shortMsg[12] = '.';
      shortMsg[13] = '.';
      shortMsg[14] = '\0';
      u8g2.print(shortMsg);
    } else {
      u8g2.print(currentMsg->text);
    }
  } else {
    u8g2.setCursor(2, 42);
    u8g2.print("No messages");
  }

  drawStatusBar(u8g2);
}

void drawLightSensor(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  int raw = lightSensor.getRawValue();
  LightState state = lightSensor.getState();

  // Title
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(30, 14);
  u8g2.print("光照传感器");

  // ADC value large
  char adcStr[16];
  snprintf(adcStr, sizeof(adcStr), "%d", raw);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(5, 45);
  u8g2.print(adcStr);

  // State label
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(100, 45);
  switch (state) {
    case LIGHT_DARK:    u8g2.print("暗"); break;
    case LIGHT_DIM:     u8g2.print("弱"); break;
    case LIGHT_NORMAL:  u8g2.print("常"); break;
    case LIGHT_BRIGHT:  u8g2.print("亮"); break;
    default:            u8g2.print("?"); break;
  }

  // Progress bar (0-4095 mapped to 0-120)
  int barW = map(raw, 0, 4095, 0, 118);
  u8g2.drawFrame(5, 56, 118, 6);
  u8g2.drawBox(6, 57, max(barW - 1, 0), 4);

  drawStatusBar(u8g2);
}

void drawScreenSaver(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
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

ScreenMode getNextEnabledMode(ScreenMode current) {
  for (int i = 1; i < NUM_DISPLAY_MODES; i++) {
    int next = (current + i) % NUM_DISPLAY_MODES;

    // Skip countdown mode if timer is not active
    if (next == MODE_COUNTDOWN && !countdownTimer.isRunning() &&
        countdownTimer.getState() != TIMER_PAUSED &&
        countdownTimer.getState() != TIMER_FINISHED) {
      continue;
    }

    // Skip MQTT monitor if MQTT is not enabled
    if (next == MODE_MQTT_MONITOR && !mqttWrapper.isConnected()) {
      continue;
    }

    // Skip price mode if data not ready
    if (next == MODE_PRICE && !priceClient.isReady()) {
      continue;
    }

    // Skip GitHub mode if not configured or data not ready
    if (next == MODE_GITHUB && !githubClient.isReady()) {
      continue;
    }

    // Skip weather mode if data not ready
    if (next == MODE_WEATHER && !weatherClient.isReady()) {
      continue;
    }

    // Skip light sensor mode if sensor not initialized
    if (next == MODE_LIGHT_SENSOR && lightSensor.getRawValue() == 0) {
      continue;
    }

    if (MODE_ENABLED[next]) {
      return (ScreenMode)next;
    }
  }
  return MODE_CLOCK; // Fallback
}

void autoCycleMode() {
  if (millis() - lastActivityTime > SCREEN_CYCLE_INTERVAL) {
    ScreenMode next = getNextEnabledMode(currentMode);
    switchMode(next);
    lastActivityTime = millis();
  }
}

void checkScreenSaver() {
  if (millis() - lastActivityTime > SCREENSAVER_TIMEOUT) {
    screenSaverActive = true;
  }
}

void adjustBrightness(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  int targetBrightness;

  // Use light sensor if available (raw > 0 means it's been read)
  if (lightSensor.getRawValue() > 0) {
    // Map sensor reading: dark (<300) → BRIGHTNESS_NIGHT, bright (>3000) → BRIGHTNESS_DAY
    targetBrightness = map(lightSensor.getRawValue(), 300, 3000, BRIGHTNESS_NIGHT, BRIGHTNESS_DAY);
    if (targetBrightness < BRIGHTNESS_NIGHT) targetBrightness = BRIGHTNESS_NIGHT;
    if (targetBrightness > BRIGHTNESS_DAY) targetBrightness = BRIGHTNESS_DAY;
  } else {
    // Fallback: time-based brightness
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      int hour = timeinfo.tm_hour;
      if (hour >= NIGHT_START_HOUR || hour < DAY_START_HOUR) {
        targetBrightness = BRIGHTNESS_NIGHT;
      } else {
        targetBrightness = BRIGHTNESS_DAY;
      }
    } else {
      targetBrightness = BRIGHTNESS_DAY;
    }
  }

  if (targetBrightness != currentBrightness) {
    currentBrightness = targetBrightness;
    u8g2.setContrast(currentBrightness);
  }
}
