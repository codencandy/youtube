#include "CNC_Pong.h"

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    Pong* pong = ALLOC_STRUCT( pool, Pong );

    InitApplication( pong, pool, transient, services, renderer );

    return pong;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* app )
{

}