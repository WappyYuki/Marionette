/******************************************************************************
 * ingameplay.h: インゲーム（遊んでいる最中）の進行を管理する基底クラス
 *
 * ゲームごとのルールは InGamePlay を継承したクラスに書き、OnStart / OnUpdate /
 * OnDraw / OnFinish を実装する。呼び出し側（main.cpp）は具体的なゲームを知らず、
 * Start → 毎フレーム Update と Draw → Finish の順に InGamePlay として動かす。
 *
 *   NotStarted --Start()--> Playing <--Pause()/Resume()--> Paused
 *                              |  （EndPlay() / Finish()）
 *                              v
 *                           Finished
 *
 * 一時停止中は OnUpdate を呼ばない（ルールの時間が止まる）が、OnDraw は呼ぶ。 */
#ifndef __INGAMEPLAY_H__
#define __INGAMEPLAY_H__

#include <cstdint>

class InGamePlay {
public:
    enum class State { NotStarted, Playing, Paused, Finished };

    virtual ~InGamePlay() = default;
    InGamePlay( const InGamePlay & ) = delete;
    InGamePlay &operator=( const InGamePlay & ) = delete;

    // インゲームを始める。OnStart が失敗したら false を返し、Finished になる
    bool Start();

    // 1フレーム進める。Playing の時だけ OnUpdate を呼び、経過フレーム数を数える
    void Update();

    // インゲームの物を描く。始まってから終わるまで（一時停止中も）OnDraw を呼ぶ
    void Draw();

    // インゲームを終える。OnFinish は1回だけ呼ばれる
    void Finish();

    void Pause();
    void Resume();

    State GetState() const { return state_; }
    bool IsPlaying() const { return state_ == State::Playing; }
    bool IsFinished() const { return state_ == State::Finished; }

    // Playing で進んだフレーム数
    std::uint64_t FrameCount() const { return frameCount_; }

    // カメラが注目する点（客観視点など）。既定は原点
    virtual void GetViewTarget( float &x, float &y, float &z ) const;

protected:
    InGamePlay() = default;

    // 派生クラスが実装するルール
    virtual bool OnStart() = 0;     // 初期化。失敗したら false
    virtual void OnUpdate() = 0;    // 1フレーム分のルールの進行
    virtual void OnDraw() {}        // インゲームの物の描画
    virtual void OnFinish() {}      // 後片付け

    // ルール上の終わり（勝ち・負けなど）を派生クラスから伝える
    void EndPlay() { Finish(); }

private:
    State state_ = State::NotStarted;
    std::uint64_t frameCount_ = 0;
};

#endif
