#include <iostream>
#include <map>
#include <queue>
#include "ThreadHandler.h"
#include "Thread.h"

std::map<int, Thread*> ThreadHandler::_threads;
std::queue<int> ThreadHandler::_ready_states;
int ThreadHandler::_current_thread_id = 0;
int ThreadHandler::_quantum_time = 0;
int ThreadHandler::_quantum_count = 0;
struct itimerval ThreadHandler::timer = {};


void print_system_error_message(std::string str)
{
    std::cerr << "system error: " << str << std::endl;
}


void remove_element_from_queue(std::queue<int>& q, const int& value) {
    std::queue<int> temp_queue;

    while (!q.empty()) {
        if (q.front() != value) {
            temp_queue.push(q.front());
        }

        q.pop();
    }

    q = std::move(temp_queue);
}


void scheduler(int sig)
{
    for (auto thread_pair: ThreadHandler::get_threads())
    {
        Thread* blocked_thread = thread_pair.second;
        if (blocked_thread->_quanto_block_time == 0)
        {
            blocked_thread->set_status(READY);
            ThreadHandler::add_thread_to_ready_queue(thread_pair.first); //add thread to end of queue
        }
        if (blocked_thread->get_status() == BLOCKED and blocked_thread->_quanto_block_time > 0)
        {
            blocked_thread->_quanto_block_time--;
        }
    }

  int curr_id  = ThreadHandler::get_current_thread_id();
  STATE thread_status = ThreadHandler::get_current_thread()->get_status();

  if (thread_status != BLOCKED && thread_status != TERMINATED) // if quantum ended
    {
        ThreadHandler::get_current_thread()->set_status(READY);
        ThreadHandler::add_thread_to_ready_queue(curr_id); //add thread to end of queue
    }

    ThreadHandler::set_first_ready_to_running(curr_id); //set current thread running (if false

    ThreadHandler::get_current_thread()->inc_count();
    thread_entry_point entry_point = ThreadHandler::get_current_thread()->get_entry_point(); // running func
    if (entry_point)
    {
        entry_point();
    }
}


ThreadHandler::ThreadHandler()
{
  _threads.insert({0, new Thread(0, nullptr)});
}



Thread* ThreadHandler::get_thread (int id)
{
  // this function gets thread id and return the thread with that id
  return _threads.at (id);
}

Thread* ThreadHandler::pop_thread()
{
  int thread_id = _ready_states.front();
  _ready_states.pop();
  return _threads.at(thread_id);
}

void ThreadHandler::add_thread (int id, thread_entry_point _entry_point)
{
  _threads.insert({id, new Thread(id,_entry_point)});
  _ready_states.push (id);
}

int ThreadHandler::get_number_of_threads ()
{
  return (int) _threads.size();
}
void ThreadHandler::set_quantum_time (int quantum_time)
{
  _quantum_time = quantum_time;
}

void ThreadHandler::delete_thread(int id) {
    remove_element_from_queue(_ready_states, id);
    _threads.at(id)->free_thread();
    _threads.erase(id);
}

std::map<int, Thread*> &ThreadHandler::get_threads ()
{
  return _threads;
}
std::queue<int> &ThreadHandler::get_ready_states ()
{
  return _ready_states;
}
Thread *ThreadHandler::get_current_thread ()
{
  return _threads.at(_current_thread_id);;
}
int ThreadHandler::get_quantum_time ()
{
  return _quantum_time;
}
int ThreadHandler::get_current_thread_id ()
{
  return _current_thread_id;
}
void ThreadHandler::block_thread (int id)
{
  remove_element_from_queue(_ready_states, id);

  _threads.at(id)->set_status(BLOCKED);
  _threads.at(id)->_quanto_block_time = -1;
  if(_current_thread_id == id){
    reset_timer();
  }
}

void ThreadHandler::free_all_threads()
{
    for (auto & _thread : _threads)
    {
        _thread.second->free_thread();
        delete(_thread.second);
    }
}

void ThreadHandler::add_thread_to_ready_queue (int id)
{
  _ready_states.push(id);
}

int ThreadHandler::set_first_ready_to_running (int id)
{
  if(_ready_states.empty()){
    _current_thread_id = id;
    _threads.at(id)->set_status (RUNNING);
    return -1;
  }
  int first_thread_id = _ready_states.front();
  _ready_states.pop();
  _threads.at(first_thread_id)->set_status (RUNNING);
  _current_thread_id = first_thread_id;
  return 0;
}


void ThreadHandler::init_timer() {
    struct sigaction sa = {0};

    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &scheduler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
    {
        print_system_error_message("sigaction error.");
    }

    timer.it_value.tv_sec = 0;        // first time interval, seconds part
    timer.it_value.tv_usec = _quantum_time;        // first time interval, microseconds part

    timer.it_interval.tv_sec = 0;    // following time intervals, seconds part
    timer.it_interval.tv_usec = _quantum_time;    // following time intervals, microseconds part

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        print_system_error_message("setitimer error.");
    }
}

int ThreadHandler::get_quantum_count() {
    return _quantum_count;
}

void ThreadHandler::reset_timer() {
    timer.it_value.tv_sec = 0;        // first time interval, seconds part
    timer.it_value.tv_usec = _quantum_time;        // first time interval, microseconds part

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        print_system_error_message("setitimer error.");
    }
}

void ThreadHandler::inc_global_quantum() {
    _quantum_count++;
}


