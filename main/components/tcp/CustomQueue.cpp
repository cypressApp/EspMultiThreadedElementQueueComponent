#include "CustomQueue.hpp"

std::counting_semaphore<1> semPush(0);
std::counting_semaphore<1> semPop(0);