#ifndef WEATHER_CLIENT_H
#define WEATHER_CLIENT_H

#include <Arduino.h>

struct WeatherData {
  float temperature;
  int humidity;
  int weatherCode;
  char description[64];
  char city[64];
  unsigned long lastFetch;
  bool valid;
};

class WeatherClient {
private:
  WeatherData data;
  unsigned long fetchIntervalMs;
  bool fetching;
  unsigned long lastAttempt;

public:
  WeatherClient(unsigned long intervalMs = 900000);

  void begin();
  void update();
  WeatherData& getData();
  bool isReady();
};

extern WeatherClient weatherClient;

#endif
