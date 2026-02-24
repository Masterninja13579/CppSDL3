#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

// DEBUG
//#define ENABLE_THREADPOOL_DEBUG_OUTPUT
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
#include <iostream>
#endif

namespace Threading
{
    class Task
    {
        friend class ThreadPool;

    protected:
        struct TaskData
        {
            bool finished = false;
            std::mutex mutex;
            std::condition_variable condition;
            std::function<void()> taskFunction;
        };

        std::shared_ptr<TaskData> data;

        Task(std::shared_ptr<TaskData> data);

    public:
        Task() = delete;
        Task(const Task& other);
        Task& operator=(const Task& other);

        bool isFinished();
        void wait();
    };

    template<typename T>
    class ValueTask : public Task
    {
        friend class ThreadPool;

        std::shared_ptr<T> value;

        inline ValueTask(std::shared_ptr<TaskData> data, std::shared_ptr<T> value)
            : Task(data), value(value) {}

    public:
        ValueTask() = delete;
        inline ValueTask(const ValueTask& other)
            : Task(other), value(other.value) {}
        inline ValueTask& operator=(const ValueTask& other)
        {
            Task::operator=(other);
            value = other.value;
        }

        inline T& result()
        {
            std::lock_guard<std::mutex> lock(data->mutex);
            return *value;
        }
    };

    class ThreadPool
    {
        static std::vector<std::thread> poolThreads;
        static bool shutdown;

        static std::mutex taskQueueMutex;
        static std::condition_variable taskQueueCondition;
        static std::queue<std::shared_ptr<Task::TaskData>> taskQueue;

        static void ThreadFunction();

    public:
        ThreadPool() = delete;

        static bool Initialize(int threadCount);
        static bool Shutdown();

        static Task Submit(std::function<void()> taskFunction);

        template<typename T>
        inline static ValueTask<T> Submit(std::function<T()> taskFunction)
        {
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
            std::cout << "Adding value task...\n";
#endif
            std::shared_ptr<Task::TaskData> data = std::make_shared<Task::TaskData>();
            std::shared_ptr<T> value = std::make_shared<T>();
            data->taskFunction = [taskFunction, value](){ *value = taskFunction(); };
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            taskQueue.push(data);
            taskQueueCondition.notify_one();
            return ValueTask(data, value);
        }
    };
}
