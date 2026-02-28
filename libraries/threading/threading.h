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
    struct _TaskData
    {
        bool finished;
        std::mutex mutex;
        std::condition_variable condition;
        std::function<void()> taskFunction;
    };

    template<typename T>
    class ITask
    {
    protected:
        std::shared_ptr<_TaskData> data;

        inline ITask(std::shared_ptr<_TaskData> data)
            : data(data) {}

    public:
        inline bool isNull()
        {
            return data.get() == nullptr;
        }
        inline bool isFinished()
        {
            std::lock_guard<std::mutex> lock(data->mutex);
            return data->finished;
        }
        inline void wait()
        {
            std::unique_lock<std::mutex> lock(data->mutex);
            if (data->finished)
                return;
            data->condition.wait(lock, [this]{ return data->finished; });
        }
        virtual T result() = 0;
    };

    class Task : public ITask<void>
    {
        friend class ThreadPool;

        inline Task(std::shared_ptr<_TaskData> data)
            : ITask(data) {}
        
    public:
        inline Task()
            : ITask(nullptr) {}
        inline Task(const Task& other)
            : ITask(other.data) {}
        inline Task& operator=(const Task& other)
        {
            data = other.data;
            return *this;
        }

        inline void result() {}
    };

    template<typename T>
    class ValueTask : public ITask<T>
    {
        friend class ThreadPool;

        std::shared_ptr<T> value;

        inline ValueTask(std::shared_ptr<_TaskData> data, std::shared_ptr<T> value)
            : ITask<T>(data), value(value) {}

    public:
        inline ValueTask()
            :ITask<T>(nullptr), value(nullptr) {}
        inline ValueTask(const ValueTask& other)
            : ITask<T>(other), value(other.value) {}
        inline ValueTask& operator=(const ValueTask& other)
        {
            ITask<T>::operator=(other);
            value = other.value;
        }

        inline T& result()
        {
            std::lock_guard<std::mutex> lock(ITask<T>::data->mutex);
            return *value;
        }
    };

    class ThreadPool
    {
        static std::vector<std::thread> poolThreads;
        static bool shutdown;

        static std::mutex taskQueueMutex;
        static std::condition_variable taskQueueCondition;
        static std::queue<std::shared_ptr<_TaskData>> taskQueue;

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
            std::shared_ptr<_TaskData> data = std::make_shared<_TaskData>();
            std::shared_ptr<T> value = std::make_shared<T>();
            data->taskFunction = [taskFunction, value](){ *value = taskFunction(); };
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            taskQueue.push(data);
            taskQueueCondition.notify_one();
            return ValueTask(data, value);
        }
    };
}
