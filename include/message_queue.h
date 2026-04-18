#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <Arduino.h>

#define MAX_MSG_LENGTH 128
#define MSG_QUEUE_MAX_SIZE 10

struct Message {
  char text[MAX_MSG_LENGTH];
  unsigned long timestamp;
  bool read;
};

class MessageQueue {
private:
  Message messages[MSG_QUEUE_MAX_SIZE];
  int head;
  int tail;
  int count;

public:
  MessageQueue();
  
  // 添加消息到队列
  bool addMessage(const char* msg);
  
  // 获取当前消息（用于显示）
  Message* getCurrentMessage();
  
  // 切换到下一条消息
  bool nextMessage();
  
  // 切换到上一条消息
  bool prevMessage();
  
  // 获取消息数量
  int getCount();
  
  // 清空队列
  void clear();
  
  // 获取队列信息字符串
  String getQueueInfo();
};

// 全局消息队列
extern MessageQueue msgQueue;

#endif
