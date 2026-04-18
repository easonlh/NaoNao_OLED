#ifndef MQTT_CLIENT_WRAPPER_H
#define MQTT_CLIENT_WRAPPER_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

class MqttClientWrapper {
private:
  WiFiClient wifiClient;
  PubSubClient* mqttClient;
  unsigned long lastPublish;
  unsigned long lastReconnectAttempt;
  int reconnectDelay;
  bool enabled;

  bool reconnect();

public:
  MqttClientWrapper();

  void callback(char* topic, byte* payload, unsigned int length);
  void begin();
  void loop();
  bool isConnected();
  void publish(const char* topic, const char* payload);
  void publishStatus();
};

extern MqttClientWrapper mqttWrapper;

#endif
