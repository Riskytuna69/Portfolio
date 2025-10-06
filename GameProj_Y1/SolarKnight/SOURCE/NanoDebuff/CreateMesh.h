/*!************************************************************************
\file       CreateMesh.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef CREATEMESH_H
#define CREATEMESH_H
#include "pch.h"
namespace CreateMesh
{
	AEGfxVertexList* CreateSquareMesh(f32 vertexPoint, u32 color);
	AEGfxVertexList* CreateSpriteAnimationMesh(f32 vertexPoint, u32 color, u32 row, u32 col);
}
#endif

