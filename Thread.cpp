#include "Thread.h"
#include <iostream>


#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
  address_t ret;
  asm volatile("xor    %%fs:0x30,%0\n"
               "rol    $0x11,%0\n"
      : "=g" (ret)
      : "0" (addr));
  return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
            : "=g" (ret)
            : "0" (addr));
    return ret;
}

#endif


Thread::Thread(int id, thread_entry_point entry_point)
{
    _id = id;
    _state = READY;
    _entry_point = entry_point;
    _quantum_time_spent_in_running = 0;
    _quanto_block_time = -1;
    if (id != 0)
    {
        try {
            stack = new char[STACK_SIZE];
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "system error: memory allocation (stack)" << std::endl;
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
