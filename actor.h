/******************************************************************************
 * actor.h: 描画物の基底クラス
 *
 * 描画物は Actor を継承し、DrawShape() に「原点に置いた時の形」だけを書く。
 * 位置・姿勢への移動、表示/非表示の判定、行列の退避と復帰は Draw() が行う。
 *
 * Actor は個別に削除できるよう、memsys::ObjectPool<派生クラス> に置くことを前提にしている。
 * ObjectPool::Destroy() はデストラクタを呼ぶので、派生クラスはデストラクタで後片付けしてよい。
 * デストラクタは仮想なので、Actor のポインタ経由で破棄しても派生クラスのものが呼ばれる。
 * （アリーナの New/NewArray はデストラクタを呼ばないので、Actor は置けない） */
#ifndef __ACTOR_H__
#define __ACTOR_H__

class Actor {
public:
    virtual ~Actor() = default;

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
    Actor( const Actor & ) = default;
    Actor &operator=( const Actor & ) = default;

    // 原点に置いた時の形を描く（マテリアルの設定も含む）
    virtual void DrawShape() const = 0;

    float x_ = 0.0f, y_ = 0.0f, z_ = 0.0f;          // 位置
    float roll_ = 0.0f, pitch_ = 0.0f, yaw_ = 0.0f; // 姿勢（度）
    bool visible_ = true;
};

#endif
