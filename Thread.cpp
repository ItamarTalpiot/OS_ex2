#include "Thread.h"
#include <iostream>


Thread::Thread(int id, thread_entry_point entry_point)
{
    _id = id;
    _state = READY;
    _entry_point = entry_point;
    _quantum_time_spent_in_running = 0;
    _quanto_block_time = -1;

    try {
        stack = new char[STACK_SIZE];
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "system error: memory allocation (stack)" << std::endl;
        exit(1);
    }


}

void Thread::free_thread()
{
    if (stack)
        delete[] stack;
}

void Thread::set_status(STATE s)
{
    _state = s;
}
int Thread::get_quantum_time_spent_in_running ()
{
  return _quantum_time_spent_in_running;
}



STATE Thread::get_status() {
    return _state;
}

thread_entry_point Thread::get_entry_point() {
    return _entry_point;
}

void Thread::inc_count() {
    _quantum_time_spent_in_running++;
}
