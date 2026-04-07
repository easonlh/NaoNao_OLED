#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"

// ==================== 硬件配置 ====================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
WebServer server(80);

// ==================== 滚动逻辑全局变量 ====================
String displayMessage = "等待 OpenClaw 指令...";
int scrollPos = 0;           // 当前滚动位移
unsigned long lastScrollTime = 0;
const int SCROLL_SPEED = 30; // 滚动速度（毫秒/像素），数值越小越快
const int DISPLAY_WIDTH = 80; // 文字显示区域的宽度（预留右侧给兔子）

// ==================== 大鸟绘制模块（翅膀会动） ====================
const int BIRD_X = 70;
const int BIRD_Y = 10;
int wingState = 0;
unsigned long lastWingToggleTime = 0;

// 绘制展翅飞翔的大鸟
void drawBird(int wingPos) {
  // wingPos: 0=翅膀向上，1=翅膀平展，2=翅膀向下
  
  // === 身体（椭圆形） ===
  for (int x = 18; x <= 28; x++) {
    u8g2.drawPixel(BIRD_X + x, BIRD_Y + 15);
    u8g2.drawPixel(BIRD_X + x, BIRD_Y + 35);
  }
  for (int y = 16; y <= 34; y++) {
    u8g2.drawPixel(BIRD_X + 18, BIRD_Y + y);
    u8g2.drawPixel(BIRD_X + 28, BIRD_Y + y);
  }
  // 身体填充
  for (int x = 19; x <= 27; x++) {
    for (int y = 18; y <= 32; y++) {
      u8g2.drawPixel(BIRD_X + x, BIRD_Y + y);
    }
  }
  
  // === 头部（圆形） ===
  for (int x = 28; x <= 38; x++) {
    for (int y = 12; y <= 22; y++) {
      if ((x-33)*(x-33) + (y-17)*(y-17) <= 25) {
        u8g2.drawPixel(BIRD_X + x, BIRD_Y + y);
      }
    }
  }
  
  // === 眼睛 ===
  u8g2.setDrawColor(0);
  u8g2.drawPixel(BIRD_X + 34, BIRD_Y + 15);
  u8g2.drawPixel(BIRD_X + 35, BIRD_Y + 15);
  u8g2.setDrawColor(1);
  
  // === 鸟喙（三角形） ===
  for (int i = 0; i <= 8; i++) {
    for (int j = 0; j <= (8-i)/2; j++) {
      u8g2.drawPixel(BIRD_X + 38 + i, BIRD_Y + 18 + j);
    }
  }
  
  // === 翅膀（根据状态变化） ===
  if (wingPos == 0) {
    // 翅膀向上展开
    for (int i = 0; i <= 15; i++) {
      int height = 12 - i * 4 / 5;
      for (int j = 0; j <= height; j++) {
        u8g2.drawPixel(BIRD_X + 5 + i, BIRD_Y + 15 - j);
        u8g2.drawPixel(BIRD_X + 35 - i, BIRD_Y + 15 - j);
      }
    }
    // 翅膀羽毛细节
    for (int i = 0; i <= 12; i += 3) {
      u8g2.drawPixel(BIRD_X + 8 + i, BIRD_Y + 8 - i/3);
      u8g2.drawPixel(BIRD_X + 32 - i, BIRD_Y + 8 - i/3);
    }
  } else if (wingPos == 1) {
    // 翅膀平展
    for (int i = 0; i <= 18; i++) {
      int width = 8 + i / 3;
      for (int j = 0; j <= width; j++) {
        u8g2.drawPixel(BIRD_X + 5 + i, BIRD_Y + 20 + j);
        u8g2.drawPixel(BIRD_X + 35 - i, BIRD_Y + 20 + j);
      }
    }
    // 翅膀羽毛细节
    for (int i = 0; i <= 15; i += 3) {
      u8g2.drawPixel(BIRD_X + 8 + i, BIRD_Y + 25 + i/4);
      u8g2.drawPixel(BIRD_X + 32 - i, BIRD_Y + 25 + i/4);
    }
  } else {
    // 翅膀向下拍打
    for (int i = 0; i <= 14; i++) {
      int height = 8 + i * 3 / 4;
      for (int j = 0; j <= height; j++) {
        u8g2.drawPixel(BIRD_X + 6 + i, BIRD_Y + 28 + j);
        u8g2.drawPixel(BIRD_X + 34 - i, BIRD_Y + 28 + j);
      }
    }
    // 翅膀羽毛细节
    for (int i = 0; i <= 11; i += 3) {
      u8g2.drawPixel(BIRD_X + 9 + i, BIRD_Y + 32 + i/3);
      u8g2.drawPixel(BIRD_X + 31 - i, BIRD_Y + 32 + i/3);
    }
  }
  
  // === 尾巴（扇形） ===
  for (int i = 0; i <= 10; i++) {
    for (int j = -i/2; j <= i/2; j++) {
      u8g2.drawPixel(BIRD_X + 18 - i, BIRD_Y + 25 + j);
    }
  }
  
  // === 脚爪（简单线条） ===
  u8g2.drawLine(BIRD_X + 20, BIRD_Y + 35, BIRD_X + 17, BIRD_Y + 40);
  u8g2.drawLine(BIRD_X + 26, BIRD_Y + 35, BIRD_X + 29, BIRD_Y + 40);
  u8g2.drawLine(BIRD_X + 17, BIRD_Y + 40, BIRD_X + 15, BIRD_Y + 42);
  u8g2.drawLine(BIRD_X + 17, BIRD_Y + 40, BIRD_X + 19, BIRD_Y + 42);
  u8g2.drawLine(BIRD_X + 29, BIRD_Y + 40, BIRD_X + 27, BIRD_Y + 42);
  u8g2.drawLine(BIRD_X + 29, BIRD_Y + 40, BIRD_X + 31, BIRD_Y + 42);
}

// ==================== HTTP 请求处理 ====================
void handleMsg() {
  String oldMsg = displayMessage;
  if (server.hasArg("plain")) {
    displayMessage = server.arg("plain");
  } else if (server.args() > 0) {
    displayMessage = server.argName(0);
  }
  
  // 如果收到新消息，重置滚动位置
  if (oldMsg != displayMessage) {
    scrollPos = 0; 
  }
  
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(0, 30);
  u8g2.print("正在连接 Wi-Fi...");
  u8g2.sendBuffer();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  server.on("/msg", HTTP_POST, handleMsg);
  server.begin();
}

void loop() {
  server.handleClient();

  // 1. 鸟翅膀动画逻辑（3 个状态循环：向上->平展->向下）
  if (millis() - lastWingToggleTime >= 200) {
    wingState = (wingState + 1) % 3;
    lastWingToggleTime = millis();
  }

  u8g2.clearBuffer();

  // 2. 状态栏
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(0, 10);
  u8g2.print("IP:"); u8g2.print(WiFi.localIP().toString());
  u8g2.drawHLine(0, 13, 128);

  // 3. 滚动文字逻辑
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  int textWidth = u8g2.getUTF8Width(displayMessage.c_str());

  u8g2.setCursor(0, 30);
  u8g2.print("最新通知:");

  // 如果文字宽度超过预留区域，执行滚动
  if (textWidth > DISPLAY_WIDTH) {
    if (millis() - lastScrollTime >= SCROLL_SPEED) {
      scrollPos--;
      // 如果文字完全消失在左侧，重置到右侧边缘
      if (scrollPos < -textWidth) {
        scrollPos = DISPLAY_WIDTH;
      }
      lastScrollTime = millis();
    }
  } else {
    scrollPos = 0; // 短句子居左显示
  }

  // 设置一个剪切窗口，防止文字叠到鸟身上
  u8g2.setCursor(scrollPos, 50);
  u8g2.print(displayMessage.c_str());

  // 4. 绘制大鸟（放在最后绘制，覆盖可能飘过来的文字边缘）
  // 先画一个黑色矩形遮挡文字，确保鸟背景干净
  u8g2.setDrawColor(0);
  u8g2.drawBox(BIRD_X, 10, 128-BIRD_X, 64-10);
  u8g2.setDrawColor(1);
  drawBird(wingState);

  u8g2.sendBuffer();
  delay(10);
}