/******************************************************************************
 * actor.cpp: 描画物の基底クラス */

#include "actor.h"

#include "glheaders.h"

void Actor::Draw() const
{
	if( !visible_ ) return;

	glPushMatrix();
	{
		glTranslatef( x_, y_, z_ ); //オブジェクト位置
		glRotatef( yaw_, 0.0, 1.0, 0.0 ); //オブジェクト回転：ヨー角
		glRotatef( pitch_, 1.0, 0.0, 0.0 ); //オブジェクト回転：ピッチ角
		glRotatef( roll_, 0.0, 0.0, 1.0 ); //オブジェクト回転：ロール角
		DrawShape();
	}
	glPopMatrix();
}
