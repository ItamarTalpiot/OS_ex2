#include <iostream>
#include <map>
#include <queue>
#include "ThreadHandler.h"
#include "Thread.h"


ThreadHandler::ThreadHandler()
{
  _threads[0] = Thread(0, nullptr);
}

Thread& ThreadHandler::get_thread (int id)
{
  // this function gets thread id and return the thread with that id
  return _threads.at (id);
}

int ThreadHandler::pop_thread()
{
  return 0;
}

int main() {
  std::cout << "here" << std::endl;
  return 0;
}