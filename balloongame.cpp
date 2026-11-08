/******************************************************************************
 * balloongame.cpp: 風船集めのインゲーム（以前の sim.cpp） */

#include "balloongame.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h> //数学関数用ヘッダファイル

#include "common.h"
#include "balloon.h"

SimDataT simdata; // 入力（keybdmouse.cpp）とカメラ（projview.cpp）が読み書きする共有データ

/*-------- 乱数シードを初期化する
*
*/
static void initRandom( void )
{
	srand( (unsigned int)GetTickCount64() );
}

/*-------- 乱数生成関数
* 最小値rminと最大値rmaxの間の値をランダムに返す
*/
float Random( float rmin, float rmax ){
	double r;
	r = (double)rand()/(double)RAND_MAX;
	r = r * ( rmax - rmin ) + rmin;
	return static_cast<float>(r);
}

/*---------------------------------------------------------------- OnStart
* OnStart: シーンデータの初期化（以前の InitScene）
*--------*/
bool BalloonGame::OnStart()
{
	////// シーンデータの初期化
	initRandom();

	//カメラの初期化
	simdata.camera0.x = 0.0;
	simdata.camera0.y = 0.0;
	simdata.camera0.z = 0.0;
	simdata.camera0.roll = 0.0;
	simdata.camera0.pitch = 0.0;
	simdata.camera0.yaw = 0.0;
	simdata.camera0.state = 0;


	simdata.camera1.x = 0.0;
	simdata.camera1.y = 0.0;
	simdata.camera1.z = 0.0;
	simdata.camera1.roll = 0.0;
	simdata.camera1.pitch = 0.0;
	simdata.camera1.yaw = 0.0;
	simdata.camera1.state = 0;

	simdata.move = 0.0;
	simdata.rot = 0.0;


	simdata.mouse_left =false;
	simdata.mouse_right = false;
	simdata.mouse_middle= false;


	simdata.mouse_x= 0.0;
	simdata.mouse_y = 0.0;


	//プレイヤー（memsys のプールヒープに1人分の枠を確保して置く）
	if( !playerPool_.Init( memsys::PoolHeap(), 1 ) || ( player_ = playerPool_.Create() ) == nullptr ){
		fprintf( stderr, "プレイヤーのメモリを確保できませんでした\n" );
		return false;
	}
	player_->Init( 1.0f );

	//風船（集合は常駐領域、NUM_BALLOON 個分の枠はプールヒープに確保し、全て置く）
	balloons_ = BalloonSet::Create( memsys::Persistent(), memsys::PoolHeap(), NUM_BALLOON, NUM_BALLOON, 0.5f );
	if( balloons_ == nullptr ){
		fprintf( stderr, "風船のメモリを確保できませんでした\n" );
		return false;
	}

	//////
	return true;
}
/*-------------------------------------------------------------- OnFinish
* OnFinish: 終了時にシーンのオブジェクトを片付ける（以前の ShutdownScene）
*--------*/
void BalloonGame::OnFinish()
{
	if( balloons_ != nullptr ){
		balloons_->RemoveAll();
		balloons_ = nullptr; // BalloonSet 自体の領域は memsys::Shutdown() でまとめて返す
	}
	playerPool_.DestroyAll();
	player_ = nullptr;
}
/*-------------------------------------------------------------- OnUpdate
* OnUpdate: 1フレーム分の進行（以前の UpdateScene）
*--------*/
void BalloonGame::OnUpdate()
{
	balloons_->ForEach( [this]( const Balloon &balloon ){
		//printf("y座標%f\n",balloon.Y());
		//printf("プレイヤー状態%d\n",player_->State());
	} );

	//プレイヤーの上昇・下降（主観カメラも同じだけ動かす）
	simdata.camera0.y += player_->UpdateLift();

	//風船の接触判定と移動（今回くっついた数を数える）
	player_->AddBalloons( balloons_->Update( player_->X(), player_->Y(), player_->Z(),
	                                         player_->Dia(), player_->State() ) );

	//プレイヤーの回転と移動
	player_->Move( simdata.move, simdata.rot );


	if(simdata.camera.state ==0){

	//回転方向の更新
	simdata.camera0.yaw += simdata.rot;

	//位置の更新
	float dx2;  //X軸方向の移動量
	float dz2;  //Z軸方向の移動量

	dx2 = -simdata.move * sinf( simdata.camera0.yaw /180.0f *3.14f);
	dz2 = - simdata.move * cosf(simdata.camera0.yaw / 180.0f * 3.14f);


	simdata.camera0.x += dx2;
	simdata.camera0.z += dz2;
	}

	////////
	return;
}
/*-------------------------------------------------------------- OnDraw
* OnDraw: プレイヤーと風船を描く（以前は DrawScene の中にあった）
*--------*/
void BalloonGame::OnDraw()
{
	//// playerの位置にオブジェクトを描く
	player_->Draw();

	// 主観視点の間はプレイヤーをカメラの位置に合わせる
	// （以前と同じく、立方体を描いた後・風船を描く前に合わせる）
	if(simdata.camera.state ==0){
		player_->SetPosition( simdata.camera0.x, simdata.camera0.y, simdata.camera0.z );
	}

	//風船と糸
	balloons_->Draw( player_->X(), player_->Y(), player_->Z(), player_->State() );
}
/*-------------------------------------------------------------- GetViewTarget
* GetViewTarget: 客観視点のカメラが見る点（プレイヤーの位置）
*--------*/
void BalloonGame::GetViewTarget( float &x, float &y, float &z ) const
{
	if( player_ == nullptr ){
		InGamePlay::GetViewTarget( x, y, z );
		return;
	}
	x = player_->X();
	y = player_->Y();
	z = player_->Z();
}
