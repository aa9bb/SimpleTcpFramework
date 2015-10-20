#ifndef BERT_ATOMIC_H
#define BERT_ATOMIC_H


#if defined(__gnu_linux__)

#define AtomicChange(plAddEnd, delta) __sync_fetch_and_add((plAddEnd), (delta))
#define AtomicChange64                AtomicChange

#define AtomicSet(plTarget, value)    __sync_lock_test_and_set ((plTarget), (value))
#define AtomicSet64                   AtomicSet

#define AtomicTestAndSet(plDest, lExchange, lComparand)\
    __sync_val_compare_and_swap((plDest), (lComparand), (lExchange))

#define AtomicTestAndSet64            AtomicTestAndSet


#else
#include <Windows.h>

#define AtomicChange(plAddEnd, delta)    InterlockedExchangeAdd((long*)(plAddEnd), (long)(delta))
#define AtomicChange64(pllAddEnd, delta) InterlockedExchangeAdd64((long long*)(pllAddEnd), (long long)(delta))

#define AtomicSet(plTarget, value)       InterlockedExchange((long*)(plTarget), (long)(value))
#define AtomicSet64(pllTarget, value)    InterlockedExchange64((long long*)(pllTarget), (long long)(value))

#define AtomicTestAndSet(plDest, lExchange, lComparand)\
    InterlockedCompareExchange((long*)(plDest), (long)(lExchange), (long)(lComparand))

#define AtomicTestAndSet64(pllDest, llExchange, llComparand)\
    InterlockedCompareExchange64((long long*)(pllDest), (long long)(llExchange), (long long)(llComparand))

#endif

#define AtomicGet(plTarget)      AtomicChange(plTarget, 0)
#define AtomicGet64(plTarget)    AtomicChange64(plTarget, 0)



template <typename DummyType>
class IdCreator32
{
public:
    static unsigned int CreateID();
private:
    static unsigned int m_id;
};

template <typename DummyType>
unsigned int IdCreator32<DummyType>::m_id = 0;

template <typename DummyType>
unsigned int IdCreator32<DummyType>::CreateID()
{
    return AtomicChange(&m_id, 1);
}



template <typename DummyType>
class IdCreator64
{
public:
    static unsigned long long CreateID();
private:
    static unsigned long long m_id;
};

template <typename DummyType>
unsigned long long IdCreator64<DummyType>::m_id = 0;

template <typename DummyType>
unsigned long long IdCreator64<DummyType>::CreateID()
{
    return AtomicChange(&m_id, 1);
}

#endif
