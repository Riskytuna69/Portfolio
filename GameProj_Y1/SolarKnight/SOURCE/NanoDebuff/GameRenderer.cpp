/*!************************************************************************
\file       GameRenderer.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (15%)
			Lau Jia Win (jiawin.lau) (40%)
			Gavin Ang Jun Liang (ang.g) (15%)
			Chua Wen Shing Bryan (c.wenshingbryan) (15%)
			Min Khant Ko (ko.m) (15%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "GameRenderer.h"

#include "GameObject.h"
#include "Enemies.h"
#include "Collision.h"
#include "CreateMesh.h"
// For string formatting
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <iomanip>

namespace GR {

	// anon functions
	namespace
	{
		void SetRender(AEGfxRenderMode rm, float transparency = 1.0f)
		{
			AEGfxSetRenderMode(rm);
			AEGfxSetColorToAdd(0, 0, 0, 0); // reset due to particle changing it
			AEGfxSetColorToMultiply(1, 1, 1, 1); // reset due to particle changing it
			AEGfxSetTransparency(transparency);
		}
	}

	// Constructor
	GameRenderer::GameRenderer(
		GameObject::GameObject* goList,
		u32 const& refspritesheet_cols, u32 const& refspritesheet_rows)
		: objects{ goList },
		scaleMtx{ 0 }, translateMtx{ 0 }, rotMtx{ 0 }, transformMtx{ 0 }
	{
		playerFaceDir = P_IDLE;
		// Fonts
		font = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);

		// Textures
		TextureList[T_GAME_BG]			= AEGfxTextureLoad("Assets/game_background.png");
		TextureList[T_GAME_TRANS_SCR]	= AEGfxTextureLoad("Assets/room_transition.png");
		TextureList[T_GAME_TRANS_DEAD]	= AEGfxTextureLoad("Assets/dead_transition.png");
		TextureList[T_GAME_SUN]			= AEGfxTextureLoad("Assets/sun.png");
		TextureList[T_GAME_MOON]		= AEGfxTextureLoad("Assets/moon.png");
					   
		TextureList[T_CHEST_UNOPENED]	= AEGfxTextureLoad("Assets/chest_unopen.png");
		TextureList[T_CHEST_OPEN]		= AEGfxTextureLoad("Assets/chest_open.png");
					   
		TextureList[T_DROPS_ATK_UP]		= AEGfxTextureLoad("Assets/atkup.png");
		TextureList[T_DROPS_PWR_UP]		= AEGfxTextureLoad("Assets/pwrup.png");
		TextureList[T_DROPS_SOL_UP]		= AEGfxTextureLoad("Assets/solarup.png");
		TextureList[T_DROPS_HP_REG]		= AEGfxTextureLoad("Assets/hp_regen.png");
		TextureList[T_DROPS_SOL_REG]	= AEGfxTextureLoad("Assets/solar_regen.png");

		TextureList[T_SP_INACTIVE] = AEGfxTextureLoad("Assets/Textures/SP_unused.png");
		TextureList[T_SP_ACTIVE] = AEGfxTextureLoad("Assets/Textures/SP_used.png");
		
		

		TextureList[T_ENEMY]			= AEGfxTextureLoad("Assets/enemies.png");
		TextureList[T_BOSS_ATK_ONE]		= AEGfxTextureLoad("Assets/boss_attack_1.png");
		TextureList[T_BOSS_ATK_TWO]		= AEGfxTextureLoad("Assets/boss_attack_2.png");

		TextureList[T_PLAYER]			= AEGfxTextureLoad("Assets/player.png");
		TextureList[T_PLAYER_BULLET]	= AEGfxTextureLoad("Assets/player_bullet.png");

		TextureList[T_PLAYER_UI_HP]		= AEGfxTextureLoad("Assets/heart_icon.png");
		TextureList[T_PLAYER_UI_SOLAR]	= AEGfxTextureLoad("Assets/solar_icon.png");

		TextureList[T_BUTTON_RESUME]	= AEGfxTextureLoad("Assets/resume_button.png");
		TextureList[T_BUTTON_EXIT]		= AEGfxTextureLoad("Assets/quit_button.png");
		
		TextureList[T_CONTROL_UI]		= AEGfxTextureLoad("Assets/controls.png");
					   
		TextureList[T_MAP_GRASS]		= AEGfxTextureLoad("Assets/Textures/collis_grass.png");
		TextureList[T_MAP_DIRT]			= AEGfxTextureLoad("Assets/Textures/collis_dirt.png");
		TextureList[T_MAP_STONE_PATH]	= AEGfxTextureLoad("Assets/Textures/collis_path.png");
		TextureList[T_MAP_STONE_WALL]	= AEGfxTextureLoad("Assets/Textures/collis_stonewall.png");
		TextureList[T_MAP_BG_STONE]		= AEGfxTextureLoad("Assets/Textures/non_collis_stone.png");
		TextureList[T_MAP_BG_CAVE]		= AEGfxTextureLoad("Assets/Textures/non_collis_cavewall.png");
		TextureList[T_MAP_BG_DIRT]		= AEGfxTextureLoad("Assets/Textures/non_collis_dirt.png");

		TextureList[T_KEY_F]			= AEGfxTextureLoad("Assets/f.png");

		// Vertices
		f32 vertexPoint = 0.5f;
		u32 colorHex = 0xFFFFFFFF;

		colorHex = 0xFFFFFFFF;
		MeshList[M_SQUARE]				= CreateMesh::CreateSquareMesh(vertexPoint, colorHex);
		colorHex = 0xFF000000;
		MeshList[M_BLACK_SQUARE]		= CreateMesh::CreateSquareMesh(vertexPoint, colorHex);
		colorHex = 0x8080800F;
		MeshList[M_MAP_DOOR]			= CreateMesh::CreateSquareMesh(vertexPoint, colorHex);
		colorHex = 0xFF0000FF;
		MeshList[M_PLAYER]				= CreateMesh::CreateSquareMesh(vertexPoint, colorHex);

		MeshList[M_SPRITE]				= CreateMesh::CreateSpriteAnimationMesh(vertexPoint, colorHex, refspritesheet_cols, refspritesheet_rows);

		colorHex = 0xFFFFFFFF;
		MeshList[M_PARTICLE]			= CreateMesh::CreateSquareMesh(vertexPoint, colorHex);
	}

	// Destructor
	GameRenderer::~GameRenderer()
	{
		AEGfxDestroyFont(font);
		
		for (size_t i{ 0 }; i < T_TEXTURE_MAX; ++i)
		{
			AEGfxTextureUnload(TextureList[i]);
		}

		for (size_t i{ 0 }; i < M_MESHES_MAX; ++i)
		{
			AEGfxMeshFree(MeshList[i]);
		}
		
	}

	void GameRenderer::RenderEnvironment(
		f32 const& windowWidth, f32 const& windowHeight,
		f32 const& camPosX, f32 const& camPosY)
	{
		SetRender(AE_GFX_RM_TEXTURE);

		AEMtx33Scale(&scaleMtx, windowWidth * 1.1f, windowHeight);
		AEMtx33Trans(&translateMtx, camPosX, camPosY);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
		AEGfxTextureSet(TextureList[T_GAME_BG], 0, 0);
		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
	}

	void GameRenderer::RenderSunMoon(f32 const& sunRot, f32 const& halfWindowWidth,
		AEVec2 const& sunPos, DayNightCycle::DayNightState const& dnState)
	{
		SetRender(AE_GFX_RM_TEXTURE);

		AEMtx33Scale(&scaleMtx, 200.0f, 200.0f);
		AEMtx33Rot(&rotMtx, -sunRot); // offset the rot at origin
		AEMtx33Concat(&transformMtx, &rotMtx, &scaleMtx);
		AEMtx33Trans(&translateMtx, halfWindowWidth, 0.0f); // set position to bottom middle of the screen
		AEMtx33Concat(&transformMtx, &translateMtx, &transformMtx);

		AEMtx33Rot(&rotMtx, sunRot); // rot at bottom middle of the screen
		AEMtx33Concat(&transformMtx, &rotMtx, &transformMtx);

		// now push to right side of the screen
		AEMtx33Trans(&translateMtx, sunPos.x, sunPos.y);
		AEMtx33Concat(&transformMtx, &translateMtx, &transformMtx);

		if (dnState == DayNightCycle::STATE_DAY || dnState == DayNightCycle::STATE_EVENING)
			AEGfxTextureSet(TextureList[T_GAME_SUN], 0, 0);
		else if (dnState == DayNightCycle::STATE_NIGHT)
			AEGfxTextureSet(TextureList[T_GAME_MOON], 0, 0);
		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
	}

	// Render background etc. things that isnt GameObjects in room
	void GameRenderer::RenderMapBG(Map2::MapLevel* level, float const& TILE_SIZE,
		float const& HALF_TILE_SIZE)
	{
		static int currRoomMaxRows{}, currRoomMaxCols{};
		level->GetActiveRoomSize(currRoomMaxCols, currRoomMaxRows);
		const int* roomGrid = level->ActiveRoomBGGrid();

		SetRender(AE_GFX_RM_TEXTURE);

		static Map2::MAP_GRID mapGrid;
		static TEXTURES textListNum{ T_MAP_BG_STONE };

		for (int row = 0; row < currRoomMaxRows; ++row)
		{
			for (int col = 0; col < currRoomMaxCols; ++col)
			{
				mapGrid = static_cast<Map2::MAP_GRID>(roomGrid[row * currRoomMaxCols + col]);

				// Get grid texture
				switch (mapGrid)
				{
				case Map2::MAP_GRID::BG_STONE:
					textListNum = T_MAP_BG_STONE; break;
				case Map2::MAP_GRID::BG_CAVE:
					textListNum = T_MAP_BG_CAVE; break;
				case Map2::MAP_GRID::BG_DIRT:
					textListNum = T_MAP_BG_DIRT; break;
				default:
					break;
				}

				// Draw grid
				switch (mapGrid)
				{
				case Map2::MAP_GRID::BG_STONE:
				case Map2::MAP_GRID::BG_CAVE:
				case Map2::MAP_GRID::BG_DIRT:
					AEMtx33Scale(&scaleMtx, TILE_SIZE, TILE_SIZE);
					AEMtx33Trans(&translateMtx, col * TILE_SIZE + HALF_TILE_SIZE, (currRoomMaxRows - row - 1) * TILE_SIZE + HALF_TILE_SIZE);
					AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

					AEGfxTextureSet(TextureList[textListNum], 0.0f, 0.0f);

					AEGfxSetTransform(transformMtx.m);
					AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
					break;
				default:
					break;
				}
			}
		}
	}

	void GameRenderer::RenderPause(
		f32 const& windowWidth, f32 const& windowHeight,
		f32 const& camPosX, f32 const& camPosY,
		Button::Button& btnResume, Button::Button& btnExit)
	{
		// background
		SetRender(AE_GFX_RM_COLOR, 0.75f);
		AEMtx33Scale(&scaleMtx, windowWidth * 1.1f, windowHeight);
		AEMtx33Trans(&translateMtx, camPosX, camPosY);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_BLACK_SQUARE], AE_GFX_MDM_TRIANGLES);

		// controls UI
		SetRender(AE_GFX_RM_TEXTURE);

		AEMtx33Scale(&scaleMtx, static_cast<f32>(AEGfxGetWindowWidth()), static_cast<f32>(AEGfxGetWindowHeight()));
		AEMtx33Trans(&translateMtx, camPosX, camPosY);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
		AEGfxTextureSet(TextureList[T_CONTROL_UI], 0, 0);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		// buttons
		// start button
		AEMtx33Scale(&scaleMtx, btnResume.size.x, btnResume.size.y);
		AEMtx33Trans(&translateMtx, btnResume.pos.x + camPosX, btnResume.pos.y + camPosY);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
		AEGfxTextureSet(TextureList[T_BUTTON_RESUME], 0, 0);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		// exit button
		AEMtx33Scale(&scaleMtx, btnExit.size.x, btnExit.size.y);
		AEMtx33Trans(&translateMtx, btnExit.pos.x + camPosX, btnExit.pos.y + camPosY);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
		AEGfxTextureSet(TextureList[T_BUTTON_EXIT], 0, 0);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
	}

	void GameRenderer::RenderPlayerUI(GameObject::PlayerObject* playerObject, 
		const AEVec2& cameraFinalPosition, const AEVec2& cameraCurrPosition)
	{
		SetRender(AE_GFX_RM_TEXTURE);
		// camera lock offset position
		AEVec2 camOffSetAmount = { cameraCurrPosition.x - cameraFinalPosition.x , cameraCurrPosition.y - cameraFinalPosition.y };

		// health icon
		f32 xPos = cameraFinalPosition.x - 750.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		f32 yPos = cameraFinalPosition.y + 400.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen

		f32 icon_scale{ 50.0f };

		AEMtx33Scale(&scaleMtx, icon_scale, icon_scale);
		AEMtx33Trans(&translateMtx, xPos, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxTextureSet(TextureList[T_PLAYER_UI_HP], 0.0f, 0.0f);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		// health bar
		// scale
		f32 maxScaleSize = 15.0f * playerObject->go->healthMaximum;
		f32 offsetScaleSize = 15.0f *  playerObject->go->healthCurrent;
		f32 yScale = 25.0f;

		// position
		xPos = cameraFinalPosition.x - 570.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		yPos = cameraFinalPosition.y + 400.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen
		f32 offsetPosition = (maxScaleSize - offsetScaleSize) * 0.5f; // for green bar to adjust

		SetRender(AE_GFX_RM_COLOR, 0.85f);
		AEGfxSetColorToMultiply(1, 0, 0, 1);
		AEMtx33Scale(&scaleMtx, maxScaleSize, yScale);
		AEMtx33Trans(&translateMtx, xPos, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		SetRender(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(0, 1, 0, 1);
		AEMtx33Scale(&scaleMtx, offsetScaleSize, yScale);
		AEMtx33Trans(&translateMtx, xPos - offsetPosition, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);


		// solar icon
		SetRender(AE_GFX_RM_TEXTURE);
		xPos = cameraFinalPosition.x - 750.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		yPos = cameraFinalPosition.y + 325.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen

		AEMtx33Scale(&scaleMtx, icon_scale, icon_scale);
		AEMtx33Trans(&translateMtx, xPos, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxTextureSet(TextureList[T_PLAYER_UI_SOLAR], 0.0f, 0.0f);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		// solar energy bar
		maxScaleSize = 5.0f * playerObject->solarEnergyMax;
		offsetScaleSize = 5.0f * playerObject->solarEnergy;

		// position
		xPos = cameraFinalPosition.x - 595.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		yPos = cameraFinalPosition.y + 325.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen
		offsetPosition = (maxScaleSize - offsetScaleSize) * 0.5f; // for green bar to adjust
		f32 solarOffSet = (maxScaleSize - (5.0f * GameObject::SOLAR_ENERGY_MAX_DEFAULT)) * 0.5f;

		SetRender(AE_GFX_RM_COLOR, 0.85f);
		AEGfxSetColorToMultiply(1, 0, 0, 1);
		AEMtx33Scale(&scaleMtx, maxScaleSize, yScale);
		AEMtx33Trans(&translateMtx, xPos + solarOffSet, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		SetRender(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(1, 0.65f, 0, 1);
		AEMtx33Scale(&scaleMtx, offsetScaleSize, yScale);
		AEMtx33Trans(&translateMtx, xPos - offsetPosition + solarOffSet, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);


		// Player buffs====================

		// fire rate up speed icon
		SetRender(AE_GFX_RM_TEXTURE);
		xPos = cameraFinalPosition.x - 750.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		yPos = cameraFinalPosition.y - 325.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen

		AEMtx33Scale(&scaleMtx, icon_scale, icon_scale);
		AEMtx33Trans(&translateMtx, xPos, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxTextureSet(TextureList[T_DROPS_PWR_UP], 0.0f, 0.0f);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

		// render fire rate up speed boxes =====

		SetRender(AE_GFX_RM_COLOR);
		AEMtx33Scale(&scaleMtx, icon_scale * 0.75f, icon_scale * 0.75f);
		xPos += icon_scale;

		int activeCount = static_cast<int>(playerObject->buffFireRate * 2.0f); // same as div by 0.5
		static int MAXFIRERATEBUFF = static_cast<int>(GameObject::FIRE_RATE_MAX_BUFF * 2.0f);
		for (int i = 0; i < MAXFIRERATEBUFF; i++)
		{
			AEMtx33Trans(&translateMtx, xPos + i * icon_scale, yPos);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
			AEGfxSetTransform(transformMtx.m);

			if (i <= activeCount - 1)
				AEGfxSetColorToMultiply(0, 1, 0, 1); // highlight
			else
				AEGfxSetColorToMultiply(1, 0, 0, 1); // empty


			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}



		// dmg up icon
		SetRender(AE_GFX_RM_TEXTURE);
		xPos = cameraFinalPosition.x - 750.0f + camOffSetAmount.x; // cameraX - an offset to the left of screen
		yPos = cameraFinalPosition.y - 400.0f + camOffSetAmount.y; // cameraY + an offset to the top of screen

		AEMtx33Scale(&scaleMtx, icon_scale, icon_scale);
		AEMtx33Trans(&translateMtx, xPos, yPos);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxTextureSet(TextureList[T_DROPS_ATK_UP], 0.0f, 0.0f);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);


		// render dmg up boxes =====

		SetRender(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(0, 1, 0, 1);
		AEMtx33Scale(&scaleMtx, icon_scale * 0.75f, icon_scale * 0.75f);
		xPos += icon_scale;

		activeCount = playerObject->buffAttack;
		for (int i = 0; i < GameObject::ATTACK_MAX_BUFF; i++)
		{
			AEMtx33Trans(&translateMtx, xPos + i * icon_scale, yPos);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
			AEGfxSetTransform(transformMtx.m);

			if (i <= activeCount - 1)
				AEGfxSetColorToMultiply(0, 1, 0, 1); // highlight
			else
				AEGfxSetColorToMultiply(1, 0, 0, 1); // empty

			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderTransition(
		f32 const& windowWidth, f32 const& windowHeight,
		float const& levelTSPosX,
		f32 const& camPosX, f32 const& camPosY,
		bool const& showDiedScreen)
	{
		SetRender(AE_GFX_RM_TEXTURE);

		// Set the texture offseting position
		if (showDiedScreen)
			AEGfxTextureSet(TextureList[T_GAME_TRANS_DEAD], 0, 0);
		else
			AEGfxTextureSet(TextureList[T_GAME_TRANS_SCR], 0, 0);

		// Creates a scaling matrix
		AEMtx33Scale(&scaleMtx, windowWidth, windowHeight);

		static AEVec2 levelTransitionScreenPos{ levelTSPosX, 0 };
		levelTransitionScreenPos.x = levelTSPosX;

		// Create a translation matrix that translates by
		AEMtx33Trans(&translateMtx, levelTransitionScreenPos.x + camPosX, levelTransitionScreenPos.y + camPosY);

		// Concatenate the matrices into the 'transform' variable.
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
		AEGfxSetTransform(transformMtx.m);

		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
	}

	void GameRenderer::RenderInteractableChestText(GameObject::GameObject* interactableChest)
	{
		f32 transparency = 0.75f;
		SetRender(AE_GFX_RM_TEXTURE, transparency);

		f32 scale{ 25.0f };
		f32 offset{};

		if (interactableChest->type == GameObject::OBJ_CHEST) {
			offset = 30.0f;
		}
		else if (interactableChest->type == GameObject::OBJ_SP) {
			offset = 90.0f;
		}

		AEMtx33Scale(&scaleMtx, scale, scale);
		AEMtx33Trans(&translateMtx, interactableChest->position.x, interactableChest->position.y + offset);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxTextureSet(TextureList[T_KEY_F], 0.0f, 0.0f);

		AEGfxSetTransform(transformMtx.m);
		AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
	}

	void GameRenderer::RenderVictoryScreen()
	{
		SetRender(AE_GFX_RM_TEXTURE);
		// debug for player statistics
		std::string victory{ "YOU WIN!" };
		AEGfxPrint(font, victory.c_str(), 0, 0, 1, 1, 0, 0, 1);
	}
	
	
	// Game objects ========================================


	void GameRenderer::RenderGOPlayer(bool const& gameTransitioning, int i)
	{
		if (!objects[i].active)
			return;

	
		// RENDER THE PLAYER
		SetRender(AE_GFX_RM_TEXTURE); // Trying to set different color, dk why black?
		// Scale the tile
		AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);

		// Translate to center tiles
		AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);

		if (gameTransitioning)
		{
			// if game is transitioning, use prev texture
			switch (playerFaceDir)
			{
			case GR::GameRenderer::P_LEFT:
				AEGfxTextureSet(TextureList[T_PLAYER], 1.40f, 1.f);
				break;
			case GR::GameRenderer::P_LEFT_SHOOT:
				AEGfxTextureSet(TextureList[T_PLAYER_BULLET], 1.80f, 1.f);
				break;
			case GR::GameRenderer::P_RIGHT:
				AEGfxTextureSet(TextureList[T_PLAYER], 1.20f, 1.f);
				break;
			case GR::GameRenderer::P_RIGHT_SHOOT:
				AEGfxTextureSet(TextureList[T_PLAYER_BULLET], 1.60f, 1.f);
				break;
			default:
				AEGfxTextureSet(TextureList[T_PLAYER], 1.f, 1.f);
				break;
			}
		}
		else
		{
			if (!bPlayerArmour)//Time delay OFF
			{
				//Increment by 0.20 for x axis, plz look at player sprite sheet for formation info
				//Increment by 0.50 for Y axis, bottom half
				AEGfxTextureSet(TextureList[T_PLAYER], 1.f, 1.f);
				playerFaceDir = P_IDLE;
				//Player texture
				if (playerState == P_RIGHT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.20f, 1.f);
					playerFaceDir = P_RIGHT;
					
				}
				else if (playerState == P_RIGHT_SHOOT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.60f, 1.f);
					playerFaceDir = P_RIGHT_SHOOT;
				}
				else if (playerState == P_LEFT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.40f, 1.f);
					playerFaceDir = P_LEFT;
					
				}
				else if (playerState == P_LEFT_SHOOT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.80f, 1.f);
					playerFaceDir = P_LEFT_SHOOT;
				}
			}
			else
			{
				//Increment by 0.20 for x axis, plz look at player sprite sheet for formation info
				//Increment by 0.50 for Y axis, bottom half
				AEGfxTextureSet(TextureList[T_PLAYER], 1.f, 1.5f);
				playerFaceDir = P_IDLE;
				//Player texture
				if (playerState == P_RIGHT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.20f, 1.5f);
					playerFaceDir = P_RIGHT;
					
				}
				else if (playerState == P_RIGHT_SHOOT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.60f, 1.5f);
					playerFaceDir = P_RIGHT_SHOOT;
				}
				else if (playerState == P_LEFT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.40f, 1.5f);
					playerFaceDir = P_LEFT;
					
				}
				else if (playerState == P_LEFT_SHOOT)
				{
					AEGfxTextureSet(TextureList[T_PLAYER], 1.80f, 1.5f);
					playerFaceDir = P_LEFT_SHOOT;
				}
			}
		}

		// Get matrix
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
		// the vertices of the mesh that we are about to choose to draw in the next line.
		AEGfxSetTransform(transformMtx.m);

		AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
	}

	void GameRenderer::RenderAllyProjectile(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			if (objects[i].healthMaximum != -1) // Player bullet condition
			{
				if (objects[i].knockbackType == -10) { // Normal shot
					// RENDER THE PLAYER BULLET
					SetRender(AE_GFX_RM_TEXTURE);
					// Scale the tile
					AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);

					// Translate to center tiles
					AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);

					//Player bullet NOT sharing same png wif player
					AEGfxTextureSet(TextureList[T_PLAYER_BULLET], 1.f, 1.f);

					// Get matrix
					AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

					// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
					// the vertices of the mesh that we are about to choose to draw in the next line.
					AEGfxSetTransform(transformMtx.m);

					AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES); // CHECK FOR BULLET MESH
				}
				else { // Charged shot
					// RENDER THE PLAYER BULLET
					SetRender(AE_GFX_RM_TEXTURE);
					// Scale the tile
					AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);

					// Translate to center tiles
					AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);

					//Player bullet NOT sharing same png wif player
					AEGfxTextureSet(TextureList[T_PLAYER_BULLET], 1.f, 1.5f);

					// Get matrix
					AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

					// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
					// the vertices of the mesh that we are about to choose to draw in the next line.
					AEGfxSetTransform(transformMtx.m);

					AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES); // CHECK FOR BULLET MESH
				}
				


			}
		}
	}

	void GameRenderer::RenderEnemy(int const& MIN, int const& MAX, GameObject::PlayerObject* playerObject)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			Enemies::enemyRedHpBar(&objects[i]);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			Enemies::enemyGreenHpBar(&objects[i]);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			SetRender(AE_GFX_RM_TEXTURE);
			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);
			//Increment by 0.20 for x axis, plz look at player sprite sheet for formation info
			//Increment by 0.50 for Y axis, bottom half
			if (objects[i].healthMaximum == RANGE_ID) //Render Range enemy
			{
				if (!Collision::checkCirclesIntersection(&objects[i], playerObject->go))
				{
					// Set the texture to enemyPng, not detected
					AEGfxTextureSet(TextureList[T_ENEMY], 1.60f, 1.f);
				}
				else
				{
					// Set the texture to enemyPng, detected!
					AEGfxTextureSet(TextureList[T_ENEMY], 1.80f, 1.f);
				}
			}
			else //Render Melee enemy
			{
				if (!Collision::checkCirclesIntersection(&objects[i], playerObject->go))
				{
					// Set the texture to enemyPng, not detected
					AEGfxTextureSet(TextureList[T_ENEMY], 1.f, 1.f);
				}
				else
				{
					// Set the texture to enemyPng, detected!
					AEGfxTextureSet(TextureList[T_ENEMY], 1.20f, 1.f);
				}
			}
			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderEnemyElite(int const& MIN, int const& MAX, GameObject::PlayerObject* playerObject)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			Enemies::enemyRedHpBar(&objects[i]);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			Enemies::enemyGreenHpBar(&objects[i]);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			SetRender(AE_GFX_RM_TEXTURE);
			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			if (!Collision::checkCirclesIntersection(&objects[i], playerObject->go))
			{
				// Set the texture to enemyPng, not detected
				AEGfxTextureSet(TextureList[T_ENEMY], 1.f, 1.5f);
			}
			else
			{
				if (objects[i].position.x < objects[0].position.x)
				{
					AEGfxTextureSet(TextureList[T_ENEMY], 1.40f, 1.5f);
				}
				else
				{
					AEGfxTextureSet(TextureList[T_ENEMY], 1.20f, 1.5f);
				}
			}
			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderEnemyBoss(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			Enemies::bossPinkHpBar();
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			Enemies::bossPurpleHpBar(&objects[i]);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);

			SetRender(AE_GFX_RM_TEXTURE);
			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			AEGfxTextureSet(TextureList[T_ENEMY], 1.60f, 1.5f);
				
			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderEnemyProjectile(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_TEXTURE);
			if (objects[i].enemyFlagsID < Enemies::bossBulletIDBegin || (objects[i].enemyFlagsID > Enemies::bossBulletIDEnd))
			{

				AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
				AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
				AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

				AEGfxTextureSet(TextureList[T_ENEMY], 1.80f, 1.5f);


				AEGfxSetTransform(transformMtx.m);
				AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
			}
			else
			{
				AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
				AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
				AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

				if(objects[i].enemyFlagsID < Enemies::bossBulletIDBegin + 5)// 5 = amt of projectiles Boss use for atk 1
					Enemies::bossAtkRenderType(TextureList[T_BOSS_ATK_ONE], &objects[i], BOSS_ATTACK_ONE);
				else			
					Enemies::bossAtkRenderType(TextureList[T_BOSS_ATK_TWO], &objects[i], BOSS_ATTACK_TWO);

				AEGfxSetTransform(transformMtx.m);
				AEGfxMeshDraw(MeshList[M_SPRITE], AE_GFX_MDM_TRIANGLES);
			}
		}
	}

	void GameRenderer::RenderPowerups(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_TEXTURE);

			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			switch (objects[i].type)
			{
			case GameObject::OBJ_ATK_UP:
				AEGfxTextureSet(TextureList[T_DROPS_ATK_UP], 0.0f, 0.0f);
				break;
			case GameObject::OBJ_PWR_UP:
				AEGfxTextureSet(TextureList[T_DROPS_PWR_UP], 0.0f, 0.0f);
				break;
			case GameObject::OBJ_SOLAR_UP:
				AEGfxTextureSet(TextureList[T_DROPS_SOL_UP], 0.0f, 0.0f);
				break;
			case GameObject::OBJ_HP_REGEN:
				AEGfxTextureSet(TextureList[T_DROPS_HP_REG], 0.0f, 0.0f);
				break;
			case GameObject::OBJ_SOLAR_REGEN:
				AEGfxTextureSet(TextureList[T_DROPS_SOL_REG], 0.0f, 0.0f);
				break;
			default:
				break;
			}
			
			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderChest(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_TEXTURE);

			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			if (!objects[i].loots.bOpened)
				AEGfxTextureSet(TextureList[T_CHEST_UNOPENED], 0.0f, 0.0f);
			else
				AEGfxTextureSet(TextureList[T_CHEST_OPEN], 0.0f, 0.0f);

			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderSP(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_TEXTURE);

			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y - (objects[i].height / 8)); // Slight offset
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			if (objects[i].knockbackState)
				AEGfxTextureSet(TextureList[T_SP_ACTIVE], 0.0f, 0.0f);
			else
				AEGfxTextureSet(TextureList[T_SP_INACTIVE], 0.0f, 0.0f);

			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderDoor(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_COLOR);

			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);


			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_MAP_DOOR], AE_GFX_MDM_TRIANGLES);
		}
	}

	void GameRenderer::RenderMapGrid(int const& MIN, int const& MAX)
	{
		for (int i{ MAX - 1 }; i >= MIN; --i)
		{
			if (!objects[i].active)
				continue;

			SetRender(AE_GFX_RM_TEXTURE);

			AEMtx33Scale(&scaleMtx, objects[i].width, objects[i].height);
			AEMtx33Trans(&translateMtx, objects[i].position.x, objects[i].position.y);
			AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

			AEGfxTextureSet(TextureList[objects[i].levelNumber], 0.0f, 0.0f);

			AEGfxSetTransform(transformMtx.m);
			AEGfxMeshDraw(MeshList[M_SQUARE], AE_GFX_MDM_TRIANGLES);
		}
	}


	// Main ================================================
	void GameRenderer::RenderGameObjects(GameObject::PlayerObject* playerObject, ParticleSystem& particleSystem,
		bool const& gameTransitioning)
	{
		RenderMapGrid(GameObject::MINGRID, GameObject::MAXGRID);
		RenderDoor(GameObject::MINDOOR, GameObject::MAXDOOR);
		RenderChest(GameObject::MINCHEST, GameObject::MAXCHEST);
		RenderSP(GameObject::MINSP, GameObject::MAXSP);
		RenderEnemyBoss(GameObject::MINENEMYBOSS, GameObject::MAXENEMYBOSS);
		RenderEnemy(GameObject::MINENEMY, GameObject::MAXENEMY, playerObject);
		RenderEnemyElite(GameObject::MINENEMYELITE, GameObject::MAXENEMYELITE, playerObject);
		RenderPowerups(GameObject::MINPOWERUPS, GameObject::MAXPOWERUPS);
		RenderEnemyProjectile(GameObject::MINENEMYPROJECTILE, GameObject::MAXENEMYPROJECTILE);
		RenderAllyProjectile(GameObject::MINALLYPROJECTILE, GameObject::MAXALLYPROJECTILE);
		RenderGOPlayer(gameTransitioning);

		//Putting this here because the smokes become player png lol
		SetRender(AE_GFX_RM_COLOR);
		particleSystem.OnParticleRender(MeshList[M_PARTICLE]);

		SetRender(AE_GFX_RM_COLOR, 0.75f);
		particleSystem.OnBuffParticleRender(MeshList[M_PARTICLE]);
	}



	// =====================================================
}