// ThreadPool.cpp
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t workerCount) : stopping_(false) {
    if (workerCount == 0) workerCount = 1;
    workers_.reserve(workerCount);
    for (size_t i = 0; i < workerCount; ++i) {
        workers_.emplace_back(&ThreadPool::WorkerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    Stop();
}

bool ThreadPool::Enqueue(std::function<void()> task) {
    if (stopping_.load()) return false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stopping_.load()) return false;
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
    return true;
}

void ThreadPool::Stop() {
    bool expected = false;
    if (!stopping_.compare_exchange_strong(expected, true)) {
        // already stopping
    }
    cv_.notify_all();
    for (auto& t : workers_) {
        if (t.joinable()) t.join();
    }
    workers_.clear();

    // optional: clear leftover tasks
    std::lock_guard<std::mutex> lock(mutex_);
    while (!tasks_.empty()) tasks_.pop();
}

void ThreadPool::WorkerLoop() {
    while (!stopping_.load()) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&] { return stopping_.load() || !tasks_.empty(); });
            if (stopping_.load() && tasks_.empty()) return;
            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            else {
                continue;
            }
        }

        try {
            task();
        }
        catch (...) {
            // Not doing anything, so that worker thread are keep alived
        }
    }
}
