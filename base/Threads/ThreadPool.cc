#include "Thread.h"
#include "ThreadPool.h"

bool ThreadPool::PrepareThread(Runnable* toRun)
{
    ScopeMutex    guard(m_threadsLock);
    return m_threads.insert(new Thread(toRun)).second;
}

bool ThreadPool::StartAllThreads()
{
    ScopeMutex   guard(m_threadsLock);
    ThreadIterator it = m_threads.begin();
    while (it != m_threads.end())
    {
        if (!(*it)->Start())
            return false;
        else
            ++ it;
    }

    return true;
}

bool ThreadPool::ExecuteTask(Runnable* toRun )
{
    if (NULL == toRun)
        return false;

    Thread* t = NULL;
    bool new_thread = false;

    m_threadsLock.Lock();
    ThreadIterator it = m_threads.begin();
    while (it != m_threads.end())
    {
        if ((*it)->Idle())
        {
            t = *it;
            break;
        }
        else
        {
            ++ it;
        }
    }

    if (NULL == t)
    {
        t = new Thread(toRun);
        new_thread = true;
        m_threads.insert(t);
    }
    else
    {
        t->SetRunnableEntity(toRun);
    }
    m_threadsLock.Unlock();

    if (new_thread)
        return t->Start();
    else
        t->Resume();

    return true;
}

void ThreadPool::StopAllThreads()
{
    m_threadsLock.Lock();
    std::set<Thread* > tmpSet(m_threads);
    m_threads.clear();
    m_threadsLock.Unlock();

    ThreadIterator it = tmpSet.begin();
    while (it != tmpSet.end())
    {
        (*it)->StopMe();
        (*it)->Resume();
        (*it)->Join();
        delete *it;
        ++ it;        
    }
}

int  ThreadPool::Size()
{
    ScopeMutex  guard(m_threadsLock);
    return  static_cast<int>(m_threads.size()); 
}