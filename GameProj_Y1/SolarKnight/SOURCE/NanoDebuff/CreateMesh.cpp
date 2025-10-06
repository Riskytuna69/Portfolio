/*!************************************************************************
\file       CreateMesh.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"
#include "CreateMesh.h"

namespace CreateMesh
{
	AEGfxVertexList* CreateSquareMesh(f32 vertexPoint, u32 color)
	{
		// inform system to start creating mesh
		AEGfxMeshStart();

		// AEGfxTriAdd() takes in 3 sets of 5 parameters to form a triangle
		// making 2 triangles makes up a white square.
		//f32 vertexPoint = 0.5f;
		AEGfxTriAdd(
			-vertexPoint, -vertexPoint, color, 0.0f, 1.0f,
			vertexPoint, -vertexPoint, color, 1.0f, 1.0f,
			-vertexPoint, vertexPoint, color, 0.0f, 0.0f);

		AEGfxTriAdd(
			vertexPoint, -vertexPoint, color, 1.0f, 1.0f,
			vertexPoint, vertexPoint, color, 1.0f, 0.0f,
			-vertexPoint, vertexPoint, color, 0.0f, 0.0f);

		// return the saved mesh (list of triangles) back.
		return AEGfxMeshEnd();
	}

	AEGfxVertexList* CreateSpriteAnimationMesh(f32 vertexPoint, u32 color, u32 row, u32 col)
	{
		f32 spriteUVWidth = 1.f / static_cast<f32>(row);
		f32 spriteUVHeight = 1.f / static_cast<f32>(col);
		// inform system to start creating mesh
		AEGfxMeshStart();

		// AEGfxTriAdd() takes in 3 sets of 5 parameters to form a triangle
		// making 2 triangles makes up a white square.
		//f32 vertexPoint = 0.5f;
		AEGfxTriAdd(
			-vertexPoint, -vertexPoint, color, 0.0f, spriteUVHeight,
			vertexPoint, -vertexPoint, color, spriteUVWidth, spriteUVHeight,
			-vertexPoint, vertexPoint, color, 0.0f, 0.0f);

		AEGfxTriAdd(
			vertexPoint, -vertexPoint, color, spriteUVWidth, spriteUVHeight,
			vertexPoint, vertexPoint, color, spriteUVWidth, 0.0f,
			-vertexPoint, vertexPoint, color, 0.0f, 0.0f);

		// return the saved mesh (list of triangles) back.
		return AEGfxMeshEnd();
	}

}