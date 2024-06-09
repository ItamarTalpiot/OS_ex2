#include "uthreads.h"
#include "ThreadHandler.h"

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

}

void print_library_error_message(std::string str)
{
    std::cerr << "thread library error" << str << std::endl;
}

void print_system_error_message(std::string str)
{
    std::cerr << "system error" << str << std::endl;
}
