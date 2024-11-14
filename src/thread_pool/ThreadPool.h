#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool{
    private:
        std::vector<std::thread> workers;
        std::mutex queue_mutex;
        std::condition_variable cv;
        std::queue<std::function<void()>> tasks;
        bool stop = false;
    public:
        ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
        ~ThreadPool();
        void enqueue(std::function<void()> task);
};