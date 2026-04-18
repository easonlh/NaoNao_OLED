#include "config.h"

// ==================== Wi-Fi 配置 ====================
// 从 .env 文件通过 platformio.ini 的 build_flags 注入
// 用法: source .env && pio run --target upload
// 如果 .env 中未配置，则使用以下默认值
#ifndef CONFIG_WIFI_SSID
  #define CONFIG_WIFI_SSID "YOUR_WIFI_SSID"
#endif
#ifndef CONFIG_WIFI_PASSWORD
  #define CONFIG_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif

const char* WIFI_SSID = CONFIG_WIFI_SSID;
const char* WIFI_PASSWORD = CONFIG_WIFI_PASSWORD;

// ==================== NTP 时间服务器配置 ====================
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 8 * 3600;   // 中国时区 GMT+8
const int DAYLIGHT_OFFSET_SEC = 0;      // 无夏令时

// ==================== 显示配置 ====================
const int SCREEN_CYCLE_INTERVAL = 5000;  // 屏幕自动切换间隔（毫秒）
const int SCROLL_SPEED = 30;             // 滚动速度（毫秒/像素）
const int MSG_QUEUE_MAX_SIZE = 10;       // 消息队列最大长度

// ==================== 亮度配置 ====================
const int BRIGHTNESS_DAY = 255;          // 白天亮度
const int BRIGHTNESS_NIGHT = 50;         // 夜间亮度
const int NIGHT_START_HOUR = 22;         // 夜间模式开始时间（22:00）
const int DAY_START_HOUR = 7;            // 白天模式开始时间（07:00）

// ==================== 屏幕 saver 配置 ====================
const unsigned long SCREENSAVER_TIMEOUT = 60000;  // 无操作后进入屏保时间（毫秒）
