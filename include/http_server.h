#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <WebServer.h>
#include <Arduino.h>

class NaoNaoServer {
private:
  WebServer* server;
  
  // 处理消息 POST 请求
  void handleMsg();
  
  // 处理主页 GET 请求（Web UI）
  void handleRoot();
  
  // 处理状态查询
  void handleStatus();
  
  // 处理消息历史查询
  void handleMessages();
  
  // 处理清除消息
  void handleClearMessages();
  
  // 生成 Web UI HTML
  String generateWebPage();

public:
  NaoNaoServer(int port = 80);
  void begin();
  void handleClient();
};

// 全局服务器实例
extern NaoNaoServer naoNaoServer;

#endif
