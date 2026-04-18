#include "message_queue.h"
#include <cstring>

MessageQueue::MessageQueue() : head(0), tail(0), count(0) {
}

bool MessageQueue::addMessage(const char* msg) {
  if (strlen(msg) >= MAX_MSG_LENGTH) {
    return false;
  }
  
  // 如果队列满了，移除最旧的消息
  if (count >= MSG_QUEUE_MAX_SIZE) {
    head = (head + 1) % MSG_QUEUE_MAX_SIZE;
    count--;
  }
  
  // 添加新消息到队尾
  strncpy(messages[tail].text, msg, MAX_MSG_LENGTH - 1);
  messages[tail].text[MAX_MSG_LENGTH - 1] = '\0';
  messages[tail].timestamp = millis();
  messages[tail].read = false;
  
  tail = (tail + 1) % MSG_QUEUE_MAX_SIZE;
  count++;
  
  return true;
}

Message* MessageQueue::getCurrentMessage() {
  if (count == 0) {
    return nullptr;
  }
  return &messages[head];
}

bool MessageQueue::nextMessage() {
  if (count <= 1) {
    return false;
  }
  head = (head + 1) % MSG_QUEUE_MAX_SIZE;
  count--;
  
  // 重新排列：将旧head放到tail位置
  // 简化实现：只是移动head指针
  return true;
}

bool MessageQueue::prevMessage() {
  if (count <= 1) {
    return false;
  }
  head = (head - 1 + MSG_QUEUE_MAX_SIZE) % MSG_QUEUE_MAX_SIZE;
  count++;
  return true;
}

int MessageQueue::getCount() {
  return count;
}

void MessageQueue::clear() {
  head = 0;
  tail = 0;
  count = 0;
}

String MessageQueue::getQueueInfo() {
  String info = "Messages: ";
  info += count;
  info += "/";
  info += MSG_QUEUE_MAX_SIZE;
  return info;
}

// 全局消息队列实例
MessageQueue msgQueue;
