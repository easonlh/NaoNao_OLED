#ifndef SLEEP_TRACKER_H
#define SLEEP_TRACKER_H

#include <Arduino.h>

// 存储最近 7 天的睡眠记录
#define SLEEP_HISTORY_DAYS 7

struct SleepRecord {
  unsigned long sleepStart;   // 入睡时间戳 (millis)
  unsigned long wakeUp;       // 醒来时间戳 (millis)
  unsigned long durationMin;  // 睡眠时长 (分钟)
  bool valid;
};

class SleepTracker {
public:
  SleepTracker();

  void begin();
  void update();

  // 光照变化时调用
  void onLightChanged(bool isDark);

  // 查询
  unsigned long getLastSleepMinutes() const;
  unsigned long getAverageSleepMinutes() const;
  int getRecordCount() const;
  const SleepRecord& getRecord(int index) const;
  bool isCurrentlySleeping() const;

  // API 用：格式化输出
  String getSleepSummary() const;

private:
  SleepRecord records[SLEEP_HISTORY_DAYS];
  int recordIndex;
  int recordCount;
  bool sleeping;
  unsigned long sleepStartTime;

  void addRecord(unsigned long start, unsigned long end, unsigned long duration);
};

extern SleepTracker sleepTracker;

#endif  // SLEEP_TRACKER_H
