#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include "sync_queue.h"

const int MAX_TASK_COUNT = 100;

class Thread_Pool
{
public:
    using Task = std::function<void()>;
    Thread_Pool(int num_threads = std::thread::hardware_concurrency())
        : m_queue(MAX_TASK_COUNT)
    {
        start(num_threads);
    }

    ~Thread_Pool()
    {
        stop();
    }

    void stop()
    {
        std::call_once(m_flag, [this]{stop_thread_group();});
    }

    void add_task(Task&&task)
    {
        m_queue.put(std::forward<Task>(task));
    }

    void add_task(const Task& task)
    {
        m_queue.put(task);
    }

private:
    void start(int num_threads)
    {
        m_running = true;
        for(int i = 0; i < num_threads; ++i)
        {
            m_thread_group.push_back(std::make_shared<std::thread>(&Thread_Pool::run_in_thread, this));
        }
    }

    void run_in_thread()
    {
        while( m_running )
        {
            std::list<Task> list;
            m_queue.take(list);

            for(auto& task : list)
            {
                if(!m_running)
                    return;

                task();
            }
        }
    }

    void stop_thread_group()
    {
        m_queue.stop();
        m_running = false;

        for(auto thread : m_thread_group)
        {
            if( thread )
                thread->join();
        }
        m_thread_group.clear();
    }

private:
    std::list<std::shared_ptr<std::thread>> m_thread_group;
    Sync_Queue<Task>    m_queue;
    atomic_bool         m_running;
    std::once_flag      m_flag;
};

#endif //THREAD_POOL_H
