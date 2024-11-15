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
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void workerThread();
};

template <class Func, class... Args>
void ThreadPool::enqueue(Func&& func, Args&&... args) 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([=]() { func(args...); });
    }
    condition.notify_one();
}
