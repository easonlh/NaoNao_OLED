#include "sleep_tracker.h"

SleepTracker sleepTracker;

SleepTracker::SleepTracker()
  : recordIndex(0), recordCount(0), sleeping(false), sleepStartTime(0) {
  for (int i = 0; i < SLEEP_HISTORY_DAYS; i++) {
    records[i].valid = false;
    records[i].durationMin = 0;
  }
}

void SleepTracker::begin() {
  // 初始化，不需要特殊操作
}

void SleepTracker::update() {
  // 主要由 onLightChanged 驱动
}

void SleepTracker::onLightChanged(bool isDark) {
  unsigned long now = millis();

  if (isDark && !sleeping) {
    // 天黑了，开始记录睡眠
    sleeping = true;
    sleepStartTime = now;
  } else if (!isDark && sleeping) {
    // 天亮了，结束睡眠记录
    sleeping = false;
    unsigned long durationMs = now - sleepStartTime;
    unsigned long durationMin = durationMs / 60000;

    // 只记录超过 30 分钟的"睡眠"（避免短暂遮光误触发）
    if (durationMin >= 30) {
      addRecord(sleepStartTime, now, durationMin);
    }
  }
}

unsigned long SleepTracker::getLastSleepMinutes() const {
  if (recordCount == 0) return 0;
  int lastIdx = (recordIndex - 1 + SLEEP_HISTORY_DAYS) % SLEEP_HISTORY_DAYS;
  return records[lastIdx].valid ? records[lastIdx].durationMin : 0;
}

unsigned long SleepTracker::getAverageSleepMinutes() const {
  if (recordCount == 0) return 0;
  unsigned long total = 0;
  int count = 0;
  for (int i = 0; i < SLEEP_HISTORY_DAYS; i++) {
    if (records[i].valid) {
      total += records[i].durationMin;
      count++;
    }
  }
  return count > 0 ? total / count : 0;
}

int SleepTracker::getRecordCount() const { return recordCount; }

const SleepRecord& SleepTracker::getRecord(int index) const {
  return records[index % SLEEP_HISTORY_DAYS];
}

bool SleepTracker::isCurrentlySleeping() const { return sleeping; }

String SleepTracker::getSleepSummary() const {
  String json = "{";
  json += "\"currently_sleeping\":";
  json += sleeping ? "true" : "false";
  json += ",\"last_sleep_minutes\":";
  json += getLastSleepMinutes();
  json += ",\"average_sleep_minutes\":";
  json += getAverageSleepMinutes();
  json += ",\"records\":[";
  for (int i = 0; i < recordCount && i < SLEEP_HISTORY_DAYS; i++) {
    int idx = (recordIndex - recordCount + i + SLEEP_HISTORY_DAYS) % SLEEP_HISTORY_DAYS;
    if (i > 0) json += ",";
    json += "{\"duration_min\":";
    json += records[idx].durationMin;
    json += ",\"valid\":";
    json += records[idx].valid ? "true" : "false";
    json += "}";
  }
  json += "]}";
  return json;
}

void SleepTracker::addRecord(unsigned long start, unsigned long end, unsigned long duration) {
  records[recordIndex].sleepStart = start;
  records[recordIndex].wakeUp = end;
  records[recordIndex].durationMin = duration;
  records[recordIndex].valid = true;
  recordIndex = (recordIndex + 1) % SLEEP_HISTORY_DAYS;
  if (recordCount < SLEEP_HISTORY_DAYS) recordCount++;
}
