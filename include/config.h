#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Wi-Fi 配置（由 .env 通过 build_flags 注入）
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// NTP 时间服务器配置
extern const char* NTP_SERVER;
extern const long GMT_OFFSET_SEC;
extern const int DAYLIGHT_OFFSET_SEC;

// 显示配置
extern const int SCREEN_CYCLE_INTERVAL;
extern const int SCROLL_SPEED;
extern const int MSG_QUEUE_MAX_SIZE;

// 亮度配置
extern const int BRIGHTNESS_DAY;
extern const int BRIGHTNESS_NIGHT;
extern const int NIGHT_START_HOUR;
extern const int DAY_START_HOUR;

// 屏幕 saver 配置
extern const unsigned long SCREENSAVER_TIMEOUT;

// OpenWeatherMap 配置
extern const char* CFG_WEATHER_API_KEY;
extern const char* CFG_WEATHER_CITY;
extern const unsigned long WEATHER_FETCH_INTERVAL_MS;

// MQTT 配置
extern const char* CFG_MQTT_BROKER;
extern const int CFG_MQTT_PORT;
extern const char* CFG_MQTT_CLIENT_ID;
extern const char* CFG_MQTT_USERNAME;
extern const char* CFG_MQTT_PASSWORD;
extern const char* CFG_MQTT_SUBSCRIBE_TOPIC;
extern const char* CFG_MQTT_PUBLISH_TOPIC;
extern const int CFG_MQTT_PUBLISH_INTERVAL_SEC;

// GitHub 配置
extern const char* CFG_GITHUB_REPO;
extern const unsigned long GITHUB_FETCH_INTERVAL_MS;

// 倒计时默认时长（秒）
extern const unsigned long TIMER_DEFAULT_DURATION_SEC;

// 显示模式轮播配置
extern const int NUM_DISPLAY_MODES;
extern const bool MODE_ENABLED[];

#endif  // CONFIG_H
