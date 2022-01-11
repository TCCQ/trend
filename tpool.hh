#include <functional>
#include <list>
#include <semaphore>
#include <atomic>
#include <mutex>

#ifndef TPOOL
#define TPOOL
#define MAXWORKTHREADS 64
class tpool {
  private:
    std::list<std::function<void(void)>> queue;
    std::counting_semaphore<MAXWORKTHREADS> inProgress = std::counting_semaphore<MAXWORKTHREADS>(0);
    std::mutex listAccess;
    std::atomic<bool> continueWork;
    std::atomic<int> currentThreads;
    std::atomic<int> midTask;
  public:

    tpool();
    ~tpool();
    void poolWork(void); //what each thread worker should run
    void enqueue(std::function<void(void)>); //how to add a task to the pool
    void waitUntilEmpty(void);//wait for all threads to finish current batch of work
    //if no work, should return instantly
    
};
#endif
