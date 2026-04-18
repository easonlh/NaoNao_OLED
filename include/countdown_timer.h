#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include <Arduino.h>

enum TimerState {
  TIMER_IDLE,
  TIMER_RUNNING,
  TIMER_PAUSED,
  TIMER_FINISHED
};

class CountdownTimer {
private:
  unsigned long totalDurationMs;
  unsigned long remainingMs;
  unsigned long lastTickMs;
  TimerState state;
  bool finishedNotified;

public:
  CountdownTimer();

  void start(unsigned long durationMs);
  void pause();
  void resume();
  void reset();
  void update();
  bool isRunning();
  unsigned long getRemainingSec();
  unsigned long getTotalSec();
  TimerState getState();
  bool hasFinishedNotified();
  void markNotified();
};

extern CountdownTimer countdownTimer;

#endif
