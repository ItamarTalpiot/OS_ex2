#ifndef OS_EX2_THREAD_H
#define OS_EX2_THREAD_H

#include "uthreads.h"

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>


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



enum STATE {READY, BLOCKED, RUNNING, TERMINATED};

class Thread {
    STATE _state;
    int _id;
    thread_entry_point _entry_point;
    char* stack;
    sigjmp_buf _env;
    int _quantum_time_spent_in_running;

public:
    Thread(int id, thread_entry_point entry_point);
    Thread(const Thread& other);

    void inc_count();
    STATE get_status();
    void set_status(STATE s);
    sigjmp_buf& get_buf_ref();
    thread_entry_point get_entry_point();
    void free_thread();
    int get_quantum_time_spent_in_running();
};

#endif //OS_EX2_THREAD_H
