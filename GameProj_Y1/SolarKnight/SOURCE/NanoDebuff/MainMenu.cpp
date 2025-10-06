/*!************************************************************************
\file       MainMenu.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "MainMenu.h"
#include "pch.h"
#include "CreateMesh.h"
#include "GameStateManager.h"
#include "Button.h"

namespace MainMenu {
	// const variable
	// title
	const f32 GAME_TITLE_SIZE_X = 600.0f;
	const f32 GAME_TITLE_SIZE_Y = 300.0f;
	const f32 GAME_TITLE_POS_X = 0.0f;
	const f32 GAME_TITLE_POS_Y = 250.0f;
	// button
	const f32 BUTTON_SIZE_X = 300.0f;
	const f32 BUTTON_SIZE_Y = 150.0f;
	// camera
	const f32 CAMERA_POS_X = 0.0f;
	const f32 CAMERA_POS_Y = 0.0f;


	// Texture
	AEGfxTexture* pTexStart;
	AEGfxTexture* pTexExit;
	AEGfxTexture* pTexCredit;
	AEGfxTexture* pTexTitle;
	AEGfxTexture* pTexBackground;

	// Mesh
	AEGfxVertexList* pMesh;

	// Variables
	f32 windowWidth;
	f32 windowHeight;
	f32 halfWindowWidth;
	f32 halfWindowHeight;
	s32 cursorScreenPosX;
	s32 cursorScreenPosY;

	// Buttons
	Button::Button btnStart;
	Button::Button btnExit;
	Button::Button btnCredit;

} // namespace MainMenu

namespace MainMenu {
	void Init()
	{	
		// Load Assets
		pTexStart = AEGfxTextureLoad("Assets/start_button.png");
		pTexExit = AEGfxTextureLoad("Assets/quit_button.png");
		pTexCredit = AEGfxTextureLoad("Assets/credits_button.png");
		pTexTitle = AEGfxTextureLoad("Assets/game_title.png");
		pTexBackground = AEGfxTextureLoad("Assets/main_background.png");

		// Create Mesh
		f32 vertexPoint = 0.5f;
		u32 colorHex = 0xFFFFFFFF;
		pMesh = CreateMesh::CreateSquareMesh(vertexPoint, colorHex);

		// Variables
		windowWidth = static_cast<f32>(AEGfxGetWindowWidth());
		windowHeight = static_cast<f32>(AEGfxGetWindowHeight());
		halfWindowWidth = windowWidth * 0.5f;
		halfWindowHeight = windowHeight * 0.5f;
		cursorScreenPosX = 0;
		cursorScreenPosY = 0;

		//Buttons
		btnStart.pos = AEVec2{ 0.0f, -300.0f };
		btnStart.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		btnExit.pos = AEVec2{ 350.0f, -300.0f };
		btnExit.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		btnCredit.pos = AEVec2{ -350.0f, -300.0f };
		btnCredit.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		// set camera position to (0,0)
		AEGfxSetCamPosition(CAMERA_POS_X, CAMERA_POS_Y);

	}

	void Set()
	{

	}

	void Update()
	{
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			// Cursor Position
			AEInputGetCursorPosition(&cursorScreenPosX, &cursorScreenPosY);
			cursorScreenPosX -= static_cast<s32>(halfWindowWidth);
			cursorScreenPosY += static_cast<s32>(halfWindowHeight) - static_cast<s32>(windowHeight);
			cursorScreenPosY = -cursorScreenPosY;

			AEVec2 mousePos{static_cast<f32>(cursorScreenPosX), static_cast<f32>(cursorScreenPosY)};
			if (Button::checkButtonClick(mousePos, btnStart.pos, btnStart.size))
			{
				GSM::stateNext = GS_LEVEL1;
			}
			if (Button::checkButtonClick(mousePos, btnExit.pos, btnExit.size))
			{
				GSM::stateNext = GS_QUIT;
			}
			if (Button::checkButtonClick(mousePos, btnCredit.pos, btnCredit.size))
			{
				GSM::stateNext = GS_CREDITS;
			}
		}
	}

	void Render()
	{
		AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		AEMtx33 scale = { 0 };
		AEMtx33 translate = { 0 };
		AEMtx33 transform = { 0 };

		// background
		AEMtx33Scale(&scale, windowWidth, windowHeight);
		AEMtx33Trans(&translate, 0.0f, 0.0f);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexBackground, 0, 0);

		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// title
		AEMtx33Scale(&scale, GAME_TITLE_SIZE_X, GAME_TITLE_SIZE_Y);
		AEMtx33Trans(&translate, GAME_TITLE_POS_X, GAME_TITLE_POS_Y);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexTitle, 0, 0);

		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// start button
		AEMtx33Scale(&scale, btnStart.size.x, btnStart.size.y);
		AEMtx33Trans(&translate, btnStart.pos.x, btnStart.pos.y);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexStart, 0, 0);

		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// exit button
		AEMtx33Scale(&scale, btnExit.size.x, btnExit.size.y);
		AEMtx33Trans(&translate, btnExit.pos.x, btnExit.pos.y);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexExit, 0, 0);

		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// credit button
		AEMtx33Scale(&scale, btnCredit.size.x, btnCredit.size.y);
		AEMtx33Trans(&translate, btnCredit.pos.x, btnCredit.pos.y);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexCredit, 0, 0);

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
		AEGfxTextureUnload(pTexStart);
		AEGfxTextureUnload(pTexExit);
		AEGfxTextureUnload(pTexCredit);
		AEGfxTextureUnload(pTexTitle);
		AEGfxTextureUnload(pTexBackground);
	}
} // namespace MainMenu