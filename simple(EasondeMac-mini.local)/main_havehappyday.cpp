#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化屏幕
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ==================== 配置区域 ====================
// const char* scrollText = "萧闹闹的 ESP32 硬件实验室 - 正在进行跑马灯压力测试 - NaoNao OLED v1.0 ";
// ==================== 闺女出游定制文案 ====================
const char* scrollText = " *** 祝刘毓泽和小伙伴们：出游超级开心！ *** [ 快乐起飞 ]  *** 全世界最可爱的小公主们出发啦！！HAVE A HAPPY  DAY！！！ *** (^_^)  ";
int scrollPos = 128; // 初始位置在屏幕最右侧外
int textWidth = 0;   // 存储字符串的总像素宽度

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  
  // 选择中文字体并计算文字的总长度
  u8g2.setFont(u8g2_font_wqy14_t_gb2312);
  textWidth = u8g2.getUTF8Width(scrollText); 
}

// void loop() {
//   u8g2.clearBuffer();

//   // 1. 固定页眉：项目名称
//   u8g2.setFont(u8g2_font_6x12_tr);
//   u8g2.drawStr(0, 10, "Project: NaoNao_OLED");
//   u8g2.drawHLine(0, 14, 128); // 画一条横线分割

//   // 2. 动态跑马灯内容
//   u8g2.setFont(u8g2_font_wqy16_t_gb2312); // 使用大一点的中文
//   u8g2.setCursor(scrollPos, 45);
//   u8g2.print(scrollText);

//   // 3. 固定页脚：状态栏
//   u8g2.setFont(u8g2_font_4x6_tf);
//   u8g2.drawStr(0, 63, "FPS: 60 | System Stable | CPU: 240MHz");

//   u8g2.sendBuffer();

//   // 4. 逻辑控制：移动坐标
//   scrollPos -= 2; // 数值越大滚动越快

//   // 如果文字完全走出了屏幕左侧，就重置到屏幕右侧
//   if (scrollPos < -textWidth) {
//     scrollPos = 128;
//   }

//   delay(10); // 控制刷新频率，数值越小越丝滑
// }
// void loop() {
//   u8g2.clearBuffer();

//   // 1. 顶部：滚动的小星星 (营造气氛)
//   u8g2.setFont(u8g2_font_6x12_tr);
//   for(int i=0; i<128; i+=20) {
//     u8g2.drawStr(i + (millis()/50 % 20), 10, "* "); 
//   }
//   u8g2.drawHLine(0, 14, 128);

//   // 2. 中间：大大的文案 (刘毓泽专属)
//   u8g2.setFont(u8g2_font_wqy16_t_gb2312); 
//   u8g2.setCursor(scrollPos, 42);
//   u8g2.print(scrollText);

//   // 3. 底部：动态心跳/加油包
//   u8g2.setFont(u8g2_font_6x12_tr);
//   if ((millis() / 500) % 2 == 0) {
//     u8g2.drawStr(30, 62, ">>> HAPPY DAY! <<<");
//   } else {
//     u8g2.drawStr(30, 62, "--- GO! GO! GO! ---");
//   }

//   u8g2.sendBuffer();

//   // 4. 速度控制
//   scrollPos -= 3; // 稍微快一点点，显得很兴奋

//   if (scrollPos < -u8g2.getUTF8Width(scrollText)) {
//     scrollPos = 128;
//   }

//   delay(20); 
// }

void loop() {
  u8g2.clearBuffer();

  // 1. 顶部：流动的小星星 (装饰层)
  u8g2.setFont(u8g2_font_6x12_tr);
  int starOffset = (millis() / 50) % 20; // 随时间流动的位移
  for (int i = -20; i < 128; i += 20) {
    u8g2.drawStr(i + starOffset, 10, "* "); 
  }
  u8g2.drawHLine(0, 14, 128); // 分割波

  // 2. 中间：刘毓泽出游应援文案 (核心层)
  u8g2.setFont(u8g2_font_wqy16_t_gb2312); 
  
  // 关键修复：动态获取当前文案的像素宽度
  int currentTextWidth = u8g2.getUTF8Width(scrollText);
  
  u8g2.setCursor(scrollPos, 42);
  u8g2.print(scrollText);

  // 3. 底部：状态文字 (页脚层)
  u8g2.setFont(u8g2_font_6x12_tr);
  // 修复：X 坐标设为 0 即为绝对靠左
  if ((millis() / 500) % 2 == 0) {
    u8g2.drawStr(0, 62, ">>> HAPPY DAY! <<<");
  } else {
    u8g2.drawStr(0, 62, "-- GO! GO! GO! - -");
  }

  u8g2.sendBuffer();

  // 4. 滚动逻辑调优
  scrollPos -= 3; // 步进值

  // 修复：必须减去完整的 currentTextWidth，文字才会完全消失
  if (scrollPos < -currentTextWidth) {
    scrollPos = 128; // 从屏幕右侧重新进入
  }

  delay(20); 
}