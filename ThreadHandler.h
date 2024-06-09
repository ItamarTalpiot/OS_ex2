//
// Created by TLP-264 on 09/06/2024.
//

#ifndef _THREADHANDLER_H_
#define _THREADHANDLER_H_

#include "Thread.h"

class ThreadHandler{
 private:
  static std::map<int, Thread> _threads;
  static std::queue<int> ready_states;
  static Thread _current_thread;

 public:
  ThreadHandler();
  static Thread& get_thread(int id);
  static int pop_thread();
  static int add_thread(int id);
};
#endif //_THREADHANDLER_H_

