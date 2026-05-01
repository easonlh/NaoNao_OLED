#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "display.h"
#include "animations.h"
#include "message_queue.h"
#include "http_server.h"
#include "weather_client.h"
#include "price_client.h"
#include "github_client.h"
#include "countdown_timer.h"
#include "mqtt_client_wrapper.h"
#include "servo_control.h"
#include "light_sensor.h"
#include "dht_sensor.h"
#include "pomodoro.h"
#include "sleep_tracker.h"
#include "smart_night.h"

// ==================== 硬件初始化 ====================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ==================== 全局变量 ====================
char displayMessage[MAX_MSG_LENGTH] = "等待 OpenClaw 指令...";
int scrollPos = 0;
unsigned long lastScrollTime = 0;

// 动画实例
BirdAnimation bird;
StarAnimation stars;
WeatherIconAnimation weatherIcon;
ScreenSaverAnimation screenSaver;

// NTP 同步
unsigned long lastNtpSyncTime = 0;
const unsigned long NTP_SYNC_INTERVAL = 3600000; // 1 小时

// 光敏舵机联动的非阻塞状态
static bool servoTriggerActive = false;
static unsigned long servoTriggerStart = 0;
static unsigned long servoTriggerDuration = 0;

// ==================== 辅助函数 ====================
int getMsgCount() {
  return msgQueue.getCount();
}

int getCurrentMsgIndex() {
  return 0;
}

// ==================== WiFi 连接 ====================
void connectWiFi() {
  Serial.println("\n========== WiFi Connect ==========");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);

  if (strlen(WIFI_SSID) == 0 || strcmp(WIFI_SSID, "YOUR_WIFI_SSID") == 0) {
    Serial.println("ERROR: WiFi SSID not configured!");
    Serial.println("Please set your WiFi credentials in .env file:");
    Serial.println("  cp .env.example .env");
    Serial.println("  source .env && pio run --target upload");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 25);
    u8g2.print("WiFi not configured!");
    u8g2.setCursor(10, 45);
    u8g2.print("Check .env file");
    u8g2.sendBuffer();
    return;
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(10, 25);
  u8g2.print("Connecting WiFi...");
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(10, 45);
  u8g2.print(WIFI_SSID);
  u8g2.sendBuffer();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  const int maxAttempts = 40;

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    if (attempts % 10 == 0 && attempts > 0) {
      wl_status_t status = WiFi.status();
      Serial.printf("\n[Status: %d] ", status);
      switch (status) {
        case WL_NO_SSID_AVAIL: Serial.print("SSID not found!"); break;
        case WL_CONNECT_FAILED: Serial.print("Password incorrect!"); break;
        case WL_CONNECTION_LOST: Serial.print("Connection lost!"); break;
        case WL_DISCONNECTED: Serial.print("Disconnected..."); break;
        default: Serial.print("Connecting..."); break;
      }
    }
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI (Signal): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 25);
    u8g2.print("WiFi Connected!");
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.setCursor(10, 45);
    u8g2.print("IP: ");
    u8g2.print(WiFi.localIP().toString());
    u8g2.sendBuffer();
  } else {
    Serial.printf("WiFi Failed! Status: %d\n", WiFi.status());
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 25);
    u8g2.print("WiFi Failed!");
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.setCursor(10, 45);
    u8g2.print("Check .env & reboot");
    u8g2.sendBuffer();
  }
}

// ==================== NTP 时间同步 ====================
void initNTP() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  Serial.println("Waiting for NTP time...");
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 30) {
    delay(500);
    attempts++;
  }

  if (attempts < 30) {
    Serial.println("Time synchronized!");
    lastNtpSyncTime = millis();
  } else {
    Serial.println("Time sync failed!");
  }
}

void syncNTP() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("NTP re-synced!");
    lastNtpSyncTime = millis();
  }
}

// ==================== 主程序 ====================
void setup() {
  Serial.begin(115200);

  // 初始化显示
  initDisplay(u8g2);

  // 显示启动画面
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(20, 35);
  u8g2.print("NaoNao");
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(15, 55);
  u8g2.print("Starting up...");
  u8g2.sendBuffer();

  // I2C 诊断扫描
  Serial.println("\n========== I2C Scan ==========");
  Wire.begin();
  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.printf("Found device at 0x%02X\n", addr);
      count++;
    }
  }
  if (count == 0) {
    Serial.println("NO I2C devices found!");
    Serial.println("Check: SDA=21, SCL=22, VCC, GND");
  }
  Serial.printf("Found %d I2C device(s)\n", count);
  Serial.println("================================\n");

  delay(1000);

  // 连接 WiFi
  connectWiFi();

  // 初始化 NTP
  if (WiFi.status() == WL_CONNECTED) {
    initNTP();
  }

  // 初始化消息队列
  msgQueue.addMessage(displayMessage);

  // 初始化 API 客户端
  weatherClient.begin();
  priceClient.begin();
  githubClient.begin();

  // 初始化 MQTT
  if (WiFi.status() == WL_CONNECTED) {
    mqttWrapper.begin();
    if (mqttWrapper.isConnected()) {
      Serial.println("MQTT connected");
    }
  }

  // 启动 HTTP 服务器
  naoNaoServer.begin();

  // 初始化舵机
  servoCtrl.begin();
  Serial.println("Servo initialized");

  // 初始化光敏传感器
  lightSensor.begin();

  // 初始化温湿度传感器
  dhtSensor.begin();

  // 初始化番茄钟
  pomodoro.begin();

  // 初始化睡眠追踪
  sleepTracker.begin();

  // 初始化智能夜灯
  smartNight.begin();

  Serial.println("Setup complete!");
}

void loop() {
  // 处理 HTTP 请求
  naoNaoServer.handleClient();

  // NTP 定期同步
  if (WiFi.status() == WL_CONNECTED &&
      millis() - lastNtpSyncTime > NTP_SYNC_INTERVAL) {
    syncNTP();
  }

  // 更新 API 客户端（非阻塞）
  weatherClient.update();
  priceClient.update();
  githubClient.update();

  // MQTT 处理
  mqttWrapper.loop();

  // 更新消息队列到显示变量
  Message* currentMsg = msgQueue.getCurrentMessage();
  if (currentMsg && strcmp(currentMsg->text, displayMessage) != 0) {
    strncpy(displayMessage, currentMsg->text, MAX_MSG_LENGTH - 1);
    displayMessage[MAX_MSG_LENGTH - 1] = '\0';
    scrollPos = 0;
    lastActivityTime = millis();
  }

  // 更新倒计时
  countdownTimer.update();

  // 更新光敏传感器
  lightSensor.update();

  // 更新温湿度传感器
  dhtSensor.update();

  // 更新番茄钟
  pomodoro.update();
  if (pomodoro.needsServoAlert()) {
    servoCtrl.setSpeed(POMO_SERVO_ALERT_SPEED);
    servoTriggerActive = true;
    servoTriggerStart = millis();
    servoTriggerDuration = POMO_SERVO_ALERT_MS;
    Serial.println("Pomodoro: Servo alert triggered!");
  }

  // 更新智能夜灯
  smartNight.update();

  // 光敏联动：智能夜灯 + 睡眠追踪
  if (lightSensor.justGotDark()) {
    smartNight.onLightChanged(true);
    sleepTracker.onLightChanged(true);
    lastActivityTime = millis();
    screenSaverActive = false;
    Serial.println("Light→dark: SmartNight + SleepTracker notified!");
  } else if (lightSensor.justGotBright()) {
    smartNight.onLightChanged(false);
    sleepTracker.onLightChanged(false);
    lastActivityTime = millis();
    screenSaverActive = false;
    Serial.println("Light→bright: SmartNight + SleepTracker notified!");
  }

  // 非阻塞：到时间自动停止舵机
  if (servoTriggerActive && millis() - servoTriggerStart >= servoTriggerDuration) {
    servoCtrl.setSpeed(90);
    servoTriggerActive = false;
    Serial.println("Servo auto-stopped");
  }

  // 更新动画
  bird.update();
  stars.update();
  weatherIcon.update();
  screenSaver.update();

  // 自动切换模式
  autoCycleMode();

  // 检查屏保
  checkScreenSaver();

  // 调整亮度
  adjustBrightness(u8g2);

  // 渲染显示
  u8g2.clearBuffer();

  if (screenSaverActive) {
    drawScreenSaver(u8g2);
    screenSaver.draw(u8g2);
  } else {
    switch (currentMode) {
      case MODE_CLOCK:
        drawClock(u8g2);
        bird.x = 75;
        bird.y = 8;
        bird.draw(u8g2);
        break;

      case MODE_DATE:
        drawDate(u8g2);
        break;

      case MODE_NOTIFICATION:
        drawNotification(u8g2);
        break;

      case MODE_WEATHER:
        drawWeatherInfo(u8g2);
        break;

      case MODE_SYSTEM:
        drawSystemStatus(u8g2);
        break;

      case MODE_COUNTDOWN:
        drawCountdownTimer(u8g2);
        break;

      case MODE_PRICE:
        drawPriceDisplay(u8g2);
        break;

      case MODE_GITHUB:
        drawGitHubStars(u8g2);
        break;

      case MODE_MQTT_MONITOR:
        drawMqttMonitor(u8g2);
        break;

      case MODE_LIGHT_SENSOR:
        drawLightSensor(u8g2);
        break;

      case MODE_DHT_SENSOR:
        drawDhtSensor(u8g2);
        break;

      case MODE_POMODORO:
        drawPomodoro(u8g2);
        break;

      case MODE_INDOOR_OUTDOOR:
        drawIndoorOutdoor(u8g2);
        break;

      case MODE_SLEEP:
        drawSleepTracker(u8g2);
        break;
    }
  }

  u8g2.sendBuffer();
  delay(10);
}
