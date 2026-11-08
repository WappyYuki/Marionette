#include <stdlib.h>
#include <stdio.h>
#include "glheaders.h"

#include "common.h"

extern SimDataT simdata;
extern WindowDataT window;

/*---------------------------------------------------------------- Viewing
 * Viewing:
 *--------*/
void Viewing( void )
{  
	///// カメラ操作（位置と向き）
	///// ビューイング変換

	///// カメラ固定
	switch(simdata.camera.state)
	{
		case 0:
			gluLookAt( 0.0, 1.0, 2.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0 );
			//カメラの位置情報を使ったビューイング変換の設定
			glRotatef( - simdata.camera0.roll, 0.0, 0.0, 1.0 ); // roll回転の逆変換/Z軸
			glRotatef( - simdata.camera0.pitch, 1.0, 0.0, 0.0 ); // pitch回転の逆変換/X
			glRotatef( - simdata.camera0.yaw, 0.0, 1.0, 0.0 ); // yaw回転の逆変換/Y
			glTranslatef( - simdata.camera0.x, - simdata.camera0.y, - simdata.camera0.z ); 
			// (x,y,z)の平行移動の逆変換
			break;

		case 1:
			gluLookAt( 0.0,  5.0, 8.0,simdata.player.x, simdata.player.y, simdata.player.z,0.0, 1.0, 0.0 );
			break;
	}
}

/*---------------------------------------------------------------- Projection
 * Projection:
 *--------*/
void Projection( void )
{
	//---- begin projection transformation
	//画角の調整
	//クリッピング深さの調整
    gluPerspective( 45.0, window.aspect, 0.1, 100.0 );
}
