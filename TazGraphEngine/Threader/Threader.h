#pragma once
#pragma once
#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

struct TaskQueue {
    std::deque<std::function<void()>> tasks;
    std::mutex                        mutex_;
    std::condition_variable taskCondition;
    std::atomic<int>                  remaining_tasks = 0;
    bool shuttingDown = false;

    void addTask(std::function<void()>&& callback) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks.emplace_back(std::move(callback));
        }
        remaining_tasks++;
        taskCondition.notify_one();
    }

    bool getTask(std::function<void()>& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        taskCondition.wait(lock, [this] { return !tasks.empty() || shuttingDown; });

        if (shuttingDown || tasks.empty()) return false;  // Shouldn't happen, but just in case

        task = std::move(tasks.front());
        tasks.pop_front();
        return true;
    }

    void waitUntilDone() const {
        while (remaining_tasks > 0) {
            std::this_thread::yield();
        }
    }

    void completeTask() {
        remaining_tasks--;
    }
};

struct Thread {
    int                   id = 0;
    std::thread           cur_thread;
    std::function<void()> task = nullptr;
    bool                  running = true;
    TaskQueue* t_queue = nullptr;

    Thread(TaskQueue& task_queue_, int id_)
        : id{ id_ }
        , t_queue{ &task_queue_ }
    {
        cur_thread = std::thread([this]() {
            run();
            });
    }

    void run() {
        while (running) {
            std::function<void()> task;
            if (t_queue->getTask(task)) {
                task();
                t_queue->completeTask();
            }
        }
    }

    void stop() {
        running = false;
        t_queue->shuttingDown = true;
        t_queue->taskCondition.notify_all();
        if (cur_thread.joinable()) {
            cur_thread.join();
        }
    }
};

struct Threader {
    TaskQueue t_queue;
    int num_threads = 1;
    std::vector<Thread> threads;

    Threader(int num_threads_)
        : num_threads{ num_threads_ }
    {
        threads.reserve(num_threads_);
        for (int i = 0; i < num_threads_; i++)
            threads.emplace_back(t_queue, i);
    }

    void parallel(int num_obj, std::function<void(int start, int end)>&& callback) {
        if (num_obj == 0) return;
        int slice_size = num_obj / num_threads;
        for (int i = 0; i < num_threads; i++) {
            int start = i * slice_size;
            int end = start + slice_size;
            t_queue.addTask([start, end, &callback]() { callback(start, end);});
        }
        if (slice_size * num_threads < num_obj) {
            int start = slice_size * num_threads;
            callback(start, num_obj);
        }
        t_queue.waitUntilDone();
    }
};