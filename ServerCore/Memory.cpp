#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
    int32 size = 0;
    int32 tableIndex = 0;

    for (size = 32; size <= 1024; size += 32)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);

        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
    for (; size <= 2048; size += 128)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);

        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }

    for (; size <= 4096; size += 256)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);

        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
}

Memory::~Memory()
{
    for (MemoryPool* pool : _pools)
        delete pool;

    _pools.clear();
}

void* Memory::Allocate(int32 size)
{
    MemoryHeader* header = nullptr;

    const int32 allocSize = size + sizeof(MemoryHeader);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        // �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� �Ҵ�
        header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
    }
    else
    {
        //�޸� Ǯ���� �����´�
        header = _poolTable[allocSize]->Pop();
    }
    return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
    MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

    const int32 allocSize = header->allocSize;
    ASSERT_CRASH(allocSize > 0);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        //�޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ�����
        ::free(header);
    }
    else
    {
        //�޸� Ǯ�� �ݳ��Ѵ�
        _poolTable[allocSize]->Push(header);
    }

}