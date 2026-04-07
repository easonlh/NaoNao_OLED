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

// ==================== 兔子绘制模块 ====================
const int RABBIT_X = 85;  
const int RABBIT_Y = 20;  
int earState = 0;
unsigned long lastToggleTime = 0;

void drawRabbit(bool earsUp) {
  // ... (保持你之前的绘制逻辑不变)
  if (earsUp) {
    for (int x = 8; x <= 11; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y);
    for (int x = 20; x <= 23; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y);
    for (int x = 7; x <= 12; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 1);
    for (int x = 19; x <= 24; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 1);
    for (int x = 5; x <= 14; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 4);
    for (int x = 17; x <= 26; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 4);
  } else {
    for (int x = 3; x <= 10; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 2);
    for (int x = 1; x <= 13; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 5);
    for (int x = 21; x <= 28; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 2);
    for (int x = 18; x <= 30; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 5);
  }
  for (int x = 5; x <= 26; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 6);
  for (int x = 3; x <= 28; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 8);
  for (int x = 3; x <= 10; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 9); 
  for (int x = 21; x <= 28; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 9);
  for (int x = 2; x <= 29; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 11);
  for (int x = 5; x <= 26; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 13);
  for (int x = 8; x <= 23; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 16);
  for (int x = 11; x <= 20; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 19);
  for (int x = 5; x <= 13; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 21);
  for (int x = 18; x <= 26; x++) u8g2.drawPixel(RABBIT_X + x, RABBIT_Y + 21);
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

  // 1. 兔子动画逻辑
  if (millis() - lastToggleTime >= 400) {
    earState = !earState;
    lastToggleTime = millis();
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

  // 设置一个剪切窗口，防止文字叠到兔子身上
  // (虽然 u8g2 没法直接简单剪切，但我们可以通过控制绘制位置实现)
  u8g2.setCursor(scrollPos, 50);
  u8g2.print(displayMessage.c_str());

  // 4. 绘制闹闹兔 (放在最后绘制，覆盖可能飘过来的文字边缘)
  // 先画一个黑色矩形遮挡文字，确保兔子背景干净
  u8g2.setDrawColor(0);
  u8g2.drawBox(RABBIT_X, 14, 128-RABBIT_X, 64-14);
  u8g2.setDrawColor(1);
  drawRabbit(earState);

  u8g2.sendBuffer();
  delay(10);
}