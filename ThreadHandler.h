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
  static int _current_thread_id;
  static int _quantum_time;
  static int _quantum_count;


public:
  static struct itimerval timer;
  ThreadHandler();
  static const std::map<int, Thread> &get_threads ();
  static const std::queue<int> &get_ready_states ();
  static Thread& get_current_thread();
  static  int get_current_thread_id();
  static int get_quantum_time ();
  static Thread& get_thread(int id);
  static Thread& pop_thread();
  static int get_number_of_threads();
  static void add_thread(int id, thread_entry_point _entry_point);
  static void set_quantum_time(int quantum_time);
  static void delete_thread(int id);
  static void free_all_threads();
  static void block_thread(int id);
  static void init_timer();
  static void reset_timer();
  static int get_quantum_count();
  static void add_thread_to_ready_queue(int id);
  static int set_first_ready_to_running(int id);
};

#endif //_THREADHANDLER_H_

