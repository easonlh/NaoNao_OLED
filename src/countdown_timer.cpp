#include "countdown_timer.h"

CountdownTimer::CountdownTimer()
    : totalDurationMs(0), remainingMs(0), lastTickMs(0),
      state(TIMER_IDLE), finishedNotified(false) {}

void CountdownTimer::start(unsigned long durationMs) {
  totalDurationMs = durationMs;
  remainingMs = durationMs;
  lastTickMs = millis();
  state = TIMER_RUNNING;
  finishedNotified = false;
}

void CountdownTimer::pause() {
  if (state == TIMER_RUNNING) {
    state = TIMER_PAUSED;
  }
}

void CountdownTimer::resume() {
  if (state == TIMER_PAUSED) {
    lastTickMs = millis();
    state = TIMER_RUNNING;
  }
}

void CountdownTimer::reset() {
  state = TIMER_IDLE;
  remainingMs = 0;
  totalDurationMs = 0;
  finishedNotified = false;
}

void CountdownTimer::update() {
  if (state != TIMER_RUNNING) return;

  unsigned long now = millis();
  unsigned long elapsed = now - lastTickMs;
  lastTickMs = now;

  if (elapsed >= remainingMs) {
    remainingMs = 0;
    state = TIMER_FINISHED;
  } else {
    remainingMs -= elapsed;
  }
}

bool CountdownTimer::isRunning() {
  return state == TIMER_RUNNING;
}

unsigned long CountdownTimer::getRemainingSec() {
  return remainingMs / 1000;
}

unsigned long CountdownTimer::getTotalSec() {
  return totalDurationMs / 1000;
}

TimerState CountdownTimer::getState() {
  return state;
}

bool CountdownTimer::hasFinishedNotified() {
  return finishedNotified;
}

void CountdownTimer::markNotified() {
  finishedNotified = true;
}

CountdownTimer countdownTimer;
