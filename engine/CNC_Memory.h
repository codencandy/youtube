#ifndef CNC_MEMORY_H
#define CNC_MEMORY_H

namespace cnc
{
    typedef struct MemoryPool
    {
        unsigned int   m_size;
        unsigned int   m_used;
        void*          m_base;
    
    } MemoryPool;
    
    void* _allocateMemory( MemoryPool* pool, unsigned int size )
    {
        void* memory  = (unsigned char*)pool->m_base + pool->m_used;
        pool->m_used += size;
    
        return memory;
    }
    
    #define ALLOC_STRUCT( p, t)    (t*)_allocateMemory( p,     sizeof( t ) )
    #define ALLOC_ARRAY( p, t, n ) (t*)_allocateMemory( p, n * sizeof( t ) )
    
    #ifdef CNC_MEMORY_IMPLEMENTATION
    
    #include <stddef.h>
    extern "C" void* malloc( size_t size );
    extern "C" void* memset( void*, int c, size_t size );
    
    MemoryPool* CreateMemoryPool( unsigned int size )
    {
        MemoryPool* pool = (MemoryPool*)malloc( size + sizeof( MemoryPool ) );
    
        pool->m_size = size;
        pool->m_used = 0;
        pool->m_base = pool + sizeof( MemoryPool );
    
        return pool;
    }
    #endif
}

#endif//CNC_MEMORY_H
