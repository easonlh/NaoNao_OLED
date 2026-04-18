#include "http_server.h"
#include "message_queue.h"
#include <Arduino.h>

NaoNaoServer::NaoNaoServer(int port) {
  server = new WebServer(port);
}

void NaoNaoServer::begin() {
  server->on("/", HTTP_GET, [this]() { handleRoot(); });
  server->on("/msg", HTTP_POST, [this]() { handleMsg(); });
  server->on("/status", HTTP_GET, [this]() { handleStatus(); });
  server->on("/messages", HTTP_GET, [this]() { handleMessages(); });
  server->on("/clear", HTTP_POST, [this]() { handleClearMessages(); });
  server->begin();
}

void NaoNaoServer::handleClient() {
  server->handleClient();
}

void NaoNaoServer::handleMsg() {
  String msg = "";
  
  if (server->hasArg("plain")) {
    msg = server->arg("plain");
  } else if (server->hasArg("text")) {
    msg = server->arg("text");
  } else if (server->args() > 0) {
    msg = server->argName(0);
  }
  
  if (msg.length() > 0) {
    msgQueue.addMessage(msg.c_str());
    server->send(200, "text/plain", "OK: Message added to queue");
  } else {
    server->send(400, "text/plain", "Error: No message content");
  }
}

void NaoNaoServer::handleRoot() {
  String html = generateWebPage();
  server->send(200, "text/html", html);
}

void NaoNaoServer::handleStatus() {
  String status = "{";
  status += "\"wifi_connected\":";
  status += (WiFi.status() == WL_CONNECTED ? "true" : "false");
  status += ",\"ip\":\"";
  status += WiFi.localIP().toString();
  status += "\",\"rssi\":";
  status += WiFi.RSSI();
  status += ",\"uptime\":";
  status += millis();
  status += ",\"messages\":";
  status += msgQueue.getCount();
  status += ",\"free_heap\":";
  status += ESP.getFreeHeap();
  status += "}";
  
  server->send(200, "application/json", status);
}

void NaoNaoServer::handleMessages() {
  String response = "{";
  response += "\"count\":";
  response += msgQueue.getCount();
  response += ",\"messages\":[";
  
  // 简单返回当前消息
  Message* currentMsg = msgQueue.getCurrentMessage();
  if (currentMsg) {
    response += "{\"text\":\"";
    response += currentMsg->text;
    response += "\",\"timestamp\":";
    response += currentMsg->timestamp;
    response += "}";
  }
  
  response += "]}";
  server->send(200, "application/json", response);
}

void NaoNaoServer::handleClearMessages() {
  msgQueue.clear();
  server->send(200, "text/plain", "OK: Messages cleared");
}

String NaoNaoServer::generateWebPage() {
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'><title>NaoNao OLED</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; max-width: 600px; margin: 50px auto; padding: 20px; }";
  html += "h1 { color: #333; }";
  html += ".status { background: #f0f0f0; padding: 15px; border-radius: 5px; margin: 10px 0; }";
  html += "input[type=text] { width: 70%; padding: 10px; margin: 5px 0; }";
  html += "button { padding: 10px 20px; background: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }";
  html += "button:hover { background: #45a049; }";
  html += "#result { margin-top: 10px; padding: 10px; background: #e8f5e9; display: none; }";
  html += "</style></head><body>";
  
  html += "<h1>🐦 NaoNao OLED 控制台</h1>";
  
  // 状态信息
  html += "<div class='status'>";
  html += "<h3>📊 设备状态</h3>";
  html += "<p>WiFi: ";
  html += (WiFi.status() == WL_CONNECTED ? "已连接 ✅" : "未连接 ❌");
  html += "</p>";
  html += "<p>IP 地址: ";
  html += WiFi.localIP().toString();
  html += "</p>";
  html += "<p>信号强度: ";
  html += WiFi.RSSI();
  html += " dBm</p>";
  html += "<p>消息数量: ";
  html += msgQueue.getCount();
  html += "</p>";
  html += "<p>运行时间: ";
  html += millis() / 1000;
  html += " 秒</p>";
  html += "</div>";
  
  // 发送消息
  html += "<h3>📤 发送消息到 OLED</h3>";
  html += "<form id='msgForm'>";
  html += "<input type='text' id='message' placeholder='输入消息...' required>";
  html += "<button type='submit'>发送</button>";
  html += "</form>";
  html += "<div id='result'></div>";
  
  // 快捷操作
  html += "<h3>⚡ 快捷操作</h3>";
  html += "<button onclick=\"clearMessages()\">清除所有消息</button>";
  html += "<button onclick=\"sendPreset('Hello!')\">发送 'Hello!'</button>";
  html += "<button onclick=\"sendPreset('你好！')\">发送 '你好！'</button>";
  
  html += "<script>";
  html += "document.getElementById('msgForm').addEventListener('submit', function(e) {";
  html += "  e.preventDefault();";
  html += "  var msg = document.getElementById('message').value;";
  html += "  fetch('/msg', {method:'POST', body:msg}).then(r=>r.text()).then(function(data) {";
  html += "    var result = document.getElementById('result');";
  html += "    result.style.display='block';";
  html += "    result.textContent='发送成功: ' + data;";
  html += "    document.getElementById('message').value='';";
  html += "    setTimeout(function(){result.style.display='none'}, 3000);";
  html += "  });";
  html += "});";
  html += "function clearMessages() {";
  html += "  fetch('/clear', {method:'POST'}).then(function(){alert('消息已清除')});";
  html += "}";
  html += "function sendPreset(msg) {";
  html += "  fetch('/msg', {method:'POST', body:msg}).then(function(){alert('已发送: '+msg)});";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// 全局服务器实例
NaoNaoServer naoNaoServer(80);
