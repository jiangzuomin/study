#ifndef _MEMORY_MGR_H_
#define _MEMORY_MGR_H_

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <thread>
#include <new>
#include <mutex>

class MemoryPool;
class MemoryBlock
{
public:
    int nId;
    int nRet;
    bool bPool;
    MemoryBlock* pNext;
    MemoryPool* pPool;

public:
    MemoryBlock();
    ~MemoryBlock();
};

class MemoryPool
{
protected:
    size_t m_nBlockCount;
    size_t m_nBlockSize;
    char* m_pBuf;
    MemoryBlock* m_pHeader;
    std::mutex m_mutex;

public:
    MemoryPool();
    ~MemoryPool();
    void initMemPool();
    void* alloctMem(std::size_t nSize);
    void freeMem(void* p);
};

template<size_t nBlockSize, size_t nBlockCount>
class InitMemoryPool : public MemoryPool
{
private:
public:
    InitMemoryPool()
    {
        const size_t n = sizeof(void *);
        m_nBlockSize = ((nBlockSize / n) * n) + ((nBlockSize % n) ? n : 0);
        m_nBlockCount = nBlockCount;
    }
    ~InitMemoryPool()
    {

    }
};


class MemoryMgr
{
private:
    InitMemoryPool<64, 1000000> m_memPool64;
    MemoryPool* m_szPool64[64 + 1];

private:
    MemoryMgr();
    ~MemoryMgr();
    void initMemPool(int nBegin, int nEnd, MemoryPool* pMemPool);
public:
    static MemoryMgr& Instance();
    void* alloctMem(std::size_t nSize);
    void freeMem(void* p);
};

#endif