/******************************************************************************
 * memsys.h: 起動時にまとめて確保するメモリ
 *
 * 起動時に大きなアドレス空間を1回だけ予約し（VirtualAlloc の MEM_RESERVE）、
 * 実際に使う分だけコミットして、そこから先頭詰めで切り出す。
 *
 *   [ フレーム領域 | プールヒープ | 常駐領域 ............................ ]
 *     frameSize      poolSize       残り全部（どれも使った分だけ順にコミット）
 *
 * - Frame()     : 1フレームだけ使う一時データ用。BeginFrame() で毎フレーム空になる
 * - PoolHeap()  : Pool / ObjectPool<T> の枠を切り出す専用の領域
 * - Persistent(): プログラム終了まで使うデータ用。解放は Mark/ResetTo でまとめて行う
 *
 * アリーナは個別の解放もデストラクタ呼び出しもしない。New/NewArray は
 * デストラクタが不要な型（trivially destructible）だけを受け付ける。
 *
 * 個別に削除したいもの（Actor など）は、PoolHeap() から切り出した Pool / ObjectPool<T> に置く。
 * - Pool          : 同じ大きさの枠を capacity 個用意し、空き枠を使い回す（個別に解放できる）
 * - ObjectPool<T> : Pool の型付き版。Destroy() でデストラクタを呼んでから枠を返す */
#ifndef __MEMSYS_H__
#define __MEMSYS_H__

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace memsys {

    constexpr std::size_t KiB = 1024;
    constexpr std::size_t MiB = 1024 * KiB;
    constexpr std::size_t GiB = 1024 * MiB;

    //-------- 起動時に決めるサイズ（ページサイズ単位に切り上げて使う）
    struct Config {
        std::size_t reserveSize   = 1 * GiB;  // 予約するアドレス空間の総量（フレーム領域を含む）
        std::size_t frameSize     = 16 * MiB; // フレーム領域。起動時に全てコミットする
        std::size_t poolSize      = 64 * MiB; // プールヒープの大きさ（使った分だけコミットする）
        std::size_t initialCommit = 64 * MiB; // 常駐領域のうち起動時にコミットしておく量
        std::size_t commitStep    = 1 * MiB;  // プールヒープ・常駐領域が足りない時に追加でコミットする単位
    };

    //-------- 予約済みの領域から先頭詰めで確保し、足りなくなった分だけコミットするアリーナ
    class Arena {
    public:
        // base から reserveSize バイトの予約済み領域を使う。initialCommit までを今コミットする
        bool Init( const char *name, void *base, std::size_t reserveSize,
                   std::size_t initialCommit, std::size_t commitStep );

        // 確保できない時（予約量を超える・コミット失敗）は nullptr を返す。align は2のべき乗
        void *Allocate( std::size_t size, std::size_t align = alignof( std::max_align_t ) );

        // Mark() で覚えた位置まで戻し、それ以降の確保をまとめて捨てる
        std::size_t Mark() const { return used_; }
        void ResetTo( std::size_t mark );
        void Reset() { ResetTo( 0 ); }

        const char *Name() const { return name_; }
        std::size_t Used() const { return used_; }
        std::size_t Peak() const { return peak_; }
        std::size_t Committed() const { return committed_; }
        std::size_t Reserved() const { return reserved_; }

    private:
        bool commitUpTo( std::size_t bytes );

        const char *name_ = "";
        char *base_ = nullptr;
        std::size_t reserved_ = 0;
        std::size_t committed_ = 0;
        std::size_t used_ = 0;
        std::size_t peak_ = 0;
        std::size_t commitStep_ = 0;
    };

    //-------- スコープを抜けると、作った時点の位置までアリーナを戻す
    class ScopedMark {
    public:
        explicit ScopedMark( Arena &arena ) : arena_( arena ), mark_( arena.Mark() ) {}
        ~ScopedMark() { arena_.ResetTo( mark_ ); }
        ScopedMark( const ScopedMark & ) = delete;
        ScopedMark &operator=( const ScopedMark & ) = delete;
    private:
        Arena &arena_;
        std::size_t mark_;
    };

    //-------- 全体の初期化・終了
    bool Init( const Config &config );
    void Shutdown();

    Arena &Persistent(); // プログラム終了まで使う領域
    Arena &Frame();      // 毎フレーム空になる領域
    Arena &PoolHeap();   // プールの枠を切り出す領域

    // フレームの始めに呼ぶ。前のフレームの Frame() の確保を全て捨てる
    void BeginFrame();

    // 各領域の使用量を標準出力に出す
    void PrintStats();

    //-------- 型付きの確保
    template<class T, class... Args>
    T *New( Arena &arena, Args &&...args )
    {
        static_assert( std::is_trivially_destructible_v<T>, "アリーナはデストラクタを呼ばない" );
        void *p = arena.Allocate( sizeof( T ), alignof( T ) );
        return p ? new( p ) T( std::forward<Args>( args )... ) : nullptr;
    }

    template<class T>
    T *NewArray( Arena &arena, std::size_t count )
    {
        static_assert( std::is_trivially_destructible_v<T>, "アリーナはデストラクタを呼ばない" );
        if( count > static_cast<std::size_t>( -1 ) / sizeof( T ) ) return nullptr;
        void *p = arena.Allocate( sizeof( T ) * count, alignof( T ) );
        return p ? new( p ) T[count]() : nullptr;
    }

    //-------- 同じ大きさの枠を使い回すプール（個別に確保・解放できる）
    // 枠の領域と使用中フラグは Init() でアリーナから切り出す。空き枠は枠の中に
    // 次の空き枠へのポインタを書いてつなぐ（フリーリスト）ので、余分な管理領域は要らない。
    class Pool {
    public:
        // slotSize バイト・slotAlign 境界の枠を capacity 個用意する
        bool Init( Arena &arena, std::size_t slotSize, std::size_t slotAlign, std::size_t capacity );

        // 空き枠を1つ返す。空きがなければ nullptr
        void *Allocate();

        // 枠を空きに戻す。このプールの使用中の枠でなければ（二重解放など）何もせず false
        bool Free( void *p );

        // p がこのプールの使用中の枠の先頭か
        bool Owns( const void *p ) const { return slotIndex( p ) != kInvalid; }

        bool IsUsed( std::size_t index ) const { return used_[index]; }
        void *Slot( std::size_t index ) const { return slots_ + index * slotSize_; }

        std::size_t Count() const { return count_; }
        std::size_t Capacity() const { return capacity_; }

    private:
        static constexpr std::size_t kInvalid = static_cast<std::size_t>( -1 );
        std::size_t slotIndex( const void *p ) const;

        char *slots_ = nullptr;
        bool *used_ = nullptr;
        void *freeList_ = nullptr;
        std::size_t slotSize_ = 0;
        std::size_t capacity_ = 0;
        std::size_t count_ = 0;
    };

    //-------- T 型のオブジェクトを置くプール。Destroy() でデストラクタを呼ぶ
    // 派生クラスごとに大きさが違うので、1つのプールには T そのものだけを置く。
    template<class T>
    class ObjectPool {
    public:
        bool Init( Arena &arena, std::size_t capacity )
        {
            return pool_.Init( arena, sizeof( T ), alignof( T ), capacity );
        }

        // 空き枠に T を作る。空きがなければ nullptr
        template<class... Args>
        T *Create( Args &&...args )
        {
            void *p = pool_.Allocate();
            return p ? new( p ) T( std::forward<Args>( args )... ) : nullptr;
        }

        // デストラクタを呼んで枠を空きに戻す。このプールのものでなければ何もせず false
        bool Destroy( T *object )
        {
            if( object == nullptr || !pool_.Owns( object ) ) return false;
            object->~T();
            return pool_.Free( object );
        }

        // 生きているオブジェクトを枠の順に回る。f の中で今のオブジェクトを Destroy してよい
        template<class F>
        void ForEach( F &&f )
        {
            for( std::size_t i = 0; i < pool_.Capacity(); ++i ){
                if( pool_.IsUsed( i ) ) f( *static_cast<T *>( pool_.Slot( i ) ) );
            }
        }
        template<class F>
        void ForEach( F &&f ) const
        {
            for( std::size_t i = 0; i < pool_.Capacity(); ++i ){
                if( pool_.IsUsed( i ) ) f( *static_cast<const T *>( pool_.Slot( i ) ) );
            }
        }

        // 生きているオブジェクトを全て Destroy する（終了時など）
        void DestroyAll()
        {
            ForEach( [this]( T &object ){ Destroy( &object ); } );
        }

        std::size_t Count() const { return pool_.Count(); }
        std::size_t Capacity() const { return pool_.Capacity(); }

    private:
        Pool pool_;
    };

}

#endif
