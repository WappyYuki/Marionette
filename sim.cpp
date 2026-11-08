#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "balloon.h"
#include <Windows.h>
#include <math.h> //数学関数用ヘッダファイル

SimDataT simdata;
BalloonSet *balloons = nullptr; // memsys の常駐領域に確保した風船
extern int time;



/*-------- 乱数シードを初期化する
*
*/
void initRandom( void )
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

/*---------------------------------------------------------------- InitScene
* InitScene:
*--------*/
void InitScene( void )
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

	simdata.player.x = 0.0;
	simdata.player.y = 0.0;
	simdata.player.z = 0.0;
	simdata.player.roll = 0.0;
	simdata.player.pitch = 0.0;
	simdata.player.yaw = 0.0;
	simdata.player.state = 0;

	simdata.move = 0.0;
	simdata.rot = 0.0;


	simdata.mouse_left =false;
	simdata.mouse_right = false;
	simdata.mouse_middle= false;


	simdata.mouse_x= 0.0;
	simdata.mouse_y = 0.0;


	simdata.ballooncount = 0;
	simdata.player_state= 0;
	simdata.player_dia = 1.0;

	//風船（memsys の常駐領域に確保する）
	balloons = BalloonSet::Create( memsys::Persistent(), NUM_BALLOON, 0.5f );
	if( balloons == nullptr ){
		fprintf( stderr, "風船のメモリを確保できませんでした\n" );
		exit( 1 );
	}



	//////
	return;
}
/*-------------------------------------------------------------- UpdateScene
* UpdateScene:
*--------*/
void UpdateScene( void )
{
	for(int i = 0; i < balloons->Count(); i++){
		printf("y座標%f\n",balloons->Get( i ).Y());
		printf("プレイヤー状態%d\n",simdata.player_state);
		
	}

	switch(simdata.player_state){
	case 0:
		simdata.player.y = 0.0;
		break;
	case 1:
		simdata.player.y += 0.05f;

	
		simdata.camera0.y += 0.05f;
		

		break;

	case 2:
		simdata.player.y -= 0.05f;
		simdata.ballooncount = 0;

		
		simdata.camera0.y -= 0.05f;
		

		
		break;
	}


	if(simdata.ballooncount == 5 && simdata.player.y < 10 ){
		simdata.player_state = 1; // 上昇時の状態遷移
	}

	if (simdata.player.y >= 10){
		simdata.player_state = 2; //下降時の状態遷移
	}
	else if( simdata.player.y < 0){
		simdata.player_state = 0;
	}


	//風船の接触判定と移動（今回くっついた数を数える）
	simdata.ballooncount += balloons->Update( simdata.player.x, simdata.player.y, simdata.player.z,
	                                          simdata.player_dia, simdata.player_state );

	//////// データ更新 ////////
	//printf( "move = %f\n", simdata.move );
	//printf( "rot = %f\n", simdata.rot );


	
	//// 回転方向の更新
	simdata.player.yaw += simdata.rot;
	
	//// 位置の更新
	float dx; //X軸方向の移動量
	float dz; //Z軸方向の移動量

	dx = - simdata.move * sinf( simdata.player.yaw / 180.0f * 3.14f );
	dz = - simdata.move * cosf( simdata.player.yaw / 180.0f * 3.14f );

	simdata.player.x += dx;
	simdata.player.z += dz;
	



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




