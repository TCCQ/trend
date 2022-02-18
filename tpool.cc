#include "tpool.hh"
#include <chrono>
//#include <iostream>

#define POLLTIME 1000
//one millisec to start

tpool::tpool() {
  queue = std::list<std::function<void(void)>>();
  continueWork = true;
  currentThreads = 0;
  midTask = 0;
}

tpool::~tpool() {
  continueWork = false;
  while (currentThreads) {
    if (!inProgress.try_acquire()) {
      //there are threads working or waiting, but no more work to do
      queue.push_front((std::function<void(void)>)([](){return;})); 
      //push empty func to prompt worker thread to check continueWork (will be popped)
      inProgress.release();
    } else {
      //there is still work to do (empty or real)
      inProgress.release(); //don't want to actually take anything
    }
    usleep(POLLTIME); //TODO rework this to use blocking threads instead of polling
  }
}

//cycles, blocking if there is not work to do, and on the completion of a job, check if it should terminate
void tpool::poolWork() {
  currentThreads++;
  while (continueWork) {
    inProgress.acquire(); //will block until there is work to do
//    std::cerr << midTask << std::endl;
    midTask++;
    listAccess.lock(); //ensure no two threads pop the same thing (not sure if pop is threadsafe)
    std::function<void(void)> func = queue.front();
    queue.pop_front();
    listAccess.unlock();
    func(); //do the work
    midTask--;
  }
  currentThreads--;
}

void tpool::enqueue(std::function<void(void)> toAdd) {
  listAccess.lock();
  queue.push_front(toAdd); //add the work
  listAccess.unlock();
  inProgress.release(); //tell the threads there is work to do
}

//TODO very gross, rework with blocking instead of polling (certainly avoid 2 polling cycles)
void tpool::waitUntilEmpty() {
  while (inProgress.try_acquire()) {
    //there was something there
    inProgress.release();
    usleep(POLLTIME); 
  } 

  //now there is are no tasks left to do, wait until the current tasks are finished
  while (midTask) 
    usleep(POLLTIME);
}
