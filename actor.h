/******************************************************************************
 * actor.h: 描画物の基底クラス
 *
 * 描画物は Actor を継承し、DrawShape() に「原点に置いた時の形」だけを書く。
 * 位置・姿勢への移動、表示/非表示の判定、行列の退避と復帰は Draw() が行う。
 *
 * Actor は memsys のアリーナに置くことを前提にしている。アリーナはデストラクタを
 * 呼ばないので、デストラクタは仮想にせず、自明（何もしない）のままにする。
 * - 派生クラスにもデストラクタの処理が要るメンバー（std::vector など）を持たせない
 * - Actor のポインタ経由で delete させないよう、デストラクタは protected にする
 * 仮想関数（DrawShape）は持てる。仮想関数があってもデストラクタが自明なら
 * std::is_trivially_destructible は true のままなので、memsys::New/NewArray で確保できる。 */
#ifndef __ACTOR_H__
#define __ACTOR_H__

class Actor {
public:
    // 見えている時だけ、位置・姿勢を当てて DrawShape() を呼ぶ
    void Draw() const;

    void SetPosition( float x, float y, float z ) { x_ = x; y_ = y; z_ = z; }
    void SetRotation( float roll, float pitch, float yaw ) { roll_ = roll; pitch_ = pitch; yaw_ = yaw; }
    void SetVisible( bool visible ) { visible_ = visible; }

    float X() const { return x_; }
    float Y() const { return y_; }
    float Z() const { return z_; }
    bool IsVisible() const { return visible_; }

protected:
    Actor() = default;
    ~Actor() = default; // 仮想にしない（アリーナはデストラクタを呼ばない）
    Actor( const Actor & ) = default;
    Actor &operator=( const Actor & ) = default;

    // 原点に置いた時の形を描く（マテリアルの設定も含む）
    virtual void DrawShape() const = 0;

    float x_ = 0.0f, y_ = 0.0f, z_ = 0.0f;          // 位置
    float roll_ = 0.0f, pitch_ = 0.0f, yaw_ = 0.0f; // 姿勢（度）
    bool visible_ = true;
};

#endif
