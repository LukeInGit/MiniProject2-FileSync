#ifndef SYNCTHREADS_H
#define SYNCTHREADS_H


#include <mutex>
#include <condition_variable>

extern std::mutex mtx;
extern std::condition_variable cv;
extern bool ready;

#endif // !SYNCTHREADS_H