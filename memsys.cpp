/******************************************************************************
 * memsys.cpp: 起動時にまとめて確保するメモリ */

#include "memsys.h"

#include <cstdio>
#include <cstring>
//#include <initializer_list>

#include <Windows.h>

namespace memsys {

    namespace {
        char *regionBase = nullptr; // VirtualAlloc で予約した領域の先頭
        std::size_t regionSize = 0;
        std::size_t pageSize = 0;

        Arena frameArena;
        Arena poolArena;
        Arena persistentArena;

        std::size_t alignUp( std::size_t value, std::size_t align )
        {
            return ( value + align - 1 ) & ~( align - 1 );
        }

        bool isPowerOfTwo( std::size_t value )
        {
            return value != 0 && ( value & ( value - 1 ) ) == 0;
        }
    }

    //------------------------------------------------------------------------ Arena
    bool Arena::Init( const char *name, void *base, std::size_t reserveSize,
                      std::size_t initialCommit, std::size_t commitStep )
    {
        name_ = name;
        base_ = static_cast<char *>( base );
        reserved_ = reserveSize;
        committed_ = 0;
        used_ = 0;
        peak_ = 0;
        commitStep_ = commitStep;
        return commitUpTo( initialCommit < reserveSize ? initialCommit : reserveSize );
    }

    void *Arena::Allocate( std::size_t size, std::size_t align )
    {
        if( !isPowerOfTwo( align ) ){
            std::fprintf( stderr, "memsys: %s: alignment %zu is not a power of two\n", name_, align );
            return nullptr;
        }
        // base_ はページ境界にあるので、先頭からのずれを揃えればアドレスも揃う
        const std::size_t offset = alignUp( used_, align );
        if( offset > reserved_ || size > reserved_ - offset ){
            std::fprintf( stderr, "memsys: %s: out of memory (request %zu bytes, used %zu / %zu)\n",
                          name_, size, used_, reserved_ );
            return nullptr;
        }
        const std::size_t end = offset + size;
        if( end > committed_ && !commitUpTo( end ) ) return nullptr;

        used_ = end;
        if( used_ > peak_ ) peak_ = used_;
        return base_ + offset;
    }

    void Arena::ResetTo( std::size_t mark )
    {
        if( mark > used_ ) return;
    #ifdef _DEBUG
        // 捨てた領域を読むとすぐ分かるよう、目立つ値で埋める
        std::memset( base_ + mark, 0xDD, used_ - mark );
    #endif
        used_ = mark;
    }

    bool Arena::commitUpTo( std::size_t bytes )
    {
        if( bytes <= committed_ ) return true;

        std::size_t target = alignUp( bytes, commitStep_ );
        if( target > reserved_ ) target = reserved_;
        if( VirtualAlloc( base_ + committed_, target - committed_, MEM_COMMIT, PAGE_READWRITE ) == nullptr ){
            std::fprintf( stderr, "memsys: %s: commit %zu bytes failed (error %lu)\n",
                          name_, target - committed_, GetLastError() );
            return false;
        }
        committed_ = target;
        return true;
    }

    //------------------------------------------------------------------------- Pool
    bool Pool::Init( Arena &arena, std::size_t slotSize, std::size_t slotAlign, std::size_t capacity )
    {
        if( !isPowerOfTwo( slotAlign ) ) return false;
        // 空き枠には次の空き枠へのポインタを書くので、その大きさと境界は確保する
        if( slotAlign < alignof( void * ) ) slotAlign = alignof( void * );
        if( slotSize < sizeof( void * ) ) slotSize = sizeof( void * );
        slotSize = alignUp( slotSize, slotAlign );
        if( capacity > 0 && slotSize > static_cast<std::size_t>( -1 ) / capacity ) return false;

        char *slots = static_cast<char *>( arena.Allocate( slotSize * capacity, slotAlign ) );
        bool *used = NewArray<bool>( arena, capacity );
        if( slots == nullptr || used == nullptr ) return false;

        slots_ = slots;
        used_ = used;
        slotSize_ = slotSize;
        capacity_ = capacity;
        count_ = 0;

        // 先頭の枠から順に使われるよう、後ろからつなぐ
        freeList_ = nullptr;
        for( std::size_t i = capacity; i > 0; --i ){
            void *slot = Slot( i - 1 );
            *static_cast<void **>( slot ) = freeList_;
            freeList_ = slot;
        }
        return true;
    }

    void *Pool::Allocate()
    {
        if( freeList_ == nullptr ) return nullptr;

        char *slot = static_cast<char *>( freeList_ );
        freeList_ = *reinterpret_cast<void **>( slot );
        used_[( slot - slots_ ) / slotSize_] = true;
        ++count_;
        return slot;
    }

    bool Pool::Free( void *p )
    {
        const std::size_t index = slotIndex( p );
        if( index == kInvalid ){
            std::fprintf( stderr, "memsys: pool: free of unknown or already freed pointer %p\n", p );
            return false;
        }
        used_[index] = false;
        --count_;
#ifdef _DEBUG
        // 解放後の枠を読むとすぐ分かるよう、目立つ値で埋める
        std::memset( p, 0xDD, slotSize_ );
#endif
        *static_cast<void **>( p ) = freeList_;
        freeList_ = p;
        return true;
    }

    std::size_t Pool::slotIndex( const void *p ) const
    {
        const char *c = static_cast<const char *>( p );
        if( slots_ == nullptr || c < slots_ || c >= slots_ + slotSize_ * capacity_ ) return kInvalid;
        const std::size_t offset = static_cast<std::size_t>( c - slots_ );
        if( offset % slotSize_ != 0 ) return kInvalid;
        const std::size_t index = offset / slotSize_;
        return used_[index] ? index : kInvalid;
    }

    //-------------------------------------------------------------------- 全体の管理
    bool Init( const Config &config )
    {
        if( regionBase != nullptr ) return true;

        SYSTEM_INFO info = {};
        GetSystemInfo( &info );
        pageSize = info.dwPageSize;

        const std::size_t frameSize = alignUp( config.frameSize, pageSize );
        const std::size_t poolSize = alignUp( config.poolSize, pageSize );
        const std::size_t commitStep = alignUp( config.commitStep > 0 ? config.commitStep : pageSize, pageSize );
        regionSize = alignUp( config.reserveSize, info.dwAllocationGranularity );
        if( frameSize >= regionSize || poolSize >= regionSize - frameSize ){
            std::fprintf( stderr, "memsys: frameSize (%zu) + poolSize (%zu) must be smaller than reserveSize (%zu)\n",
                          frameSize, poolSize, regionSize );
            return false;
        }

        regionBase = static_cast<char *>( VirtualAlloc( nullptr, regionSize, MEM_RESERVE, PAGE_NOACCESS ) );
        if( regionBase == nullptr ){
            std::fprintf( stderr, "memsys: reserve %zu bytes failed (error %lu)\n", regionSize, GetLastError() );
            return false;
        }

        const bool ok =
            frameArena.Init( "frame", regionBase, frameSize, frameSize, commitStep ) &&
            poolArena.Init( "pool", regionBase + frameSize, poolSize, 0, commitStep ) &&
            persistentArena.Init( "persistent", regionBase + frameSize + poolSize,
                                  regionSize - frameSize - poolSize,
                                  alignUp( config.initialCommit, pageSize ), commitStep );
        if( !ok ){
            Shutdown();
            return false;
        }
        return true;
    }

    void Shutdown()
    {
        if( regionBase == nullptr ) return;
        VirtualFree( regionBase, 0, MEM_RELEASE );
        regionBase = nullptr;
        regionSize = 0;
        frameArena = Arena();
        poolArena = Arena();
        persistentArena = Arena();
    }

    Arena &Persistent() { return persistentArena; }
    Arena &Frame() { return frameArena; }
    Arena &PoolHeap() { return poolArena; }

    void BeginFrame()
    {
        frameArena.Reset();
    }

    void PrintStats()
    {
        std::printf( "memsys: reserved %zu MiB at %p\n", regionSize / MiB, static_cast<void *>( regionBase ) );
        for( const Arena *arena : { &frameArena, &poolArena, &persistentArena } ){
            std::printf( "memsys:   %-10s used %zu KiB (peak %zu KiB) / committed %zu KiB / reserved %zu KiB\n",
                         arena->Name(), arena->Used() / KiB, arena->Peak() / KiB,
                         arena->Committed() / KiB, arena->Reserved() / KiB );
        }
    }

}
