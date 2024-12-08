#include <client/thread_pool.hpp>

ThreadPool::ThreadPool(size_t numThreads) : _stop(false) 
{
    for (size_t i = 0; i < numThreads; ++i) {
        _workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() 
{
    _stop = true;
    _condition.notify_all();
    for (auto& worker : _workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::workerThread() 
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _condition.wait(lock, [this]() { return _stop || !_tasks.empty(); });
            if (_stop && _tasks.empty()) {
                return;
            }
            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}