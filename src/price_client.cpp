#include "price_client.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

PriceClient::PriceClient(unsigned long intervalMs)
    : fetchIntervalMs(intervalMs), fetching(false), lastAttempt(0) {
  data.valid = false;
  data.priceUsd = 0;
  data.change24h = 0;
  data.lastFetch = 0;
  strncpy(data.symbol, "BTC", sizeof(data.symbol) - 1);
}

void PriceClient::begin() {
  // Nothing to initialize
}

void PriceClient::update() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (fetching) return;
  if (millis() - lastAttempt < fetchIntervalMs) return;

  lastAttempt = millis();
  fetching = true;

  HTTPClient http;
  http.setTimeout(5000);
  http.begin("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd&include_24hr_change=true");

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error && doc.containsKey("bitcoin")) {
      data.priceUsd = doc["bitcoin"]["usd"].as<float>();
      data.change24h = doc["bitcoin"]["usd_24h_change"].as<float>();
      data.lastFetch = millis();
      data.valid = true;
    }
  } else {
    Serial.printf("Price API failed, HTTP code: %d\n", httpCode);
  }

  http.end();
  fetching = false;
}

PriceData& PriceClient::getData() {
  return data;
}

bool PriceClient::isReady() {
  return data.valid;
}

PriceClient priceClient;
