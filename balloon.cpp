/******************************************************************************
 * balloon.cpp: 風船のデータと描画 */

#include "balloon.h"

#include <cmath>
#include <cstdio>
#include <type_traits>

#include "glheaders.h"
#include "shapes.h"

// balloongame.cpp
float Random( float rmin, float rmax );

// draw.cpp
void setMaterialRedPlastic( void );

// BalloonSet はアリーナに置く（デストラクタは呼ばれない）。風船は RemoveAll() で片付ける
static_assert( std::is_trivially_destructible_v<BalloonSet>, "アリーナに置くのでデストラクタ不要にする" );

//---------------------------------------------------------------------- Balloon
void Balloon::Init( int index, float radius )
{
	x_ = Random( -30.0f, 30.0f );
	y_ = 0;
	z_ = Random( -30.0f, 30.0f );
	roll_ = index * 0.5f;
	pitch_ = index * 0.5f;
	yaw_ = index * 0.5f;
	visible_ = true;
	radius_ = radius;
	attachX_ = attachY_ = attachZ_ = 0.0f;
	state_ = Free;
}

bool Balloon::Update( float playerX, float playerY, float playerZ,
                      float playerDia, int playerState )
{
	switch( state_ ){
	  case Free: {
		//プレイヤと風船の距離を求める
		const float dx = powf( playerX - x_, 2 );
		const float dy = powf( playerY - y_, 2 );
		const float dz = powf( playerZ - z_, 2 );
		const float dist = sqrtf( dx + dy + dz );

		//衝突判定
		if( dist < radius_ + playerDia / 2 ){
			//風船の移動距離を求める
			attachX_ = Random( -3.0f, 3.0f );
			attachY_ = Random( 0.6f, 3.0f );
			attachZ_ = Random( -3.0f, 3.0f );
			state_ = Attached;
			return true;
		}
		break;
	  }
	  case Attached:
		if( playerState < 2 ){
			x_ = playerX + attachX_;
			y_ = playerY + attachY_;
			z_ = playerZ + attachZ_;
		}
		else if( playerState == 2 ){
			std::printf( "空に飛ぶ～～～\n" );
			y_ += 0.01f;
			if( playerY <= 0.2 ){
				visible_ = false;
			}
		}
		break;
	}
	return false;
}

void Balloon::DrawShape() const
{
	setMaterialRedPlastic();
	shapes::SolidSphere( radius_, 20, 25 );
}

void Balloon::DrawString( float playerX, float playerY, float playerZ, int playerState ) const
{
	if( state_ != Attached || !visible_ ) return;
	if( playerState != 0 && playerState != 1 ) return;

	glBegin( GL_LINE_LOOP );
	glColor4f( 1.0f, 0.2f, 0.2f, 0.0f ); // 線分の色(RGBA)
	glVertex3f( playerX, playerY, playerZ );
	glVertex3f( x_, y_, z_ );
	glEnd();
}

//------------------------------------------------------------------- BalloonSet
BalloonSet *BalloonSet::Create( memsys::Arena &arena, memsys::Arena &poolArena,
                                int capacity, int count, float radius )
{
	if( capacity < 0 || count < 0 || count > capacity ) return nullptr;

	BalloonSet *set = memsys::New<BalloonSet>( arena );
	if( set == nullptr || !set->pool_.Init( poolArena, static_cast<std::size_t>( capacity ) ) ) return nullptr;

	set->radius_ = radius;
	for( int i = 0; i < count; ++i ){
		set->Spawn();
	}
	return set;
}

Balloon *BalloonSet::Spawn()
{
	Balloon *balloon = pool_.Create();
	if( balloon == nullptr ) return nullptr;
	balloon->Init( spawned_++, radius_ );
	return balloon;
}

bool BalloonSet::Remove( Balloon *balloon )
{
	return pool_.Destroy( balloon );
}

int BalloonSet::Update( float playerX, float playerY, float playerZ, float playerDia, int playerState )
{
	int attached = 0;
	pool_.ForEach( [&]( Balloon &balloon ){
		if( balloon.Update( playerX, playerY, playerZ, playerDia, playerState ) ){
			++attached;
		}
	} );
	return attached;
}

void BalloonSet::Draw( float playerX, float playerY, float playerZ, int playerState ) const
{
	pool_.ForEach( [&]( const Balloon &balloon ){
		balloon.Draw();
		balloon.DrawString( playerX, playerY, playerZ, playerState );
	} );
}
