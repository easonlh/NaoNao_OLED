#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>

// ==================== 配置区域 ====================

// WiFi 配置
const char* WIFI_SSID = "CMCC-cmsu";        // 修改为你的 WiFi 名称
const char* WIFI_PASSWORD = "w7i3q7sy"; // 修改为你的 WiFi 密码

// NTP 时间服务器配置
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET = 8 * 3600;  // 中国时区 GMT+8
const int DAYLIGHT_OFFSET = 0;     // 无夏令时

// 屏幕初始化 (I2C)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ==================== 全局变量 ====================

bool wifiConnected = false;
unsigned long lastSyncTime = 0;
const unsigned long SYNC_INTERVAL = 3600000; // 1 小时同步一次

// ==================== 函数声明 ====================

void connectWiFi();
void initTime();
void updateTime();
void displayTime();
void displayDate();
void displayStatus();

// ==================== 设置函数 ====================

void setup() {
  Serial.begin(115200);
  
  // 初始化屏幕
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  
  // 显示启动信息
  u8g2.setFont(u8g2_font_ncenB12_tr);
  u8g2.setCursor(20, 30);
  u8g2.print("Starting...");
  u8g2.sendBuffer();
  
  // 连接 WiFi
  connectWiFi();
  
  // 初始化时间
  initTime();
  
  Serial.println("Setup complete!");
}

// ==================== 主循环（增加 sendBuffer） ====================
void loop() {
  unsigned long currentTime = millis();
  
  if (wifiConnected && (currentTime - lastSyncTime > SYNC_INTERVAL)) {
    updateTime();
    lastSyncTime = currentTime;
  }
  
  u8g2.clearBuffer();  // 每帧开始清空缓存
  displayTime();
  displayDate();
  displayStatus();
  u8g2.sendBuffer();   // 关键：必须在这里统一推送，屏幕才会刷新
  
  delay(500); // 刷新太快没必要，0.5秒一次即可
}

// ==================== WiFi 连接函数（增加扫描诊断） ====================
void connectWiFi() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(0, 15);
  u8g2.print("Scanning WiFi...");
  u8g2.sendBuffer();

  Serial.println("Scanning for 2.4G networks...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found. Check your 2.4G settings.");
  } else {
    for (int i = 0; i < n; ++i) {
      Serial.print(WiFi.SSID(i));
      Serial.print(" ("); Serial.print(WiFi.RSSI(i)); Serial.println(")");
    }
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    
    // 在屏幕上显示进度条或点，给用户反馈
    u8g2.setCursor(0, 30);
    u8g2.print("Attempt: "); u8g2.print(attempts);
    u8g2.sendBuffer();
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi Connected!");
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi Failed. Try phone hotspot!");
  }
}
// ==================== 时间初始化函数 ====================

void initTime() {
  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
  
  Serial.println("Waiting for NTP time...");
  
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (attempts < 30) {
    Serial.println("\nTime synchronized!");
    lastSyncTime = millis();
  } else {
    Serial.println("\nTime sync failed!");
  }
}

// ==================== 时间更新函数 ====================

void updateTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("Time re-synced!");
    lastSyncTime = millis();
  }
}

// ==================== 显示时间函数 ====================

void displayTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  
  char timeStr[20];
  sprintf(timeStr, "%02d:%02d:%02d", 
          timeinfo.tm_hour, 
          timeinfo.tm_min, 
          timeinfo.tm_sec);
  
  // 显示时间 (大号字体)
  u8g2.setFont(u8g2_font_logisoso38_tf); // 38px 数字字体
  u8g2.setCursor(10, 35);
  u8g2.print(timeStr);
}

// ==================== 显示日期函数 ====================

void displayDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  
  const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* weekday = weekdays[timeinfo.tm_wday];
  
  char dateStr[30];
  sprintf(dateStr, "%04d-%02d-%02d %s", 
          timeinfo.tm_year + 1900, 
          timeinfo.tm_mon + 1, 
          timeinfo.tm_mday,
          weekday);
  
  // 显示日期
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(10, 55);
  u8g2.print(dateStr);
}

// ==================== 显示状态函数 ====================

void displayStatus() {
  // WiFi 状态指示
  u8g2.setFont(u8g2_font_6x12_tr);
  
  if (wifiConnected) {
    u8g2.setCursor(100, 12);
    u8g2.print("[WiFi]");
  } else {
    u8g2.setCursor(100, 12);
    u8g2.print("[No WiFi]");
  }
  
  // 秒点闪烁效果
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    u8g2.setFont(u8g2_font_logisoso38_tf);
    if (timeinfo.tm_sec % 2 == 0) {
      u8g2.setCursor(72, 35);
      u8g2.print(":");
      u8g2.setCursor(72, 35);
      u8g2.print(":");
    }
  }
}
