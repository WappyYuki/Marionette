#include <stdlib.h>
#include <stdio.h>
#include "glheaders.h"
#include "shapes.h"

#include "ezUtil.h"

#include "common.h"
#include "balloon.h"

extern WindowDataT window;
extern SimDataT simdata;
extern BalloonSet *balloons;

//-------- prototypes for user functions
void setLightMain( void );
void setLightSub( void );
void setMaterialRedPlastic( void );
void setMaterialMetal( void );

/*----------------------------------------------------------------- setLight
 * setLight:
 *--------*/
void setLightMain( void )
{
    float light_color[] = { 1.0, 1.0, 1.0, 1.0 };
    float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[] = { 0.0, 10.0, 0.0, 1.0 };

 	glEnable( GL_LIGHT0 ); 
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color );
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
	glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );

	//glEnable( GL_LIGHT0 );

    return;
}
/*----------------------------------------------------------------- setLight
 * setLight:
 *--------*/
void setLightSub( void )
{
    float light_color[] = { 0.2, 0.2, 0.2, 1.0 };
    float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    float light_pos[] = { 0.0, -10.0, 0.0, 0.0 };

	glEnable( GL_LIGHT1 );  
    glLightfv( GL_LIGHT1, GL_DIFFUSE, light_color );
    //glLightfv( GL_LIGHT1, GL_SPECULAR, light_color );

    glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient );
    glLightfv( GL_LIGHT1, GL_POSITION, light_pos );

	//glEnable( GL_LIGHT1 );

    return;
}

/*-------------------------------------------------------------- setMaterial
 * setMaterial:
 *--------*/
void setMaterialWhite( void )
{
    float diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    float specular[] = { 0.3, 0.3, 0.3, 1.0 };
	float ambient[] = { 0.1, 0.1, 0.1, 1.0 };

    //glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );

    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 32.0 );
	//glMaterialfv( GL_FRONT, GL_AMBIENT, ambient );
    return;
}
/*-------------------------------------------------------------- setMaterial
 * setMaterial:
 *--------*/

void setMaterialMetal( void )
{
    float diffuse[] = { 0.1, 0.2, 0.3, 1.0 };
    float specular[] = { 0.9, 1.0, 1.0, 1.0 };
	float ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 64.0 );
    return;
}



/*-------------------------------------------------------------- setMaterial
 * setMaterial:
 *--------*/


void setMaterialRedPlastic( void )
{
    float diffuse[] = { 0.8, 0.2, 0.2, 1.0 };
    float specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 32.0 );
    return;
}


/*------------------------------------------------------------------ drawObj
 * drawWireCube:
 *--------*/
void drawWireCube( void )
{
    glColor3f( 1.0, 1.0, 1.0 );
    shapes::WireCube( 1.0 );
    return;
}
/*------------------------------------------------------------------ drawObj
 * drawSolidCube:
 *--------*/
void drawSolidCube( void )
{
	glPushMatrix();
	{
		glTranslatef( 0.0, 0.0, 0.0 ); //オブジェクト基準位置調整
		glRotatef( 0.0, 0.0, 1.0, 0.0 ); //オブジェクト基準姿勢調整：ヨー角
		glRotatef( 0.0, 1.0, 0.0, 0.0 ); //オブジェクト基準姿勢調整：ピッチ角
		glRotatef( 0.0, 0.0, 0.0, 1.0 ); //オブジェクト基準姿勢調整：ロール角
		shapes::SolidCube( simdata.player_dia );
	}
	glPopMatrix();
    return;
}
/*---------------------------------------------------------------- DrawScene
 * DrawScene:
 *--------*/
void DrawScene( void )
{
	glDisable( GL_LIGHTING ); //一旦光源オフ
	//XZ平面上のグリッドの描画
	glPushMatrix();
	glRotatef( 90.0, 1.0, 0.0, 0.0 );
	glColor3f( 0.6, 0.6, 0.6 );
	ezUtil_drawGrid( 1000.0, 1000.0 ); //線の本数，描画する領域の広さ
	glPopMatrix();

	//座標軸の描画
	glLineWidth( 2.0 );
	ezUtil_drawAxes( 100.0 );//座標軸の長さ
	glLineWidth( 1.0 );
	glEnable( GL_LIGHTING ); //光源復活

	//-------- background color
    glClearColor( 0.2, 0.3, 0.4, 1.0 );
	
	setLightMain();
	setLightSub();
	

	

	glPushMatrix(); //// playerの位置にオブジェクトを描く
	{
		glTranslatef( simdata.player.x, simdata.player.y, simdata.player.z ); //オブジェクト位置
		glRotatef( simdata.player.yaw, 0.0, 1.0, 0.0 ); //オブジェクト回転：ヨー角
		glRotatef( simdata.player.pitch, 1.0, 0.0, 0.0 ); //オブジェクト回転：ピッチ角
		glRotatef( simdata.player.roll, 0.0, 0.0, 1.0 ); //オブジェクト回転：ロール角
		

		if(simdata.camera.state ==0){
			simdata.player.x = simdata.camera0.x;
			simdata.player.y = simdata.camera0.y;
			simdata.player.z = simdata.camera0.z;
		}




        setMaterialRedPlastic();
		drawSolidCube();
	}
	glPopMatrix();


	
	//風船と糸
	balloons->Draw( simdata.player.x, simdata.player.y, simdata.player.z, simdata.player_state );

	

    return;
}
