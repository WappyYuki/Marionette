#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <GL/GLUT.h>
#include <math.h> //数学関数用ヘッダファイル

SimDataT simdata;
BalloonDataT balloondata;
extern int time;



/*-------- 乱数シードを初期化する
*
*/
void initRandom( void )
{
	srand( (unsigned int)glutGet( GLUT_ELAPSED_TIME ) );
}

/*-------- 乱数生成関数
* 最小値rminと最大値rmaxの間の値をランダムに返す
*/
float Random( float rmin, float rmax ){
	double r;
	r = (double)rand()/(double)RAND_MAX;
	r = r * ( rmax - rmin ) + rmin;
	return r;
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


	simdata.playerdist = 1.0;
	simdata.ballooncount = 0;
	simdata.player_state= 0;
	simdata.player_dia = 1.0;

	balloondata.rad = 0.5;
	



	for(int i = 0; i < NUM_BALLOON; i++){
		//balloon
		simdata.balloon[i].state = 0;
		simdata.balloon[i].x = Random(-30.0, 30.0);
		simdata.balloon[i].y = 0;
		simdata.balloon[i].z = Random(-30.0, 30.0);
		simdata.balloon[i].roll = i*0.5;
		simdata.balloon[i].pitch = i*0.5;
		simdata.balloon[i].yaw = i*0.5;
		balloondata.visuable[i] = true;

		//getBalloon
		simdata.getballoon[i][0] = 0.0;
		simdata.getballoon[i][1] = 0.0;
		simdata.getballoon[i][2] = 0.0;
	}



	//////
	return;
}
/*-------------------------------------------------------------- UpdateScene
* UpdateScene:
*--------*/
void UpdateScene( void )
{
	for(int i = 0; i < NUM_BALLOON; i++){
		printf("y座標%f\n",simdata.balloon[i].y);
		printf("プレイヤー状態%d\n",simdata.player_state);
		
	}

	switch(simdata.player_state){
	case 0:
		simdata.player.y = 0.0;
		break;
	case 1:
		simdata.player.y += 0.05;

	
		simdata.camera0.y += 0.05;
		

		break;

	case 2:
		simdata.player.y -= 0.05;
		simdata.ballooncount = 0;

		
		simdata.camera0.y -= 0.05;
		

		
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


	float dxx;  //球と物体のx座標間距離
	float dyy;  //球と物体のy座標間距離
	float dzz;  //球と物体のz座標間距離

	for(int i = 0; i< NUM_BALLOON; i++){
		if (simdata.dist_state[i] ==0){
			//プレイヤと風船の距離を求める
			dxx = powf (simdata.player.x - simdata.balloon[i].x ,2);  //中心間のx座標の差
			dyy = powf (simdata.player.y - simdata.balloon[i].y ,2);  //中心間のy座標の差　
			dzz	= powf (simdata.player.z - simdata.balloon[i].z ,2);  //中心間のz座標の差
			simdata.playerdist = sqrtf (dxx + dyy + dzz);

			//衝突判定
			if (simdata.playerdist < balloondata.rad + simdata.player_dia /2 ){
				simdata.ballooncount++;
				//風船の移動距離を求める
				simdata.getballoon[i][0] = Random(-3.0, 3.0);
				simdata.getballoon[i][1] = Random(0.6, 3.0);
				simdata.getballoon[i][2] = Random(-3.0, 3.0);

				//風船の状態を更新する
				if(simdata.dist_state[i] == 0){
					simdata.dist_state[i] = 1;		
				}
			}
		}
		else{
			switch (simdata.dist_state[i]){

			case 0:
				break;
			case 1:  //風船の衝突
				if(simdata.player_state <2){
					simdata.balloon[i].x = simdata.player.x + simdata.getballoon[i][0];
					simdata.balloon[i].y = simdata.player.y + simdata.getballoon[i][1];
					simdata.balloon[i].z = simdata.player.z + simdata.getballoon[i][2];
				}
				else if(simdata.player_state == 2){
					printf("空に飛ぶ～～～\n");
					simdata.balloon[i].y += 0.01; 
					if(simdata.player.y <= 0.2){
						balloondata.visuable[i] = false;
					}
					
				
				}
				break;
			}

		}
	}

	//////// データ更新 ////////
	//printf( "move = %f\n", simdata.move );
	//printf( "rot = %f\n", simdata.rot );


	
	//// 回転方向の更新
	simdata.player.yaw += simdata.rot;
	
	//// 位置の更新
	float dx; //X軸方向の移動量
	float dz; //Z軸方向の移動量

	dx = - simdata.move * sinf( simdata.player.yaw / 180.0 * 3.14 );
	dz = - simdata.move * cosf( simdata.player.yaw / 180.0 * 3.14 );

	simdata.player.x += dx;
	simdata.player.z += dz;
	



	if(simdata.camera.state ==0){

	//回転方向の更新
	simdata.camera0.yaw += simdata.rot;

	//位置の更新
	float dx2;  //X軸方向の移動量
	float dz2;  //Z軸方向の移動量

	dx2 = -simdata.move * sinf( simdata.camera0.yaw /180.0 *3.14);
	dz2 = - simdata.move * cosf(simdata.camera0.yaw / 180.0 * 3.14);


	simdata.camera0.x += dx2;
	simdata.camera0.z += dz2;
	}




	////////
	return;
}




