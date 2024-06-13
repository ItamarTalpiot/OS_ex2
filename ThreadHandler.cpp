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
    _threads.at(id).free_thread();
    _threads.erase(id);
}

const std::map<int, Thread> &ThreadHandler::get_threads ()
{
  return _threads;
}
const std::queue<int> &ThreadHandler::get_ready_states ()
{
  return _ready_states;
}
const Thread &ThreadHandler::get_current_thread ()
{
  return _current_thread;
}
int ThreadHandler::get_quantum_time ()
{
  return _quantum_time;
}

void ThreadHandler::free_all_threads()
{
    for (auto t_it = myMap.begin(); t_it != myMap.end(); ++t_it)
    {
        Thread t = t_it->second;
        t.free_thread();
    }
}
