/*!************************************************************************
\file       GameStateManager.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (90%)
			Chua Wen Shing Bryan (c.wenshingbryan) (10%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

#include "AEEngine.h"
#include <array>

#include "DayNight.h"	// DayNightState
#include "MapLevel.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "Button.h"

namespace GR {

	class GameRenderer
	{
	public:
		// constructor
		GameRenderer(
			GameObject::GameObject* goList,
			u32 const& refspritesheet_cols, u32 const& refspritesheet_rows);
		
		// Prevents other constructors
		GameRenderer() = delete;
		GameRenderer(GameRenderer const&) = delete;
		GameRenderer& operator=(GameRenderer const&) = delete;
		
		// destructor
		~GameRenderer();

		enum PlayerDirection {
			P_IDLE,
			P_LEFT,
			P_LEFT_SHOOT,
			P_RIGHT,
			P_RIGHT_SHOOT
		};

		// Various Render Functions

		void RenderEnvironment(
			f32 const& windowWidth, f32 const& windowHeight,
			f32 const& camPosX, f32 const& camPosY);

		void RenderSunMoon(f32 const& sunRot, f32 const& halfWindowWidth,
			AEVec2 const& sunPos, DayNightCycle::DayNightState const& dnState);

		void RenderMapBG(Map2::MapLevel* level, float const& TILE_SIZE,
			float const& HALF_TILE_SIZE);

		void RenderPause(
			f32 const& windowWidth, f32 const& windowHeight,
			f32 const& camPosX, f32 const& camPosY,
			Button::Button& btnResume, Button::Button& btnExit);

		void RenderPlayerUI(GameObject::PlayerObject* playerObject, const AEVec2& cameraFinalPosition, const AEVec2& cameraCurrPosition);

		void RenderTransition(
			f32 const& windowWidth, f32 const& windowHeight,
			float const& levelTSPosX,
			f32 const& camPosX, f32 const& camPosY,
			bool const& showDiedScreen);

		void RenderInteractableChestText(GameObject::GameObject* interactableChest);

		void RenderVictoryScreen();

		//  Game objects =======================================
		void RenderGameObjects(GameObject::PlayerObject* playerObject, ParticleSystem& particleSystem, bool const& gameTransitioning);
		
	private:
		// Game objects individuals ============================
		void RenderGOPlayer(bool const& gameTransitioning, int i = 0);

		void RenderAllyProjectile(int const& MIN, int const& MAX);

		void RenderEnemy(int const& MIN, int const& MAX, GameObject::PlayerObject* playerObject);

		void RenderEnemyElite(int const& MIN, int const& MAX, GameObject::PlayerObject* playerObject);

		void RenderEnemyBoss(int const& MIN, int const& MAX);

		void RenderEnemyProjectile(int const& MIN, int const& MAX);

		void RenderPowerups(int const& MIN, int const& MAX);

		void RenderChest(int const& MIN, int const& MAX);

		void RenderSP(int const& MIN, int const& MAX);

		void RenderDoor(int const& MIN, int const& MAX);

		void RenderMapGrid(int const& MIN, int const& MAX);
		// =====================================================

	public:
		// AEGfxTextures Used in Map Reading (SwitchRooms) so public
		enum TEXTURES
		{
			// Background
			T_GAME_BG,
			T_GAME_TRANS_SCR,
			T_GAME_TRANS_DEAD,
			T_GAME_SUN,
			T_GAME_MOON,


			// Chest
			T_CHEST_UNOPENED,
			T_CHEST_OPEN,


			// Drops
			T_DROPS_ATK_UP,
			T_DROPS_PWR_UP,
			T_DROPS_SOL_UP,
			T_DROPS_HP_REG,
			T_DROPS_SOL_REG,

			// Save point
			T_SP_INACTIVE,
			T_SP_ACTIVE,

			// Enemies
			T_ENEMY,
			T_BOSS_ATK_ONE,
			T_BOSS_ATK_TWO,

			// Player
			T_PLAYER,
			T_PLAYER_BULLET,

			// Player UI
			T_PLAYER_UI_HP,
			T_PLAYER_UI_SOLAR,

			// Buttons
			T_BUTTON_RESUME,
			T_BUTTON_EXIT,

			// Controls
			T_CONTROL_UI,


			// Map Tiles
			T_MAP_GRASS,
			T_MAP_DIRT,
			T_MAP_STONE_PATH,
			T_MAP_STONE_WALL,

			T_MAP_BG_STONE,
			T_MAP_BG_CAVE,
			T_MAP_BG_DIRT,

			// Keys
			T_KEY_F,


			T_TEXTURE_MAX
		};

	private:
		
		enum MESHES
		{
			// Level
			M_MAP_DOOR,


			// Square
			M_SQUARE,
			M_BLACK_SQUARE,


			// Player
			M_PLAYER,


			// Enemy
			M_SPRITE,

			
			// Particle
			M_PARTICLE,

			M_MESHES_MAX
		};

	
		// Store all font, textures and meshes
		s8 font;
		std::array<AEGfxTexture*, TEXTURES::T_TEXTURE_MAX> TextureList;
		std::array<AEGfxVertexList*, MESHES::M_MESHES_MAX> MeshList;

		// Pointer to Level's GO list
		GameObject::GameObject* objects;

		// Resusable Matrices in GameRenderer.cpp
		AEMtx33 scaleMtx, translateMtx, rotMtx;
		AEMtx33 transformMtx;

		// to determine where player face when transition
		PlayerDirection playerFaceDir;

	};
	extern bool bPlayerArmour; //To display player armour
	extern int playerState;
}