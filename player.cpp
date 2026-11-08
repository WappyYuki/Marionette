/******************************************************************************
 * player.cpp: プレイヤー（赤い立方体）のデータと描画 */

#include "player.h"

#include <cmath>

#include "glheaders.h"
#include "shapes.h"

// draw.cpp
void setMaterialRedPlastic( void );

void Player::Init( float dia )
{
	x_ = y_ = z_ = 0.0f;
	roll_ = pitch_ = yaw_ = 0.0f;
	visible_ = true;
	dia_ = dia;
	liftState_ = OnGround;
	balloonCount_ = 0;
}

float Player::UpdateLift()
{
	float lift = 0.0f;
	switch( liftState_ ){
	  case OnGround:
		y_ = 0.0f;
		break;
	  case Rising:
		y_ += 0.05f;
		lift = 0.05f;
		break;
	  case Falling:
		y_ -= 0.05f;
		balloonCount_ = 0;
		lift = -0.05f;
		break;
	}

	if( balloonCount_ == 5 && y_ < 10 ){
		liftState_ = Rising; // 上昇時の状態遷移
	}

	if( y_ >= 10 ){
		liftState_ = Falling; //下降時の状態遷移
	}
	else if( y_ < 0 ){
		liftState_ = OnGround;
	}
	return lift;
}

void Player::Move( float move, float rot )
{
	//// 回転方向の更新
	yaw_ += rot;

	//// 位置の更新
	const float dx = - move * sinf( yaw_ / 180.0f * 3.14f ); //X軸方向の移動量
	const float dz = - move * cosf( yaw_ / 180.0f * 3.14f ); //Z軸方向の移動量
	x_ += dx;
	z_ += dz;
}

void Player::DrawShape() const
{
	setMaterialRedPlastic();
	shapes::SolidCube( dia_ );
}
