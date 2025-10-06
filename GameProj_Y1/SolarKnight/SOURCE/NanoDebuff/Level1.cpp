/*!************************************************************************
\file       Level1.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (20%)
			Lau Jia Win (jiawin.lau) (20%)
			Gavin Ang Jun Liang (ang.g) (20%)
			Chua Wen Shing Bryan (c.wenshingbryan) (20%)
			Min Khant Ko (ko.m) (20%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"

#include "Level1.h"
#include "GameStateManager.h"
#include "CreateMesh.h"
#include "GameObject.h"
#include "Collision.h"
#include "ObjectPhysics.h"
#include "PlayerController.h"
#include "Camera.h"
#include "Enemies.h"
#include "DayNight.h"
#include "Orbit.h"
#include "Drops.h"
#include "Chest.h"
#include "ParticleSystem.h"
#include "Button.h"
#include "Sound.h"
#include "MapLevel.h"
#include "Color.h"

#include "GameRenderer.h"
#include "TransitionScreen.h"
#include "GameVictory.h"

// C++ libs
#include <cstdlib>

// Global variables...
f32 gameDeltaTime{};
//f64 gameTotalTime();
bool GR::bPlayerArmour; //For rendering player armour
int GR::playerState = GR::GameRenderer::P_IDLE;//Set to idle by default


namespace Level1 {
	GR::GameRenderer* gameRender;
	
#pragma region LEVEL GRID VARIABLES
	//==========================================
	const float TILE_SIZE = 50.f;
	const float HALF_TILE_SIZE = TILE_SIZE * 0.5f;
	Map2::MapLevel* level;		    // Level obj

	// Switch level variables===
	bool levelRoomSwitchFlag;	    // Calls switchroom() if true
	bool levelPlayerDiedSwitchFlag; // if true and roomswitchflag also true, render different transition screen
	AEVec2 levelNextRoomPlayerPos;	// the player's new pos in the new room
	int levelNextRoomID = -1;	    // Used to determine which room to switch TO

	// Switch level transition===
	GR::TransitionScreen* levelTS;

	//==========================

	AEVec2 levelPlayerRespawnPos;	// when player respawn
	int levelPlayerRespawnRoom;		// when player respawn

	//==========================================
#pragma endregion // LEVEL GRID VARIABLES

#pragma region VARIABLES
	// Game variables
	bool bGamePause;
	f32 windowWidth;
	f32 windowHeight;
	f32 halfWindowWidth;
	f32 halfWindowHeight;

	// Camera
	f32 camPosX;
	f32 camPosY;
	AEVec2 currentCamPos;
	f32 camDefaultPosX;
	f32 camDefaultPosY;

	// Cursor
	s32 cursorWorldPosX;
	s32 cursorWorldPosY;
	s32 cursorScreenPosX;
	s32 cursorScreenPosY;

	// Day / Night
	DayNightCycle::DayNightState dnState;
	DayNightCycle::DayNightState dnPrevState;
	bool bSkyColorTransiting;
	f32 dayTime;
	f32 eveningTime;
	f32 nightTime;
	f32 skyTranisitionTime;

	// Color
	Colors::Color skyColor;

	// Sun / Moon
	AEVec2 sunPos;
	f32 sunRot;

	// Player
	GameObject::PlayerObject* playerObject;
	bool isJumping{};
	bool timeDelayStatus{};
	std::vector<GameObject::GameObject*> pBulletList{};
	f32 actualDeltaTime{};

	// Save file
	FILE* saveFile = NULL;

	//Platform
	GameObject::GameObject* platformOne;
	AEVec2 platformOnePos;

	// Physics
	int playerTouchingGround{};
	f32 defaultJumpVelocity{}, jumpVelocity{};
	f32 jumpForce{}, jumpForceRemaining{}; // CHECK
	f32 movementValue{};

	// Input
	s32 mouseX{}, mouseY{};
	int lMouseHeldDown, rMouseHeldDown;

	// Level
	f32 levelXSize;
	f32 levelYSize;

	// Save points
	std::vector<PlayerController::SavePointPos> savePointsPos;

	// Button
	const f32 BUTTON_SIZE_X = 300.0f;
	const f32 BUTTON_SIZE_Y = 150.0f;
	Button::Button btnResume;
	Button::Button btnExit;

	GameObject::GameObject objects[GameObject::MAXGAMEOBJECT]{};

	//particle system
	ParticleSystem m_ParticleSystem;

	//chests
	std::vector<Chest::ChestSpawnPoint> chestSP;

	// victory
	bool levelVictoryFlag;
#pragma endregion // VARIABLES
}

namespace Level1 {
	void Init()
	{
		// Loads all textures and meshes
		gameRender = new GR::GameRenderer(
			objects,
			spritesheet_cols, spritesheet_rows);

		GameVictory::Initialize();
		
		// Set Random
		srand(static_cast<unsigned int>(time(0)));
		
		// Game System
		windowWidth = static_cast<f32>(AEGfxGetWindowWidth());
		windowHeight = static_cast<f32>(AEGfxGetWindowHeight());
		halfWindowWidth = windowWidth * 0.5f;
		halfWindowHeight = windowHeight * 0.5f;
		bGamePause = true;

		// Clear all objects at the start of the game
		for (int i = sizeof(objects) / sizeof(objects[0]) - 1; i >= 0; --i)
		{
			GameObject::ClearGameObject(&objects[i]);				
		}

		
		//==================== LEVEL INIT ====================
		// START
		level = new Map2::MapLevel("level1");

		levelNextRoomPlayerPos.x = levelNextRoomPlayerPos.y = 0;
		
		levelRoomSwitchFlag = false;
		levelPlayerDiedSwitchFlag = false;

		// New transition screen code
		levelTS = new GR::TransitionScreen(1.f, static_cast<float>(AEGfxGetWindowWidth()), SwitchRoomProxy);

		// This loop spawns permanent positions
		// aka, Door that will be set active true/false instead of
		// delete and create each time room is switched.
		// Also store spawn points of enemies/loot, which is used to spawn
		// when switch rooms
		for (Map2::MapRoom* r : level->RoomList())
		{
			int const* grid = r->DataGrid();
			int maxrows = r->SizeRows();
			int maxcols = r->SizeCols();
			int roomID = r->RoomID();

			// generate grid objects
			for (int row = 0; row < maxrows; row++)
			{
				for (int col = 0; col < maxcols; col++)
				{
					// Grid starts from top left
					switch (static_cast<Map2::MAP_GRID>(grid[row * maxcols + col]))
					{
					case Map2::MAP_GRID::PLAYER_SPAWN:
					{
						levelNextRoomPlayerPos.x = col * TILE_SIZE + HALF_TILE_SIZE;
						levelNextRoomPlayerPos.y = (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE;

						levelPlayerRespawnPos = levelNextRoomPlayerPos;
						levelPlayerRespawnRoom = roomID;

					}
					break;
					case Map2::MAP_GRID::LOOT_SPAWN:
					{
						// This is the spawnpoint, not actual spawn the enemy
						AEVec2 position{};
						position.x = col * TILE_SIZE + HALF_TILE_SIZE;
						position.y = (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE;
						chestSP.push_back(Chest::ChestSpawnPoint{position, roomID});
		
					}
					break;
					case Map2::MAP_GRID::ENEMY_SPAWN:
					{
						Enemies::storeEnemiesInfo(col * TILE_SIZE + HALF_TILE_SIZE, (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE, roomID, MELEE_ID);
					}
					break;
					case Map2::MAP_GRID::RANGE_SPAWN:
					{
						Enemies::storeEnemiesInfo(col * TILE_SIZE + HALF_TILE_SIZE, (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE, roomID, RANGE_ID);
					}
					break;
					case Map2::MAP_GRID::ELITE_SPAWN:
					{
						Enemies::storeEnemiesInfo(col * TILE_SIZE + HALF_TILE_SIZE, (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE, roomID, ELITE_ID);
					}
					break;
					case Map2::MAP_GRID::BOSS_SPAWN:
					{
						Enemies::storeEnemiesInfo(col * TILE_SIZE + HALF_TILE_SIZE, (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE, roomID, BOSS_ID);
					}
					break;
					case Map2::MAP_GRID::SAVE_POINT:
					{
						// Similar to chest, save the position of save point
						AEVec2 positionSP{};
						positionSP.x = col * TILE_SIZE + HALF_TILE_SIZE;
						positionSP.y = (maxrows - row - 1) * TILE_SIZE + HALF_TILE_SIZE;
						savePointsPos.push_back(PlayerController::SavePointPos{ positionSP, roomID, false });
					}
					default:
						break;
					}
				}
			}
		}
		// generate door objs
		Map2::SpawnDoors(level, TILE_SIZE, HALF_TILE_SIZE, objects);
		//==================== LEVEL INIT ====================

		// Camera
		Camera::CameraInit(camPosX, camPosY, halfWindowWidth, halfWindowHeight);
		currentCamPos = AEVec2{ camPosX, camPosY };
		camDefaultPosX = camPosX;
		camDefaultPosY = camPosY;


		// Cursor
		cursorWorldPosX = 0;
		cursorWorldPosY = 0;
		cursorScreenPosX = 0;
		cursorScreenPosY = 0;

		// Day / Night
		DayNightCycle::Initialize(dnState, dnPrevState,
			dayTime, eveningTime, nightTime, 
			skyColor, bSkyColorTransiting, skyTranisitionTime);

		Orbit::SunMoonInit(sunPos, sunRot, camPosX);

		// Player, position and speed currently same as camera
		isJumping = false;
		timeDelayStatus = false;
		GR::bPlayerArmour = false;
		pBulletList = PlayerController::CreateAndGetPlayerBullets(objects);

		// Create/Check save file status
		if (PlayerController::CreateAndCheckSave(&saveFile, &savePointsPos[0]) == 2) { // Only checking for first SP currently
			// Save file not empty
		}	
		// Reset save point inteactability
		PlayerController::SetSavePoint(nullptr);

		// Physics 
		playerTouchingGround = 1;
		jumpVelocity = defaultJumpVelocity = 650.0f;
		jumpForce = 150.0f; // Temporary
		jumpForceRemaining = 0.0f;
		movementValue = 0.0f;

		// Input
		mouseX = -1;
		mouseY = -1;
		lMouseHeldDown = 0, rMouseHeldDown = 0;

		// Level
		levelXSize = 4800.0f;
		levelYSize = 2650.0f;

		// Buttons
		btnResume.pos = AEVec2{ -180.0f, -350.0f };
		btnResume.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		btnExit.pos = AEVec2{ 180.0f, -350.0f };
		btnExit.size = AEVec2{ BUTTON_SIZE_X, BUTTON_SIZE_Y };

		if ( (GameObject::GetGameObject(GameObject::OBJ_PLAYER, objects)) != nullptr)
		{
			objects[0] = GameObject::CreateGameObject(
				/* type */					GameObject::OBJ_PLAYER,
				/* position */				levelNextRoomPlayerPos, 
				/* width*/					50.0f,
				/* height */				70.0f,
				/* active */				true,
				/* Max HP */				GameObject::PLAYER_MAX_HEALTH,
				/* Speed */					PlayerController::playerSpeed,
				/* Radius */				0.0f,
				/* Lifetime */				7.0f,
				/* levelNumber */			-1

			);
			playerObject = new GameObject::PlayerObject{};
			*playerObject = GameObject::CreatePlayerObject(
				/* GO pointer */			&objects[0], 
				/* solarEnergyMax */		GameObject::SOLAR_ENERGY_MAX_DEFAULT,
				/* solarEnergyChargeRate */	1.0f,
				/* abilityCooldown */		3.0f,
				/* fireRate */				2.0f,
				/* jumpCount */				2,
				/* dashCount */				2
			);
			playerObject->solarEnergyDepleteRate = 2.0f;
			playerObject->buffAttack = static_cast<int>(GameObject::BUFF_DEFAULT_MULTIPLIER);
			playerObject->buffFireRate = GameObject::BUFF_DEFAULT_MULTIPLIER;
			playerObject->buffSolarMax = GameObject::BUFF_DEFAULT_MULTIPLIER;
		}

		// Spawn Chest
		for (size_t i = 0; i < chestSP.size(); ++i)
		{
			Chest::CreateChestThroughSpawnPoint(chestSP[i].pos, objects, chestSP[i].roomID);
		}

		// Spawn Save Point
		for (size_t i = 0; i < savePointsPos.size(); ++i)
		{
			PlayerController::SpawnSavePoint(savePointsPos[i].pos, objects, savePointsPos[i].roomID, savePointsPos[i].currentSP);
		}

		// "Switch" to first room
		levelNextRoomID = level->ActiveRoomID();
		SwitchRooms(true);


		//Sounds
		SoundInit();
		playSounds(SFX_MAINMENU, 0.05f);


		levelVictoryFlag = false;
	
	}
	// End of Initialize

	void Set()
	{
		currentCamPos = AEVec2{ camPosX, camPosY };
		camPosX = camDefaultPosX;
		camPosY = camDefaultPosY;
	}

	void Update()
	{
		actualDeltaTime = static_cast<f32>(AEFrameRateControllerGetFrameTime());

		if (levelVictoryFlag)
		{
			// Keep allow control till timer is over
			if (!GameVictory::DisplayVictory(actualDeltaTime))
			{
				// Quit to main menu
				GSM::stateNext = GS_MAINMENU;
			}
		}

		if (playerObject->solarEnergy <= 0.0f) {
			timeDelayStatus = false;
		}
		if (timeDelayStatus)
		{
			gameDeltaTime = actualDeltaTime * 0.25f;
			playerObject->solarEnergy -= playerObject->solarEnergyDepleteRate * actualDeltaTime;
		}
		else {
			gameDeltaTime = actualDeltaTime;
		}

		// Check if room is to be switched
		if (levelRoomSwitchFlag)
		{
			// Reset mouse input
			lMouseHeldDown = 0, rMouseHeldDown = 0;
			PlayerController::chargeShotTimer = 0.0f;

			// Despawn -1 room ID that are not constant
			// Player bullets
			for (GameObject::GameObject* go : pBulletList) {
				go->healthMaximum = -1;
				playerObject->currentBulletIndex = 0;
			}

			// Transition Screen
			if (levelTS->IsFadingInOut(actualDeltaTime))
			{
				return;
			}
			else
			{
				levelRoomSwitchFlag = false;
				levelPlayerDiedSwitchFlag = false;
			}

		}

		// Make player stop movin when no movement key input...
		playerObject->go->velocity = { 0.0f };

		// Cursor Position
		AEInputGetCursorPosition(&cursorScreenPosX, &cursorScreenPosY);
		cursorScreenPosX -= static_cast<s32>(halfWindowWidth);
		cursorScreenPosY += static_cast<s32>(halfWindowHeight) - static_cast<s32>(windowHeight);
		cursorScreenPosY = -cursorScreenPosY;

		cursorWorldPosX = cursorScreenPosX + static_cast<s32>(camPosX);
		cursorWorldPosY = cursorScreenPosY + static_cast<s32>(camPosY);

		
		// Input
		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			bGamePause = !bGamePause;
		}
		
		if (bGamePause)
		{
			if (AEInputCheckTriggered(AEVK_LBUTTON))
			{
				AEVec2 mousePos{ static_cast<f32>(cursorScreenPosX), static_cast<f32>(cursorScreenPosY) };
				if (Button::checkButtonClick(mousePos, btnResume.pos, btnResume.size))
				{
					bGamePause = !bGamePause;
				}
				if (Button::checkButtonClick(mousePos, btnExit.pos, btnExit.size))
				{
					Enemies::clearBulletsWhenExitClicked(objects);			
					GSM::stateNext = GS_MAINMENU;
				}
			}
		}
		else if (!bGamePause)
		{
			// Temporary time delay block
			if (AEInputCheckTriggered(AEVK_Q))
			{
				if (timeDelayStatus == false /* add cooldown check */) {
					timeDelayStatus = true;
					playSounds(SFX_TIMESLOW, 0.3f);
				}
				else {
					timeDelayStatus = false;
				}
			}

			GR::bPlayerArmour = timeDelayStatus; //<-Player armour display will be properly updated here
			// End temporary time delay
			
			if ( (AEInputCheckTriggered(AEVK_SPACE) || AEInputCheckTriggered(AEVK_W)) && (playerObject->jumpRemaining > 0) )
			{
				jumpForceRemaining = jumpForce;
				isJumping = true;
				playerObject->jumpRemaining -= 1;
			}
			if (AEInputCheckCurr(AEVK_A))
			{
				playerObject->go->velocity.x = -1.0f;
				GR::playerState = GR::GameRenderer::P_LEFT;//For the gamerender to render LEFT
				if (lMouseHeldDown || rMouseHeldDown)
					GR::playerState = GR::GameRenderer::P_LEFT_SHOOT;//For the gamerender to render SHOOT LEFT
			}
			else if (AEInputCheckCurr(AEVK_D))
			{
				playerObject->go->velocity.x = 1.0f;
				GR::playerState = GR::GameRenderer::P_RIGHT;//For the gamerender to render LEFT
				if (lMouseHeldDown || rMouseHeldDown)
					GR::playerState = GR::GameRenderer::P_RIGHT_SHOOT;//For the gamerender to render SHOOT RIGHT
			}
			else
				GR::playerState = GR::GameRenderer::P_IDLE;

			if (AEInputCheckTriggered(AEVK_LBUTTON)) {
				lMouseHeldDown = 1;
			}
			if (AEInputCheckReleased(AEVK_LBUTTON)) {
				lMouseHeldDown = 0;			
			}
			if (AEInputCheckTriggered(AEVK_RBUTTON)) {
				rMouseHeldDown = 1;
				playSounds(SFX_CHARGING, 0.05f);
			}
			if (AEInputCheckReleased(AEVK_RBUTTON)) {
				rMouseHeldDown = 0;
				PlayerController::chargeShotTimer = 0.0f;
			}
			if (lMouseHeldDown && (playerObject->bulletCooldownRemaining <= 0.0f))
			{
				PlayerController::ActivatePlayerBullet(static_cast<float>(cursorWorldPosX), static_cast<float>(cursorWorldPosY), playerObject, pBulletList);
				playerObject->bulletCooldownRemaining = playerObject->bulletCooldown;
				playSounds(SFX_NORMALSHOT, 0.02f);
			}
			else if (rMouseHeldDown && (playerObject->bulletCooldownRemaining <= 0.0f)) {
				PlayerController::chargeShotTimer += actualDeltaTime;			
	
				if (PlayerController::chargeShotTimer >= PlayerController::chargeShotTime) {
					PlayerController::ActivateChargedPlayerBullet(static_cast<float>(cursorWorldPosX), static_cast<float>(cursorWorldPosY), playerObject, pBulletList);
					playerObject->bulletCooldownRemaining = playerObject->bulletCooldown;
					PlayerController::chargeShotTimer = 0.0f;
					playSounds(SFX_CHARGEDSHOT, 0.02f);
				}
			}

			// Interaction
			if (AEInputCheckTriggered(AEVK_F))
			{
				if (Chest::GetInteractableObject()) {
					Chest::OpenChest(Chest::GetInteractableObject(), objects); // Chest
				}
				else if (PlayerController::GetSavePoint()) {
					PlayerController::UseSavePoint(PlayerController::GetSavePoint()); // Use save point
					if (PlayerController::GetSavePoint()->knockbackState) {
						PlayerController::WriteToSaveFile(&saveFile, playerObject, PlayerController::GetSavePoint());
					}
					else {
						PlayerController::ClearSaveFile(&saveFile);
					}	
				}
			}

			// Testing / Cheat codes
			if (AEInputCheckCurr(AEVK_LSHIFT) && AEInputCheckTriggered(AEVK_3))
			{
				playerObject->go->healthCurrent = 3;
			}
			if (AEInputCheckCurr(AEVK_LSHIFT) && AEInputCheckTriggered(AEVK_G))
			{
				Drops::MaxBuffCheats(playerObject);
			}
		}
		// End of Input
		

		// Game State Logic
		if (bGamePause)
		{
			// Set your if statement of why game is pause
			// pauseMenu open, shop etc

		}
		else if (!bGamePause) // Game logic
		{
			// Throw your logic code here

#pragma region DAY / NIGHT STATE
			// Day/Night State
			DayNightUpdate(dnState, dnPrevState, dayTime, eveningTime, nightTime, bSkyColorTransiting, gameDeltaTime);

			// Change the color of the sky
			DayNightCycle::SkyColorTransition(bSkyColorTransiting, skyTranisitionTime, dnPrevState, skyColor, gameDeltaTime);

			if (dayTime < 0.0)
			{
				DayNightStateChange(dnState, dayTime, sunRot);
			}
			else if (eveningTime < 0.0)
			{
				DayNightStateChange(dnState, eveningTime, sunRot);
			}
			else if (nightTime < 0.0)
			{
				DayNightStateChange(dnState, nightTime, sunRot);
			}

			// Rotation of Sun and Moon
			sunRot += Orbit::SunMoonRotationUpdate(dnState, gameDeltaTime);

#pragma endregion // DAY / NIGHT STATE

			// Player State (Dead or Alive)
			// AI State (Dead or Alive)

			// Physics / Movement here
			if (isJumping) {
				movementValue = jumpVelocity * actualDeltaTime;
				playerObject->go->position.y += movementValue;
				jumpForceRemaining -= movementValue;
				if (jumpForceRemaining <= 0) {
					isJumping = false;
				}
			}
			playerObject->go->position.y += GameObject::GRAVITY_DEFAULT * 28 * actualDeltaTime;
			// Add gravity for others...?

			
			// GameObject Position Update / Velocity Update / Logic
			for (int t = sizeof(objects) / sizeof(objects[0]) - 1; t >= 0; --t)
			{
				if (!objects[t].active)
					continue;
				switch (objects[t].type)
				{
				case GameObject::OBJ_PLAYER:
				{
					// Check and handle player death
					if (objects[t].healthCurrent <= 0)
					{
						PlayerDied();
					}
					// End player death

					// Shooting logic
					playerObject->bulletCooldownRemaining -= actualDeltaTime;


					playerObject->go->position.x += playerObject->go->velocity.x * playerObject->go->speed * actualDeltaTime ;

					// Keep player in map boundary
					playerObject->go->position.x = AEClamp(playerObject->go->position.x, HALF_TILE_SIZE, levelXSize * 0.5f - HALF_TILE_SIZE);
					playerObject->go->position.y = AEClamp(playerObject->go->position.y, HALF_TILE_SIZE, levelYSize * 0.5f - HALF_TILE_SIZE);

					// Regen solar engine when during day and evening time
					if (!timeDelayStatus)
					{
						if (dnState == DayNightCycle::STATE_DAY || dnState == DayNightCycle::STATE_EVENING)
							playerObject->solarEnergy += playerObject->solarEnergyChargeRate * gameDeltaTime;
						if (playerObject->solarEnergy >= playerObject->solarEnergyMax)
							playerObject->solarEnergy = playerObject->solarEnergyMax;
					}

					break;
				}
				case GameObject::OBJ_ALLY_PROJECTILE:

					if (objects[t].active == true && objects[t].healthMaximum != -1)
					{
						ObjectPhysics::updatePosition(&objects[t], actualDeltaTime);
						objects[t].lifetime -= actualDeltaTime;
						if (objects[t].lifetime <= 0)
						{
							objects[t].healthMaximum = -1;
						}
					}
					break;

				case GameObject::OBJ_ENEMY:
				{
					if (objects[t].healthCurrent <= 0)
					{
						f32 enemyX = objects[t].position.x, enemyY = objects[t].position.y;
						m_ParticleSystem.SpawnEnemyParticle(enemyX, enemyY);
						Drops::CreateDrops(objects[t].position, objects);
						GameObject::ClearGameObject(&objects[t]);

					}
					if (objects[t].knockbackState == true) {
						ObjectPhysics::applyKnockback(&objects[t], gameDeltaTime);
					}
					else
					{
						Enemies::enemyUpdate(&objects[t],&objects[0], objects, gameDeltaTime);
					}
					break;
				}
				case GameObject::OBJ_ENEMY_ELITE:
				{
					if (objects[t].healthCurrent <= 0)
					{

						f32 enemyX = objects[t].position.x, enemyY = objects[t].position.y;
						m_ParticleSystem.SpawnEnemyParticle(enemyX, enemyY);
						Drops::CreateDrops(objects[t].position, objects);
						GameObject::ClearGameObject(&objects[t]);
					}
					Enemies::eliteEnemyUpdate(&objects[t],&objects[0], gameDeltaTime);
					break;
				}
				case GameObject::OBJ_ENEMY_BOSS:
				{
					if (objects[t].healthCurrent <= 0)
					{
						levelVictoryFlag = true;

						f32 enemyX = objects[t].position.x, enemyY = objects[t].position.y;
						m_ParticleSystem.SpawnEnemyParticle(enemyX, enemyY);
						Drops::CreateDrops(objects[t].position, objects);
						GameObject::ClearGameObject(&objects[t]);
					}
						Enemies::bossUpdate(&objects[t], objects[0], objects, gameDeltaTime);
						break;
				}
				case GameObject::OBJ_ENEMY_PROJECTILE:
				{
					Enemies::enemyProjUpdate(&objects[t], gameDeltaTime);
					break;
				}				
				case GameObject::OBJ_ATK_UP:
				case GameObject::OBJ_PWR_UP:
				case GameObject::OBJ_SOLAR_UP:
				case GameObject::OBJ_HP_REGEN:
				case GameObject::OBJ_SOLAR_REGEN:
					Drops::DropsMovementUpdate(objects[t], gameDeltaTime);
					break;
				case GameObject::OBJ_DOOR:
					break;
				case GameObject::OBJ_GRID:
					break;
				case GameObject::OBJ_CLEAR:
					break;
				default:
					break;
				}
			}
			
#pragma region CAMERA UPDATE
			// update camera position with lerp
			Camera::CameraPositionUpdate(camPosX, camPosY, currentCamPos, playerObject);
			
			Camera::CameraLock(camPosX, camPosY, halfWindowWidth, halfWindowHeight, levelXSize, levelYSize);
#pragma endregion // CAMERA UPDATE

			// Update Sun and Moon Position
			Orbit::SunMoonUpdate(sunPos, currentCamPos, halfWindowHeight);

			// Collision(?) here
			// GameObject Collision Check
			for (int t = sizeof(objects) / sizeof(objects[0]) - 1; t >= 0; --t)
			{
				if (!objects[t].active)
					continue;

				switch (objects[t].type)
				{
				case GameObject::OBJ_PLAYER:
				{
					for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++)
					{
						if (objects[i].type == GameObject::OBJ_GRID)
						{
							int flag = 0;

							flag = Collision::checkBinarySquaresIntersection(&objects[t], &objects[i]);
							if (flag & Collision::COLLISION_BOTTOM)
							{
								playerTouchingGround = 1; // Currently this variable unncessary
								playerObject->jumpRemaining = playerObject->jumpCount;
								objects[t].velocity.y = 0.0f;
								Collision::snapToPositionPlayer(&objects[t], &objects[i], Collision::FLAG_BOTTOM);
							}

							flag = Collision::checkBinarySquaresIntersection(&objects[t], &objects[i]);
							if (flag & Collision::COLLISION_RIGHT)
							{
								Collision::snapToPositionPlayer(&objects[t], &objects[i], Collision::FLAG_RIGHT);
							}
							if (flag & Collision::COLLISION_LEFT)
							{
								Collision::snapToPositionPlayer(&objects[t], &objects[i], Collision::FLAG_LEFT);
							}
							if (flag & Collision::COLLISION_BOTTOM)
							{
								playerTouchingGround = 1; // Currently this variable unncessary
								playerObject->jumpRemaining = playerObject->jumpCount;
								objects[t].velocity.y = 0.0f;
								Collision::snapToPositionPlayer(&objects[t], &objects[i], Collision::FLAG_BOTTOM);
							}
							if (flag & Collision::COLLISION_TOP)
							{
								Collision::snapToPositionPlayer(&objects[t], &objects[i], Collision::FLAG_TOP);
							}
							
							if (flag != Collision::Collision_Flag::FLAG_BOTTOM) {
								playerTouchingGround = false;
							}
							
						}
					}
					break;
				}
				case GameObject::OBJ_ALLY_PROJECTILE:
				{
					if (objects[t].healthMaximum != -1)
					{
						for (int g = (sizeof(objects) / sizeof(objects[0])) - 1; g >= 0; g--)
						{		
							if (objects[t].knockbackType == -10) {
								if ( (objects[g].type == GameObject::OBJ_GRID || objects[g].type == GameObject::OBJ_DOOR) && objects[g].active) {
									if (Collision::checkSquaresIntersection(&objects[t], &objects[g])) {
										m_ParticleSystem.SpawnBulletParticle(&objects[t]);
										// Play hit non-enemy sound?
										objects[t].healthMaximum = -1; // Reset bullet
										objects[t].position = AEVec2{ -1.0f, -1.0f };
									}
								}
							}

							if (objects[g].type == GameObject::OBJ_ENEMY || objects[g].type == GameObject::OBJ_ENEMY_ELITE || objects[g].type == GameObject::OBJ_ENEMY_BOSS)
							{
								if (Collision::checkSquaresIntersection(&objects[t], &objects[g]))
								{
									m_ParticleSystem.SpawnBulletParticle(&objects[t]);
									playSounds(SFX_ENEMYHIT, 0.05f); //Enemy hit sound
									objects[g].healthCurrent -= objects[t].healthMaximum; // Adjust damage based on buff
									objects[t].healthMaximum = -1; // Reset bullet
									objects[t].position = AEVec2{ -1.0f, -1.0f };
		
									if (objects[g].healthMaximum != RANGE_ID) { // CURRENTLY RANGE ENEMY IMMUNE TO KNOCKBACK
										// Knockback apply
										objects[g].knockbackState = true;

										objects[g].knockbackDurationRemaining = 0.1f;
										if (objects[t].velocity.x >= 0) { // Bullet travelling right
											objects[g].knockbackType = 1; // Knockback right
										}
										else if (objects[t].velocity.x < 0) { // Bullet travelling left
											objects[g].knockbackType = 0; // Knockback left
										}
									}																
									
								}
							}
						}
					}
					break;
				}
					break;
				case GameObject::OBJ_ENEMY:
				{
					for (int g = (sizeof(objects) / sizeof(objects[0]))-1; g >= 0; g--)
					{
						if (objects[g].type == GameObject::OBJ_GRID)//collision with walls 
						{
						
							if (objects[g].active == TRUE)
							{
								Enemies::enemyCollisionUpdate(&objects[t], &objects[g]);
							}							
						}
						if (objects[g].type == GameObject::OBJ_PLAYER)
						{
							Enemies::enemyTouchPlayerUpdate(&objects[t], &objects[g], gameDeltaTime);	
						}
					}
					break;
				}			
				case GameObject::OBJ_ENEMY_ELITE:
				{
					for (int g = (sizeof(objects) / sizeof(objects[0])) - 1; g >= 0; g--)
					{
						if (objects[g].type == GameObject::OBJ_GRID)//collision with walls 
						{
							if (objects[g].active == TRUE)
							{
								Enemies::enemyCollisionUpdate(&objects[t], &objects[g]);
							}
						}
						if (objects[g].type == GameObject::OBJ_PLAYER)
						{
							Enemies::enemyTouchPlayerUpdate(&objects[t], &objects[g], gameDeltaTime);
						}
					}	
				}
					break;
				case GameObject::OBJ_ENEMY_BOSS:
				{

				}
					break;
				case GameObject::OBJ_ENEMY_PROJECTILE:
				{
					if (Enemies::enemyProjectileUpdate(&objects[t], &objects[0]) == PLAYER_DEAD)
					{
						PlayerDied();
					}			
				}
					break;
				case GameObject::OBJ_SP:
				{
					if (Collision::checkSquaresIntersection(playerObject->go, &objects[t]))
					{ 
						PlayerController::SetSavePoint(&objects[t]);
					}
					else
					{
						PlayerController::SetSavePoint(nullptr);
					}
					break;
				}
				case GameObject::OBJ_CHEST:
				{
					if (Chest::GetInteractableObject()
						&& Collision::checkSquaresIntersection(playerObject->go, Chest::GetInteractableObject()))
					{
						break;
					}
					else if (!objects[t].loots.bOpened && Collision::checkSquaresIntersection(playerObject->go, &objects[t]))
					{
						Chest::SetInteractableObject(&objects[t]);
					}
					else
					{
						Chest::SetInteractableObject(nullptr);
					}
					break;
				}
				case GameObject::OBJ_ATK_UP:
				case GameObject::OBJ_PWR_UP:
				case GameObject::OBJ_SOLAR_UP:
				case GameObject::OBJ_HP_REGEN:
				case GameObject::OBJ_SOLAR_REGEN:
				{
					if (!objects[t].loots.bCollision)
					{
						Drops::NoCollisionTime(&objects[t], actualDeltaTime);
					}
					else
					{
						Drops::LootLifeTimeUpdate(&objects[t], gameDeltaTime);

						// Collision check with player
						if (Collision::checkSquaresIntersection(&objects[t], playerObject->go))
						{
							Drops::PickUpDrops(&objects[t], playerObject, m_ParticleSystem);
							GameObject::ClearGameObject(&objects[t]);
						}
						// Collision check with grids
						for (int i = GameObject::MINGRID; i <= GameObject::MAXGRID; ++i)
						{
							if (!objects[i].active)
								continue;

							Drops::LootCollisionToGridCheck(&objects[t], &objects[i]);		
						}
					}
					break;
				}
				case GameObject::OBJ_DOOR:
					// Check player collide with door
					if (Collision::checkSquaresIntersection(&objects[t], playerObject->go))
					{
						// Go to next room
						PrepareSwitchRooms(&objects[t]);
					}
					break;
				case GameObject::OBJ_GRID:
					// Player, enemy etc should check collide with this
					// This is the map's collision
				
					break;
				case GameObject::OBJ_CLEAR:
					break;
				default:
					break;
				}
			}

			
		}
		// End of Game State Logic


		//particle system update
		m_ParticleSystem.OnParticleUpdate(actualDeltaTime); // CONSIDER IF SHOULD BE AFFECTED BY TIME DELAY
		m_ParticleSystem.OnBuffParticleUpdate(actualDeltaTime, playerObject); // CONSIDER IF SHOULD BE AFFECTED BY TIME DELAY
	}

	void Render()
	{
		
		// Draw / Render - note that it has render priority
		// Put your code here if its always rendering
		// Set the background to sky color
		AEGfxSetBackgroundColor(skyColor.r, skyColor.g, skyColor.b);

		// Enviroment Background
		gameRender->RenderEnvironment(windowWidth, windowHeight, camPosX, camPosY);

		// Sun and Moon
		gameRender->RenderSunMoon(sunRot, halfWindowWidth, sunPos, dnState);
		
		// RENDER BG GRID LEVEL MAP
		gameRender->RenderMapBG(level, TILE_SIZE, HALF_TILE_SIZE);

		// GameObject
		gameRender->RenderGameObjects(playerObject, m_ParticleSystem, levelRoomSwitchFlag);


		// INTERACTABLE CHEST TEXT
		if (Chest::GetInteractableObject())
			gameRender->RenderInteractableChestText(Chest::GetInteractableObject());
		// Using same interactable text as chest
		if (PlayerController::GetSavePoint())
			gameRender->RenderInteractableChestText(PlayerController::GetSavePoint());

		// Pause 
		// Render the assets for pausing
		// Put your code here if it only Render during Pause state 
		if (bGamePause)
		{
			// Pause Menu
			gameRender->RenderPause(windowWidth, windowHeight, camPosX, camPosY, btnResume, btnExit);
		}	
		else // ONLY WHEN GAME UNPAUSE RENDER
		{
			// Player UI
			gameRender->RenderPlayerUI(playerObject, AEVec2{ camPosX ,camPosY}, currentCamPos);
		}

		// This renders the transition screen
		if (levelRoomSwitchFlag)
		{
			gameRender->RenderTransition(windowWidth, windowHeight, levelTS->PositionX(), camPosX, camPosY, levelPlayerDiedSwitchFlag);
		}

		if (levelVictoryFlag)
		{
			gameRender->RenderVictoryScreen();
		}
		// End of Draw / Render
	}

	void Unset()
	{
		// Free - Clean game objects instances
		for (int i = sizeof(objects) / sizeof(objects[0]) - 1; i >= 0; --i)
		{
			GameObject::ClearGameObject(&objects[i]);
		}
		
		// If going next level
		// Anything that needs to be saved for next level
		if (GSM::stateNext != GS_MAINMENU)
			DayNightCycle::Save(dnState, dayTime, eveningTime, nightTime);

		// End of Free
	}

	void Exit()
	{
		// Clear vectors
		chestSP.clear();
		savePointsPos.clear();
		pBulletList.clear();

		// Renders
		delete gameRender;

		// Enemy
		Enemies::enemyVecClear();

		// Sounds
		unloadSounds();

		// player
		delete playerObject;

		// level
		delete level;
		// transition screen
		delete levelTS;
	}

	// This switches the room to the next one
	void PrepareSwitchRooms(GameObject::GameObject const* doorGOThatCollided)
	{
		// Reset mouse inputs for shooting
		lMouseHeldDown = 0, rMouseHeldDown = 0;
		PlayerController::chargeShotTimer = 0;

		// Reset player bullets
		playerObject->currentBulletIndex = 0;
		for (size_t i = 0; i < pBulletList.size(); i++) {
			pBulletList[i]->healthMaximum = -1;
		}

		// Find in level, which door this GO belongs to
		Map2::MapDoor* collidedDoor{ nullptr };
		// Go through all the Doors and find which door has same GO that detected collision with player
		for (Map2::MapDoor* d : level->DoorList())
		{
			if (d->AssignedGO() == doorGOThatCollided) {
				collidedDoor = d;
				break;
			}
		}

		if (collidedDoor == nullptr) {
			//SwitchRooms() unable to find door!
			return;
		}

		// See which room this door is pointing to
		levelNextRoomID = collidedDoor->LinkedDoor()->RoomLocatedIn()->RoomID();
		
		// Set flag and make preparation to switch
		int doorPosX{}, doorPosY{};
		float playerOffsetX{}, playerOffsetY{};
		collidedDoor->LinkedDoor()->GetDoorPosXY(doorPosX, doorPosY);
		// Find out how to know door's dir (when loading?) "><^v"
		// Offset position based on exit direction
		switch (collidedDoor->LinkedDoor()->ExitDirection())
		{
		case Map2::MapDoor::DOOR_EXIT_DIR::EXIT_LEFT:
			doorPosX -= 1;
			playerOffsetX = -1 * (0.2f * playerObject->go->width);
			break;
		case Map2::MapDoor::DOOR_EXIT_DIR::EXIT_RIGHT:
			doorPosX += 1;
			playerOffsetX = (0.2f * playerObject->go->width);
			break;
		case Map2::MapDoor::DOOR_EXIT_DIR::EXIT_UP:
			doorPosY += 1;
			playerOffsetY = (0.2f * playerObject->go->height);
			break;
		case Map2::MapDoor::DOOR_EXIT_DIR::EXIT_DOWN:
			doorPosY -= 1;
			playerOffsetY = -1 * (0.2f * playerObject->go->height);
			break;
		default:
			break;
		}
		// offset based on tile size, etc
		// DO NOT SET PLAYER POSITION YET
		// SETTING HAPPENS IN SWITCHROOMS()
		levelNextRoomPlayerPos.x = doorPosX * TILE_SIZE + HALF_TILE_SIZE + playerOffsetX;
		levelNextRoomPlayerPos.y = doorPosY * TILE_SIZE + HALF_TILE_SIZE + playerOffsetY;
		levelRoomSwitchFlag = true;
	}
	void SwitchRooms(bool firstTime)
	{
		// Reset mouse inputs for plyaer shooting
		lMouseHeldDown = 0, rMouseHeldDown = 0;
		PlayerController::chargeShotTimer = 0;

		// Reset player bullets
		playerObject->currentBulletIndex = 0;
		for (size_t i = 0; i < pBulletList.size(); i++) {
			pBulletList[i]->healthMaximum = -1;
		}

		// 1)Set curr room GO to inactive
		int currRoomID = level->ActiveRoomID();

		if (firstTime)
		{
			// First time init
			currRoomID = -99;
		}

		//==============================================================================
		// Better version: Destroy all current grid (currroom) and create new (nextroom)
		for (int i{ GameObject::MINGRID }; i < GameObject::MAXGRID; ++i)
		{
			// Destroy all current grid
			if (objects[i].active)
				GameObject::ClearGameObject(&objects[i]);
		}

		// Set next room
		level->SetActiveRoom(levelNextRoomID);

		// Read room again and create grids
		int nextRoomMaxRows{}, nextRoomMaxCols{};
		level->GetActiveRoomSize(nextRoomMaxCols, nextRoomMaxRows);
		Map2::SpawnCollidables(level, TILE_SIZE, HALF_TILE_SIZE, objects);
		//==============================================================================

		// Change relevant GO in:
		// CURRENT room to inactive
		// NEXT room to active
		for (int i = 0; i < sizeof(objects)/sizeof(objects[0]); i++)
		{
			// Objects with CURRENT and NEXT room will be touched ONLY
			// Player should set to room "-1" to not be touched
			if (objects[i].levelNumber == currRoomID) {
				switch (objects[i].type)
				{
				case GameObject::OBJ_DOOR:
				{
					// Only Door will always set inactive
					// prevents door from disapearing
					if (currRoomID == levelNextRoomID)
						break;

					objects[i].active = false;
					break;
				}
				case GameObject::OBJ_CHEST:
				{
					objects[i].active = false;
					objects[i].loots.bOpened = false;
					break;
				}
				case GameObject::OBJ_SP:
				{
					objects[i].active = false;
					break;
				}
				case GameObject::OBJ_ENEMY:
				{
					GameObject::ClearGameObject(&objects[i]);
					break;
				}
				case GameObject::OBJ_ENEMY_ELITE:
				{
					GameObject::ClearGameObject(&objects[i]);
					break;
				}
				case GameObject::OBJ_ENEMY_BOSS:
				{
					GameObject::ClearGameObject(&objects[i]);
					break;
				}
				default:
					break;
				}
				
			}
			if (objects[i].levelNumber == levelNextRoomID) {
				switch (objects[i].type)
				{
				// Only Door will always set active
				case GameObject::OBJ_DOOR:
				{
					objects[i].active = true;
					break;
				}
				case GameObject::OBJ_CHEST:
				{
					int random = rand() % 5 + 1;
					if (random <= 4) // 80% spawn rate
					{
						objects[i].active = true;
						objects[i].loots.bOpened = false;
					}
					break;
				}
				case GameObject::OBJ_SP:
				{
					objects[i].active = true;
					break;
				}
				default:
					break;
				}
			}
			if (objects[i].levelNumber == -2) // clear all projectiles
			{
				Enemies::removeBullet(objects[i].enemyFlagsID);// To turn off active flag for enemy bullet inside Enemies.cpp
				//Need to put removeBullet function before clearing gameObj
				//clearGameObj will set the flagsID to 0 making it imppossile to set
				//the bullet flag to false
				GameObject::ClearGameObject(&objects[i]);		
				
			}
		}

		Enemies::spawnEnemies(objects, levelNextRoomID);

		// Set new limits
		levelXSize = nextRoomMaxCols * TILE_SIZE * 2.f;
		levelYSize = nextRoomMaxRows * TILE_SIZE * 2.f;

		// 3) Set player pos to door pos (offseted)
		playerObject->go->position = levelNextRoomPlayerPos;
        
		camPosX = levelNextRoomPlayerPos.x;
		camPosY = levelNextRoomPlayerPos.y;
		// Lock camera
		Camera::CameraLock(camPosX, camPosY, halfWindowWidth, halfWindowHeight, levelXSize, levelYSize);
		// Update Sun and Moon Position
		Orbit::SunMoonUpdate(sunPos, AEVec2{ camPosX , camPosY}, halfWindowHeight);
		// Set camera pos
		AEGfxSetCamPosition(camPosX, camPosY);
		
		// Chest
		Chest::SetInteractableObject(nullptr);

		// Save point
		PlayerController::SetSavePoint(nullptr);

		// Particle
		m_ParticleSystem.OnParticleSetFalse();

		// Cleaning part
		levelNextRoomID = -1;
	}

	// Called from transition screen
	void SwitchRoomProxy()
	{
		// Unless want transition at start of game, else this is fine.
		SwitchRooms();
	}

	void PlayerDied()
	{
		// dont die if win
		if (levelVictoryFlag)
			return;

		// Called when player dies,
		// Reset mouse input
		lMouseHeldDown = 0;
		rMouseHeldDown = 0;
		PlayerController::chargeShotTimer = 0.0f;

		
		// reset stats
		// health
		playerObject->go->healthCurrent = playerObject->go->healthMaximum;
		
		// solar energy
		playerObject->solarEnergyMax = GameObject::SOLAR_ENERGY_MAX_DEFAULT;
		playerObject->solarEnergy = playerObject->solarEnergyMax;
		playerObject->solarEnergyDepleteRate = 2.0f;

		// buffs
		playerObject->buffAttack = static_cast<int>(GameObject::BUFF_DEFAULT_MULTIPLIER);
		playerObject->buffFireRate = GameObject::BUFF_DEFAULT_MULTIPLIER;
		playerObject->buffSolarMax = GameObject::BUFF_DEFAULT_MULTIPLIER;
		
		// velocity
		playerObject->go->velocity = { 0.0f };

		// bullets
		playerObject->currentBulletIndex = 0;


		// Go back to starting room and pos
		levelRoomSwitchFlag = true;
		levelPlayerDiedSwitchFlag = true;

		if (PlayerController::ReadFromSaveFile("save_file.txt", playerObject)) {
			levelNextRoomID = playerObject->go->levelNumber;
			levelNextRoomPlayerPos = playerObject->go->position;
		}
		else {
			levelNextRoomID = levelPlayerRespawnRoom;
			levelNextRoomPlayerPos = levelPlayerRespawnPos;
		}
		
	}
} // namespace Level1