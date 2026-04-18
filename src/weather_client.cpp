#include "weather_client.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WeatherClient::WeatherClient(unsigned long intervalMs)
    : fetchIntervalMs(intervalMs), fetching(false), lastAttempt(0) {
  data.valid = false;
  data.temperature = 0;
  data.humidity = 0;
  data.weatherCode = 0;
  data.lastFetch = 0;
  strncpy(data.description, "N/A", sizeof(data.description) - 1);
  strncpy(data.city, CFG_WEATHER_CITY, sizeof(data.city) - 1);
}

void WeatherClient::begin() {
  // Nothing to initialize, data is set in constructor
}

void WeatherClient::update() {
  if (strlen(CFG_WEATHER_API_KEY) == 0 || strcmp(CFG_WEATHER_API_KEY, "your_openweathermap_api_key") == 0) {
    return; // API key not configured
  }
  if (WiFi.status() != WL_CONNECTED) return;
  if (fetching) return; // Still fetching from previous attempt

  if (millis() - lastAttempt < fetchIntervalMs) return;

  lastAttempt = millis();
  fetching = true;

  char url[256];
  snprintf(url, sizeof(url),
           "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric",
           CFG_WEATHER_CITY, CFG_WEATHER_API_KEY);

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      data.temperature = doc["main"]["temp"].as<float>();
      data.humidity = doc["main"]["humidity"].as<int>();
      data.weatherCode = doc["weather"][0]["id"].as<int>();

      const char* desc = doc["weather"][0]["description"];
      if (desc) strncpy(data.description, desc, sizeof(data.description) - 1);

      const char* name = doc["name"];
      if (name) strncpy(data.city, name, sizeof(data.city) - 1);

      data.lastFetch = millis();
      data.valid = true;
    }
  } else {
    Serial.printf("Weather API failed, HTTP code: %d\n", httpCode);
  }

  http.end();
  fetching = false;
}

WeatherData& WeatherClient::getData() {
  return data;
}

bool WeatherClient::isReady() {
  return data.valid;
}

WeatherClient weatherClient;
