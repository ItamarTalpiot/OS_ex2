#include "uthreads.h"
#include "ThreadHandler.h"
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
        return -1;

    ThreadHandler::add_thread(0, nullptr);
    ThreadHandler::get_thread(0).set_status(RUNNING);
    ThreadHandler::set_quantum_time(quantum_usecs);

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
        ThreadHandler::free_all_threads();
        exit(0);
    }
    if (ThreadHandler::get_threads().find(tid) == ThreadHandler::get_threads().end())
    {
        print_library_error_message("Id to terminate doesn't exist");
        return -1;
    }

    if (tid == ThreadHandler::get_current_thread_id())
    {
        ThreadHandler::delete_thread(tid);
        // Todo: recall the schedular
    }
    else
    {
        ThreadHandler::delete_thread(tid);
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
  if (ThreadHandler::get_threads().find(tid) == ThreadHandler::get_threads().end() or tid == 0){
    print_library_error_message ("there is no thread with that id!");
    return -1;
  }
  ThreadHandler::block_thread (tid);
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
int uthread_resume(int tid);
