#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化 I2C 屏幕
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print(); // 必须开启

  u8g2.clearBuffer();

// 第一行：Eason's Mac Mini (保持现状)
  u8g2.setFont(u8g2_font_6x12_tr); 
  u8g2.setCursor(0, 12);
  u8g2.print("Eason's Mac Mini");

  // 第二行：HELLO! (可以稍微调小一点，给大名字留空间)
  u8g2.setFont(u8g2_font_ncenB12_tr); 
  u8g2.setCursor(0, 32);
  u8g2.print("HELLO!");

  // 第三行：刘毓泽 (换成 16 像素全量字库)
  // 16像素是 OLED 显示中文最舒服的尺寸，清晰且大气
  u8g2.setFont(u8g2_font_wqy16_t_gb2312); 
  u8g2.setCursor(0, 60); // Y坐标 60 接近底部
  u8g2.print("刘毓泽");

  u8g2.sendBuffer();
  Serial.println("Updated Display!");
}

void loop() {}