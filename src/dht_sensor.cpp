#include "dht_sensor.h"
#include <DHT.h>

static DHT dht(DHT_DATA_PIN, DHT11);

DhtSensor dhtSensor;

DhtSensor::DhtSensor()
  : lastReadTime(0), initialized(false) {
  data.valid = false;
  data.temperature = 0;
  data.humidity = 0;
  data.heatIndex = 0;
  data.comfort = COMFORT_UNKNOWN;
  data.lastRead = 0;
}

void DhtSensor::begin() {
  dht.begin();
  initialized = true;
  Serial.println("DHT11 sensor initialized on GPIO5");

  // 首次读取需要等待传感器就绪（DHT11 上电后 1 秒）
  delay(1000);
}

void DhtSensor::update() {
  if (!initialized) return;

  unsigned long now = millis();
  if (now - lastReadTime < DHT_READ_INTERVAL) return;
  lastReadTime = now;

  float h = dht.readHumidity();
  float t = dht.readTemperature();  // 默认摄氏度

  // 跳过无效读数（DHT 偶尔会返回 NaN）
  if (isnan(h) || isnan(t)) return;

  data.temperature = t;
  data.humidity = h;
  data.heatIndex = calculateHeatIndex(t, h);
  data.comfort = evaluateComfort(t, h);
  data.valid = true;
  data.lastRead = now;
}

DhtData& DhtSensor::getData() {
  return data;
}

bool DhtSensor::isReady() const {
  return data.valid;
}

float DhtSensor::getTemperature() const {
  return data.temperature;
}

float DhtSensor::getHumidity() const {
  return data.humidity;
}

float DhtSensor::getHeatIndex() const {
  return data.heatIndex;
}

ComfortLevel DhtSensor::getComfort() const {
  return data.comfort;
}

const char* DhtSensor::comfortName() const {
  switch (data.comfort) {
    case COMFORT_COMFORTABLE: return "舒适";
    case COMFORT_HOT:         return "偏热";
    case COMFORT_COLD:        return "偏冷";
    case COMFORT_HUMID:       return "潮湿";
    case COMFORT_DRY:         return "干燥";
    case COMFORT_HOT_HUMID:   return "闷热";
    case COMFORT_COLD_DRY:    return "寒干";
    default:                  return "未知";
  }
}

float DhtSensor::calculateHeatIndex(float t, float h) {
  // 简化版体感温度公式（Rothfusz 回归）
  // 适用于温度 > 27°C 且湿度 > 40% 的场景
  if (t < 27.0f) return t;

  float hi = 0.5f * (t + 61.0f + ((t - 68.0f) * 1.2f) + (h * 0.094f));
  return (hi + t) / 2.0f;  // 简化平均
}

ComfortLevel DhtSensor::evaluateComfort(float t, float h) {
  bool hot = t > COMFORT_TEMP_HIGH;
  bool cold = t < COMFORT_TEMP_LOW;
  bool humid = h > COMFORT_HUM_HIGH;
  bool dry = h < COMFORT_HUM_LOW;

  if (hot && humid) return COMFORT_HOT_HUMID;
  if (hot) return COMFORT_HOT;
  if (cold && dry) return COMFORT_COLD_DRY;
  if (cold) return COMFORT_COLD;
  if (humid) return COMFORT_HUMID;
  if (dry) return COMFORT_DRY;
  return COMFORT_COMFORTABLE;
}
