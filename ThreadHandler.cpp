#include <iostream>
#include <map>
#include <queue>
#include "ThreadHandler.h"
#include "Thread.h"


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


std::map<int, Thread*> ThreadHandler::_threads;
std::queue<int> ThreadHandler::_ready_states;
int ThreadHandler::_current_thread_id = 0;
int ThreadHandler::_quantum_time = 0;
int ThreadHandler::_quantum_count = 0;
struct itimerval ThreadHandler::timer = {};


// Function to block SIGVTALRM
void ThreadHandler::block_sig() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGVTALRM);
    if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
        perror("sigprocmask error");
    }
}

// Function to unblock SIGVTALRM
void ThreadHandler::unblock_sig() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGVTALRM);
    if (sigprocmask(SIG_UNBLOCK, &sigset, NULL) < 0) {
        perror("sigprocmask error");
    }
}


void print_system_error_message(std::string str)
{
    std::cerr << "system error: " << str << std::endl;
}

void printMap(const std::map<int, Thread*>& myMap) {
    for (const auto& pair : myMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
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


void setup_thread(int tid, char *stack, thread_entry_point entry_point, sigjmp_buf env)
{
    // initializes env[tid] to use the right stack, and to run from the function 'entry_point', when we'll use
    // siglongjmp to jump into the thread.

    address_t sp = (address_t) stack + STACK_SIZE - sizeof(address_t);
    address_t pc = (address_t) entry_point;
    sigsetjmp(env, 1);
    (env->__jmpbuf)[JB_SP] = translate_address(sp);
    (env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&env->__saved_mask);
}


/**
 * @brief Saves the current thread state, and jumps to the other thread.
 */
void yield(Thread* last_thread, Thread* new_thread)
{
    if (last_thread == nullptr || sigsetjmp(last_thread->env, 1) == 0)
    {
        siglongjmp(new_thread->env, 1);
    }
}

void ThreadHandler::scheduler(int sig)
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
    Thread* last_thread = ThreadHandler::get_current_thread();

    if (curr_id == 1)
    {
        ;
    }

    if (last_thread != nullptr) // if not terminated
    {
        STATE thread_status = last_thread->get_status();

        if (thread_status != BLOCKED && thread_status != TERMINATED) // if quantum ended
        {
            ThreadHandler::get_current_thread()->set_status(READY);
            ThreadHandler::add_thread_to_ready_queue(curr_id); //add thread to end of queue
        }
    }
    ThreadHandler::set_first_ready_to_running(curr_id); //set current thread running (if false

    ThreadHandler::get_current_thread()->inc_count();
    ThreadHandler::inc_global_quantum();

    yield(last_thread, ThreadHandler::get_current_thread());
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

void ThreadHandler::add_thread (int id, thread_entry_point entry_point)
{
    Thread* new_thread = new Thread(id,entry_point);
  _threads.insert({id, new_thread});

  setup_thread(id, new_thread->stack, entry_point, new_thread->env);

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
    if (_threads.count(_current_thread_id) != 0)
    {
        _threads.at(id)->free_thread();
        delete _threads.at(id);
        _threads.erase(id);
    }
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
  if(_threads.count(_current_thread_id) == 0){
    return nullptr;
  }
  return _threads.at(_current_thread_id);
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
  _ready_states.push (id);
  //printQueue (_ready_states);
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
    sigemptyset(&sa.sa_mask);
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
void ThreadHandler::printQueue (const std::queue<int> &q)
{
  std::queue<int> temp = q; // Make a copy of the queue
  while (!temp.empty()) {
      std::cout << temp.front() << " ";
      temp.pop();
    }
  std::cout << std::endl;
}




