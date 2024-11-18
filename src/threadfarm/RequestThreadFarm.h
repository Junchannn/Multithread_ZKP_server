#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class RequestThreadFarm {
private:
    std::vector<std::thread> workers;               // Worker threads
    std::mutex task_mutex;                          // Mutex for task queue
    std::condition_variable cv1;                   // Condition variable to signal task availability
    std::condition_variable cv3;                   // Condition variable to signal all tasks completion
    std::queue<std::function<void()>> tasks;       // Queue of tasks
    size_t num_jobs = 0;                            // Total number of enqueued tasks
    size_t active_tasks = 0;                        // Number of active tasks being processed
    bool stop = false;                              // Flag to stop the thread pool

public:
    explicit RequestThreadFarm(size_t num_tasks);   // Constructor to initialize the thread pool
    ~RequestThreadFarm();                           // Destructor to clean up resources

    void enqueue(const std::vector<std::function<void()>>& tasks); // Add tasks to the queue
    void waitForAll();                              // Wait for all tasks to complete
};
