//
// Created by TLP-264 on 09/06/2024.
//

#ifndef _THREADHANDLER_H_
#define _THREADHANDLER_H_

#include "Thread.h"

class ThreadHandler{
 private:
  static std::map<int, Thread> _threads;
  static std::queue<int> _ready_states;
  static Thread _current_thread;
  static int _quantum_time;

 public:
  ThreadHandler();
  static Thread& get_thread(int id);
  static Thread& pop_thread();
  static int get_number_of_threads();
  static void add_thread(int id, thread_entry_point _entry_point);
  static void set_quantum_time(int quantum_time);
};

#endif //_THREADHANDLER_H_

