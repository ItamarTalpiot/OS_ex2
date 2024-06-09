#include <iostream>
#include <map>
#include <queue>
#include "ThreadHandler.h"
#include "Thread.h"


ThreadHandler::ThreadHandler()
{
  _threads.insert({0, Thread(0, nullptr)});
}



Thread& ThreadHandler::get_thread (int id)
{
  // this function gets thread id and return the thread with that id
  return _threads.at (id);
}

Thread& ThreadHandler::pop_thread()
{
  int thread_id = _ready_states.front();
  _ready_states.pop();
  return _threads.at(thread_id);
}

void ThreadHandler::add_thread (int id, thread_entry_point _entry_point)
{
  _threads.insert({id, Thread(id,_entry_point)});
  _ready_states.push (id);
}

int ThreadHandler::get_number_of_threads ()
{
  return _threads.size();
}
void ThreadHandler::set_quantum_time (int quantum_time)
{
  _quantum_time = quantum_time;
}

void ThreadHandler::delete_thread(int id) {
    _threads.erase(id);
}
