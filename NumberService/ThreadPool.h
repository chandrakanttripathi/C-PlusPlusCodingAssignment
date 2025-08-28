// ThreadPool.h
#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t workerCount = std::thread::hardware_concurrency());
    ~ThreadPool();

    // enqueue returns false if pool is stopping
    bool Enqueue(std::function<void()> task);

    // Stop and join all workers; after Stop(), Enqueue returns false.
    void Stop();

private:
    void WorkerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopping_;
};
