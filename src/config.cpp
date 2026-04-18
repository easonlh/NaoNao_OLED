#include "config.h"

// ==================== Wi-Fi 配置 ====================
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
const long GMT_OFFSET_SEC = 8 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

// ==================== 显示配置 ====================
const int SCREEN_CYCLE_INTERVAL = 5000;
const int SCROLL_SPEED = 30;
const int MSG_QUEUE_MAX_SIZE = 10;

// ==================== 亮度配置 ====================
const int BRIGHTNESS_DAY = 255;
const int BRIGHTNESS_NIGHT = 50;
const int NIGHT_START_HOUR = 22;
const int DAY_START_HOUR = 7;

// ==================== 屏幕 saver 配置 ====================
const unsigned long SCREENSAVER_TIMEOUT = 60000;

// ==================== OpenWeatherMap 配置 ====================
const char* CFG_WEATHER_API_KEY = "";
const char* CFG_WEATHER_CITY = "Beijing";
const unsigned long WEATHER_FETCH_INTERVAL_MS = 900000;

// ==================== MQTT 配置 ====================
const char* CFG_MQTT_BROKER = "";
const int CFG_MQTT_PORT = 1883;
const char* CFG_MQTT_CLIENT_ID = "NaoNao_OLED";
const char* CFG_MQTT_USERNAME = "";
const char* CFG_MQTT_PASSWORD = "";
const char* CFG_MQTT_SUBSCRIBE_TOPIC = "naonao/cmd";
const char* CFG_MQTT_PUBLISH_TOPIC = "naonao/status";
const int CFG_MQTT_PUBLISH_INTERVAL_SEC = 60;

// ==================== GitHub 配置 ====================
const char* CFG_GITHUB_REPO = "";
const unsigned long GITHUB_FETCH_INTERVAL_MS = 600000;

// ==================== 倒计时默认时长 ====================
const unsigned long TIMER_DEFAULT_DURATION_SEC = 300;

// ==================== 显示模式轮播配置 ====================
const int NUM_DISPLAY_MODES = 9;
const bool MODE_ENABLED[] = {
  true,  // MODE_CLOCK
  true,  // MODE_DATE
  true,  // MODE_NOTIFICATION
  true,  // MODE_WEATHER
  true,  // MODE_SYSTEM
  true,  // MODE_COUNTDOWN
  true,  // MODE_PRICE
  true,  // MODE_GITHUB
  true,  // MODE_MQTT_MONITOR
};
