/******************************************************************************
 * player.h: プレイヤー（赤い立方体）のデータと描画
 *
 * 風船を5個集めると上昇し、高さ10に達すると下降して地面に戻る。
 * memsys::ObjectPool<Player> から確保する（BalloonGame）。 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "actor.h"

class Player : public Actor {
public:
    // 上昇の状態（以前の simdata.player_state の値）
    enum LiftState { OnGround = 0, Rising = 1, Falling = 2 };

    // 原点に置き、地面にいる状態にする。dia は立方体の一辺（当たり判定の直径にも使う）
    void Init( float dia );

    // 上昇・下降を1フレーム進め、状態を切り替える。今回動いた高さを返す（カメラを合わせるため）
    float UpdateLift();

    // 今回くっついた風船の数を足す
    void AddBalloons( int count ) { balloonCount_ += count; }

    // 向きを rot 度回し、向いている方向へ move だけ進む
    void Move( float move, float rot );

    float Dia() const { return dia_; }
    int State() const { return liftState_; }
    int BalloonCount() const { return balloonCount_; }

protected:
    // 原点に赤い立方体を描く
    void DrawShape() const override;

private:
    float dia_ = 1.0f;
    int liftState_ = OnGround;
    int balloonCount_ = 0; // くっついている風船の数（下降を始めると 0 に戻る）
};

#endif
