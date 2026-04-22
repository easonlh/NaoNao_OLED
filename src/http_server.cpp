#include "http_server.h"
#include "config.h"
#include "message_queue.h"
#include "weather_client.h"
#include "price_client.h"
#include "github_client.h"
#include "countdown_timer.h"
#include "display.h"
#include "servo_control.h"
#include "light_sensor.h"
#include <Arduino.h>
#include <ArduinoJson.h>

NaoNaoServer::NaoNaoServer(int port) {
  server = new WebServer(port);
}

void NaoNaoServer::begin() {
  server->on("/", HTTP_GET, [this]() { handleRoot(); });
  server->on("/msg", HTTP_POST, [this]() { handleMsg(); });
  server->on("/status", HTTP_GET, [this]() { handleStatus(); });
  server->on("/messages", HTTP_GET, [this]() { handleMessages(); });
  server->on("/clear", HTTP_POST, [this]() { handleClearMessages(); });
  server->on("/timer", HTTP_GET, [this]() { handleTimer(); });
  server->on("/timer", HTTP_POST, [this]() { handleTimer(); });
  server->on("/weather", HTTP_GET, [this]() { handleWeather(); });
  server->on("/price", HTTP_GET, [this]() { handlePrice(); });
  server->on("/mode", HTTP_POST, [this]() { handleMode(); });
  server->on("/reboot", HTTP_POST, [this]() { handleReboot(); });
  server->on("/servo", HTTP_GET, [this]() { handleServo(); });
  server->on("/servo", HTTP_POST, [this]() { handleServo(); });
  server->on("/light", HTTP_GET, [this]() { handleLight(); });
  server->on("/light", HTTP_POST, [this]() { handleLight(); });
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

void NaoNaoServer::handleTimer() {
  if (server->method() == HTTP_GET) {
    StaticJsonDocument<256> doc;
    doc["state"] = (int)countdownTimer.getState();
    doc["remaining"] = countdownTimer.getRemainingSec();
    doc["total"] = countdownTimer.getTotalSec();

    char buf[256];
    serializeJson(doc, buf, sizeof(buf));
    server->send(200, "application/json", buf);
    return;
  }

  // POST: parse JSON body
  String body = server->arg("plain");
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  const char* action = doc["action"];
  if (!action) {
    server->send(400, "application/json", "{\"error\":\"Missing action\"}");
    return;
  }

  StaticJsonDocument<256> resp;

  if (strcmp(action, "start") == 0) {
    unsigned long duration = doc["duration"] | 300; // default 5 min
    countdownTimer.start(duration * 1000);
    resp["ok"] = true;
    resp["state"] = "running";
    resp["remaining"] = countdownTimer.getRemainingSec();
  } else if (strcmp(action, "stop") == 0 || strcmp(action, "pause") == 0) {
    countdownTimer.pause();
    resp["ok"] = true;
    resp["state"] = "paused";
  } else if (strcmp(action, "resume") == 0) {
    countdownTimer.resume();
    resp["ok"] = true;
    resp["state"] = "running";
  } else if (strcmp(action, "reset") == 0) {
    countdownTimer.reset();
    resp["ok"] = true;
    resp["state"] = "idle";
  } else {
    server->send(400, "application/json", "{\"error\":\"Unknown action\"}");
    return;
  }

  char buf[256];
  serializeJson(resp, buf, sizeof(buf));
  server->send(200, "application/json", buf);
}

void NaoNaoServer::handleWeather() {
  WeatherData& w = weatherClient.getData();
  StaticJsonDocument<384> doc;
  doc["valid"] = w.valid;
  doc["temperature"] = w.temperature;
  doc["humidity"] = w.humidity;
  doc["weather_code"] = w.weatherCode;
  doc["description"] = w.description;
  doc["city"] = w.city;
  doc["last_fetch"] = w.lastFetch;

  char buf[384];
  serializeJson(doc, buf, sizeof(buf));
  server->send(200, "application/json", buf);
}

void NaoNaoServer::handlePrice() {
  PriceData& p = priceClient.getData();
  StaticJsonDocument<256> doc;
  doc["valid"] = p.valid;
  doc["symbol"] = p.symbol;
  doc["price_usd"] = p.priceUsd;
  doc["change_24h"] = p.change24h;
  doc["last_fetch"] = p.lastFetch;

  char buf[256];
  serializeJson(doc, buf, sizeof(buf));
  server->send(200, "application/json", buf);
}

void NaoNaoServer::handleMode() {
  String body = server->arg("plain");
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  int mode = doc["mode"];
  if (mode >= 0 && mode < NUM_DISPLAY_MODES) {
    switchMode((ScreenMode)mode);
    StaticJsonDocument<128> resp;
    resp["ok"] = true;
    resp["mode"] = mode;
    char buf[128];
    serializeJson(resp, buf, sizeof(buf));
    server->send(200, "application/json", buf);
  } else {
    server->send(400, "application/json", "{\"error\":\"Invalid mode\"}");
  }
}

void NaoNaoServer::handleServo() {
  if (server->method() == HTTP_GET) {
    StaticJsonDocument<128> doc;
    doc["speed"] = servoCtrl.getSpeed();
    doc["attached"] = servoCtrl.isAttached();
    doc["stopped"] = (servoCtrl.getSpeed() == 90);
    char buf[128];
    serializeJson(doc, buf, sizeof(buf));
    server->send(200, "application/json", buf);
    return;
  }

  // POST: accept JSON body
  String body = server->arg("plain");
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  StaticJsonDocument<128> resp;

  if (doc.containsKey("action")) {
    const char* action = doc["action"];

    if (strcmp(action, "stop") == 0) {
      servoCtrl.setSpeed(90);
      resp["ok"] = true;
      resp["speed"] = 90;
      resp["message"] = "Servo stopped";

    } else if (strcmp(action, "status") == 0) {
      resp["speed"] = servoCtrl.getSpeed();
      resp["attached"] = servoCtrl.isAttached();
      resp["stopped"] = (servoCtrl.getSpeed() == 90);

    } else if (strcmp(action, "rotate") == 0) {
      // Rotate at given speed for given duration (ms), then auto-stop
      int speed = doc["speed"] | 0;
      unsigned long duration = doc["duration"] | 1000;
      servoCtrl.setSpeed(speed);
      delay(duration);
      servoCtrl.setSpeed(90);
      resp["ok"] = true;
      resp["speed"] = speed;
      resp["duration_ms"] = duration;
      resp["message"] = "Rotated then stopped";

    } else if (strcmp(action, "pulse") == 0) {
      // Short burst in a direction
      const char* dir = doc["direction"] | "cw";
      unsigned long duration = doc["duration"] | 500;
      int pulseSpeed = (strcmp(dir, "cw") == 0 || strcmp(dir, "clockwise") == 0) ? 0 : 180;
      servoCtrl.setSpeed(pulseSpeed);
      delay(duration);
      servoCtrl.setSpeed(90);
      resp["ok"] = true;
      resp["direction"] = dir;
      resp["duration_ms"] = duration;
      resp["message"] = "Pulsed then stopped";

    } else {
      server->send(400, "application/json", "{\"error\":\"Unknown action\"}");
      return;
    }
  } else if (doc.containsKey("us")) {
    int us = doc["us"];
    servoCtrl.setSpeedUs(us);
    resp["ok"] = true;
    resp["us"] = us;
    resp["message"] = "PWM set";
  } else if (doc.containsKey("speed")) {
    int speed = doc["speed"];
    servoCtrl.setSpeed(speed);
    resp["ok"] = true;
    resp["speed"] = speed;
    resp["message"] = "Speed set";
  } else {
    server->send(400, "application/json", "{\"error\":\"Missing speed or action\"}");
    return;
  }

  char buf[128];
  serializeJson(resp, buf, sizeof(buf));
  server->send(200, "application/json", buf);
}

void NaoNaoServer::handleReboot() {
  server->send(200, "application/json", "{\"ok\":true,\"message\":\"Rebooting...\"}");
  delay(500);
  ESP.restart();
}

void NaoNaoServer::handleLight() {
  if (server->method() == HTTP_GET) {
    StaticJsonDocument<256> doc;
    doc["raw"] = lightSensor.getRawValue();
    doc["state"] = lightSensor.stateName();
    doc["is_dark"] = lightSensor.isDark();
    doc["is_bright"] = lightSensor.isBright();
    doc["brightness_factor"] = lightSensor.getBrightnessFactor();
    doc["threshold_dark"] = LIGHT_DARK_THRESHOLD;
    doc["threshold_bright"] = LIGHT_BRIGHT_THRESHOLD;

    char buf[256];
    serializeJson(doc, buf, sizeof(buf));
    server->send(200, "application/json", buf);
    return;
  }

  // POST: configure thresholds or trigger servo
  String body = server->arg("plain");
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  StaticJsonDocument<128> resp;

  if (doc.containsKey("action")) {
    const char* action = doc["action"];

    if (strcmp(action, "trigger_servo_dark") == 0) {
      // Trigger servo when dark (e.g., close shutter)
      int speed = doc["speed"] | 0;
      unsigned long duration = doc["duration"] | 2000;
      servoCtrl.setSpeed(speed);
      delay(duration);
      servoCtrl.setSpeed(90);
      resp["ok"] = true;
      resp["message"] = "Servo triggered on dark";
    } else if (strcmp(action, "trigger_servo_bright") == 0) {
      // Trigger servo when bright (e.g., open shutter)
      int speed = doc["speed"] | 180;
      unsigned long duration = doc["duration"] | 2000;
      servoCtrl.setSpeed(speed);
      delay(duration);
      servoCtrl.setSpeed(90);
      resp["ok"] = true;
      resp["message"] = "Servo triggered on bright";
    } else {
      server->send(400, "application/json", "{\"error\":\"Unknown action\"}");
      return;
    }
  } else {
    server->send(400, "application/json", "{\"error\":\"Missing action\"}");
    return;
  }

  char buf[128];
  serializeJson(resp, buf, sizeof(buf));
  server->send(200, "application/json", buf);
}

String NaoNaoServer::generateWebPage() {
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>NaoNao OLED</title>";
  html += "<style>";
  html += "body{font-family:-apple-system,BlinkMacSystemFont,sans-serif;max-width:600px;margin:20px auto;padding:15px;background:#f5f5f5}";
  html += "h1{color:#333;margin-bottom:5px}";
  html += ".card{background:#fff;border-radius:8px;padding:15px;margin:10px 0;box-shadow:0 1px 3px rgba(0,0,0,.1)}";
  html += ".card h3{margin:0 0 10px;color:#444}";
  html += "input[type=text],input[type=number]{width:65%;padding:8px;margin:5px 0;border:1px solid #ddd;border-radius:4px}";
  html += "button{padding:8px 16px;background:#4CAF50;color:#fff;border:none;border-radius:4px;cursor:pointer;margin:3px}";
  html += "button:hover{background:#45a049}";
  html += "button.danger{background:#f44336}button.danger:hover{background:#d32f2f}";
  html += "button.secondary{background:#2196F3}button.secondary:hover{background:#1976D2}";
  html += "#result{margin-top:10px;padding:10px;background:#e8f5e9;display:none;border-radius:4px}";
  html += ".status-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}";
  html += ".status-item{padding:5px;background:#f9f9f9;border-radius:4px}";
  html += ".status-item label{font-size:12px;color:#888}";
  html += ".status-item span{font-weight:bold}";
  html += "</style></head><body>";

  html += "<h1>NaoNao OLED</h1>";

  // Status
  html += "<div class='card'><h3>Device Status</h3>";
  html += "<div class='status-grid'>";
  html += "<div class='status-item'><label>WiFi</label><br><span>";
  html += (WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  html += "</span></div>";
  html += "<div class='status-item'><label>IP</label><br><span>";
  html += WiFi.localIP().toString();
  html += "</span></div>";
  html += "<div class='status-item'><label>RSSI</label><br><span>";
  html += WiFi.RSSI();
  html += " dBm</span></div>";
  html += "<div class='status-item'><label>Messages</label><br><span>";
  html += msgQueue.getCount();
  html += "</span></div>";
  html += "<div class='status-item'><label>Uptime</label><br><span>";
  html += (millis() / 1000);
  html += "s</span></div>";
  html += "<div class='status-item'><label>Free Heap</label><br><span>";
  html += ESP.getFreeHeap() / 1024;
  html += " KB</span></div>";
  html += "</div></div>";

  // Send message
  html += "<div class='card'><h3>Send Message</h3>";
  html += "<form id='msgForm'>";
  html += "<input type='text' id='message' placeholder='Enter message...' required>";
  html += "<button type='submit'>Send</button>";
  html += "</form><div id='result'></div></div>";

  // Timer
  html += "<div class='card'><h3>Countdown Timer</h3>";
  html += "<input type='number' id='timerDuration' value='300' min='1' placeholder='Seconds'>";
  html += "<button onclick='startTimer()'>Start</button><br>";
  html += "<button class='secondary' onclick='pauseTimer()'>Pause</button>";
  html += "<button class='secondary' onclick='resumeTimer()'>Resume</button>";
  html += "<button class='danger' onclick='resetTimer()'>Reset</button>";
  html += "<div id='timerStatus' style='margin-top:8px;font-size:14px'></div></div>";

  // Quick actions
  html += "<div class='card'><h3>Quick Actions</h3>";
  html += "<button onclick=\"sendPreset('Hello!')\">Hello!</button>";
  html += "<button onclick=\"sendPreset('你好！')\">你好！</button>";
  html += "<button class='danger' onclick=\"clearMessages()\">Clear All</button>";
  html += "<button class='danger' onclick=\"reboot()\">Reboot</button></div>";

  html += "<script>";
  html += "document.getElementById('msgForm').addEventListener('submit',function(e){";
  html += "e.preventDefault();var msg=document.getElementById('message').value;";
  html += "fetch('/msg',{method:'POST',body:msg}).then(r=>r.text()).then(function(data){";
  html += "var r=document.getElementById('result');r.style.display='block';";
  html += "r.textContent='Sent: '+data;document.getElementById('message').value='';";
  html += "setTimeout(function(){r.style.display='none'},3000)});});";
  html += "function sendPreset(m){fetch('/msg',{method:'POST',body:m}).then(function(){alert('Sent: '+m)})}";
  html += "function clearMessages(){fetch('/clear',{method:'POST'}).then(function(){alert('Cleared')})}";
  html += "function reboot(){if(confirm('Reboot device?')){fetch('/reboot',{method:'POST'}).then(function(){alert('Rebooting...')})}}";

  html += "function startTimer(){";
  html += "var d=document.getElementById('timerDuration').value||300;";
  html += "fetch('/timer',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'start',duration:parseInt(d)})}).then(r=>r.json()).then(showTimer)};";
  html += "function pauseTimer(){fetch('/timer',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'pause'})}).then(r=>r.json()).then(showTimer)};";
  html += "function resumeTimer(){fetch('/timer',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'resume'})}).then(r=>r.json()).then(showTimer)};";
  html += "function resetTimer(){fetch('/timer',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'reset'})}).then(r=>r.json()).then(showTimer)};";
  html += "function showTimer(d){var s=document.getElementById('timerStatus');var state=['Idle','Running','Paused','Done'][d.state]||'?';s.textContent=state+' | '+d.remaining+'s remaining'};";
  html += "</script>";

  html += "</body></html>";
  return html;
}

// Global server instance
NaoNaoServer naoNaoServer(80);
