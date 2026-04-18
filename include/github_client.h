#ifndef GITHUB_CLIENT_H
#define GITHUB_CLIENT_H

#include <Arduino.h>

struct GitHubData {
  int starCount;
  int forksCount;
  char repoName[128];
  char language[32];
  unsigned long lastFetch;
  bool valid;
};

class GitHubClient {
private:
  GitHubData data;
  unsigned long fetchIntervalMs;
  bool fetching;
  unsigned long lastAttempt;

public:
  GitHubClient(unsigned long intervalMs = 600000);

  void begin();
  void update();
  GitHubData& getData();
  bool isReady();
};

extern GitHubClient githubClient;

#endif
