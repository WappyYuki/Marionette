/******************************************************************************
 * interaction.cpp: keyboard and mouse callback functions 
 */
#include <stdlib.h>
#include <stdio.h>
#include "inputsys.h"

#include "common.h"

extern SimDataT simdata;
extern WindowDataT window;

//-------- modifier key status
bool isShiftKeyDown( void );
bool isCtrlKeyDown( void );
bool isAltKeyDown( void );

/*-------------------------------------------------- modifier key status
 * Shift, Ctrl, Alt key status
 */
bool isShiftKeyDown( void )
{ return inputsys::IsKeyDown( VK_SHIFT ); }
bool isCtrlKeyDown( void )
{ return inputsys::IsKeyDown( VK_CONTROL ); }
bool isAltKeyDown( void )
{ return inputsys::IsKeyDown( VK_MENU ); }

/*------------------------------------------------------------- charKeyDown/Up
 * charKeyDown/Up - keyboard callback function (inputsys.cpp から GameInput の入力で呼ぶ)
 * key: ASCII code
 * x, y: mouse position
 *--------*/
void charKeyDown( unsigned char key, int x, int y )
{
    switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        break;
      case 'q': // quit
        exit(0);
        break;

	  case 'c': // camera change
		 switch(simdata.camera.state){
		 case 0:

			  simdata.camera.state = 1;
		  break;
		 case 1:
			  simdata.camera.state = 0;
			  break;
		  }
		  break;

      default:
        break;
    }
    return;
}
void charKeyUp( unsigned char key, int x, int y )
{
    switch( key ){
	  default:
		break;
	}
}
/*--------------------------------------------------------------- funcKeyDown/Up
 * funcKeyDown/Up - special key callback (inputsys.cpp から GameInput の入力で呼ぶ)
 * key: Windows virtual-key code (VK_*)
 * x, y: mouse position
 */
void funcKeyDown( int key, int x, int y )
{
	//ファンクションキー
	//[F*]: VK_F*(*は1～12)
	//[F1]: VK_F1
	//[F12]:VK_F12
	//その他の機能キー
    //[PageUp]  : VK_PRIOR:
    //[PageDown]: VK_NEXT:
    //[Home]    : VK_HOME:
    //[End]     : VK_END:
    //[Insert]  : VK_INSERT:

	//カーソルキー
	switch( key ){
	  case VK_LEFT: //[←]
		  simdata.rot = 2.5;
		break;
  	  case VK_RIGHT://[→]
		  simdata.rot -=  2.5;
		break;
	  case VK_UP: //[↑]
		  simdata.move = 0.1;
		break;
	  case VK_DOWN://[↓]
		  simdata.move -=  0.1;
		break;
	}
}
void funcKeyUp( int key, int x, int y )
{
	switch( key ){
	  case VK_LEFT: //[←]
		  simdata.rot = 0.0;
		break;
  	  case VK_RIGHT://[→]
		  simdata.rot = 0.0;
		break;
	  case VK_UP: //[↑]
		  simdata.move = 0.0;
		break;
	  case VK_DOWN://[↓]
		  simdata.move = 0.0;
		break;
	}
}
/*------------------------------------------------------------- mouseClick
 * mouseClick - mouse callback function (inputsys.cpp から GameInput の入力で呼ぶ)
 * button: mouse button (GameInputMouse*Button)
 * state：press or release
 * x, y：mouse position
 *--------*/
void mouseClick( int button , int state, int x, int y )
{


	simdata.mouse_x = (float)x *2.0 / window.width -1.0;
	simdata.mouse_y = -(float)y *2.0 / window.height +1.0;
	printf(" mouse x,y:%f,%f\n",simdata.mouse_x,simdata.mouse_y);

		
    switch( button ){
      case GameInputMouseLeftButton:
        switch( state ){
          case MOUSE_DOWN: // press
			  simdata.mouse_left = true;  // <-押す
			  simdata.move = simdata.mouse_y *0.2;  //ボタンを押した瞬間の速度設定
			  simdata.rot = - simdata.mouse_x * 2.0;
            break;
          case MOUSE_UP: // release
			  simdata.mouse_left =false; // <-離す
			  simdata.move = 0.0;  //ボタンを押した瞬間の速度設定(停止）
			    simdata.rot = 0.0;
            break;
          default:
            break;
        }
        break;
      case GameInputMouseMiddleButton:
        switch( state ){
          case MOUSE_DOWN: // press
			  simdata.mouse_middle = true; //<-押す

            break;
          case MOUSE_UP: // release
			  simdata.mouse_middle = false; //<-離す
            break;
          default:
            break;
        }
        break;
      case GameInputMouseRightButton:
        switch( state ){
          case MOUSE_DOWN: // press
			  simdata.mouse_right =true;//<-押す

            break;
          case MOUSE_UP: // release

			  simdata.mouse_right =false;//<-離す
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return;
}
/*---------------------------------------------------------------- mouseDrag 
 * mouseDrag - mouse drag callback function
 */
void mouseDrag( int x, int y )
{
	////////
	if(simdata.mouse_left){

	printf("MOUSE DRAG:x, y = %d, %d\n",x , y);
	simdata.mouse_x = (float)x *2.0 / window.width -1.0;
	simdata.mouse_y = -(float)y *2.0 / window.height +1.0;
	printf(" mouse x,y:%f,%f\n",simdata.mouse_x,simdata.mouse_y);
	simdata.move = simdata.mouse_y * 0.2;  //押している間の速度設定
	simdata.rot = - simdata.mouse_x * 2.0;
	}


	////////
    return;
}
/*-------------------------------------------------------------- mouseMotion
 * mouseMotion - passive mouse motion callback function
 */
void mouseMotion( int x, int y )
{
	////////
	printf("MOUSE MOTION:x, y = %d, %d\n",x , y);

	////////
    return;
}
