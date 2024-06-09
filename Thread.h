


#ifndef OS_EX2_THREAD_H
#define OS_EX2_THREAD_H

#include "uthreads.h"

STATES = enum STATE {READY, BLOCKED, RUNNING};

class Thread {
    STATE _state;
    int _id;
    thread_entry_point _entry_point;
    sigjmp_buf _env;

public:
    sigjmp_buf& get_buf_ref()
    {
        return _env;
    }


};

#endif //OS_EX2_THREAD_H
