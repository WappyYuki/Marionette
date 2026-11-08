/******************************************************************************
 * balloon.h: 風船のデータと描画
 *
 * BalloonSet は memsys のアリーナから Create() で確保し、風船そのものは
 * BalloonSet が持つ ObjectPool<Balloon> に置く。風船は Remove() で個別に削除でき、
 * 空いた枠は次の Spawn() で使い回す。 */
#ifndef __BALLOON_H__
#define __BALLOON_H__

#include "actor.h"
#include "memsys.h"

//-------- 風船1個
class Balloon : public Actor {
public:
    // index 番目の風船として、地面のランダムな位置に置く
    void Init( int index, float radius );

    // プレイヤーとの接触判定と、くっついた後の動き。今回くっついたら true
    bool Update( float playerX, float playerY, float playerZ,
                 float playerDia, int playerState );

    // プレイヤーとの間の糸を描く（くっついていて、上昇が終わる前だけ）
    void DrawString( float playerX, float playerY, float playerZ, int playerState ) const;

protected:
    // 原点に赤い球を描く
    void DrawShape() const override;

private:
    enum State { Free = 0, Attached = 1 }; // 以前の dist_state の値

    float radius_ = 0.0f;
    float attachX_ = 0.0f, attachY_ = 0.0f, attachZ_ = 0.0f; // くっついた時のプレイヤーからのずれ
    int state_ = Free;
};

//-------- 風船全体
class BalloonSet {
public:
    // BalloonSet 自体を arena に、capacity 個分の風船の枠を poolArena に確保し、
    // count 個の風船を置く。失敗したら nullptr
    static BalloonSet *Create( memsys::Arena &arena, memsys::Arena &poolArena,
                               int capacity, int count, float radius );

    // 風船を1個増やす。枠が空いていなければ nullptr
    Balloon *Spawn();

    // 風船を1個削除する（デストラクタを呼び、枠を空ける）。この集合のものでなければ false
    bool Remove( Balloon *balloon );

    // 全ての風船を削除する（終了時）
    void RemoveAll() { pool_.DestroyAll(); }

    // 全ての風船を更新し、今回くっついた個数を返す
    int Update( float playerX, float playerY, float playerZ, float playerDia, int playerState );

    // 全ての風船と糸を描く
    void Draw( float playerX, float playerY, float playerZ, int playerState ) const;

    // 生きている風船を回る。f の中で今の風船を Remove してよい
    template<class F>
    void ForEach( F &&f ) { pool_.ForEach( std::forward<F>( f ) ); }
    template<class F>
    void ForEach( F &&f ) const { pool_.ForEach( std::forward<F>( f ) ); }

    int Count() const { return static_cast<int>( pool_.Count() ); }
    int Capacity() const { return static_cast<int>( pool_.Capacity() ); }

private:
    memsys::ObjectPool<Balloon> pool_; // 風船の枠（poolArena に確保）
    float radius_ = 0.0f;
    int spawned_ = 0; // これまでに作った数（姿勢の初期値に使う）
};

#endif
