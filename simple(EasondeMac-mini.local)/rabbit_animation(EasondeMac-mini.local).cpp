#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// ==================== 屏幕初始化 (I2C) ====================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ==================== 蚂蚁配置 ====================

const int ANT_X = 64;   // 中心 X 位置
const int ANT_Y = 32;   // 中心 Y 位置

// ==================== 全局变量 ====================

int legFrame = 0;  // 0 或 1
unsigned long lastLegTime = 0;
const unsigned long LEG_DELAY = 150;  // 150ms 切换一次腿

// ==================== 绘制蚂蚁函数（俯视图）====================

void drawAnt(int legState) {
  // === 绘制头部（圆形）===
  // 头轮廓 - 画一个圆
  for (int angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180.0;
    int x = ANT_X - 25 + cos(rad) * 10;
    int y = ANT_Y + sin(rad) * 10;
    u8g2.drawPixel(x, y);
  }
  
  // 填充头部
  for (int dy = -9; dy <= 9; dy++) {
    for (int dx = -9; dx <= 9; dx++) {
      if (dx * dx + dy * dy <= 81) {
        u8g2.drawPixel(ANT_X - 25 + dx, ANT_Y + dy);
      }
    }
  }
  
  // 眼睛（2 个小圆点）
  u8g2.setDrawColor(0);
  u8g2.drawPixel(ANT_X - 28, ANT_Y - 4);
  u8g2.drawPixel(ANT_X - 28, ANT_Y + 4);
  u8g2.drawPixel(ANT_X - 27, ANT_Y - 4);
  u8g2.drawPixel(ANT_X - 27, ANT_Y + 4);
  u8g2.setDrawColor(1);
  
  // === 绘制触角（2 根弯曲的）===
  // 左触角
  for (int i = 0; i < 8; i++) {
    u8g2.drawPixel(ANT_X - 32 - i, ANT_Y - 8 - i / 2);
    u8g2.drawPixel(ANT_X - 33 - i, ANT_Y - 8 - i / 2);
  }
  // 右触角
  for (int i = 0; i < 8; i++) {
    u8g2.drawPixel(ANT_X - 32 - i, ANT_Y + 8 + i / 2);
    u8g2.drawPixel(ANT_X - 33 - i, ANT_Y + 8 + i / 2);
  }
  
  // === 绘制胸部（小椭圆连接头部和腹部）===
  for (int dy = -6; dy <= 6; dy++) {
    for (int dx = -6; dx <= 6; dx++) {
      if (dx * dx / 4 + dy * dy <= 36) {
        u8g2.drawPixel(ANT_X + dx, ANT_Y + dy);
      }
    }
  }
  
  // === 绘制腹部（大椭圆，带条纹）===
  // 腹部轮廓
  for (int dy = -14; dy <= 14; dy++) {
    for (int dx = 8; dx <= 35; dx++) {
      int normX = dx - 20;
      if (normX * normX / 700 + dy * dy / 196 <= 1) {
        u8g2.drawPixel(ANT_X + dx, ANT_Y + dy);
      }
    }
  }
  
  // 腹部条纹（4 条黑色条纹）
  u8g2.setDrawColor(0);
  // 条纹 1
  for (int dy = -10; dy <= 10; dy++) {
    u8g2.drawPixel(ANT_X + 14, ANT_Y + dy);
    u8g2.drawPixel(ANT_X + 15, ANT_Y + dy);
  }
  // 条纹 2
  for (int dy = -11; dy <= 11; dy++) {
    u8g2.drawPixel(ANT_X + 20, ANT_Y + dy);
    u8g2.drawPixel(ANT_X + 21, ANT_Y + dy);
  }
  // 条纹 3
  for (int dy = -12; dy <= 12; dy++) {
    u8g2.drawPixel(ANT_X + 26, ANT_Y + dy);
    u8g2.drawPixel(ANT_X + 27, ANT_Y + dy);
  }
  // 条纹 4
  for (int dy = -11; dy <= 11; dy++) {
    u8g2.drawPixel(ANT_X + 32, ANT_Y + dy);
    u8g2.drawPixel(ANT_X + 33, ANT_Y + dy);
  }
  u8g2.setDrawColor(1);
  
  // === 绘制 6 条腿（带动画）===
  // 左前腿
  if (legState == 0) {
    // 向前伸
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 18 - i, ANT_Y - 8 - i / 3);
      u8g2.drawPixel(ANT_X - 5 - i, ANT_Y - 15 - i / 4);
    }
  } else {
    // 向后伸
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 18 + i / 2, ANT_Y - 8 - i / 3);
      u8g2.drawPixel(ANT_X - 5 + i / 2, ANT_Y - 15 - i / 4);
    }
  }
  
  // 左中腿
  if (legState == 0) {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 3 - i / 3, ANT_Y - 10 - i / 2);
      u8g2.drawPixel(ANT_X + 5 - i / 3, ANT_Y - 18 - i / 3);
    }
  } else {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 3 + i / 2, ANT_Y - 10 - i / 2);
      u8g2.drawPixel(ANT_X + 5 + i / 2, ANT_Y - 18 - i / 3);
    }
  }
  
  // 左后腿
  if (legState == 0) {
    for (int i = 0; i < 12; i++) {
      u8g2.drawPixel(ANT_X + 10 + i / 4, ANT_Y - 10 - i / 3);
      u8g2.drawPixel(ANT_X + 15 + i / 3, ANT_Y - 18 - i / 4);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      u8g2.drawPixel(ANT_X + 10 - i / 3, ANT_Y - 10 - i / 3);
      u8g2.drawPixel(ANT_X + 15 - i / 4, ANT_Y - 18 - i / 4);
    }
  }
  
  // 右前腿
  if (legState == 0) {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 18 - i, ANT_Y + 8 + i / 3);
      u8g2.drawPixel(ANT_X - 5 - i, ANT_Y + 15 + i / 4);
    }
  } else {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 18 + i / 2, ANT_Y + 8 + i / 3);
      u8g2.drawPixel(ANT_X - 5 + i / 2, ANT_Y + 15 + i / 4);
    }
  }
  
  // 右中腿
  if (legState == 0) {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 3 - i / 3, ANT_Y + 10 + i / 2);
      u8g2.drawPixel(ANT_X + 5 - i / 3, ANT_Y + 18 + i / 3);
    }
  } else {
    for (int i = 0; i < 10; i++) {
      u8g2.drawPixel(ANT_X - 3 + i / 2, ANT_Y + 10 + i / 2);
      u8g2.drawPixel(ANT_X + 5 + i / 2, ANT_Y + 18 + i / 3);
    }
  }
  
  // 右后腿
  if (legState == 0) {
    for (int i = 0; i < 12; i++) {
      u8g2.drawPixel(ANT_X + 10 + i / 4, ANT_Y + 10 + i / 3);
      u8g2.drawPixel(ANT_X + 15 + i / 3, ANT_Y + 18 + i / 4);
    }
  } else {
    for (int i = 0; i < 12; i++) {
      u8g2.drawPixel(ANT_X + 10 - i / 3, ANT_Y + 10 + i / 3);
      u8g2.drawPixel(ANT_X + 15 - i / 4, ANT_Y + 18 + i / 4);
    }
  }
}

// ==================== 设置函数 ====================

void setup() {
  Serial.begin(115200);

  // 初始化屏幕
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  Serial.println("Ant animation started!");
}

// ==================== 主循环 ====================

void loop() {
  unsigned long currentTime = millis();
  
  // 更新腿动画
  if (currentTime - lastLegTime >= LEG_DELAY) {
    legFrame = 1 - legFrame;  // 切换 0 和 1
    lastLegTime = currentTime;
  }
  
  // 清屏并绘制
  u8g2.clearBuffer();
  
  // 绘制动画蚂蚁
  drawAnt(legFrame);
  
  u8g2.sendBuffer();
  
  delay(50);
}
