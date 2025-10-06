/*!************************************************************************
\file       Drops.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"
#include "GameObject.h"
#include "Drops.h"
#include "Collision.h"

namespace Drops
{
	void CreateDrops(AEVec2 pos, GameObject::GameObject* objList)
	{
		GameObject::GameObject* go{};
		int random = rand() % 10 + 1;

		if (random >= 3 && random <= 6)
		{
			if ((go = GameObject::GetGameObject(GameObject::OBJ_HP_REGEN, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_HP_REGEN,
					/*position*/		pos,
					/*width*/			25.0f,
					/*height*/			25.0f,
					/*active*/			true,
					/*Max HP*/			0,
					/*Speed*/			10.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		10.0f,
					/*levelNumber*/		-2		// so that it will be cleared everytime room changed
				);
				go->velocity.y = DEFAULT_VELOCITY;
				go->gravity = GameObject::GRAVITY_DEFAULT;
			}
		}
		else if (random >= 7 && random <= 10)
		{
			if ((go = GameObject::GetGameObject(GameObject::OBJ_SOLAR_REGEN, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_SOLAR_REGEN,
					/*position*/		pos,
					/*width*/			25.0f,
					/*height*/			25.0f,
					/*active*/			true,
					/*Max HP*/			0,
					/*Speed*/			10.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		10.0f,
					/*levelNumber*/		-2		// so that it will be cleared everytime room changed
				);
				go->velocity.y = DEFAULT_VELOCITY;
				go->gravity = GameObject::GRAVITY_DEFAULT;
			}
		}
	}

	void CreateBuff(GameObject::GameObject* obj, GameObject::GameObject* objList)
	{
		GameObject::GameObject* go{};
		int random = rand() % 3 + 1;

		switch (random)
		{
		case 1:
		{
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ATK_UP, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ATK_UP,
					/*position*/		obj->position,
					/*width*/			25.0f,
					/*height*/			25.0f,
					/*active*/			true,
					/*Max HP*/			0,
					/*Speed*/			10.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		10.0f,
					/*levelNumber*/		-2		// so that it will be cleared everytime room changed
				);
				go->velocity.y = DEFAULT_VELOCITY;
				go->gravity = GameObject::GRAVITY_DEFAULT;
			}
			break;
		}
		case 2:
		{
			if ((go = GameObject::GetGameObject(GameObject::OBJ_PWR_UP, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_PWR_UP,
					/*position*/		obj->position,
					/*width*/			25.0f,
					/*height*/			25.0f,
					/*active*/			true,
					/*Max HP*/			0,
					/*Speed*/			10.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		10.0f,
					/*levelNumber*/		-2		// so that it will be cleared everytime room changed
				);
				go->velocity.y = DEFAULT_VELOCITY;
				go->gravity = GameObject::GRAVITY_DEFAULT;
			}
			break;
		}
		case 3:
		{
			if ((go = GameObject::GetGameObject(GameObject::OBJ_SOLAR_UP, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_SOLAR_UP,
					/*position*/		obj->position,
					/*width*/			25.0f,
					/*height*/			25.0f,
					/*active*/			true,
					/*Max HP*/			0,
					/*Speed*/			10.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		10.0f,
					/*levelNumber*/		-2		// so that it will be cleared everytime room changed
				);
				go->velocity.y = DEFAULT_VELOCITY;
				go->gravity = GameObject::GRAVITY_DEFAULT;
			}
			break;
		}
		default:
			break;
		}
	}

	void LootLifeTimeUpdate(GameObject::GameObject* lootObj, f32 gameDT)
	{
		lootObj->lifetime -= gameDT; // Buff lifetime decreased by time delay
		if (lootObj->lifetime <= 0.0f)
		{
			GameObject::ClearGameObject(lootObj);
		}
	}

	void NoCollisionTime(GameObject::GameObject* lootObj, f32 actualDT)
	{
		lootObj->loots.noCollisionTime -= actualDT; // does not affected by time delay
		if (lootObj->loots.noCollisionTime <= 0.0f)
		{
			lootObj->loots.bCollision = !lootObj->loots.bCollision;
		}
	}

	void PickUpDrops(GameObject::GameObject* drops, GameObject::PlayerObject* player, ParticleSystem& m_particleSystem)
	{
		switch (drops->type)
		{
		case GameObject::OBJ_ATK_UP:
		{
			++player->buffAttack;
			if (player->buffAttack >= GameObject::ATTACK_MAX_BUFF)
				player->buffAttack = GameObject::ATTACK_MAX_BUFF;
			m_particleSystem.SpawnBuffParticle(drops, player);
			break;
		}
		case GameObject::OBJ_PWR_UP:
		{
			player->buffFireRate += 0.5f;
			if (player->buffFireRate >= GameObject::FIRE_RATE_MAX_BUFF)
				player->buffFireRate = GameObject::FIRE_RATE_MAX_BUFF;
			player->fireRate = player->buffFireRate;
			player->bulletCooldown = (1.0f / player->fireRate);
			m_particleSystem.SpawnBuffParticle(drops, player);
			break;
		}
		case GameObject::OBJ_SOLAR_UP:
		{
			player->buffSolarMax += 0.25f;
			if (player->buffSolarMax >= GameObject::SOLAR_ENERGY_MAX_BUFF)
				player->buffSolarMax = GameObject::SOLAR_ENERGY_MAX_BUFF;
			player->solarEnergyMax = GameObject::SOLAR_ENERGY_MAX_DEFAULT * player->buffSolarMax;
			m_particleSystem.SpawnBuffParticle(drops, player);
			break;
		}
		case GameObject::OBJ_HP_REGEN:
		{
			player->go->healthCurrent += 3;
			if (player->go->healthCurrent >= player->go->healthMaximum)
				player->go->healthCurrent = player->go->healthMaximum;
			m_particleSystem.SpawnBuffParticle(drops, player);
			break;
		}
		case GameObject::OBJ_SOLAR_REGEN:
		{
			player->solarEnergy += 5.0f;
			if (player->solarEnergy >= player->solarEnergyMax)
				player->solarEnergy = player->solarEnergyMax;
			m_particleSystem.SpawnBuffParticle(drops, player);
			break;
		}
		default:
			break;
		}

	}

	void DropsMovementUpdate(GameObject::GameObject& obj, f32 gameDT)
	{
		obj.velocity.y = obj.gravity * obj.speed * gameDT + obj.velocity.y;
		obj.position.y = obj.velocity.y * gameDT + obj.position.y;
	}

	void LootCollisionToGridCheck(GameObject::GameObject* lootObj, GameObject::GameObject* gridObj)
	{
		int flag = 0;
		flag = Collision::checkSquaresIntersectionUpDown(lootObj, gridObj);
		if (flag == Collision::Collision_Flag::FLAG_BOTTOM)
		{
			Collision::snapToPosition(lootObj, gridObj, flag);
			lootObj->velocity.y = 0.0f;
		}
		else if (flag == Collision::Collision_Flag::FLAG_TOP)
		{
			Collision::snapToPosition(lootObj, gridObj, flag);
		}
		//flag collision to check which side colliding
		flag = Collision::checkSquaresIntersectionLeftRight(lootObj, gridObj);
		if (flag == Collision::Collision_Flag::FLAG_RIGHT)
		{
			Collision::snapToPosition(lootObj, gridObj, flag);
		}
		else if (flag == Collision::Collision_Flag::FLAG_LEFT)
		{
			Collision::snapToPosition(lootObj, gridObj, flag);
		}
	}

	void MaxBuffCheats(GameObject::PlayerObject* player)
	{
		player->buffAttack = GameObject::ATTACK_MAX_BUFF;

		player->buffFireRate = GameObject::FIRE_RATE_MAX_BUFF;
		player->fireRate = player->buffFireRate;
		player->bulletCooldown = (1.0f / player->fireRate);

		player->buffSolarMax = GameObject::SOLAR_ENERGY_MAX_BUFF;
		player->solarEnergyMax = GameObject::SOLAR_ENERGY_MAX_DEFAULT * player->buffSolarMax;

		player->go->healthCurrent = player->go->healthMaximum;

		player->solarEnergy = player->solarEnergyMax;
	}
}
