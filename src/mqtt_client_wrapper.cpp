#include "mqtt_client_wrapper.h"
#include "config.h"
#include "message_queue.h"
#include <WiFi.h>
#include <ArduinoJson.h>

// Static callback wrapper to forward to instance
static MqttClientWrapper* g_instance = nullptr;
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (g_instance) g_instance->callback(topic, payload, length);
}

MqttClientWrapper::MqttClientWrapper()
    : mqttClient(nullptr), lastPublish(0), lastReconnectAttempt(0),
      reconnectDelay(1000), enabled(false) {
  g_instance = this;
}

void MqttClientWrapper::begin() {
  if (strlen(CFG_MQTT_BROKER) == 0) {
    enabled = false;
    return;
  }

  enabled = true;
  mqttClient = new PubSubClient(wifiClient);
  mqttClient->setServer(CFG_MQTT_BROKER, CFG_MQTT_PORT);
  mqttClient->setCallback(mqttCallback);
  mqttClient->setBufferSize(512);

  Serial.printf("MQTT: Connecting to %s:%d\n", CFG_MQTT_BROKER, CFG_MQTT_PORT);
  reconnect();
}

bool MqttClientWrapper::reconnect() {
  if (!enabled || !mqttClient) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  Serial.println("MQTT: Attempting reconnect...");

  if (strlen(CFG_MQTT_USERNAME) > 0) {
    if (mqttClient->connect(CFG_MQTT_CLIENT_ID, CFG_MQTT_USERNAME, CFG_MQTT_PASSWORD)) {
      Serial.println("MQTT: Connected with auth");
    }
  } else {
    if (mqttClient->connect(CFG_MQTT_CLIENT_ID)) {
      Serial.println("MQTT: Connected");
    }
  }

  if (mqttClient->connected()) {
    mqttClient->subscribe(CFG_MQTT_SUBSCRIBE_TOPIC);
    Serial.printf("MQTT: Subscribed to %s\n", CFG_MQTT_SUBSCRIBE_TOPIC);
    reconnectDelay = 1000;
    return true;
  }

  reconnectDelay = min(reconnectDelay * 2, 30000);
  Serial.printf("MQTT: Connect failed, retry in %dms\n", reconnectDelay);
  return false;
}

void MqttClientWrapper::callback(char* topic, byte* payload, unsigned int length) {
  char msgBuf[256];
  unsigned int copyLen = min(length, sizeof(msgBuf) - 1);
  memcpy(msgBuf, payload, copyLen);
  msgBuf[copyLen] = '\0';

  Serial.printf("MQTT [%s]: %s\n", topic, msgBuf);
  msgQueue.addMessage(msgBuf);
}

void MqttClientWrapper::loop() {
  if (!enabled || !mqttClient) return;

  if (!mqttClient->connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectDelay) {
      lastReconnectAttempt = now;
      reconnect();
    }
    return;
  }

  mqttClient->loop();

  unsigned long now = millis();
  if (now - lastPublish > CFG_MQTT_PUBLISH_INTERVAL_SEC * 1000UL) {
    lastPublish = now;
    publishStatus();
  }
}

bool MqttClientWrapper::isConnected() {
  return enabled && mqttClient && mqttClient->connected();
}

void MqttClientWrapper::publish(const char* topic, const char* payload) {
  if (!isConnected()) return;
  mqttClient->publish(topic, payload);
}

void MqttClientWrapper::publishStatus() {
  if (!isConnected()) return;

  StaticJsonDocument<256> doc;
  doc["ip"] = WiFi.localIP().toString();
  doc["rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();

  char buf[256];
  serializeJson(doc, buf, sizeof(buf));
  mqttClient->publish(CFG_MQTT_PUBLISH_TOPIC, buf);
}

MqttClientWrapper mqttWrapper;
