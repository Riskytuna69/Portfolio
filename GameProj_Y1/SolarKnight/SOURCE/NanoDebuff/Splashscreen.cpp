/*!************************************************************************
\file       Splashscreen.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Splashscreen.h"
#include "pch.h"
#include "CreateMesh.h"
#include "GameStateManager.h"

namespace Splashscreen {
	
	// Texture
	AEGfxTexture* pTexDigipenLogo;
	AEGfxTexture* pTexNanoDebuffLogo;

	// Mesh
	AEGfxVertexList* pMesh;

	// Variables
	bool bDigipenScreenTime;
	f32 digipenScreenTime;
	bool bNanoDebuffScreenTime;
	f32 nanoDebuffScreenTime;
	const f32 maxScreenTime = 3.0f;
	bool bIncreaseTransparency;
	f32 transparency;
} // namespace Splashscreen

namespace Splashscreen {
	void Init()
	{
		// Load Assets
		pTexDigipenLogo = AEGfxTextureLoad("Assets/DigiPen_Singapore_WEB_WHITE.png");
		pTexNanoDebuffLogo = AEGfxTextureLoad("Assets/nanodebuff_logo.png");

		// Create Mesh
		f32 vertexPoint = 0.5f;
		u32 colorHex = 0xFFFFFFFF;
		pMesh = CreateMesh::CreateSquareMesh(vertexPoint, colorHex);

		// Variables
		bDigipenScreenTime = true;
		digipenScreenTime = maxScreenTime;
		bNanoDebuffScreenTime = false;
		nanoDebuffScreenTime = maxScreenTime;
		bIncreaseTransparency = true;
		transparency = 0.0f;
	}

	void Set()
	{

	}

	void Update()
	{
		if (AEInputCheckTriggered(AEVK_SPACE))
		{
			if (bDigipenScreenTime)
			{
				bDigipenScreenTime = !bDigipenScreenTime;
				bNanoDebuffScreenTime = !bNanoDebuffScreenTime;
			}
			else if (bNanoDebuffScreenTime)
			{
				bNanoDebuffScreenTime = !bNanoDebuffScreenTime;
			}
			transparency = 0.0f;
			bIncreaseTransparency = true;
		}
		
		if (bDigipenScreenTime)
		{
			digipenScreenTime -= static_cast<f32>(AEFrameRateControllerGetFrameTime());
			if (bIncreaseTransparency)
			{
				transparency += static_cast<f32>(AEFrameRateControllerGetFrameTime());
				if (transparency >= 1.0f)
					transparency = 1.0f;
			}
			else
			{
				transparency -= 2.0f * static_cast<f32>(AEFrameRateControllerGetFrameTime());
				if (transparency <= 0.0f)
					transparency = 0.0f;
			}
			
			if (digipenScreenTime <= maxScreenTime * 0.5f)
			{
				bIncreaseTransparency = !bIncreaseTransparency;
			}
			if (digipenScreenTime <= 0.0f)
			{
				bDigipenScreenTime = !bDigipenScreenTime;
				bNanoDebuffScreenTime = !bNanoDebuffScreenTime;
				transparency = 0.0f;
				bIncreaseTransparency = true;
			}
		}
		else if (bNanoDebuffScreenTime)
		{
			nanoDebuffScreenTime -= static_cast<f32>(AEFrameRateControllerGetFrameTime());
			if (bIncreaseTransparency)
			{
				transparency += static_cast<f32>(AEFrameRateControllerGetFrameTime());
				if (transparency >= 1.0f)
					transparency = 1.0f;
			}
			else
			{
				transparency -= 2.0f * static_cast<f32>(AEFrameRateControllerGetFrameTime());
				if (transparency <= 0.0f)
					transparency = 0.0f;
			}

			if (nanoDebuffScreenTime <= maxScreenTime * 0.5f)
			{
				bIncreaseTransparency = !bIncreaseTransparency;
			}
			if (nanoDebuffScreenTime <= 0.0f)
			{
				bNanoDebuffScreenTime = !bNanoDebuffScreenTime;
				transparency = 0.0f;
				bIncreaseTransparency = true;
			}
		}
		else if (!bDigipenScreenTime && !bNanoDebuffScreenTime)
		{
			GSM::stateNext = GS_MAINMENU;
		}
	}

	void Render()
	{
		AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(transparency);
		
		AEMtx33 scale = { 0 };
		AEMtx33 translate = { 0 };
		AEMtx33 transform = { 0 };
		if (bDigipenScreenTime)
		{
			AEMtx33Scale(&scale, 960.0f, 540.0f);
			AEMtx33Trans(&translate, 0, 0);		
			AEMtx33Concat(&transform, &translate, &scale);
			AEGfxTextureSet(pTexDigipenLogo, 0, 0);
		}
		else if (bNanoDebuffScreenTime)
		{
			AEMtx33Scale(&scale, 640.0f, 344.0f);
			AEMtx33Trans(&translate, 0, 0);
			AEMtx33Concat(&transform, &translate, &scale);
			AEGfxTextureSet(pTexNanoDebuffLogo, 0, 0);
		}

		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
	}

	void Unset()
	{

	}

	void Exit()
	{
		// free
		AEGfxMeshFree(pMesh);
		AEGfxTextureUnload(pTexDigipenLogo);
		AEGfxTextureUnload(pTexNanoDebuffLogo);
	}
} // namespace Splashscreen