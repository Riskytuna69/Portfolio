/*!************************************************************************
\file       Credits.cpp
\project    Solar Knight
\author(s)  Min Khant Ko (ko.m) (50%)
			Chng Kai Rong, Leonard (k.chng) (50%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "Credits.h"
#include "pch.h"
#include "CreateMesh.h"
#include "GameStateManager.h"
#include "Button.h"

#include <stdio.h> // For text file reading
#include <errno.h> // For file open error

namespace Credits {
	// const variable

	// button
	const f32 BUTTON_SIZE_X = 200.0f;
	const f32 BUTTON_SIZE_Y = 100.0f;
	// camera
	const f32 CAMERA_POS_X = 0.0f;
	const f32 CAMERA_POS_Y = 0.0f;
	// Credits start delay time
	const f32 delayForScroll = 1.0f; 
	const f32 offsetSpeed = 200.0f;


	// Texture
	AEGfxTexture* pTexBack;

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
	Button::Button btnBack;

	// New variables
	float startingPosX, scaledPosX, defaultStartingPosY, startingPosY, scaledPosY, lineSpacing, screenOffset, scaledOffset, timeElapsedCredits;
	FILE* rFile;
	int lineCounter, fileLineTotal;
	char bufferStr[100];
	char* bufferPointer;

	s8 pFont;
	f32 fW, fH;

} // namespace Credits

namespace Credits {
	void Init()
	{
		// Load Assets
		pTexBack = AEGfxTextureLoad("Assets/quit_button.png");

		pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 30); // New variables

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
		btnBack.pos = AEVec2{ -700.0f, 350.0f };
		btnBack.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		// set camera position to (0,0)
		AEGfxSetCamPosition(CAMERA_POS_X, CAMERA_POS_Y);


		// New variables
		timeElapsedCredits = 0.0f;
		screenOffset = 0.0f;
		lineCounter = 0, fileLineTotal = 0;
		lineSpacing = windowHeight * 0.06f;
		startingPosX = windowWidth * 0.5f; // Check...
		startingPosY = defaultStartingPosY = halfWindowHeight - lineSpacing;
		

		errno_t err;
		if ((err = fopen_s(&rFile, "Assets/Credits.txt", "r")) != 0) { // Read only
			// File could not be opened. filepoint was set to NULL
			// error code is returned in err.
			// error message can be retrieved with strerror(err);
		}
		else if (rFile == NULL) {
			// Pointer pointing to nothing error...
		}
	}

	void Set()
	{

	}

	void Update()
	{
		// New variables
		timeElapsedCredits += static_cast<f32>(AEFrameRateControllerGetFrameTime());

		if (timeElapsedCredits >= delayForScroll) {
			screenOffset = static_cast<f32>(AEFrameRateControllerGetFrameTime()) * offsetSpeed;
			startingPosY += screenOffset;
		}

		fW = 0.0f, fH = 0.0f; // Reset AEGfxGetPrintSize values just in case

		/* Normalized coordinates, scale from -1 to 1. 
		Hence make coordinate scale to 0 to 2 first (2 being the highest value)
		then deduct 1 to make scale -1 to 1. 
		To wrap a certain coordinate, modulo the scaled value at 0 to 2 (before deducting 1)
		then deduct 1 after modulo (modulo doesn't account "negative scale") */

		scaledPosX = 2.0f * (startingPosX / windowWidth) - 1.0f; // SCALE BY FORMULA
		scaledPosY = 2.0f * (startingPosY / windowHeight) - 1.0f; // SCALE BY FORMULA

		lineCounter = 0;
		while (fgets(bufferStr, 100, rFile)) {
			fileLineTotal++;
			bufferPointer = bufferStr;
			bufferPointer += 2; // Ignore first 2 characters ('t' or 'n' and whitespace)

			
			scaledOffset = 2.0f * ( (lineCounter * lineSpacing) / windowHeight) - 1.0f;


			if (bufferStr[0] == 't') {
				// Text full size, color yellow (currently)
				AEGfxGetPrintSize(pFont, bufferStr, 1.0f, &fW, &fH); // Scale width and height of current line
				AEGfxPrint(pFont, bufferPointer, -fW/2, scaledPosY - scaledOffset, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

				lineCounter++;
			}
			else if (bufferStr[0] == 'n') {
				// Text smaller, color white (currently)
				AEGfxGetPrintSize(pFont, bufferStr, 0.8f, &fW, &fH); // Scale width and height of current line
				AEGfxPrint(pFont, bufferPointer, -fW/2, scaledPosY - scaledOffset, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);

				lineCounter++;
			}
			else if (bufferStr[0] == '*') {
				lineCounter += 2;
			}
			else if (bufferStr[0] == ' ' || bufferStr[0] == '\n') {
				lineCounter++;
			}
		
		}
		fseek(rFile, 0, SEEK_SET); // Reposition file pointer of Credits.txt to the start again, so the while loop can keep re-printing

		// Place starting position at "bottom" for infinite scrolling
		if ( startingPosY >= (windowHeight + (fileLineTotal * lineSpacing)) ) {
			startingPosY = -halfWindowHeight;
		}
		fileLineTotal = 0; // Reset fileLineTotal after checking


		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			// Cursor Position
			AEInputGetCursorPosition(&cursorScreenPosX, &cursorScreenPosY);
			cursorScreenPosX -= static_cast<s32>(halfWindowWidth);
			cursorScreenPosY += static_cast<s32>(halfWindowHeight) - static_cast<s32>(windowHeight);
			cursorScreenPosY = -cursorScreenPosY;

			AEVec2 mousePos{ static_cast<f32>(cursorScreenPosX), static_cast<f32>(cursorScreenPosY) };
			if (Button::checkButtonClick(mousePos, btnBack.pos, btnBack.size))
			{
				GSM::stateNext = GS_MAINMENU;
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

		// back button
		AEMtx33Scale(&scale, btnBack.size.x, btnBack.size.y);
		AEMtx33Trans(&translate, btnBack.pos.x, btnBack.pos.y);
		AEMtx33Concat(&transform, &translate, &scale);
		AEGfxTextureSet(pTexBack, 0, 0);

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
		AEGfxTextureUnload(pTexBack);

		// New variables
		AEGfxDestroyFont(pFont);
	}
} // namespace Credits