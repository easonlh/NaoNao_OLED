#ifndef PRICE_CLIENT_H
#define PRICE_CLIENT_H

#include <Arduino.h>

struct PriceData {
  float priceUsd;
  float change24h;
  char symbol[16];
  unsigned long lastFetch;
  bool valid;
};

class PriceClient {
private:
  PriceData data;
  unsigned long fetchIntervalMs;
  bool fetching;
  unsigned long lastAttempt;

public:
  PriceClient(unsigned long intervalMs = 300000);

  void begin();
  void update();
  PriceData& getData();
  bool isReady();
};

extern PriceClient priceClient;

#endif
