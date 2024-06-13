#include "Thread.h"

Thread::Thread(int id, thread_entry_point entry_point)
{
    _id = id;
    _state = READY;
    _entry_point = entry_point;
    if (id != 0)
    {
        try {
            stack = new char[STACK_SIZE];
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "system error: memory allocation (stack)" << str << std::endl;
            exit(1);
        }


    }
    else
        stack = nullptr;
}

sigjmp_buf& Thread::get_buf_ref()
{
    return _env;
}

void Thread::free_thread()
{
    if (stack)
        free(stack);
}

void Thread::set_status(STATE s)
{
    _state = s;
}