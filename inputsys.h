/******************************************************************************
 * inputsys.h: GameInput によるキーボード・マウス入力
 * 毎フレーム Poll() で状態を読み、変化を keybdmouse.cpp のコールバックへ渡す */
#ifndef __INPUTSYS_H__
#define __INPUTSYS_H__

#include <Windows.h>
#include <GameInput.h> // GameInputMouseLeftButton などのボタン定数

//-------- mouseClick の state（GLUT_DOWN / GLUT_UP と同じ値）
enum { MOUSE_DOWN = 0, MOUSE_UP = 1 };

namespace inputsys {

// GameInput を初期化する。hwnd はマウス座標をクライアント座標に直すのに使う
bool Init( HWND hwnd );
void Shutdown();

// 現在の入力を読み、押す・離す・動かすの変化をコールバックへ渡す
void Poll();

// 仮想キー（VK_SHIFT など）が押されているか。VK_SHIFT・VK_CONTROL・VK_MENU は左右どちらでもよい
bool IsKeyDown( int virtualKey );

}

#endif
