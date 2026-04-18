#include "animations.h"

// ==================== 小鸟动画 ====================
void BirdAnimation::update() {
  if (millis() - lastToggleTime >= 200) {
    wingState = (wingState + 1) % 3;
    lastToggleTime = millis();
  }
}

void BirdAnimation::draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  int wingPos = wingState;
  int bx = x; // bird x position
  int by = y; // bird y position

  // === 身体（椭圆形） ===
  for (int x_offset = 18; x_offset <= 28; x_offset++) {
    u8g2.drawPixel(x_offset + bx, by + 15);
    u8g2.drawPixel(x_offset + bx, by + 35);
  }
  for (int y_offset = 16; y_offset <= 34; y_offset++) {
    u8g2.drawPixel(18 + bx, y_offset + by);
    u8g2.drawPixel(28 + bx, y_offset + by);
  }
  // 身体填充
  for (int x_offset = 19; x_offset <= 27; x_offset++) {
    for (int y_offset = 18; y_offset <= 32; y_offset++) {
      u8g2.drawPixel(x_offset + bx, y_offset + by);
    }
  }

  // === 头部（圆形） ===
  for (int x_offset = 28; x_offset <= 38; x_offset++) {
    for (int y_offset = 12; y_offset <= 22; y_offset++) {
      if ((x_offset-33)*(x_offset-33) + (y_offset-17)*(y_offset-17) <= 25) {
        u8g2.drawPixel(x_offset + bx, y_offset + by);
      }
    }
  }

  // === 眼睛 ===
  u8g2.setDrawColor(0);
  u8g2.drawPixel(34 + bx, 15 + by);
  u8g2.drawPixel(35 + bx, 15 + by);
  u8g2.setDrawColor(1);

  // === 鸟喙（三角形） ===
  for (int i = 0; i <= 8; i++) {
    for (int j = 0; j <= (8-i)/2; j++) {
      u8g2.drawPixel(38 + i + bx, 18 + j + by);
    }
  }

  // === 翅膀（根据状态变化） ===
  if (wingPos == 0) {
    // 翅膀向上展开
    for (int i = 0; i <= 15; i++) {
      int height = 12 - i * 4 / 5;
      for (int j = 0; j <= height; j++) {
        u8g2.drawPixel(5 + i + bx, 15 - j + by);
        u8g2.drawPixel(35 - i + bx, 15 - j + by);
      }
    }
    for (int i = 0; i <= 12; i += 3) {
      u8g2.drawPixel(8 + i + bx, 8 - i/3 + by);
      u8g2.drawPixel(32 - i + bx, 8 - i/3 + by);
    }
  } else if (wingPos == 1) {
    // 翅膀平展
    for (int i = 0; i <= 18; i++) {
      int width = 8 + i / 3;
      for (int j = 0; j <= width; j++) {
        u8g2.drawPixel(5 + i + bx, 20 + j + by);
        u8g2.drawPixel(35 - i + bx, 20 + j + by);
      }
    }
    for (int i = 0; i <= 15; i += 3) {
      u8g2.drawPixel(8 + i + bx, 25 + i/4 + by);
      u8g2.drawPixel(32 - i + bx, 25 + i/4 + by);
    }
  } else {
    // 翅膀向下拍打
    for (int i = 0; i <= 14; i++) {
      int height = 8 + i * 3 / 4;
      for (int j = 0; j <= height; j++) {
        u8g2.drawPixel(6 + i + bx, 28 + j + by);
        u8g2.drawPixel(34 - i + bx, 28 + j + by);
      }
    }
    for (int i = 0; i <= 11; i += 3) {
      u8g2.drawPixel(9 + i + bx, 32 + i/3 + by);
      u8g2.drawPixel(31 - i + bx, 32 + i/3 + by);
    }
  }

  // === 尾巴（扇形） ===
  for (int i = 0; i <= 10; i++) {
    for (int j = -i/2; j <= i/2; j++) {
      u8g2.drawPixel(18 - i + bx, 25 + j + by);
    }
  }

  // === 脚爪 ===
  u8g2.drawLine(20 + bx, 35 + by, 17 + bx, 40 + by);
  u8g2.drawLine(26 + bx, 35 + by, 29 + bx, 40 + by);
  u8g2.drawLine(17 + bx, 40 + by, 15 + bx, 42 + by);
  u8g2.drawLine(17 + bx, 40 + by, 19 + bx, 42 + by);
  u8g2.drawLine(29 + bx, 40 + by, 27 + bx, 42 + by);
  u8g2.drawLine(29 + bx, 40 + by, 31 + bx, 42 + by);
}

// ==================== 星星动画 ====================
StarAnimation::StarAnimation() {
  lastToggleTime = 0;
  for (int i = 0; i < 5; i++) {
    starPositions[i] = random(128);
    starPhases[i] = random(4);
  }
}

void StarAnimation::update() {
  if (millis() - lastToggleTime >= 300) {
    for (int i = 0; i < 5; i++) {
      starPhases[i] = (starPhases[i] + 1) % 4;
    }
    lastToggleTime = millis();
  }
}

void StarAnimation::draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int offsetX, int offsetY) {
  for (int i = 0; i < 5; i++) {
    int x = (starPositions[i] + offsetX) % 128;
    int y = 5 + i * 3 + offsetY;
    
    if (starPhases[i] == 0) {
      u8g2.drawPixel(x, y);
    } else if (starPhases[i] == 1) {
      u8g2.drawPixel(x, y);
      u8g2.drawPixel(x-1, y);
      u8g2.drawPixel(x+1, y);
      u8g2.drawPixel(x, y-1);
      u8g2.drawPixel(x, y+1);
    } else if (starPhases[i] == 2) {
      u8g2.drawPixel(x, y);
      u8g2.drawPixel(x-1, y);
      u8g2.drawPixel(x+1, y);
      u8g2.drawPixel(x, y-1);
      u8g2.drawPixel(x, y+1);
      u8g2.drawPixel(x-1, y-1);
      u8g2.drawPixel(x+1, y+1);
      u8g2.drawPixel(x-1, y+1);
      u8g2.drawPixel(x+1, y-1);
    }
  }
}

// ==================== 天气图标动画 ====================
void WeatherIconAnimation::update() {
  if (millis() - lastToggleTime >= 500) {
    phase = (phase + 1) % 4;
    lastToggleTime = millis();
  }
}

void WeatherIconAnimation::drawSunny(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y) {
  int r = 6;
  u8g2.drawCircle(x, y, r, U8G2_DRAW_ALL);
  
  // 旋转光线
  for (int i = 0; i < 8; i++) {
    if (phase < 2 || i % 2 == 0) {
      float angle = i * PI / 4 + phase * 0.2;
      int x1 = x + (r + 2) * cos(angle);
      int y1 = y + (r + 2) * sin(angle);
      int x2 = x + (r + 5) * cos(angle);
      int y2 = y + (r + 5) * sin(angle);
      u8g2.drawLine(x1, y1, x2, y2);
    }
  }
}

void WeatherIconAnimation::drawCloudy(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y) {
  // 云朵
  u8g2.drawBox(x, y, 16, 8);
  u8g2.drawCircle(x + 4, y, 4, U8G2_DRAW_UPPER_LEFT);
  u8g2.drawCircle(x + 12, y, 4, U8G2_DRAW_UPPER_RIGHT);
  
  // 移动效果
  int offset = (phase - 2) * 2;
  u8g2.drawBox(x + offset, y + 2, 12, 6);
}

void WeatherIconAnimation::drawRainy(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y) {
  // 云
  u8g2.drawBox(x, y - 4, 16, 6);
  u8g2.drawCircle(x + 4, y - 4, 3, U8G2_DRAW_UPPER_LEFT);
  u8g2.drawCircle(x + 12, y - 4, 3, U8G2_DRAW_UPPER_RIGHT);
  
  // 雨滴动画
  for (int i = 0; i < 3; i++) {
    int rainY = y + 4 + ((phase * 3 + i * 5) % 12);
    u8g2.drawPixel(x + 3 + i * 5, rainY);
  }
}

// ==================== WiFi 信号动画 ====================
void WifiSignalAnimation::draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y, int rssi) {
  int bars = 0;
  if (rssi > -50) bars = 4;
  else if (rssi > -60) bars = 3;
  else if (rssi > -70) bars = 2;
  else if (rssi > -80) bars = 1;
  
  for (int i = 0; i < 4; i++) {
    int barHeight = (i + 1) * 2;
    if (i < bars) {
      // 闪烁效果
      if (millis() / 1000 % 2 == 0 || i < 2) {
        u8g2.drawBox(x + i * 4, y - barHeight, 3, barHeight);
      }
    } else {
      u8g2.drawFrame(x + i * 4, y - barHeight, 3, barHeight);
    }
  }
}

// ==================== 心跳动画 ====================
void HeartbeatAnimation::draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2, int x, int y) {
  // 简单心跳脉冲
  int scale = 10 + sin(millis() / 200.0) * 3;
  
  // 心形
  for (int i = 0; i <= scale; i++) {
    for (int j = 0; j <= scale; j++) {
      double dx = (i - scale/2.0) / (scale/2.0);
      double dy = (j - scale/2.0) / (scale/2.0);
      double dist = dx*dx + dy*dy;
      if (dist <= 1.0) {
        u8g2.drawPixel(x + i, y + j);
      }
    }
  }
}

// ==================== 屏保动画 ====================
ScreenSaverAnimation::ScreenSaverAnimation() {
  lastUpdate = 0;
  for (int i = 0; i < 8; i++) {
    stars[i].x = random(128);
    stars[i].y = random(64);
    stars[i].dx = random(3) - 1;
    stars[i].dy = random(3) - 1;
    if (stars[i].dx == 0) stars[i].dx = 1;
    if (stars[i].dy == 0) stars[i].dy = 1;
    stars[i].size = random(2) + 1;
  }
}

void ScreenSaverAnimation::update() {
  if (millis() - lastUpdate >= 50) {
    for (int i = 0; i < 8; i++) {
      stars[i].x += stars[i].dx;
      stars[i].y += stars[i].dy;
      
      if (stars[i].x < 0 || stars[i].x >= 128) stars[i].dx *= -1;
      if (stars[i].y < 0 || stars[i].y >= 64) stars[i].dy *= -1;
    }
    lastUpdate = millis();
  }
}

void ScreenSaverAnimation::draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2) {
  for (int i = 0; i < 8; i++) {
    if (stars[i].size == 1) {
      u8g2.drawPixel(stars[i].x, stars[i].y);
    } else {
      u8g2.drawBox(stars[i].x - 1, stars[i].y - 1, 3, 3);
    }
  }
}
