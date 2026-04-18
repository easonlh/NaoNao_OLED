#include "github_client.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

GitHubClient::GitHubClient(unsigned long intervalMs)
    : fetchIntervalMs(intervalMs), fetching(false), lastAttempt(0) {
  data.valid = false;
  data.starCount = 0;
  data.forksCount = 0;
  data.lastFetch = 0;
  strncpy(data.repoName, CFG_GITHUB_REPO, sizeof(data.repoName) - 1);
  strncpy(data.language, "N/A", sizeof(data.language) - 1);
}

void GitHubClient::begin() {
  // Nothing to initialize
}

void GitHubClient::update() {
  if (strlen(CFG_GITHUB_REPO) == 0) return; // Not configured
  if (WiFi.status() != WL_CONNECTED) return;
  if (fetching) return;
  if (millis() - lastAttempt < fetchIntervalMs) return;

  lastAttempt = millis();
  fetching = true;

  char url[256];
  snprintf(url, sizeof(url),
           "https://api.github.com/repos/%s", CFG_GITHUB_REPO);

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(url);
  http.setUserAgent("NaoNao_OLED-ESP32");

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      data.starCount = doc["stargazers_count"].as<int>();
      data.forksCount = doc["forks_count"].as<int>();

      const char* lang = doc["language"];
      if (lang) strncpy(data.language, lang, sizeof(data.language) - 1);
      else strncpy(data.language, "N/A", sizeof(data.language) - 1);

      const char* fullName = doc["full_name"];
      if (fullName) strncpy(data.repoName, fullName, sizeof(data.repoName) - 1);

      data.lastFetch = millis();
      data.valid = true;
    }
  } else {
    Serial.printf("GitHub API failed, HTTP code: %d\n", httpCode);
  }

  http.end();
  fetching = false;
}

GitHubData& GitHubClient::getData() {
  return data;
}

bool GitHubClient::isReady() {
  return data.valid;
}

GitHubClient githubClient;
