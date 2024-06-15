#include "uthreads.h"
#include "ThreadHandler.h"
#include "Thread.h"
#include <iostream>

void print_library_error_message(std::string str)
{
    std::cerr << "thread library error: " << str << std::endl;
}

/**
 * @brief initializes the thread library.
 *
 * Once this function returns, the main thread (tid == 0) will be set as RUNNING. There is no need to
 * provide an entry_point or to create a stack for the main thread - it will be using the "regular" stack and PC.
 * You may assume that this function is called before any other thread library function, and that it is called
 * exactly once.
 * The input to the function is the length of a quantum in micro-seconds.
 * It is an error to call this function with non-positive quantum_usecs.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    if (quantum_usecs <= 0)
    {
        print_library_error_message("quantum needs to be positive");
        return -1;
    }

    ThreadHandler::add_thread(0, nullptr);
    ThreadHandler::get_thread(0)->set_status(RUNNING);
    ThreadHandler::get_ready_states().pop();
    ThreadHandler::set_quantum_time(quantum_usecs);
    ThreadHandler::init_timer();

    return 0;
}

/**
 * @brief Terminates the thread with ID tid and deletes it from all relevant control structures.
 *
 * All the resources allocated by the library for this thread should be released. If no thread with ID tid exists it
 * is considered an error. Terminating the main thread (tid == 0) will result in the termination of the entire
 * process using exit(0) (after releasing the assigned library memory).
 *
 * @return The function returns 0 if the thread was successfully terminated and -1 otherwise. If a thread terminates
 * itself or the main thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    if (tid == 0)
    {
        ThreadHandler::block_sig();
        ThreadHandler::free_all_threads();
        ThreadHandler::unblock_sig();
        exit(0);
    }
    if (ThreadHandler::get_threads().find(tid) == ThreadHandler::get_threads().end())
    {
        print_library_error_message("Id to terminate doesn't exist");
        return -1;
    }

    if (tid == ThreadHandler::get_current_thread_id())
    {
        ThreadHandler::block_sig();
        ThreadHandler::get_current_thread()->set_status(TERMINATED);
        ThreadHandler::delete_thread(tid);
        ThreadHandler::scheduler(SIGVTALRM);
        ThreadHandler::reset_timer();
        ThreadHandler::unblock_sig();
    }
    else
    {
        ThreadHandler::block_sig();
        ThreadHandler::get_thread(tid)->set_status(TERMINATED);
        ThreadHandler::delete_thread(tid);
        ThreadHandler::unblock_sig();
        return 0;
    }
}

/**
 * this function return the next id to use.
 * @return
 */
int get_next_id(){
  int id = 1;
  while(ThreadHandler::get_threads().find(id) != ThreadHandler::get_threads().end()){
    id++;
  }
  return id;
}

/**
 * @brief Creates a new thread, whose entry point is the function entry_point with the signature
 * void entry_point(void).
 *
 * The thread is added to the end of the READY threads list.
 * The uthread_spawn function should fail if it would cause the number of concurrent threads to exceed the
 * limit (MAX_THREAD_NUM).
 * Each thread should be allocated with a stack of size STACK_SIZE bytes.
 * It is an error to call this function with a null entry_point.
 *
 * @return On success, return the ID of the created thread. On failure, return -1.
*/
int uthread_spawn(thread_entry_point entry_point){
  if(ThreadHandler::get_number_of_threads() >= MAX_THREAD_NUM){
    print_library_error_message ("max number of thread reached!");
    return -1;
  }
  int id = get_next_id();
  ThreadHandler::add_thread (id, entry_point);
  return id;
}

/**
 * @brief Blocks the thread with ID tid. The thread may be resumed later using uthread_resume.
 *
 * If no thread with ID tid exists it is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision should be made. Blocking a thread in
 * BLOCKED state has no effect and is not considered an error.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_block(int tid){
  if (ThreadHandler::get_threads().find(tid) == ThreadHandler::get_threads().end() || tid == 0){
    print_library_error_message ("there is no thread with that id!");
    return -1;
  }
  ThreadHandler::block_sig();
  ThreadHandler::block_thread (tid);
  ThreadHandler::unblock_sig();
  return 0;
}

/**
 * @brief Resumes a blocked thread with ID tid and moves it to the READY state.
 *
 * Resuming a thread in a RUNNING or READY state has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid){
  if (ThreadHandler::get_threads().find(tid) == ThreadHandler::get_threads().end()){
      print_library_error_message ("there is no thread with that id!");
      return -1;
  }
  if(ThreadHandler::get_threads().at(tid)->get_status() == RUNNING ||
  ThreadHandler::get_threads().at(tid)->get_status() == READY){
    return 0;
  }
  ThreadHandler::block_sig();
  ThreadHandler::get_ready_states().push(tid);
  ThreadHandler::get_threads().at(tid)->set_status(READY);
  ThreadHandler::unblock_sig();
  return 0;
}

/**
 * @brief Blocks the RUNNING thread for num_quantums quantums.
 *
 * Immediately after the RUNNING thread transitions to the BLOCKED state a scheduling decision should be made.
 * After the sleeping time is over, the thread should go back to the end of the READY queue.
 * If the thread which was just RUNNING should also be added to the READY queue, or if multiple threads wake up
 * at the same time, the order in which they're added to the end of the READY queue doesn't matter.
 * The number of quantums refers to the number of times a new quantum starts, regardless of the reason. Specifically,
 * the quantum of the thread which has made the call to uthread_sleep isn’t counted.
 * It is considered an error if the main thread (tid == 0) calls this function.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_sleep(int num_quantums){
    int curr_id = ThreadHandler::get_current_thread_id();

    if (curr_id == 0)
    {
        print_library_error_message("main thread cannot be sleeped");
        return -1;
    }
    ThreadHandler::block_sig();
    Thread* curr_thread = ThreadHandler::get_current_thread();
    curr_thread->_quanto_block_time = num_quantums;
    curr_thread->set_status(BLOCKED);

    ThreadHandler::scheduler(SIGVTALRM);
    ThreadHandler::reset_timer();
    ThreadHandler::unblock_sig();

    return 0;
}

/**
 * @brief Returns the thread ID of the calling thread.
 *
 * @return The ID of the calling thread.
*/
int uthread_get_tid(){
  return ThreadHandler::get_current_thread_id();
}

/**
 * @brief Returns the total number of quantums since the library was initialized, including the current quantum.
 *
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number should be increased by 1.
 *
 * @return The total number of quantums.
*/
int uthread_get_total_quantums(){
  return ThreadHandler::get_quantum_count();
}

/**
 * @brief Returns the number of quantums the thread with ID tid was in RUNNING state.
 *
 * On the first time a thread runs, the function should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state when this function is called, include
 * also the current quantum). If no thread with ID tid exists it is considered an error.
 *
 * @return On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid){
    if (ThreadHandler::get_threads().count(tid) > 0)
        return ThreadHandler::get_threads().at(tid)->get_quantum_time_spent_in_running();
    else
    {
        print_library_error_message("no thread with this id");
        return -1;
    }

}
