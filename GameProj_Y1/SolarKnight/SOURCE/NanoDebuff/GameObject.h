/*!************************************************************************
\file       GameObject.h
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (90%)
			Chng Kai Rong, Leonard (k.chng) (5%)
			AUTHOR (5%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "pch.h"

namespace GameObject{
	// const
	const int PLAYER_MAX_HEALTH = 20;
	const f32 GRAVITY_DEFAULT = -9.8f;
	const f32 SOLAR_ENERGY_MAX_DEFAULT = 50.0f;
	const f32 SOLAR_ENERGY_MAX_BUFF = 2.0f;
	const f32 FIRE_RATE_MAX_BUFF = 3.0f;
	const f32 BUFF_DEFAULT_MULTIPLIER = 1.0f;
	const int ATTACK_MAX_BUFF = 3;

	enum GameObject_Type
	{
		OBJ_PLAYER = 0,
		OBJ_ALLY_PROJECTILE,
		OBJ_ENEMY,
		OBJ_ENEMY_ELITE,
		OBJ_ENEMY_BOSS,
		OBJ_ENEMY_PROJECTILE,
		OBJ_CHEST,
		OBJ_SP,
		OBJ_ATK_UP,
		OBJ_PWR_UP,
		OBJ_SOLAR_UP,
		OBJ_HP_REGEN,
		OBJ_SOLAR_REGEN,
		OBJ_DOOR,
		OBJ_GRID,
		OBJ_CLEAR
	};

	struct Loots {
		bool bOpened = false;
		bool bCollision = false;
		f32 noCollisionTime = 1.0f;
	};

	struct GameObject {

		GameObject_Type type{};
		AEVec2 position{};		//use to get the position of the circle objects
		AEVec2 minPosition{};
		AEVec2 maxPosition{};
		AEVec2 velocity{};
		f32 radius{};				//use to get the radius of the circle object
		f32 width{};
		f32 height{};
		bool active = false;
		int healthMaximum{};
		int healthCurrent{};
		f32 speed{};
		f32 lifetime{};
		int levelNumber = -1;
		int enemyFlagsID{}; // mainly used inside enemy.cpp 
		f32 gravity{};

		// State variables
		bool knockbackState = false;
		int knockbackType{}; // 0 for left, 1 for right, other here...
		f32 knockbackDurationRemaining{};
		// End state variables

		Loots loots{};
	};

	typedef struct PlayerObject {
		GameObject* go{};

		f32 solarEnergy{};
		f32 solarEnergyDepleteRate{};
		f32 solarEnergyMax{};
		f32 solarEnergyChargeRate{};
		f32	abilityCooldownRemaining{};
		f32 abilityCooldown{};
		f32 fireRate{};
		f32 bulletCooldownRemaining{};
		f32 bulletCooldown{};
		int jumpCount{};
		int jumpRemaining{};
		int dashRemaining{};
		int dashCount{};
		int currentBulletIndex{};

		// buff multiplier
		int buffAttack   = 1;
		f32 buffFireRate = 1.0f;
		f32 buffSolarMax = 1.0f;

	} PlayerObject;


	GameObject CreateGameObject( // Default game object
		GameObject_Type	type,
		AEVec2			position,		//use to get the position of the circle objects
		f32				width,
		f32				height,
		bool			active,
		int				healthMaximum,
		f32				speed,
		f32				radius,
		f32				lifetime,
		int				levelNumber
	);

	PlayerObject CreatePlayerObject(	// Player object

		GameObject*		go,
		f32				solarEnergyMax,
		f32				solarEnergyChargeRate, 
		f32				abilityCooldown,
		f32				fireRate,
		int				jumpCount,
		int				dashCount

	);

	void ClearGameObject(GameObject* object); // Default game object
	void ClearGameObject(PlayerObject* object); // Player version


	GameObject* GetGameObject(GameObject_Type type, GameObject* objList);

	const int MINALLYPROJECTILE = 1; 
	const int MAXALLYPROJECTILE = 50;
	const int MINENEMY = 71;
	const int MAXENEMY = 90;
	const int MINENEMYELITE = 91;
	const int MAXENEMYELITE = 93;
	const int MINENEMYBOSS = 94;
	const int MAXENEMYBOSS = 95; 
	const int MINENEMYPROJECTILE = 96;
	const int MAXENEMYPROJECTILE = 120;
	const int MINPOWERUPS = 121;
	const int MAXPOWERUPS = 165;
	const int MINSP = 166;
	const int MAXSP = 170;
	const int MINCHEST = 171;
	const int MAXCHEST = 180;
	const int MINDOOR = 181;
	const int MAXDOOR = 190;
	const int MINGRID = 191;
	const int MAXGRID = 550;
	const int MAXGAMEOBJECT = MAXGRID;
	
}

#endif GAMEOBJECT_H
