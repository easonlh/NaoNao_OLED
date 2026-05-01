#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>

// DHT11 温湿度传感器引脚
#define DHT_DATA_PIN 5  // GPIO5 (D5)

// 舒适度阈值
#define COMFORT_TEMP_LOW    18.0f
#define COMFORT_TEMP_HIGH   28.0f
#define COMFORT_HUM_LOW     30.0f
#define COMFORT_HUM_HIGH    70.0f

// 数据刷新间隔 (ms)
#define DHT_READ_INTERVAL   2000  // DHT11 最快 1Hz，留 2s 间隔

// 舒适度等级
enum ComfortLevel {
  COMFORT_UNKNOWN,
  COMFORT_COMFORTABLE,  // 舒适
  COMFORT_HOT,          // 偏热
  COMFORT_COLD,         // 偏冷
  COMFORT_HUMID,        // 潮湿
  COMFORT_DRY,          // 干燥
  COMFORT_HOT_HUMID,    // 闷热
  COMFORT_COLD_DRY      // 寒冷干燥
};

// DHT 数据结构
struct DhtData {
  bool valid;
  float temperature;    // °C
  float humidity;       // %
  float heatIndex;      // 体感温度 °C
  ComfortLevel comfort;
  unsigned long lastRead;
};

class DhtSensor {
public:
  DhtSensor();

  void begin();
  void update();

  DhtData& getData();
  bool isReady() const;

  float getTemperature() const;
  float getHumidity() const;
  float getHeatIndex() const;
  ComfortLevel getComfort() const;
  const char* comfortName() const;

private:
  DhtData data;
  unsigned long lastReadTime;
  bool initialized;

  float calculateHeatIndex(float temp, float humidity);
  ComfortLevel evaluateComfort(float temp, float humidity);
};

extern DhtSensor dhtSensor;

#endif  // DHT_SENSOR_H
