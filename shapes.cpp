/******************************************************************************
 * shapes.cpp: 基本形状の描画 */

#include "shapes.h"

#include <array>

#include "glheaders.h"

namespace {

using Vec3 = std::array<GLfloat, 3>;

// 立方体の6面の法線と、各面の頂点番号（GLUT の glutSolidCube と同じ並び）
constexpr std::array<Vec3, 6> kCubeNormals = { {
    { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },
    {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f, -1.0f },
} };
constexpr std::array<std::array<int, 4>, 6> kCubeFaces = { {
    { 0, 1, 2, 3 }, { 3, 2, 6, 7 }, { 7, 6, 5, 4 },
    { 4, 5, 1, 0 }, { 5, 6, 2, 1 }, { 7, 4, 0, 3 },
} };

void drawCube( double size, GLenum mode )
{
    const GLfloat h = static_cast<GLfloat>( size * 0.5 );

    const std::array<Vec3, 8> v = { {
        { -h, -h, -h }, { -h, -h,  h }, { -h,  h,  h }, { -h,  h, -h },
        {  h, -h, -h }, {  h, -h,  h }, {  h,  h,  h }, {  h,  h, -h },
    } };

    for( std::size_t i = 0; i < kCubeFaces.size(); ++i ){
        glBegin( mode );
        glNormal3fv( kCubeNormals[i].data() );
        for( int index : kCubeFaces[i] ){
            glVertex3fv( v[index].data() );
        }
        glEnd();
    }
}

}

namespace shapes {

void SolidSphere( double radius, int slices, int stacks )
{
    // 描画のたびに作り直さないよう、最初の呼び出しで1つだけ作る
    static GLUquadric *const quadric = [] {
        GLUquadric *q = gluNewQuadric();
        gluQuadricDrawStyle( q, GLU_FILL );
        gluQuadricNormals( q, GLU_SMOOTH );
        return q;
    }();
    gluSphere( quadric, radius, slices, stacks );
}

void SolidCube( double size )
{
    drawCube( size, GL_QUADS );
}

void WireCube( double size )
{
    drawCube( size, GL_LINE_LOOP );
}

}
