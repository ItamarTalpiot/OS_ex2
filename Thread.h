#ifndef OS_EX2_THREAD_H
#define OS_EX2_THREAD_H

#include "uthreads.h"

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>



enum STATE {READY, BLOCKED, RUNNING, TERMINATED};

class Thread {
    STATE _state;
    int _id;
    thread_entry_point _entry_point;
    char* stack;
    sigjmp_buf _env;
    int _quantum_time_spent_in_running;

public:
    int _quanto_block_time;
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
