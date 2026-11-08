/******************************************************************************
 * interaction.cpp: keyboard and mouse callback functions 
 */
#include <stdlib.h>
#include <stdio.h>
#include <GL/GLUT.h>

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
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_SHIFT); }
bool isCtrlKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_CTRL); }
bool isAltKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_ALT); }

/*------------------------------------------------------------- charKeyDown/Up
 * charKeyDown/Up - GLUT keyboard callback function
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
 * funcKeyDown/Up - GLUT special key callback
 * key: GLUT key code
 * x, y: mouse position
 */
void funcKeyDown( int key, int x, int y )
{
	//ファンクションキー
	//[F*]: GLUT_KEY_F*(*は1～12)
	//[F1]: GLUT_KEY_F1
	//[F12]:GLUT_KEY_F12
	//その他の機能キー
    //[PageUp]  : GLUT_KEY_PAGE_UP:
    //[PageDown]: GLUT_KEY_PAGE_DOWN:
    //[Home]    : GLUT_KEY_HOME:
    //[End]     : GLUT_KEY_END:
    //[Insert]  : GLUT_KEY_INSERT:

	//カーソルキー
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]
		  simdata.rot = 2.5;
		break;
  	  case GLUT_KEY_RIGHT://[→]
		  simdata.rot -=  2.5;
		break;
	  case GLUT_KEY_UP: //[↑]
		  simdata.move = 0.1;
		break;
	  case GLUT_KEY_DOWN://[↓]
		  simdata.move -=  0.1;
		break;
	}
}
void funcKeyUp( int key, int x, int y )
{
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]
		  simdata.rot = 0.0;
		break;
  	  case GLUT_KEY_RIGHT://[→]
		  simdata.rot = 0.0;
		break;
	  case GLUT_KEY_UP: //[↑]
		  simdata.move = 0.0;
		break;
	  case GLUT_KEY_DOWN://[↓]
		  simdata.move = 0.0;
		break;
	}
}
/*------------------------------------------------------------- mouseClick
 * mouseClick - GLUT mouse callback function
 * button: mouse button
 * state：press or release
 * x, y：mouse position
 *--------*/
void mouseClick( int button , int state, int x, int y )
{


	simdata.mouse_x = (float)x *2.0 / window.width -1.0;
	simdata.mouse_y = -(float)y *2.0 / window.height +1.0;
	printf(" mouse x,y:%f,%f\n",simdata.mouse_x,simdata.mouse_y);

		
    switch( button ){
      case GLUT_LEFT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
			  simdata.mouse_left = true;  // <-押す
			  simdata.move = simdata.mouse_y *0.2;  //ボタンを押した瞬間の速度設定
			  simdata.rot = - simdata.mouse_x * 2.0;
            break;
          case GLUT_UP: // release
			  simdata.mouse_left =false; // <-離す
			  simdata.move = 0.0;  //ボタンを押した瞬間の速度設定(停止）
			    simdata.rot = 0.0;
            break;
          default:
            break;
        }
        break;
      case GLUT_MIDDLE_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
			  simdata.mouse_middle = true; //<-押す

            break;
          case GLUT_UP: // release
			  simdata.mouse_middle = false; //<-離す
            break;
          default:
            break;
        }
        break;
      case GLUT_RIGHT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
			  simdata.mouse_right =true;//<-押す

            break;
          case GLUT_UP: // release

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
