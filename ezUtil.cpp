//##############################################################################
/* ezUtil.cpp
 * 
 * SAMPLE CODE FOR INTERACTIVE CG PROGRAMMING COURSES
 * AT COLLEGE OF IMAGE ARTS AND SCIENCES, RITSUMEIKAN UNIVERSITY
 * COPYRIGHT(C)2008 TOSHIKAZU OHSHIMA, ALL RIGHTS RESERVED
 */
//##############################################################################

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ezUtil.h"
#include "font.h"

#define EZ_DEGREE (180.0/3.1415926)

static GLuint _base;
static GLuint _starbase;

//-------------------------------------------------------------------- makeStars
/* makeStars:
 */
void ezUtil_makeStars( int n, float range )
{
    _starbase = glGenLists( 1 );
    glNewList( _starbase, GL_COMPILE );
    int i;
    float x, y, z;
    glColor3f( 1.0, 1.0, 1.0 );
    //glPointSize( 2.0 );
    glBegin( GL_POINTS );
    for( i = 0; i < n; i++ ){
        x = range * (float)rand()/(float)RAND_MAX - range / 2.0;
        y = range * (float)rand()/(float)RAND_MAX - range / 2.0;
        z = range * (float)rand()/(float)RAND_MAX - range / 2.0;
        glVertex3f( x, y, z );
    }
    glEnd();
    glEndList();
    return;
}
//-------------------------------------------------------------------- drawStars
/* drawStars: 星を描画する
 */
void ezUtil_drawStars( void )
{
    glPushAttrib( GL_LIST_BIT );
    //glListBase( _starbase );
    glCallList( _starbase );
    glPopAttrib();
    return;
}
//======================================================================= ezFont
//------------------------------------------------------------------- makeBitmap
/* makeFonts: フォントオブジェクトを準備する
 */
GLuint ezFont_makeBitmap( void )
{
    int i, n;

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    _base = glGenLists(0x80);//ディスプレイリスト領域を 128 個獲得
    //各文字のビットマップデータを各ディスプレイリストに割り当てる
    n = 0;
    for( i = 0x20; i < 0x80; i++ ){
        glNewList( _base + i, GL_COMPILE );
        glBitmap( 8, 16, 0.0, 0.0, 9.0, 0.0, font[n++] );
        glEndList();
    }
    return _base;
}
//------------------------------------------------------------------- drawBitmap
/* drawBitmap: ビットマップフォントで文字列を描画する
 */
void ezFont_drawBitmap( float x, float y, char *string )
{
    glPushAttrib( GL_LIST_BIT|GL_CURRENT_BIT );
    glRasterPos2f( x, y );//描画位置の設定
    //文字フォントディスプレイリストの開始位置 */
    glListBase( _base );
    //ディスプレイリスト (文字列) を描画
    glCallLists( strlen( string ), GL_UNSIGNED_BYTE, (unsigned char *)string );
    glPopAttrib();
    return;
}
//--------------------------------------------------------------- drawGlutBitmap
/* drawGlutBitmap: glutビットマップフォントで文字列を描画する
 */
void ezFont_drawGlutBitmap( float x, float y, char *string, void *font )
{
    glPushAttrib( GL_CURRENT_BIT );//現在のラスターポジションの保存
    glRasterPos2f( x, y );
    //ビットマップ文字列の描画
    while( *string ){
        glutBitmapCharacter( font, *string++ );
    }
    glPopAttrib();//保存したラスターポジションのロード
    return;
}
//--------------------------------------------------------------- drawGlutStroke
/* drawGlutStroke
 */
void ezFont_drawGlutStroke( float x, float y, char *string, void *font,
                            float xscl, float yscl )
{
    glPushMatrix();
    {
        glTranslatef( x, y, 0.0 );
        glScalef( 0.01 * xscl, 0.01 * yscl, 1.0 );
        //ストロークフォント文字列の描画
        while( *string ){
            glutStrokeCharacter( font, *string++ );
        }
    }
    glPopMatrix();
    return;
}
//--------------------------------------------------------------------- drawGrid
/* drawGrid: グリッドを描画する
 */
void ezUtil_drawGrid( int n, float range )
{
    int i;
    float x, y;
    float d = range/(float)n;
    float min = -0.5 * range;
    float max =  0.5 * range;

    x = min;
    for( i=0; i<n; i++ ){
        glBegin( GL_LINES );
        glVertex2f( x, min );
        glVertex2f( x, max );
        glEnd();
        x += d;
    }
    y = min;
    for( i=0; i<n; i++ ){
        glBegin( GL_LINES );
        glVertex2f( min, y );
        glVertex2f( max, y );
        glEnd();
        y += d;
    }
    return;
}
//--------------------------------------------------------------------- drawAxes
/* drawAxes: 座標軸を描画する
 */  
void ezUtil_drawAxes( float l )
{
    /* x軸を赤で描画する */
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-l, 0.001, 0.0 );
    glVertex3f( l, 0.001, 0.0 );
    glEnd();
    /* y軸を緑で描画する */
    glColor3f(0.0, 1.0, 0.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, l, 0.0 );
    glVertex3f( 0.0,-l, 0.0 );
    glEnd();
    /* z軸を青で描画する */
    glColor3f(0.0, 0.0, 1.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, 0.001, l );
    glVertex3f( 0.0, 0.001, -l );
    glEnd();
    return;
}
//-------------------------------------------------------------------- transform
/* transform:
 */
void ezUtil_transform( float x, float y, float z,
                       float roll, float pitch, float yaw )
{
    glTranslatef( x, y, z );
    glRotatef( yaw, 0., 1., 0. );
    glRotatef( pitch, 1., 0., 0. );
    glRotatef( roll, 0., 0., 1. );
    return;
}
//---------------------------------------------------------------------- inverse
/* inverse:
 */
void ezUtil_inverse( float x, float y, float z,
                     float roll, float pitch, float yaw )
{
    glRotatef( -roll, 0., 0., 1. );
    glRotatef( -pitch, 1., 0., 0. );
    glRotatef( -yaw, 0., 1., 0. );
    glTranslatef( -x, -y, -z );
    return;
}
//----------------------------------------------------------------------- getRot
/* getRot:
 */
void ezUtil_getRot( const float src[],
                       float *roll, float *pitch, float *yaw )
{
    float x[4], y[4], z[4];
    float buf[4];
    float m[4][4];
    memcpy( (void*)m, (void*)src, sizeof( m ) );
    /*---- GET AXES ----*/
    x[0] = m[0][0]; x[1] = m[0][1]; x[2] = m[0][2]; x[3] = 1.0;
    y[0] = m[1][0]; y[1] = m[1][1]; y[2] = m[1][2]; y[3] = 1.0;
    z[0] = m[2][0]; z[1] = m[2][1]; z[2] = m[2][2]; z[3] = 1.0;
    /*---- PITCH ----*/
    *pitch = EZ_DEGREE * asinf( - z[1] ); // [-90,90]
    /*---- YAW & ROLL ----*/
    if( -90.0 < *pitch && *pitch < 90.0 ){
        *yaw = EZ_DEGREE * atan2f( z[0], z[2] ); // [-180,180]
        buf[0] =   z[2];
        buf[1] =    0.;
        buf[2] = - z[0];
        buf[3] =    1.;
        float dot = x[0]*buf[0] + x[1]*buf[1] + x[2]*buf[2];
        float norm = sqrtf( (x[0]*x[0]+x[1]*x[1]+x[2]*x[2])
                            *(buf[0]*buf[0]+buf[1]*buf[1]+buf[2]*buf[2]) );
        float c = dot / norm;
        if( c  < -1. ) c = -1.;
        if( 1. <  c  ) c =  1.;
        *roll = EZ_DEGREE * acosf( c );
        if( x[1] < 0. ) *roll = - *roll;
    }
    else{
        if( z[1] < 0. ) *yaw = EZ_DEGREE * atan2f(   y[0],   y[2] );
        else            *yaw = EZ_DEGREE * atan2f( - y[0], - y[2] );
        *roll = 0.;
    }
    if( *roll  == -0. ) *roll  = 0.;
    if( *pitch == -0. ) *pitch = 0.;
    if( *yaw   == -0. ) *yaw   = 0.;
    return;
}
//----------------------------------------------------------------------- getPos
/* getPos:
 */
void ezUtil_getPos( const float matrix[], float *x, float *y, float *z )
{
    *x = matrix[12];//[3][0]; x座標値を抽出
    *y = matrix[13];//[3][1]; y座標値を抽出
    *z = matrix[14];//[3][2]; z座標値を抽出
    return;
}
//======================================================================== ezTex
//------------------------------------------------------------------ loadTexture
/* loadTexture:
 */
GLuint ezTex_loadTexture( const char *file, int width, int height, int depth )
{
    GLuint tex;                         //テクスチャハンドル
    GLubyte *texture;                   //テクスチャ用画像データの配列
    texture = (GLubyte *)malloc( width * height * depth );
    FILE *fptex;                        //テクスチャファイル用ファイルポインタ
    fptex = fopen( file, "rb" );        //バイナリ読み込みモードでオープン
    if( fptex == NULL ){                //ファイルが読めなかったらエラー
        perror( file );
        exit( -1 );
    }
    //テクスチャデータを読み込む
    fread( texture, width * height * depth, 1, fptex );
    fclose( fptex );

    glGenTextures( 1, &tex );            //新しいテクスチャを生成し
    glBindTexture( GL_TEXTURE_2D, tex ); //カレントに設定する
    //■■■■画像データをテクスチャメモリに登録する
    //■メモリのテクセル境界の指定
    //■テクスチャメモリに登録
    switch( depth ){
      case 3:
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, texture );
        break;
      case 4:
        glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, texture );
        break;
      default:
        break;
    }
    //■■■■各種テクスチャモード設定
    //■テクスチャフィルター
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    return tex;
}
//------------------------------------------------------------------- setReplace
void ezTexEnv_setReplace( void )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    return;
}
//------------------------------------------------------------------ setModulate
void ezTexEnv_setModulate( void )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    return;
}
//--------------------------------------------------------------------- setDecal
void ezTexEnv_setDecal( void )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    return;
}
//--------------------------------------------------------------------- setCopy
void ezTexEnv_setCopy( void )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COPY );
    return;
}
//--------------------------------------------------------------------- setBlend
void ezTexEnv_setBlend( float *color )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );
    return;
}
//------------------------------------------------------------------------- bind
void ezTex_bind( GLuint tex )
{
    glBindTexture( GL_TEXTURE_2D, tex );
    return;
}
//----------------------------------------------------------------------- enable
void ezTex_enable( void )
{
    glEnable( GL_TEXTURE_2D ); /*■テクスチャ適用開始*/
}
//---------------------------------------------------------------------- disable
void ezTex_disable( void )
{
    glDisable( GL_TEXTURE_2D ); /*■テクスチャ適用開始*/
}
//==============================================================================
