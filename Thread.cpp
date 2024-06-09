#include "Thread.h"

Thread::Thread(int id, thread_entry_point entry_point)
{
    _id = id;
    _state = READY;
    _entry_point = entry_point;
    if (id != 0)
        stack = new char[STACK_SIZE];
    else
        stack = nullptr;
}

sigjmp_buf& Thread::get_buf_ref()
{
    return _env;
}

void Thread::free_thread()
{
    free(stack);
}