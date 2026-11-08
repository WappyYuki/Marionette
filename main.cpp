/******************************************************************************
 * glsample_frame.cpp 
 * Level 2 - introducing structure */

#include <stdlib.h>
#include <stdio.h>
#include <GL/GLUT.h>

 #include <Windows.h> //Sleep

#include "common.h"

//-------- callback functions
void display( void );
void update( void );
void reshape( int width, int height );

//-------- mouse callback functions
void mouseClick( int button , int state, int x, int y );
void mouseDrag( int x, int y );
void mouseMotion( int x, int y );

//-------- keyboard callback functions
void charKeyDown( unsigned char key, int x, int y );
void charKeyUp( unsigned char key, int x, int y );
void funcKeyDown( int key, int x, int y );
void funcKeyUp( int key, int x, int y );

void initWindow( char *title );

WindowDataT window;

int time;

/*------------------------------------------------------------------- update
 * update - GLUT idle callback function
 *--------*/
void update( void )
{
	time = glutGet( GLUT_ELAPSED_TIME );

    UpdateScene();

	//-------- ready to redraw
	glutPostRedisplay();

	//-------- sync
	Sleep( 0 ); // NVIDIA Quadro
	//Sleep( 16 ) // for cheap video chips @ ~60Hz

	return;
}

/*------------------------------------------------------------------- display
 * display - GLUT display callback function
 *--------*/
void display( void )
{
	glEnable( GL_NORMALIZE ); //// 20161109追加

    //-------- viewport --------
	glViewport( 0, 0, window.width, window.height );

    //-------- clear buffers --------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//-------- projection transformation --------
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	Projection();

	//-------- viewing transformation --------
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    Viewing();

    //-------- draw --------
	glEnable( GL_DEPTH_TEST ); // ---- begin: 
    glEnable( GL_LIGHTING );
	glPushMatrix();
    DrawScene();
	glPopMatrix();
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

	//-------- swapbuffers --------
    glutSwapBuffers();

    return;
}
/*--------------------------------------------------------------------- reshape
 * reshape - GLUT reshape callback function
 *--------*/
void reshape( int width, int height )
{
	window.width = width;
	window.height = height;
	window.aspect = (float)window.width/window.height;
}
/*---------------------------------------------------------------- initWindow
 * initWindow: initializes OpenGL window
 *--------*/
void initWindow( char *winname )
{
	//////// ユーザカスタマイズ
	window.xo = 100;
	window.yo = 100;
	window.width = 800;
	window.height = 600;
	////////////////

	window.aspect = (float)window.width/window.height; // aspect ratio
	window.title = winname;

	//-------- window properties
	glutInitWindowPosition( window.xo, window.yo );
    glutInitWindowSize( window.width, window.height );

    //-------- config buffers
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	//-------- open window
	glutCreateWindow( window.title );

    return;
}
/*---------------------------------------------------------------------- main
 * main
 *--------*/
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );

    initWindow( argv[0] );

	//-------- basic callbacks
    glutDisplayFunc( display );  // display callback function
    glutIdleFunc( update );    // idle callback function
	glutReshapeFunc( reshape ); // reshape callback function

	//-------- keyboard callbacks
    glutKeyboardFunc( charKeyDown ); // character key down callback
	glutKeyboardUpFunc( charKeyUp ); // character key up callback
	glutSpecialFunc( funcKeyDown ); // function key down callback
	glutSpecialUpFunc( funcKeyUp ); // function key up callback
	glutIgnoreKeyRepeat( 1 );   // disable key-repeat

	//-------- mouse callbacks
    glutMouseFunc( mouseClick );       // mouse click callback
	glutPassiveMotionFunc( mouseMotion ); // passive motion callback
	glutMotionFunc( mouseDrag ); // mouse drag callback	
	
    printf( "[H]:Help\n" );     // indicate help instruction

    InitScene();

	time = glutGet( GLUT_ELAPSED_TIME );

    glutMainLoop(); // run main loop

    return 0;
}
/******************************************************************************
 * end of source code */
