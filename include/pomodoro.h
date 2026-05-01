#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>

// 番茄钟阶段
enum PomodoroPhase {
  POMO_IDLE,
  POMO_WORK,       // 专注
  POMO_SHORT_BREAK,// 短休息
  POMO_LONG_BREAK  // 长休息
};

// 番茄钟配置
#define POMO_WORK_MINUTES       25
#define POMO_SHORT_BREAK_MINUTES 5
#define POMO_LONG_BREAK_MINUTES  15
#define POMO_SESSIONS_BEFORE_LONG 4

// 舵机提醒
#define POMO_SERVO_ALERT_SPEED   120   // 提醒时舵机速度
#define POMO_SERVO_ALERT_MS      1500  // 提醒时舵机时长

class PomodoroTimer {
public:
  PomodoroTimer();

  void begin();
  void update();

  // 控制
  void startWork(unsigned long minutes = POMO_WORK_MINUTES);
  void startBreak(unsigned long minutes = POMO_SHORT_BREAK_MINUTES);
  void pause();
  void resume();
  void reset();
  void skip();

  // 状态查询
  PomodoroPhase getPhase() const;
  unsigned long getRemainingSec() const;
  unsigned long getTotalSec() const;
  int getCompletedSessions() const;
  bool isRunning() const;
  bool isPaused() const;
  const char* phaseName() const;

  // 配置
  void setWorkMinutes(int min);
  void setBreakMinutes(int min);

  // 舵机提醒回调
  bool needsServoAlert();

private:
  PomodoroPhase phase;
  unsigned long totalMs;
  unsigned long remainingMs;
  unsigned long lastTick;
  int completedSessions;
  int workMinutes;
  int shortBreakMinutes;
  int longBreakMinutes;
  bool paused;
  bool servoAlertPending;

  void startPhase(PomodoroPhase newPhase, unsigned long minutes);
};

extern PomodoroTimer pomodoro;

#endif  // POMODORO_H
