#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// ==================== 屏幕初始化 (I2C) ====================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ==================== 动画配置 ====================

const int SWING_DELAY = 150;  // 摆动速度
const int MAX_OFFSET = 8;     // 最大偏移量

// ==================== 全局变量 ====================

int swingOffset = 0;
int swingDirection = 1;
unsigned long lastSwingTime = 0;

// ==================== 绘制爱心函数 ====================

void drawHeart(int offsetX) {
  int cx = 64 + offsetX;  // 中心 X 坐标
  int cy = 32;            // 中心 Y 坐标
  
  // 使用大爱心图案 - 逐行绘制
  // 第 0-1 行：爱心顶部两个圆弧
  for (int x = -18; x <= -6; x++) {
    int y = -12 + abs(x + 12);
    if (y >= -12 && y <= -10) u8g2.drawPixel(cx + x, cy + y);
  }
  for (int x = 6; x <= 18; x++) {
    int y = -12 + abs(x - 12);
    if (y >= -12 && y <= -10) u8g2.drawPixel(cx + x, cy + y);
  }
  
  // 第 2-3 行
  for (int x = -20; x <= -4; x++) {
    int y = -10 + abs(x + 12) / 2;
    if (y >= -8 && y <= -6) u8g2.drawPixel(cx + x, cy + y);
  }
  for (int x = 4; x <= 20; x++) {
    int y = -10 + abs(x - 12) / 2;
    if (y >= -8 && y <= -6) u8g2.drawPixel(cx + x, cy + y);
  }
  
  // 第 4-7 行：爱心主体
  for (int row = -6; row <= 2; row++) {
    int width = 22 - abs(row) * 2;
    for (int x = -width; x <= width; x++) {
      u8g2.drawPixel(cx + x, cy + row);
    }
  }
  
  // 第 8-12 行：爱心下半部分逐渐收窄
  for (int row = 3; row <= 8; row++) {
    int width = 16 - row * 2;
    if (width > 0) {
      for (int x = -width; x <= width; x++) {
        u8g2.drawPixel(cx + x, cy + row);
      }
    }
  }
  
  // 第 13-15 行：爱心尖端
  for (int row = 9; row <= 14; row++) {
    int width = 14 - row;
    if (width >= 0) {
      for (int x = -width; x <= width; x++) {
        u8g2.drawPixel(cx + x, cy + row);
      }
    }
  }
}

// ==================== 绘制星星函数 ====================

void drawStar(int cx, int cy, int size) {
  // 绘制大星星 - 使用线条绘制八角星
  // 外圈点
  for (int i = 0; i < 360; i += 45) {
    float rad = i * PI / 180.0;
    int x = cx + cos(rad) * size;
    int y = cy + sin(rad) * size;
    u8g2.drawPixel(x, y);
  }
  
  // 绘制星星主体（填充菱形）
  // 上半部分
  for (int row = -size; row <= 0; row++) {
    int width = size + row * 2 / 3;
    for (int x = -width; x <= width; x++) {
      u8g2.drawPixel(cx + x, cy + row);
    }
  }
  // 下半部分
  for (int row = 1; row <= size; row++) {
    int width = size - row * 2 / 3;
    for (int x = -width; x <= width; x++) {
      u8g2.drawPixel(cx + x, cy + row);
    }
  }
  
  // 内十字
  for (int i = -size + 2; i <= size - 2; i++) {
    u8g2.drawPixel(cx, cy + i);
    u8g2.drawPixel(cx + i, cy);
  }
}

// ==================== 绘制所有星星 ====================

void drawStars(int offsetX) {
  // 左边星星
  drawStar(25 + offsetX, 18, 10);
  
  // 右边星星
  drawStar(103 + offsetX, 18, 10);
  
  // 左上小星星
  drawStar(15 + offsetX, 45, 5);
  
  // 右上小星星
  drawStar(113 + offsetX, 45, 5);
  
  // 左下小星星
  drawStar(20 + offsetX, 55, 4);
  
  // 右下小星星
  drawStar(108 + offsetX, 55, 4);
}

// ==================== 设置函数 ====================

void setup() {
  Serial.begin(115200);

  // 初始化屏幕
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  Serial.println("Heart and Stars animation started!");
}

// ==================== 主循环 ====================

void loop() {
  unsigned long currentTime = millis();
  
  // 更新摆动位置
  if (currentTime - lastSwingTime >= SWING_DELAY) {
    swingOffset += swingDirection;
    if (swingOffset >= MAX_OFFSET || swingOffset <= -MAX_OFFSET) {
      swingDirection = -swingDirection;
    }
    lastSwingTime = currentTime;
  }
  
  // 清屏并绘制
  u8g2.clearBuffer();
  
  // 绘制星星（在爱心后面）
  drawStars(swingOffset);
  
  // 绘制爱心（在中间）
  drawHeart(swingOffset * 2 / 3);  // 爱心摆动幅度小一点
  
  u8g2.sendBuffer();
  
  delay(50);
}
