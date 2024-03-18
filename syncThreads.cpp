#include "syncthreads.h"

std::mutex mtx;
std::condition_variable cv;
bool ready = false;