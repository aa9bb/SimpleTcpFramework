
#ifndef BERT_IPC_H
#define BERT_IPC_H

#if defined(__gnu_linux__)
    #include <pthread.h>
    #include <semaphore.h>

#else
    #include <Windows.h>
    #define    __thread    __declspec(thread)
#endif

// Recursive mutex, lock and unlock by same thread.
class Mutex
{
    friend class Condition;

#if defined(__gnu_linux__)
    pthread_mutex_t  m_mutex;
#else
    CRITICAL_SECTION m_mutex;
#endif

public:
    Mutex();
    ~Mutex();
    
    void Lock();
    bool TryLock();
    void Unlock();

private:
    // Avoid copy
    Mutex(const Mutex& );

    // Avoid assignment
    Mutex& operator= (const Mutex& );
};

// Use mutex with RAII
class ScopeMutex
{
    Mutex& m_mutex;
public:
    ScopeMutex(Mutex& m);
    ~ScopeMutex();
};

// Can not recursive
class RWLock
{
#if defined(__gnu_linux__)
    pthread_rwlock_t m_rwLock;
#else
    Mutex            m_rwLock;
    //RTL_SRWLOCK      m_rwLock;
#endif

public:
    
    RWLock();
    ~RWLock();
    
    void Rdlock();
    void Wrlock();
    void UnRdlock();
    void UnWrlock();

private:
    // Avoid copy
    RWLock(const RWLock& );

    // Avoid assignment
    RWLock& operator= (const RWLock& );
};

class ScopeReadLock
{
    RWLock& m_lock;
public:
    
    ScopeReadLock(RWLock &m);
    ~ScopeReadLock();
};

class ScopeWriteLock
{
    RWLock& m_lock;
public:
    ScopeWriteLock(RWLock & m);
    ~ScopeWriteLock();
};

#if defined(__gnu_linux__)
class Condition
{
    Mutex* m_mutex;
#if defined(__gnu_linux__)
    pthread_cond_t     m_cond;
#else
    CONDITION_VARIABLE m_cond;
#endif
    
public:
    
    Condition(Mutex* m);
    ~Condition();

    void Signal();
    void Broadcast();
    void Wait();
    void Lock();
    void Unlock();

private:
    // Avoid copy
    Condition(const Condition& );

    // Avoid assignment
    Condition& operator= (const Condition& );
};
#endif


class Semaphore
{
#if defined(__gnu_linux__)
    sem_t  m_sem;
#else
    HANDLE m_sem;
#endif

public:
    Semaphore(long lInit = 0, long lMaxForWindows = 65536);
    ~Semaphore();

    void Wait();
    void Post(long lReleaseCntForWin = 1);

private:
    // Avoid copy
    Semaphore(const Semaphore& );

    // Avoid assignment
    Semaphore& operator= (const Semaphore& );
};


#endif
