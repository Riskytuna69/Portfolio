/*!************************************************************************
\file       GameObject.cpp
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (80%)
			Chng Kai Rong, Leonard (k.chng) (10%)
			Min Khant Ko (ko.m) (10%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "GameObject.h"

namespace GameObject {

	GameObject CreateGameObject(	// Default game object
		GameObject_Type	type,
		AEVec2			position,		//use to get the position of the circle objects
		f32				width,
		f32				height,
		bool			active,
		int				healthMaximum,
		f32				speed,
		f32				radius,
		f32				lifetime,
		int				levelNumber)
	{
		GameObject object;
		object.type = type;
		AEVec2Set(&position, position.x, position.y);
		object.position.x = position.x;
		object.position.y = position.y;
		object.width = width;
		object.height = height;
		object.active = active;
		object.healthMaximum = healthMaximum;
		object.healthCurrent = healthMaximum;
		object.speed = speed;
		object.radius = radius;
		object.lifetime = lifetime;
		object.levelNumber = levelNumber;
		object.gravity = 0.0f;
		object.enemyFlagsID = 0; //mainly used inside enemy.cpp 
		return object;
	}

	PlayerObject CreatePlayerObject(	// Player object

		GameObject*		go, // Pointer to game object portion
		f32				solarEnergyMax,
		f32				solarEnergyChargeRate,
		f32				abilityCooldown,
		f32				fireRate,
		int				jumpCount,
		int				dashCount

	)
	{
		PlayerObject object;
		object.go = go;
		
		object.solarEnergy = solarEnergyMax;
		object.solarEnergyMax = solarEnergyMax;
		object.solarEnergyChargeRate = solarEnergyChargeRate;
		object.abilityCooldown = abilityCooldown;
		object.fireRate = fireRate;
		object.bulletCooldown = (1.0f / fireRate);
		object.bulletCooldownRemaining = (1.0f / fireRate);
		object.jumpCount = jumpCount;
		object.jumpRemaining = jumpCount;
		object.dashCount = dashCount;
		object.dashRemaining = dashCount;

		return object;
	}


	void ClearGameObject(GameObject* object)	// Default game object
	{
		object->type = OBJ_CLEAR;
		object->position.x = 0.0f;
		object->position.y = 0.0f;
		object->width = 0.0f;
		object->height = 0.0f;
		object->active = false;
		object->healthMaximum = 0;
		object->healthCurrent = 0;
		object->speed = 0.0f;
		object->radius = 0.0f;
		object->lifetime = 0.0f;
		object->levelNumber = -1;
		object->gravity = 0.0f;
		object->enemyFlagsID = false; //mainly used inside enemy.cpp 
	}

	void ClearGameObject(PlayerObject* object)	// Player version
	{
		ClearGameObject(object->go);

		object->solarEnergy = 0.0f;
		object->solarEnergyMax = 0.0f;
		object->solarEnergyChargeRate = 0.0f;
		object->abilityCooldownRemaining = 0.0f;
		object->abilityCooldown = 0.0f;
		object->fireRate = 0.0f;
		object->bulletCooldownRemaining = 0.0f;
		object->bulletCooldown = 0.0f;
		object->jumpCount = 0;
		object->jumpRemaining = 0;
		object->dashRemaining = 0;
		object->dashCount = 0;

	}

	GameObject* GetGameObject(GameObject_Type type, GameObject* objList) // Works for all GameObject and children struts
	{
		switch (type)
		{
		case OBJ_PLAYER:
		{
			if (!objList[0].active)
				return &objList[0];
			else {
				// Player already exists
			}
			break;
		}
		case OBJ_ALLY_PROJECTILE:
		{
			for (int i = MINALLYPROJECTILE; i < MAXALLYPROJECTILE; i++)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_ENEMY:
		{
			for (int i = MINENEMY; i < MAXENEMY; i++)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_ENEMY_ELITE:
		{
			for (int i = MINENEMYELITE; i < MAXENEMYELITE; i++)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_ENEMY_BOSS:
		{
			for (int i = MINENEMYBOSS; i < MAXENEMYBOSS; i++)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_ENEMY_PROJECTILE:
		{
			for (int i = MINENEMYPROJECTILE; i < MAXENEMYPROJECTILE; i++)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_ATK_UP:
		case OBJ_PWR_UP:
		case OBJ_SOLAR_UP:
		case OBJ_HP_REGEN:
		case OBJ_SOLAR_REGEN:
		{
			for (int i = MINPOWERUPS; i < MAXPOWERUPS; ++i)
			{
				if (!objList[i].active)
					return &objList[i];
			}
			break;
		}
		case OBJ_CHEST:
		{
			for (int i = MINCHEST; i < MAXCHEST; ++i)
			{
				if (!objList[i].active && objList[i].levelNumber == -1)
					return &objList[i];
			}
			break;
		}
		case OBJ_SP:
		{
			for (int i = MINSP; i < MAXSP; ++i)
			{
				if (!objList[i].active && objList[i].levelNumber == -1)
					return &objList[i];
			}
			break;
		}
		case OBJ_DOOR:
		{
			for (int i = MINDOOR; i < MAXDOOR; ++i)
			{
				if (!objList[i].active && objList[i].levelNumber == -1)
					return &objList[i];
			}
			break;
		}
		case OBJ_GRID:
		{
			for (int i = MINGRID; i < MAXGRID; ++i)
			{
				if (!objList[i].active && objList[i].levelNumber == -1)
					return &objList[i];
			}
			break;
		}
		default:
		{
			printf("Tried to get an object with no type!\n");
			break;
		}
		}
		printf("Not enough objects! %d\n", type);
		return NULL;
	}

}
