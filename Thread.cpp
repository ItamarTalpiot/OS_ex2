#include "Thread.h"

Thread::Thread(int id, thread_entry_point entry_point)
{
    _id = id;
    _state = READY;
    _entry_point = entry_point;
    stack = new char[STACK_SIZE]

}

sigjmp_buf& Thread::get_buf_ref()
{
    return _env;
}

void Thread::free_thread()
{
    free(stack);
}