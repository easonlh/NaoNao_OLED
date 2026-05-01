#include "pomodoro.h"
#include "servo_control.h"

PomodoroTimer pomodoro;

PomodoroTimer::PomodoroTimer()
  : phase(POMO_IDLE), totalMs(0), remainingMs(0), lastTick(0),
    completedSessions(0), workMinutes(POMO_WORK_MINUTES),
    shortBreakMinutes(POMO_SHORT_BREAK_MINUTES),
    longBreakMinutes(POMO_LONG_BREAK_MINUTES),
    paused(false), servoAlertPending(false) {}

void PomodoroTimer::begin() {
  phase = POMO_IDLE;
}

void PomodoroTimer::update() {
  if (phase == POMO_IDLE || paused) return;

  unsigned long now = millis();
  unsigned long elapsed = now - lastTick;
  lastTick = now;

  if (elapsed >= remainingMs) {
    // 阶段结束
    remainingMs = 0;

    if (phase == POMO_WORK) {
      completedSessions++;
      servoAlertPending = true;  // 触发舵机提醒

      if (completedSessions % POMO_SESSIONS_BEFORE_LONG == 0) {
        startPhase(POMO_LONG_BREAK, longBreakMinutes);
      } else {
        startPhase(POMO_SHORT_BREAK, shortBreakMinutes);
      }
    } else {
      // 休息结束，自动开始工作
      servoAlertPending = true;
      startPhase(POMO_WORK, workMinutes);
    }
  } else {
    remainingMs -= elapsed;
  }
}

void PomodoroTimer::startWork(unsigned long minutes) {
  startPhase(POMO_WORK, minutes);
}

void PomodoroTimer::startBreak(unsigned long minutes) {
  startPhase(POMO_SHORT_BREAK, minutes);
}

void PomodoroTimer::pause() {
  if (phase != POMO_IDLE) {
    paused = true;
  }
}

void PomodoroTimer::resume() {
  if (paused) {
    paused = false;
    lastTick = millis();
  }
}

void PomodoroTimer::reset() {
  phase = POMO_IDLE;
  remainingMs = 0;
  totalMs = 0;
  paused = false;
  completedSessions = 0;
  servoAlertPending = false;
}

void PomodoroTimer::skip() {
  remainingMs = 0;
  // update() 会处理阶段切换
}

PomodoroPhase PomodoroTimer::getPhase() const { return phase; }
unsigned long PomodoroTimer::getRemainingSec() const { return remainingMs / 1000; }
unsigned long PomodoroTimer::getTotalSec() const { return totalMs / 1000; }
int PomodoroTimer::getCompletedSessions() const { return completedSessions; }
bool PomodoroTimer::isRunning() const { return phase != POMO_IDLE && !paused; }
bool PomodoroTimer::isPaused() const { return paused; }

const char* PomodoroTimer::phaseName() const {
  switch (phase) {
    case POMO_WORK:        return "专注";
    case POMO_SHORT_BREAK: return "短休息";
    case POMO_LONG_BREAK:  return "长休息";
    default:               return "番茄钟";
  }
}

void PomodoroTimer::setWorkMinutes(int min) { workMinutes = min; }
void PomodoroTimer::setBreakMinutes(int min) { shortBreakMinutes = min; }

bool PomodoroTimer::needsServoAlert() {
  if (servoAlertPending) {
    servoAlertPending = false;
    return true;
  }
  return false;
}

void PomodoroTimer::startPhase(PomodoroPhase newPhase, unsigned long minutes) {
  phase = newPhase;
  totalMs = minutes * 60 * 1000;
  remainingMs = totalMs;
  lastTick = millis();
  paused = false;
}
