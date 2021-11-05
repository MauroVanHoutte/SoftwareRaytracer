#include "ThreadManager.h"
#include <iostream>

ThreadManager* ThreadManager::m_Instance = nullptr;

void ThreadManager::ThreadLoop()
{
    while (!m_Quit)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);

            m_ConditionVariable.wait(lock, [this]() { return !m_JobQueue.empty() || m_Quit; });

            if (m_Quit)
                break;

            if (m_JobQueue.empty())
                continue;

            job = m_JobQueue.front();
            m_JobQueue.pop();
        }
        job();
    }
}

ThreadManager* ThreadManager::GetInstance()
{
    if (m_Instance == nullptr)
        m_Instance = new ThreadManager;
    return m_Instance;
}

void ThreadManager::AddJob(const std::function<void()>& job)
{
    { //scope to destroy lock
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        m_JobQueue.push(job);
    }

    m_ConditionVariable.notify_one();
}

void ThreadManager::Destroy()
{
    m_Quit = true;

    m_ConditionVariable.notify_all();

    for (std::thread& thread : m_Threads)
    {
        thread.join();
    }

    m_Threads.clear();

    delete m_Instance;
}

size_t ThreadManager::GetNrThreads()
{
    return m_Threads.size();
}

ThreadManager::ThreadManager()
{
    int nrThreads = std::thread::hardware_concurrency();
    m_Threads.reserve(nrThreads);
    for (size_t i = 0; i < nrThreads; i++)
    {
        m_Threads.push_back(std::thread(std::bind(&ThreadManager::ThreadLoop, this)));
    }
}


