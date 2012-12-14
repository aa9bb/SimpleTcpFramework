#ifndef BERT_WEAKPTR_H
#define BERT_WEAKPTR_H

#ifdef SMART_PTR
#include <iostream>
#endif
#include "./SharePtr.h"

template <typename T>
class WeakPtr 
{
    template <typename Y>
    friend class SharedPtr;
public:
    WeakPtr() : m_cnt(0), m_ptr(0)
    {
    }

    ~WeakPtr()
    {
        if (m_cnt)
            m_cnt->DecWeakCnt();

#ifdef SMART_PTR
        Print();
#endif
    }

    template <typename U>
    WeakPtr(const SharedPtr<U>& sptr) : m_cnt(sptr.m_cnt), m_ptr(sptr.m_ptr)
    {
        if (m_cnt)      m_cnt->AddWeakCnt();
    }

    WeakPtr(const WeakPtr& other)
    {
        CounterBase* tmp = other.m_cnt;
        if (tmp)
        {
            tmp->AddWeakCnt();
            m_cnt = tmp;
            m_ptr = other.m_ptr;
        }
        else
        {
            m_cnt = 0;
            m_ptr = 0;
        }
    }
  
    WeakPtr& operator=(const WeakPtr& other)
    {
        if (this == &other)
            return *this;

        CounterBase* tmp = other.m_cnt;
        if (tmp)
            tmp->AddWeakCnt();

        Reset();

        m_cnt = tmp;
        m_ptr = other.m_ptr;

        return *this;
    }

    void Reset()
    {
        WeakPtr().Swap(*this);
    }

    template <typename U>
    void Reset(SharedPtr<U>& sptr)
    {
        if (sptr.m_ptr == m_ptr)  return;

        WeakPtr(sptr).Swap(*this);
    }

    void Swap(WeakPtr& other)
    {
        if (this != &other)
        {
            std::swap(m_cnt, other.m_cnt);
            std::swap(m_ptr, other.m_ptr);
        }
    }

    int UseCount() const
    {
        if (m_cnt)  return m_cnt->UseCount();

        return 0;
    }

    bool Expired() const
    {
        return 0 == UseCount();
    }

    SharedPtr<T> Lock()
    {
        if (Expired())
            return SharedPtr<T>();

        return SharedPtr<T>(*this);
    }


#ifdef SMART_PTR
    void Print()
    {     
        std::cout << "========WEAK=======\n";
        std::cout << "Weak  cnt " << (m_cnt ? m_cnt->WeakCount() : 0) << std::endl;
        std::cout << "Share cnt " << (m_cnt ? m_cnt->UseCount() : 0) << std::endl;
        std::cout << (m_ptr ? m_ptr : 0) << std::endl;
        std::cout << "========WEAK=======\n";
    }
#endif

private:
    CounterBase* m_cnt;
    T*           m_ptr;
};

#endif

