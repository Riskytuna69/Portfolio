/*!************************************************************************
\file       Enemies.cpp
\project    Solar Knight
\author(s) 	Chua Wen Shing Bryan (c.wenshingbryan) (100%)

\brief		This cpp has all the functions thats related to Enemies

			-storeEnemiesInfo function stores stores info into local varables inside this cpp
			-CreateEnemies will create the amount and type of enemies based on info from function 
			 storeEnemiesInfo
			-spawnEnemies would spawn the enemies in their respectivs room when called

			The rest of the functions is mainly about the enemies attack patterns, movement update and logic
			plus a bit of extra things like HP bar, clearing of variables, what png to use for the boss's attacks
			and the enemies projectile interacton with the player's HP.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"
#include "CreateMesh.h"
#include "GameObject.h"
#include "Collision.h"
#include "ObjectPhysics.h"
#include "Enemies.h"
#include "GameRenderer.h"
#include <vector>


typedef struct flags {

	bool boundaryFlag = false;
	bool touchLeft = false;
	bool touchRight = true;
	bool bulletFlag = false;
	//Timer
	float logicTimer = 0;
	float patrolTimer = 0;
	float attackOneTimer = 0;
	float attackTwoTimer = 0;
	//starting pos
	float startPosX = 0;
	float maxX = 0;
	float minX = 0;
	

}flags;

flags enemyFlags[50]; //<--- flags for enemy objs, variable enemyFlagsID will act as a subscript to access this array

typedef struct bossBullets {

	f32 x;
	f32 y;

}bossBullets;

bossBullets bulletRender[50]; /// Set to same am of elements as enemyFlags for consistent IDs

//CONST VARIABLES////////////////////////////

const float ENEMYWIDTH			     = 70.0f;
const float ENEMYHEIGHT				 = 70.0f;
const float ENEMYSPEED				 = 60.0f;
const float MELEERADIUS				= 300.0f;
const float RANGERADIUS				= 400.0f;

const float ELITEWIDTH				= 150.0f;
const float ELITEHEIGHT				= 150.0f;
const float ELITESPEED				= 70.0f;
const float ELITERADIUS				= 400.0f;

const float BOSSWIDTH				= 100.0f;
const float BOSSHEIGHT				= 110.0f;
const float BOSSSPEED				= 200.0f;
const float BOSSRADIUS				= 300.0f;

const float ALLENEMYLIFETIMEDEFAULT	   	 = 0;

const float ENEMYBULLETWIDTH		 = 50.0f;
const float ENEMYBULLETHEIGHT		 = 60.0f;
const int ENEMYBULLETHPDEFAULT	         = 5;
const float ENEMYBULLETSPEED		= 300.0f;
const float ENEMYBULLETRADIUS		 = 50.0f;
const float ENEMYBULLETDEFAULTLIFETIME = 2.f;

const float BOSSBULLETWIDTH			 = 75.0f;
const float BOSSBULLETHEIGHT		=  85.0f;
const int   BOSSBULLETHPDEFAULT		     = 1;
const float BOSSBULLETSPEED			 = 70.0f;
const float BOSSBULLETRADIUS		 = 50.0f;
const float BOSSBULLETDEFAULTLIFETIME  = 4.f;

//CONST VARIABLES^^^^^^^^^^^^^^^^^^^^^^^^^^^^

std::vector<enemyInfo> vecEnemy;
int incrementEnemyFlagID = 0;
int meleeType = 0;
int fakeRandValue = 0;
float timeSlowON = 1; //Value will change depending on time slow state

namespace Enemies
{
	int bossBulletIDBegin = 0;
	int bossBulletIDEnd = 0;

	//CREATE FUNCTIONS--------------------------------------------------------
	void storeEnemiesInfo(float x, float y, int TYPE_ROOM, ENEMY_ID TYPE)
	{
		AEVec2 Pos{ x, y };
		vecEnemy.push_back(enemyInfo{ Pos, TYPE_ROOM, incrementEnemyFlagID, TYPE });
		if (TYPE == BOSS_ID)
		{
			enemyFlags[incrementEnemyFlagID].startPosX = x;
			bossBulletIDBegin = incrementEnemyFlagID; //First flag ID value will be equal to enemyID
			bossBulletIDEnd = incrementEnemyFlagID + 6; //need this here because boss will be occupying extra 7 (0-6) for its bullets
			incrementEnemyFlagID = bossBulletIDEnd;  //Increment the enemyFlagID as well because the other enemy objs will need it
		}
		++incrementEnemyFlagID;
	}
	void spawnEnemies(GameObject::GameObject* objList, int TYPE_ROOM)
	{
		for (int i{ 0 }; i < vecEnemy.size(); ++i)
		{
			if (vecEnemy[i].TYPE_ROOM == TYPE_ROOM)
			{
				CreateEnemies(objList, vecEnemy[i], vecEnemy[i].TYPE);
			}

		}
	}

	void CreateEnemies(GameObject::GameObject* objList, enemyInfo v, ENEMY_ID ID)
	{
		//Using HP to differentate 
		switch (ID)
		{
		case MELEE_ID:
		{
			GameObject::GameObject* go{};
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY,
					/*position*/		v.Pos,
					/*width*/			ENEMYWIDTH,
					/*height*/			ENEMYHEIGHT,
					/*active*/			true,
					/*Max HP*/			MELEE_ID,
					/*Speed*/			ENEMYSPEED,
					/*Radius*/			MELEERADIUS,
					/*Lifetime*/		ALLENEMYLIFETIMEDEFAULT,
					/*levelNumber*/		v.TYPE_ROOM
				);
				go->velocity.x = 1.f;
				go->velocity.y = 1.f;
				go->gravity = GameObject::GRAVITY_DEFAULT;
				go->maxPosition.x = 0;
				go->minPosition.x = 0;
				go->enemyFlagsID = v.ENEMY_FLAG_ID;
				enemyFlags[go->enemyFlagsID].boundaryFlag = 0;
			}
			break;
		}
		case RANGE_ID:
		{
			GameObject::GameObject* go{};
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY,
					/*position*/		v.Pos,
					/*width*/			ENEMYWIDTH,
					/*height*/			ENEMYHEIGHT,
					/*active*/			true,
					/*Max HP*/			RANGE_ID,
					/*Speed*/			ENEMYSPEED,
					/*Radius*/			RANGERADIUS,
					/*Lifetime*/		ALLENEMYLIFETIMEDEFAULT,
					/*levelNumber*/		v.TYPE_ROOM
				);
				go->velocity.x = 1.f;
				go->velocity.y = 50.0f;
				go->gravity = GameObject::GRAVITY_DEFAULT;
				go->maxPosition.x = 0;
				go->minPosition.x = 0;
				go->enemyFlagsID = v.ENEMY_FLAG_ID;
				enemyFlags[go->enemyFlagsID].boundaryFlag = 0;
			}
			break;
		}
		case ELITE_ID:
		{
			GameObject::GameObject* go{};
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY_ELITE, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY_ELITE,
					/*position*/		v.Pos,
					/*width*/			ELITEWIDTH,
					/*height*/			ELITEHEIGHT,
					/*active*/			true,
					/*Max HP*/			ELITE_ID,
					/*Speed*/			ELITESPEED,
					/*Radius*/			ELITERADIUS,
					/*Lifetime*/		ALLENEMYLIFETIMEDEFAULT,
					/*levelNumber*/		v.TYPE_ROOM
				);
				go->velocity.x = 1.f;
				go->velocity.y = 1.f;
				go->gravity = GameObject::GRAVITY_DEFAULT;
				go->maxPosition.x = 0;
				go->minPosition.x = 0;
				go->enemyFlagsID = v.ENEMY_FLAG_ID;
				enemyFlags[go->enemyFlagsID].boundaryFlag = 0;
			}
			break;
		}
		case BOSS_ID:
		{
			GameObject::GameObject* go{};
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY_BOSS, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY_BOSS,
					/*position*/		v.Pos,
					/*width*/			BOSSWIDTH,
					/*height*/			BOSSHEIGHT,
					/*active*/			true,
					/*Max HP*/			BOSS_ID,
					/*Speed*/			BOSSSPEED,
					/*Radius*/			BOSSRADIUS,
					/*Lifetime*/		ALLENEMYLIFETIMEDEFAULT,
					/*levelNumber*/		v.TYPE_ROOM
				);
				go->velocity.x = 1.f;
				go->velocity.y = 50.0f;
				go->gravity = GameObject::GRAVITY_DEFAULT;
				go->maxPosition.x = 0;
				go->minPosition.x = 0;
				go->enemyFlagsID = v.ENEMY_FLAG_ID;
				enemyFlags[go->enemyFlagsID].boundaryFlag = 0;
			}
			break;
		}
		default:
			break;
		}
	}
	void CreateEnemyBullet(GameObject::GameObject* enemy, GameObject::GameObject* player, GameObject::GameObject* objList)
	{
		if (enemyFlags[enemy->enemyFlagsID].bulletFlag == false)
		{
			AEVec2 pos{};
			GameObject::GameObject* go{};
			pos = { enemy->position.x ,enemy->position.y };

			float directionX = player->position.x - enemy->position.x;
			float directionY = player->position.y - enemy->position.y;
			AEVec2 dirVec = { directionX , directionY };
			AEVec2 normVec{};
			AEVec2Normalize(&normVec, &dirVec);

			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY_PROJECTILE, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY_PROJECTILE,
					/*position*/		pos,
					/*width*/			ENEMYBULLETWIDTH,
					/*height*/			ENEMYBULLETHEIGHT,
					/*active*/			true,
					/*Max HP*/			ENEMYBULLETHPDEFAULT,
					/*Speed*/			ENEMYBULLETSPEED,
					/*Radius*/			ENEMYBULLETRADIUS,
					/*Lifetime*/		ENEMYBULLETDEFAULTLIFETIME,
					/*levelNumber*/		-2 //-2 for projectles
				);
				go->velocity.x = normVec.x;
				go->velocity.y = normVec.y; //store the dir of the bullet
				go->enemyFlagsID = enemy->enemyFlagsID; //assign the shooter ID to the bullet ID
			}
			enemyFlags[enemy->enemyFlagsID].bulletFlag = true;
		}
	}
	void CreateEnemyBullet(GameObject::GameObject* enemy, int extraValue, float spawnX, float spawnY, GameObject::GameObject* objList) //For Boss to use
	{
		if (enemyFlags[enemy->enemyFlagsID + extraValue].bulletFlag == false)
		{
			AEVec2 pos{};
			GameObject::GameObject* go{};
			pos = { spawnX ,spawnY };

			if ((go = GameObject::GetGameObject(GameObject::OBJ_ENEMY_PROJECTILE, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ENEMY_PROJECTILE,
					/*position*/		pos,
					/*width*/			BOSSBULLETWIDTH,
					/*height*/			BOSSBULLETHEIGHT,
					/*active*/			true,
					/*Max HP*/			BOSSBULLETHPDEFAULT,
					/*Speed*/		    BOSSBULLETSPEED,
					/*Radius*/			BOSSBULLETRADIUS,
					/*Lifetime*/		BOSSBULLETDEFAULTLIFETIME,
					/*levelNumber*/		-2 //-2 for projectles
				);
				go->velocity.x = 1.f;
				go->velocity.y = 1.f;
				go->gravity = GameObject::GRAVITY_DEFAULT;
				go->enemyFlagsID = enemy->enemyFlagsID + extraValue; //assign the shooter ID to the bullet ID
			}
			enemyFlags[enemy->enemyFlagsID + extraValue].bulletFlag = true;
		}
	}

	//LOGIC------------------------------------------------------------------------------------------------------// 

	//Shoot bullet
	void enemyShoot(GameObject::GameObject* bullet, float gameDeltaTime)
	{
		bullet->position.x += (gameDeltaTime * bullet->speed) * bullet->velocity.x;
		bullet->position.y += (gameDeltaTime * bullet->speed) * bullet->velocity.y;
	}
	//remove bullet
	void removeBullet(int enemyFlagsID)
	{
		enemyFlags[enemyFlagsID].bulletFlag = false;
	}

	void bossUpdate(GameObject::GameObject* boss,  GameObject::GameObject const&player, GameObject::GameObject* objList, float deltaTime)
	{
		if (Enemies::bossLogic(boss, deltaTime) == TYPE_BOSS_MOVE_LEFT)
		{
			boss->position.x += deltaTime * boss->speed * boss->velocity.x * -1.f;
		}
		else if (Enemies::bossLogic(boss, deltaTime) == TYPE_BOSS_MOVE_RIGHT)
		{
			boss->position.x += deltaTime * boss->speed * boss->velocity.x;
		}
		else if (Enemies::bossLogic(boss, deltaTime) == TYPE_BOSS_ATTACK_ONE)
		{
			bossAttackOne(boss, player, objList, deltaTime);
		}
		else if (Enemies::bossLogic(boss, deltaTime) == TYPE_BOSS_ATTACK_TWO)
		{
			bossAttackTwo(boss, player, objList, deltaTime);
		}
	}
	//Main boss logic for using attacks and movement
	int bossLogic(GameObject::GameObject* enemy, float deltaTime)
	{
		enemyFlags[enemy->enemyFlagsID].logicTimer += deltaTime; 

		//This pattern is assuming boss gets placed on right side of the room!!!
		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 1.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 16.f)
			return TYPE_BOSS_ATTACK_ONE; //Boss use 1st attack at 2 - 16

		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 17.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 20.f)
			return TYPE_BOSS_ATTACK_TWO; //Boss use 2nd attack at 18 - 20

		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 20.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 23.f)
			return TYPE_BOSS_MOVE_LEFT; //Boss reposition towards left for 21 - 23

		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 23.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 40.f)
			return TYPE_BOSS_ATTACK_ONE; //Boss use 1st attack at 24 - 40

		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 41.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 44.f)
			return TYPE_BOSS_ATTACK_TWO; //Boss use 2nd attack at 42 - 44

		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 44.f && enemyFlags[enemy->enemyFlagsID].logicTimer <= 50.f
			&& enemy->position.x < enemyFlags[enemy->enemyFlagsID].startPosX)
			return TYPE_BOSS_MOVE_RIGHT; //Boss repositions right for 45 - 50 & if boss X is less than starting X
	
		if (enemyFlags[enemy->enemyFlagsID].logicTimer > 50.f)
		{
			enemyFlags[enemy->enemyFlagsID].logicTimer -= enemyFlags[enemy->enemyFlagsID].logicTimer;//set timer to 0
		}

		return 0; //Default move 
	}
	//Boss ranged attack
	void bossAttackOne(GameObject::GameObject* enemy, GameObject::GameObject const& player, GameObject::GameObject* objList, float deltaTime)
	{
		enemyFlags[enemy->enemyFlagsID].attackOneTimer += deltaTime;//increments everytime this function is called
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer < 1.f)
		{
			CreateEnemyBullet(enemy, 0, player.position.x, 400 + player.position.y, objList);
		}
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 1.f && enemyFlags[enemy->enemyFlagsID].attackOneTimer < 2.1f)
		{
			CreateEnemyBullet(enemy, 1, -100 + player.position.x, 400 + player.position.y, objList);
		}
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 2.1f && enemyFlags[enemy->enemyFlagsID].attackOneTimer < 3.2f)
		{
			CreateEnemyBullet(enemy, 2, 0 + player.position.x, 400 + player.position.y, objList);
		}
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 3.2f && enemyFlags[enemy->enemyFlagsID].attackOneTimer < 4.3f)
		{
			CreateEnemyBullet(enemy, 3, 100 + player.position.x, 400 + player.position.y, objList);
		}
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 4.3f && enemyFlags[enemy->enemyFlagsID].attackOneTimer < 5.4f)
		{
			CreateEnemyBullet(enemy, 4, player.position.x, 400 + player.position.y, objList);
		}
		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 5.4f)
		{
			//Reset the timer
			enemyFlags[enemy->enemyFlagsID].attackOneTimer -= enemyFlags[enemy->enemyFlagsID].attackOneTimer;
		}

	}
	//For boss attack one
	void bossMeteors(GameObject::GameObject* bullet, float gameDeltaTime)
	{
		bullet->velocity.y += bullet->gravity * bullet->speed * gameDeltaTime;
		bullet->position.y += bullet->velocity.y * gameDeltaTime;
	}
	//Boss ranged attack 2
	void bossAttackTwo(GameObject::GameObject* enemy, GameObject::GameObject const& player, GameObject::GameObject* objList, float deltaTime)
	{
		enemyFlags[enemy->enemyFlagsID].attackTwoTimer += deltaTime;//increments everytime this function is called
		if (enemyFlags[enemy->enemyFlagsID].attackTwoTimer < 2)
		{
			CreateEnemyBullet(enemy, 5, -400 + player.position.x, 330, objList); //Boss bullet ID = Boss ID + 5 
			CreateEnemyBullet(enemy, 6, 400 + player.position.x, 330, objList); //Boss bullet ID = Boss ID + 6 
		}
		if (enemyFlags[enemy->enemyFlagsID].attackTwoTimer > 2)
		{
			//Reset the timer
			enemyFlags[enemy->enemyFlagsID].attackTwoTimer = 0;
		}

	}
	//For boss attack two
	void bossWalls(GameObject::GameObject* bullet, float gameDeltaTime)
	{
		if (bullet->enemyFlagsID == bossBulletIDEnd - 1)
		{
			bullet->velocity.x += bullet->speed * gameDeltaTime;
			bullet->position.x += bullet->velocity.x * gameDeltaTime;
		}
		else if (bullet->enemyFlagsID == bossBulletIDEnd)
		{
			bullet->velocity.x += bullet->speed * gameDeltaTime;
			bullet->position.x -= bullet->velocity.x * gameDeltaTime;
		}
	}
	//Patrols left and right, goes left first by default
	//calculates the patrol area (left and right boundary) and sets a flag so wont calculate anymore
	int enemyLogic(GameObject::GameObject* enemy, GameObject::GameObject* player, float gameDeltaTime)
	{
		enemyFlags[enemy->enemyFlagsID].patrolTimer += gameDeltaTime;

		if (enemyFlags[enemy->enemyFlagsID].patrolTimer >= 8)
			enemyFlags[enemy->enemyFlagsID].patrolTimer = 0;

		if (!Collision::checkCirclesIntersection(enemy, player)) //Player not in range
		{

			if (enemyFlags[enemy->enemyFlagsID].patrolTimer > 0 && enemyFlags[enemy->enemyFlagsID].patrolTimer < 4)
			{
				//Toggle enemy to move left if touching right side limit
				enemyFlags[enemy->enemyFlagsID].touchLeft = false;
				enemyFlags[enemy->enemyFlagsID].touchRight = true;
			}
			else if (enemyFlags[enemy->enemyFlagsID].patrolTimer >= 3 && enemyFlags[enemy->enemyFlagsID].patrolTimer < 8)
			{
				//Toggle enemy to move right if touching left side limit
				enemyFlags[enemy->enemyFlagsID].touchLeft = true;
				enemyFlags[enemy->enemyFlagsID].touchRight = false;
			}
		}
		else
		{
			//enemyFlags[enemy->enemyFlagsID].boundaryFlag = false;
			return 3;
		}
		if (!enemyFlags[enemy->enemyFlagsID].touchLeft)
			return 1; //patrolLeft
		if (!enemyFlags[enemy->enemyFlagsID].touchRight)
			return 2; //patrolRight

		return 0;
	}
	void enemyUpdate(GameObject::GameObject* enemy, GameObject::GameObject *player, GameObject::GameObject* objList, float deltaTime)
	{

		if (enemy->healthMaximum == RANGE_ID) //<--using this enemies HP to access Range enemy
		{
			if (Collision::checkCirclesIntersection(enemy, player))//if player in range
			{
				if (enemyAttackCD(enemy, static_cast<float>(deltaTime), false) == 0)
				{
					CreateEnemyBullet(enemy, player, objList);
					if (enemyAttackCD(enemy, static_cast<float>(deltaTime), true))
					{
						//empty body  if statement is used to call fucntion
					}
				}
				else
				{
					//If CD not over yet continue the timer until function returns 0 
					if (enemyAttackCD(enemy, static_cast<float>(deltaTime), true))
					{
						//empty body  if statement is used to call fucntion
					}
				}
			}
		}
		else
		{
			//Melee enemy
			if (enemyLogic(enemy, player, deltaTime) == 1)//patrol left
			{
				enemy->velocity.x = -1.f;
			}
			else if (enemyLogic(enemy, player, deltaTime) == 2)//patrol right
			{
				enemy->velocity.x = 1.f;
			}
			else if (enemyLogic(enemy, player, deltaTime) == 3)//chase
			{
				if (!Collision::checkSquaresIntersection(enemy, player))//if enemy not touching player yet
				{
					if (enemy->position.x < player->position.x) //move towards player
					{
						enemy->velocity.x = 1.f;
					}
					else if (enemy->position.x > player->position.x)
					{
						enemy->velocity.x = -1.f;
					}
				}
				else
				{
					//Normal melee enemy stands still to attack player
					enemy->position.x += 0;
				}
			}
			enemy->position.x = deltaTime * enemy->speed * enemy->velocity.x + enemy->position.x;

			enemy->velocity.y = enemy->gravity * enemy->speed * deltaTime + enemy->velocity.y;
			enemy->position.y = enemy->velocity.y * deltaTime + enemy->position.y;
		}
	}
	void eliteEnemyUpdate(GameObject::GameObject* elite, GameObject::GameObject* player, float deltaTime)
	{
		if (eliteEnemyLogic(elite, player) == 1)//go left
		{
			elite->velocity.x = -1.f;
		}
		else if (eliteEnemyLogic(elite, player) == 2)//go right
		{
			elite->velocity.x = 1.f;
		}
		else if (eliteEnemyLogic(elite, player) == 3)//chase
		{
			if (!Collision::checkSquaresIntersection(elite, player))//if enemy not touching player yet
			{
				if (elite->position.x < player->position.x)
				{
					elite->velocity.x = 1.f;
				}
				else if (elite->position.x > player->position.x)
				{
					elite->velocity.x = -1.f;
				}
			}
			else
			{
				//Elite stands still to attack player
				elite->position.x += 0;
			}
		}
		elite->position.x = deltaTime * elite->speed * elite->velocity.x + elite->position.x;

		elite->velocity.y = elite->gravity * elite->speed * deltaTime + elite->velocity.y;
		elite->position.y = elite->velocity.y * deltaTime + elite->position.y;
	}
	//Dosent move form its spot but will chase after player if they come too close
	//if player goes out of range elite will move back to origina pos
	int eliteEnemyLogic(GameObject::GameObject* elite, GameObject::GameObject* player)
	{
		//for setting patrol route
		if (!enemyFlags[elite->enemyFlagsID].boundaryFlag)//if flag  == false, statement true
		{
			enemyFlags[elite->enemyFlagsID].startPosX = elite->position.x;
			enemyFlags[elite->enemyFlagsID].boundaryFlag = !enemyFlags[elite->enemyFlagsID].boundaryFlag;//set flag to true
		}
		if (Collision::checkCirclesIntersection(elite, player)) //Player in range
		{
			return 3;//chase player
		}
		if (elite->position.x > enemyFlags[elite->enemyFlagsID].startPosX)
			return 1; //go left
		if (elite->position.x < enemyFlags[elite->enemyFlagsID].startPosX)
			return 2; //go right

		return 0;
	}

	float enemyAttackCD(GameObject::GameObject* enemy, float deltaTime, bool flag)
	{
		if (flag) //Flag == true, start CD
			enemyFlags[enemy->enemyFlagsID].attackOneTimer += deltaTime;

		if (enemyFlags[enemy->enemyFlagsID].attackOneTimer > 2)
		{
			enemyFlags[enemy->enemyFlagsID].attackOneTimer = 0; //After 2 sec set timer to 0
		}
		return enemyFlags[enemy->enemyFlagsID].attackOneTimer; //false if still on cooldown
	}

	void enemyVecClear()
	{
		vecEnemy.clear();
		incrementEnemyFlagID = 0;
	}
	
	//Render the different pngs used by the boss for its attack
	void bossAtkRenderType(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, BOSS_ATTACK_TYPE type)
	{
		
		if (GR::bPlayerArmour == true)
		{
			timeSlowON += 0.1f;
			if (timeSlowON > 1.1f)//modular for floats
				timeSlowON = 0;
		}
		else
			timeSlowON = 1;

		fakeRandValue =	(fakeRandValue + static_cast<int> ((1* timeSlowON))) % 9;

		if (type == BOSS_ATTACK_ONE)
			bossAtkOneRender(pEnemy,  bossProj, fakeRandValue);
		else
			bossAtkTwoRender( pEnemy, bossProj , fakeRandValue);
	}

	void bossAtkOneRender(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, int randomValue)
	{
	
		if (bossProj->lifetime == 4.f)
		{
			switch (randomValue)
			{
				case HAMMER_LEFT:
					bulletRender[bossProj->enemyFlagsID].x = 1.f;
					bulletRender[bossProj->enemyFlagsID].y = 1.f;
					break;
				case HAMMER_RIGHT:
					bulletRender[bossProj->enemyFlagsID].x = 1.20f;
					bulletRender[bossProj->enemyFlagsID].y = 1.f;
					break;
				case SWORD:
					bulletRender[bossProj->enemyFlagsID].x = 1.40f;
					bulletRender[bossProj->enemyFlagsID].y = 1.f;
					break;
				case SPIKEY_SWORD:
					bulletRender[bossProj->enemyFlagsID].x = 1.60f;
					bulletRender[bossProj->enemyFlagsID].y = 1.f;
					break;
				case MACE:
					bulletRender[bossProj->enemyFlagsID].x = 1.80f;
					bulletRender[bossProj->enemyFlagsID].y = 1.f;
					break;
				case STAR:
					bulletRender[bossProj->enemyFlagsID].x = 1.f;
					bulletRender[bossProj->enemyFlagsID].y = 1.5f;
					break;
				case DAGGER:
					bulletRender[bossProj->enemyFlagsID].x = 1.20f;
					bulletRender[bossProj->enemyFlagsID].y = 1.5f;
					break;
				case AXE_RIGHT:
					bulletRender[bossProj->enemyFlagsID].x = 1.40f;
					bulletRender[bossProj->enemyFlagsID].y = 1.5f;
					break;
				case AXE_LEFT:
					bulletRender[bossProj->enemyFlagsID].x = 1.60f;
					bulletRender[bossProj->enemyFlagsID].y = 1.5f;
					break;
				default:
					break;
			}
		}
		AEGfxTextureSet(pEnemy, bulletRender[bossProj->enemyFlagsID].x, bulletRender[bossProj->enemyFlagsID].y);
	}
	void bossAtkTwoRender(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, int randomValue)
	{
		if (bossProj->lifetime > 0.01f)
		{
		
			switch (randomValue)
			{
			case SPIN_0:
				bulletRender[bossProj->enemyFlagsID].x = 1.f;
				bulletRender[bossProj->enemyFlagsID].y = 1.f;
				break;
			case SPIN_1:
				bulletRender[bossProj->enemyFlagsID].x = 1.20f;
				bulletRender[bossProj->enemyFlagsID].y = 1.f;
				break;
			case SPIN_2:
				bulletRender[bossProj->enemyFlagsID].x = 1.40f;
				bulletRender[bossProj->enemyFlagsID].y = 1.f;
				break;
			case SPIN_3:
				bulletRender[bossProj->enemyFlagsID].x = 1.60f;
				bulletRender[bossProj->enemyFlagsID].y = 1.f;
				break;
			case SPIN_4:
				bulletRender[bossProj->enemyFlagsID].x = 1.80f;
				bulletRender[bossProj->enemyFlagsID].y = 1.f;
				break;
			case SPIN_5:
				bulletRender[bossProj->enemyFlagsID].x = 1.f;
				bulletRender[bossProj->enemyFlagsID].y = 1.5f;
				break;
			case SPIN_6:
				bulletRender[bossProj->enemyFlagsID].x = 1.20f;
				bulletRender[bossProj->enemyFlagsID].y = 1.5f;
				break;
			case SPIN_7:
				bulletRender[bossProj->enemyFlagsID].x = 1.40f;
				bulletRender[bossProj->enemyFlagsID].y = 1.5f;
				break;
			case SPIN_8:
				bulletRender[bossProj->enemyFlagsID].x = 1.60f;
				bulletRender[bossProj->enemyFlagsID].y = 1.5f;
				break;
			case SPIN_9:
				bulletRender[bossProj->enemyFlagsID].x = 1.80f;
				bulletRender[bossProj->enemyFlagsID].y = 1.5f;
			default:
				break;
			}
		}
		AEGfxTextureSet(pEnemy, bulletRender[bossProj->enemyFlagsID].x, bulletRender[bossProj->enemyFlagsID].y);
	}

	void enemyProjUpdate(GameObject::GameObject* enemy, float deltaTime)
	{
		if (enemy->enemyFlagsID >= bossBulletIDBegin && enemy->enemyFlagsID <= (bossBulletIDEnd - 2))//Boss bullets
		{
			bossMeteors(enemy, deltaTime);
		}
		else if (enemy->enemyFlagsID > (bossBulletIDEnd - 2) && enemy->enemyFlagsID <= bossBulletIDEnd)
		{
			bossWalls(enemy, deltaTime);
		}
		else
		{
			enemyShoot(enemy, deltaTime);
		}
		enemy->lifetime -= deltaTime;
		if (enemy->lifetime <= 0)
		{
			removeBullet(enemy->enemyFlagsID); //for clearing variable in enemy cpp
			GameObject::ClearGameObject(enemy);
		}
	}
	void bossPinkHpBar()
	{
		AEMtx33 scaleMtx, translateMtx, transformMtx;
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(1, 0, 0.5, 1);//Pink
		AEGfxSetTransparency(0.5f);
		AEMtx33Scale(&scaleMtx, 800,60);
		AEMtx33Trans(&translateMtx, AEGfxGetWinMinX() + 800.f, AEGfxGetWinMaxY() - 50.f);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
	}
	void bossPurpleHpBar(GameObject::GameObject* boss)
	{
		AEMtx33 scaleMtx, translateMtx, transformMtx;
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(0.5,0, 1, 1);//Purple
		AEGfxSetTransparency(1.0f);
		AEMtx33Scale(&scaleMtx, boss->healthCurrent*(800.f/ boss->healthMaximum), 60);
		AEMtx33Trans(&translateMtx, AEGfxGetWinMinX() + 800.f, AEGfxGetWinMaxY() - 50.f);
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
	}
	void enemyRedHpBar(GameObject::GameObject* enemy)
	{
		AEMtx33 scaleMtx, translateMtx, transformMtx;
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(1, 0, 0, 1);//Red
		AEGfxSetTransparency(0.5f);
		AEMtx33Scale(&scaleMtx, 90.0f, 15.f);
		AEMtx33Trans(&translateMtx, enemy->position.x, enemy->position.y + (enemy->height / 1.5f));
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
	}
	void enemyGreenHpBar(GameObject::GameObject* enemy)
	{
		AEMtx33 scaleMtx, translateMtx, transformMtx;
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetColorToMultiply(0, 1, 0, 1);//Green
		AEGfxSetTransparency(1.0f);
		AEMtx33Scale(&scaleMtx, enemy->healthCurrent * (90.0f / enemy->healthMaximum), 15.f);
		AEMtx33Trans(&translateMtx, enemy->position.x, enemy->position.y + (enemy->height / 1.5f));
		AEMtx33Concat(&transformMtx, &translateMtx, &scaleMtx);

		AEGfxSetTransform(transformMtx.m);
	}

	void clearBulletsWhenExitClicked(GameObject::GameObject* objects)
	{
		for (int i = 1; i < GameObject::MAXGAMEOBJECT; ++i)
		{
			if (objects[i].type == GameObject::OBJ_ENEMY_PROJECTILE)
				removeBullet(objects[i].enemyFlagsID);
		}
	}

	void enemyCollisionUpdate(GameObject::GameObject* enemy, GameObject::GameObject* grid)
	{
		int flag = 0, flag2 = 0;
		flag = Collision::checkSquaresIntersectionUpDown(enemy, grid);
		if (flag == Collision::Collision_Flag::FLAG_BOTTOM)
		{
			Collision::snapToPosition(enemy, grid, flag);
			//Enemies will stop trying to go down the ground if it is touching the ground
			enemy->velocity.y = 0;
		}
		else if (flag == Collision::Collision_Flag::FLAG_TOP)
		{
			Collision::snapToPosition(enemy, grid, flag);
		}
		//flag collision to check which side colliding
		flag2 = Collision::checkSquaresIntersectionLeftRight(enemy, grid);
		if (flag2 == Collision::Collision_Flag::FLAG_RIGHT)
		{
			Collision::snapToPosition(enemy, grid, flag2);
		}
		else if (flag2 == Collision::Collision_Flag::FLAG_LEFT)
		{
			Collision::snapToPosition(enemy, grid, flag2);
		}
	}

	void enemyTouchPlayerUpdate(GameObject::GameObject* enemy, GameObject::GameObject* player, float gameDeltaTime)
	{
		//only check collision if CD is over
		if (enemyAttackCD(enemy, static_cast<float>(gameDeltaTime), false) == 0) //Dont start the CD
		{
			if (Collision::checkSquaresIntersection(enemy, player))
			{
				player->healthCurrent--;
				//start the CD timer
				if (enemyAttackCD(enemy, static_cast<float>(gameDeltaTime), true))
				{
					//empty body  if statement is used to call fucntion
				}
			}
		}
		else
		{
			//If CD not over yet continue the timer until function returns 0 
			if (enemyAttackCD(enemy, static_cast<float>(gameDeltaTime), true)) //True to run the timer
			{
				//empty body  if statement is used to call fucntion
			}
		}
	}

	int enemyProjectileUpdate(GameObject::GameObject* enemy, GameObject::GameObject* player)
	{
		if (enemy->enemyFlagsID < bossBulletIDBegin || (enemy->enemyFlagsID > bossBulletIDEnd))
		{
			if (Collision::checkSquaresIntersection(enemy, player))
			{
				player->healthCurrent--; //each bullet does 1 dmg for now
				Enemies::removeBullet(enemy->enemyFlagsID);
				GameObject::ClearGameObject(enemy);

				if (player->healthCurrent <= 0)
				{
					return PLAYER_DEAD;
				}
			}
		}
		else if (enemy->enemyFlagsID >= bossBulletIDBegin && enemy->enemyFlagsID <= (bossBulletIDEnd - 2))//Boss bullets
		{
			//falling block attack
			if (Collision::checkSquaresIntersection(enemy, player))
			{
				player->healthCurrent -= 5; //each block from this attack does 5 dmg
				removeBullet(enemy->enemyFlagsID);
				GameObject::ClearGameObject(enemy);
			}
		}
		else if (enemy->enemyFlagsID > (bossBulletIDEnd - 2) && enemy->enemyFlagsID <= bossBulletIDEnd)
		{
			if (Collision::checkSquaresIntersection(enemy, player))
			{
				player->healthCurrent--; //each block from this attack does 1 dmg
				removeBullet(enemy->enemyFlagsID);
				GameObject::ClearGameObject(enemy);
			}
		}

		return PLAYER_ALIVE;
	}

}