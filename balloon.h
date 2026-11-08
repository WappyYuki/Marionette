/******************************************************************************
 * balloon.h: 風船のデータと描画
 *
 * BalloonSet は memsys のアリーナから Create() で確保する。
 * アリーナはデストラクタを呼ばないので、Balloon・BalloonSet とも
 * デストラクタが不要な型（値のメンバーだけ）にしている。 */
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
    // arena から BalloonSet と count 個の Balloon を確保して初期化する。失敗したら nullptr
    static BalloonSet *Create( memsys::Arena &arena, int count, float radius );

    // 全ての風船を更新し、今回くっついた個数を返す
    int Update( float playerX, float playerY, float playerZ, float playerDia, int playerState );

    // 全ての風船と糸を描く
    void Draw( float playerX, float playerY, float playerZ, int playerState ) const;

    int Count() const { return count_; }
    const Balloon &Get( int index ) const { return balloons_[index]; }

private:
    Balloon *balloons_ = nullptr; // 同じアリーナに確保した配列
    int count_ = 0;
};

#endif
