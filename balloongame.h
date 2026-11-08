/******************************************************************************
 * balloongame.h: 風船集めのインゲーム（以前の sim.cpp）
 *
 * プレイヤーで風船に触れると風船がくっつき、5個集めると上昇して、
 * 高さ10まで上がると下降して地面に戻る。
 * キー・マウスの入力とカメラの状態は、以前どおり simdata（common.h）でやり取りする。 */
#ifndef __BALLOONGAME_H__
#define __BALLOONGAME_H__

#include "ingameplay.h"
#include "memsys.h"
#include "player.h"

class BalloonSet;

class BalloonGame : public InGamePlay {
public:
    BalloonGame() = default;
    ~BalloonGame() override { Finish(); } // Finish し忘れてもプレイヤー・風船を片付ける

    // 客観視点のカメラはプレイヤーを見る
    void GetViewTarget( float &x, float &y, float &z ) const override;

protected:
    bool OnStart() override;
    void OnUpdate() override;
    void OnDraw() override;
    void OnFinish() override;

private:
    memsys::ObjectPool<Player> playerPool_; // プレイヤーの枠（プールヒープ、今は1人分）
    Player *player_ = nullptr;
    BalloonSet *balloons_ = nullptr;        // 集合は常駐領域、風船の枠はプールヒープ
};

#endif
