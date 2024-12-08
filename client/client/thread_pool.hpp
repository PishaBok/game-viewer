#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template <class Func, class... Args>
    void enqueue(Func&& func, Args&&... args);

private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;

    std::mutex _queueMutex;
    std::condition_variable _condition;
    std::atomic<bool> _stop;

    void workerThread();
};

template <class Func, class... Args>
void ThreadPool::enqueue(Func&& func, Args&&... args) 
{
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        _tasks.emplace([=]() { func(args...); });
    }
    _condition.notify_one();
}
