#include "RequestThreadFarm.h"

RequestThreadFarm::RequestThreadFarm(size_t num_tasks) 
    : stop(false), active_tasks(0), num_jobs(0) {
    for (size_t i = 0; i < num_tasks; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                // Wait for a task to be available
                {
                    std::unique_lock<std::mutex> lock(this->task_mutex);
                    this->cv1.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                    if (this->stop && this->tasks.empty()) {
                        return;  // Exit thread when stopping and no tasks are left
                    }

                    // Assign a task
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    ++this->active_tasks;  // Increment active task count
                }

                task();  // Execute the task

                // Mark task as completed
                {
                    std::lock_guard<std::mutex> lock(this->task_mutex);
                    --this->active_tasks;  // Decrement active task count
                }

                // Notify completion for waitForAll
                this->cv3.notify_one();
            }
        });
    }
}

RequestThreadFarm::~RequestThreadFarm() {
    {
        std::unique_lock<std::mutex> lock(this->task_mutex);
        this->stop = true;
    }
    this->cv1.notify_all();  // Notify all workers to stop
    for (std::thread& worker : this->workers) {
        worker.join();  // Join all worker threads
    }
}

void RequestThreadFarm::enqueue(const std::vector<std::function<void()>>& tasks) {
    {
        std::unique_lock<std::mutex> lock(this->task_mutex);
        this->num_jobs = tasks.size();  // Set the total number of jobs
        for (const auto& task : tasks) {
            this->tasks.push(task);
        }
    }
    this->cv1.notify_all();  // Notify workers about new tasks
}

void RequestThreadFarm::waitForAll() {
    std::unique_lock<std::mutex> lock(this->task_mutex);
    this->cv3.wait(lock, [this] { return this->tasks.empty() && this->active_tasks == 0; });
}
