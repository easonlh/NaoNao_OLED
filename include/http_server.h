#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <WebServer.h>
#include <Arduino.h>

class NaoNaoServer {
private:
  WebServer* server;

  void handleRoot();
  void handleMsg();
  void handleStatus();
  void handleMessages();
  void handleClearMessages();
  void handleTimer();
  void handleWeather();
  void handlePrice();
  void handleMode();
  void handleReboot();
  void handleServo();
  void handleLight();
  String generateWebPage();

public:
  NaoNaoServer(int port = 80);
  void begin();
  void handleClient();
};

// 全局服务器实例
extern NaoNaoServer naoNaoServer;

#endif
