#ifndef ___SYNCHRONIZATION_CONTEXT_H___
#define ___SYNCHRONIZATION_CONTEXT_H___

#include <experimental/coroutine>
#include <experimental/generator>
#include <future>
#include <thread>
#include <deque>
#include <functional>
#include <condition_variable>
#include <iostream>

class SynchronizationContext {
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _requestedAbort;
    std::deque<std::function<void(void)>> _functions;

public:
    SynchronizationContext();
    void Post(std::function<void(void)> func);
    void Main();

    void Abort() {
        std::lock_guard<std::mutex> lock(_mutex);
        _requestedAbort = true;
        _cv.notify_all();
    }
};

#endif