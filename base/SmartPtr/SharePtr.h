#ifndef BERT_SHAREPTR_H
#define BERT_SHAREPTR_H

#ifdef SMART_PTR
#include <iostream>
#endif
#include "./CounterImpl.h"

template <typename T>
class SharedPtr;

template <typename T>
class EnableShareMe;

template <typename T>
class WeakPtr;

inline void InitShareMe(...)
{
}

template <typename T, typename U> // Ҫ��֤ U-->T�ǺϷ���
inline void InitShareMe(EnableShareMe<T>* pMe, SharedPtr<U>* pShare)
{
    if (pMe)    pMe->AcceptOwner(pShare);
}

template <typename T>
class SharedPtr
{
    template <typename Y>
    friend class WeakPtr;

    template <typename U>
    friend class SharedPtr;

public:
    SharedPtr() : m_cnt(0), m_ptr(0)
    {
    }

    ~SharedPtr()
    {
#ifdef SMART_PTR
        Print();
#endif
        if (m_cnt)
            m_cnt->DecShareCnt();
    }

    explicit SharedPtr(T* ptr) : m_cnt(0), m_ptr(ptr)
    {
        if (ptr)
        {
            m_cnt = new Counter<T>(ptr);
            InitShareMe(ptr, this);
        }
    }

    template <typename D>
    SharedPtr(T* ptr, D del) : m_cnt(0), m_ptr(ptr)
    {
        if (ptr)
        {
            m_cnt = new CounterD<T, D>(ptr, del);
            InitShareMe(ptr, this);
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other)
    {
        CounterBase* tmp = other.m_cnt;
        if (tmp)
        {
            tmp->AddShareCnt();
            m_cnt = tmp;
            m_ptr = static_cast<T* >(other.m_ptr);
        }
        else
        {
            m_cnt = 0;
            m_ptr = 0;
        }
    }

    SharedPtr(const SharedPtr& other)
    {
        CounterBase* tmp = other.m_cnt;
        if (tmp)
        {
            tmp->AddShareCnt();
            m_cnt = tmp;
            m_ptr = other.m_ptr;
        }
        else
        {
            m_cnt = 0;
            m_ptr = 0;
        }
    }

    SharedPtr(const WeakPtr<T>& other)
    {
        CounterBase* tmp = other.m_cnt;
        if (tmp && tmp->AddShareCopy())
        {
            m_cnt = tmp;
            m_ptr = other.m_ptr;
        }
        else
        {
            m_cnt = 0;
            m_ptr = 0;
        }
    }
  
    SharedPtr& operator=(const SharedPtr& other)
    {
        if (this == &other || m_ptr == other.m_ptr)
            return *this;

        CounterBase* tmp = other.m_cnt;
        if (tmp)
        {
            tmp->AddShareCnt();
        }

        Reset();

        m_cnt = tmp;
        m_ptr = other.m_ptr;

        return *this;
    }

    void Reset()
    {
        SharedPtr().Swap(*this);
    }

    void Reset(T* ptr)
    {
        if (ptr == m_ptr)  return;

        SharedPtr(ptr).Swap(*this);
    }

    void Swap(SharedPtr& other)
    {
        if (this != &other)
        {
            std::swap(m_cnt, other.m_cnt);
            std::swap(m_ptr, other.m_ptr);
        }
    }

    T& operator*() const
    {
        return *m_ptr;
    }

    T* operator->() const
    {
        return m_ptr;
    }

    T* Get() const
    {
        return m_ptr;
    }

    int UseCount() const
    {
        if (m_cnt)  return m_cnt->UseCount();

        return 0;
    }

#ifdef SMART_PTR
    void Print()
    {        
        std::cout << "|---------------\n";
        std::cout << "|Share cnt " << (m_cnt ? m_cnt->UseCount() : 0) << std::endl;
        std::cout << "|" << (m_ptr ? m_ptr : 0) << std::endl;
        std::cout << "|---------------\n";
    }
#endif

private:
    CounterBase* m_cnt;
    T*           m_ptr;
};

template <typename T>
inline bool operator==(SharedPtr<T> a, SharedPtr<T> b)
{
    return a.Get() == b.Get();
}

template <typename T>
inline bool operator<(SharedPtr<T> a, SharedPtr<T> b)
{
    return a.Get() < b.Get();
}

#endif

