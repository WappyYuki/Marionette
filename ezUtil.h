#ifndef __EZ_UTIL_H__
#define __EZ_UTIL_H__
//##############################################################################
/* ezUtil.h
 *
 * SAMPLE CODE FOR INTERACTIVE CG PROGRAMMING COURSES
 * AT COLLEGE OF IMAGE ARTS AND SCIENCES, RITSUMEIKAN UNIVERSITY
 * COPYRIGHT(C)2008 TOSHIKAZU OHSHIMA, ALL RIGHTS RESERVED
 */
//##############################################################################

#include "glheaders.h"

//----------------------------------------------------------------------- ezFont
/* ezFont_* : GRAPHIC TEXT UTILITY FUNCTIONS
 */
/*--------*/
GLuint ezFont_makeBitmap( void );
/* makes display-list for OpenGL bitmap fonts.
 */
/*--------*/
void ezFont_drawBitmap( float x, float y, char *string );
/* draws bitmap fonts made with makeBitmap.
 * x, y: raster position in window coordinates
 * string: text string to be drawn
 */
//----------------------------------------------------------------------- ezUtil
/*--------*/
void ezUtil_drawGrid( int n, float range );
/* draws grid on XY-plane.
 * n: number of grid lines for X and Y axis
 * range: grid is drawn in area [-range, range]
 */
/*--------*/
void ezUtil_drawAxes( float l );
/* draws XYZ axes with RED, GREEN and BLUE for each axis.
 * l: length of axis to be drawn
 */
/*--------*/
void ezUtil_makeStars( int n, float range );
/* makes stars as OpenGL display list.
 * n: number of stars
 * range: stars are drawn in area [-range, range]
 */
/*--------*/
void ezUtil_drawStars();
/* draws stars made with makeStars
 */
/*--------*/
void ezUtil_transform( float x, float y, float z,
                       float roll, float pitch, float yaw );
/* makes transform matrix - Rz(roll)Rx(pitch)Ry(yaw)T(x,y,z)
 */
/*--------*/
void ezUtil_inverse( float x, float y, float z,
                     float roll, float pitch, float yaw );
/* makes transform matrix - T(-x,-y,-z)Ry(-yaw)Rx(-pitch)Rz(-roll)
 */
/*--------*/
void ezUtil_getRot( const float matrix[],
                           float *roll, float *pitch, float *yaw );
/* extracts rotation, (*roll,*pitch,*yaw), from matrix
 */
/*--------*/
void ezUtil_getPos( const float matrix[], float *x, float *y, float *z );
/* extracts position, (*x,*y,*z), from matrix
 */
//------------------------------------------------------------------------ ezTex
/*--------*/
GLuint ezTex_loadTexture( const char *file, int width, int height, int depth );
/* load texture
 * file: image file
 * width: width of image
 * height: height of image
 * depth: depth of image
 */
void ezTexEnv_setEnvMode( GLenum mode );
void ezTexEnv_setModulate( void ); //GL_MODULATE
void ezTexEnv_setReplace( void );  //GL_REPLACE
void ezTexEnv_setCopy( void );     //GL_DECAL
void ezTexEnv_setDecal( void );    //GL_COPY
void ezTexEnv_setBlend( void );    //GL_BLEND
void ezTex_bind( GLuint tex );
//==============================================================================
#endif
