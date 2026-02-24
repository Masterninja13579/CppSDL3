#include "threading.h"

// DEBUG
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
#include <iostream>
#include <sstream>
#endif

namespace Threading
{
    Task::Task(std::shared_ptr<TaskData> data)
        : data(data)
    {

    }

    Task::Task(const Task& other)
        : data(other.data)
    {
        
    }

    Task& Task::operator=(const Task& other)
    {
        data = other.data;
        return *this;
    }

    bool Task::isFinished()
    {
        std::lock_guard<std::mutex> lock(data->mutex);
        return data->finished;
    }

    void Task::wait()
    {
        std::unique_lock<std::mutex> lock(data->mutex);
        if (data->finished)
            return;
        data->condition.wait(lock, [this]{ return data->finished; });
    }

    std::vector<std::thread> ThreadPool::poolThreads;
    bool ThreadPool::shutdown = true;

    std::mutex ThreadPool::taskQueueMutex;
    std::condition_variable ThreadPool::taskQueueCondition;
    std::queue<std::shared_ptr<Task::TaskData>> ThreadPool::taskQueue;

    void ThreadPool::ThreadFunction()
    {
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
        std::thread::id id = std::this_thread::get_id();
        {
            std::stringstream ss;
            ss << "    Hello from thread " << id << "\n";
            std::cout << ss.str();
        }
#endif
        while (!shutdown)
        {
            std::shared_ptr<Task::TaskData> data;
            {
                std::unique_lock<std::mutex> lock(taskQueueMutex);
                taskQueueCondition.wait(lock, []{
                    return !ThreadPool::taskQueue.empty() || ThreadPool::shutdown;
                });
                if (shutdown)
                    break;
                data = taskQueue.front();
                taskQueue.pop();
            }
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
            {
                std::stringstream ss;
                ss << "Thread " << id << " starting task...\n";
                std::cout << ss.str();
            }
#endif
            data->taskFunction();
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
            {
                std::stringstream ss;
                ss << "Thread " << id << " finished task!\n";
                std::cout << ss.str();
            }
#endif
            {
                std::unique_lock<std::mutex> lock(data->mutex);
                data->finished = true;
            }
            data->condition.notify_all();
        }
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
        {
            std::stringstream ss;
            ss << "Thread " << id << " shutting down\n";
            std::cout << ss.str();
        }
#endif
    }

    bool ThreadPool::Initialize(int threadCount)
    {
        if (threadCount < 1 || poolThreads.size() > 0)
            return false;
        
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
        std::thread::id id = std::this_thread::get_id();
        std::cout << "Thread " << id << " creating " << threadCount << " pool threads...\n";
#endif

        shutdown = false;
        poolThreads.resize(threadCount);
        for (int i = 0; i < threadCount; ++i)
            poolThreads[i] = std::thread(ThreadPool::ThreadFunction);

        return true;
    }

    bool ThreadPool::Shutdown()
    {
        if (poolThreads.size() == 0)
            return false;
        {
            std::unique_lock<std::mutex> lock(taskQueueMutex);
            shutdown = true;
        }
        taskQueueCondition.notify_all();
        for (int i = 0; i < poolThreads.size(); ++i)
            poolThreads[i].join();
        poolThreads.clear();

        while (taskQueue.size() > 0)
            taskQueue.pop();

#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
        std::thread::id id = std::this_thread::get_id();
        std::cout << "Thread " << id << " shut down thread pool...\n";
#endif
        
        return true;
    }

    Task ThreadPool::Submit(std::function<void()> taskFunction)
    {
#ifdef ENABLE_THREADPOOL_DEBUG_OUTPUT
        std::cout << "Adding task...\n";
#endif
        std::shared_ptr<Task::TaskData> data = std::make_shared<Task::TaskData>();
        data->taskFunction = taskFunction;
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        taskQueue.push(data);
        taskQueueCondition.notify_one();
        return Task(data);
    }
}
