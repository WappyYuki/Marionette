/******************************************************************************
 * shapes.h: 基本形状の描画
 * GLUT の glutSolidSphere / glutSolidCube / glutWireCube の代わり */
#ifndef __SHAPES_H__
#define __SHAPES_H__

namespace shapes {

// 原点中心・Z軸を極とする球（法線付き）
void SolidSphere( double radius, int slices, int stacks );

// 原点中心・一辺 size の立方体（法線付き）
void SolidCube( double size );

// 原点中心・一辺 size の立方体の辺
void WireCube( double size );

}

#endif
