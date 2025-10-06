/*!************************************************************************
\file       Enemies.h
\project    Solar Knight
\author(s) 	Chua Wen Shing Bryan (c.wenshingbryan) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#pragma once

enum ENEMY_ID {
	MELEE_ID = 10,
	RANGE_ID = 5,
	ELITE_ID = 15,
	BOSS_ID = 100
};

enum TYPE_BOSS
{
	TYPE_BOSS_EMPTY,	  //0
	TYPE_BOSS_MOVE_LEFT,  //1
	TYPE_BOSS_MOVE_RIGHT, //2
	TYPE_BOSS_ATTACK_ONE, //3
	TYPE_BOSS_ATTACK_TWO  //4
};

enum TYPE_ROOM
{
	TYPE_ROOM_ONE,  //0
	TYPE_ROOM_TWO,  //1
	TYPE_ROOM_THREE,//2
	TYPE_ROOM_FOUR, //3
};

enum ENEMY_MAX
{
	MAX_RANGE_BULETS = 6,  //0
	MAX_BOSS_ATTACK_ONE_BULLETS,  //1
};

enum BOSS_ATTACK_1
{
	HAMMER_LEFT,
	HAMMER_RIGHT,
	SWORD,
	SPIKEY_SWORD,
	MACE,
	STAR,
	DAGGER,
	AXE_RIGHT,
	AXE_LEFT
};

enum BOSS_ATTACK_2
{
	SPIN_0,
	SPIN_1,
	SPIN_2,
	SPIN_3,
	SPIN_4,
	SPIN_5,
	SPIN_6,
	SPIN_7,
	SPIN_8,
	SPIN_9
};

enum BOSS_ATTACK_TYPE
{
	BOSS_ATTACK_ONE,
	BOSS_ATTACK_TWO
};

struct enemyInfo {

	AEVec2 Pos;
	int TYPE_ROOM;
	int ENEMY_FLAG_ID;
	enum ENEMY_ID TYPE;
};

enum PLAYER_DOA
{
	PLAYER_ALIVE,
	PLAYER_DEAD
};

const u32 spritesheet_rows = 2;
const u32 spritesheet_cols = 5;
const f32 sprite_uv_width = 1.f / spritesheet_cols;
const f32 sprite_uv_height = 1.f / spritesheet_rows;


namespace Enemies
{
	void spawnEnemies(GameObject::GameObject* objList, int TYPE_ROOM);
	void storeEnemiesInfo(float x, float y, int TYPE_ROOM, ENEMY_ID TYPE);

	void CreateEnemies(GameObject::GameObject* objList, enemyInfo v, ENEMY_ID ID);
	void CreateEnemyBullet(GameObject::GameObject* enemy, GameObject::GameObject* player, GameObject::GameObject* objList);
	void CreateEnemyBullet(GameObject::GameObject* enemy, int extraValue, float spawnX, float spawnY, GameObject::GameObject* objList);

	int enemyLogic(GameObject::GameObject* enemy, GameObject::GameObject* player, float gameDeltaTime);
	void enemyUpdate(GameObject::GameObject* enemy, GameObject::GameObject* player, GameObject::GameObject* objList, float deltaTime);

	int eliteEnemyLogic(GameObject::GameObject* elite, GameObject::GameObject* player);
	void eliteEnemyUpdate(GameObject::GameObject* elite, GameObject::GameObject* player,  float deltaTime);

	float enemyAttackCD(GameObject::GameObject* enemy, float deltaTime, bool flag);
	void enemyShoot(GameObject::GameObject* bullet, float gameDeltaTime);

	void enemyProjUpdate(GameObject::GameObject* bossProjectile, float deltaTime);
	void removeBullet(int enemyFlagsID);
	void enemyVecClear();
	void clearBulletsWhenExitClicked(GameObject::GameObject* objects);

	int bossLogic(GameObject::GameObject* enemy, float deltaTime);
	void bossUpdate(GameObject::GameObject* boss, GameObject::GameObject const& player, GameObject::GameObject* objList, float deltaTime);
	void bossAttackOne(GameObject::GameObject* enemy, GameObject::GameObject const& player, GameObject::GameObject* objList, float deltaTime);
	void bossAttackTwo(GameObject::GameObject* enemy, GameObject::GameObject const& player, GameObject::GameObject* objList, float deltaTime);
	void bossMeteors(GameObject::GameObject* bullet, float gameDeltaTime);
	void bossWalls(GameObject::GameObject* bullet, float gameDeltaTime);

	void bossAtkRenderType(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, BOSS_ATTACK_TYPE type);
	void bossAtkOneRender(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, int randomValue);
	void bossAtkTwoRender(AEGfxTexture* pEnemy, GameObject::GameObject* bossProj, int randomValue);

	void bossPinkHpBar();
	void bossPurpleHpBar(GameObject::GameObject* boss);
	void enemyRedHpBar(GameObject::GameObject* enemy);
	void enemyGreenHpBar(GameObject::GameObject* enemy);

	void enemyCollisionUpdate(GameObject::GameObject* enemy, GameObject::GameObject* grid);
	void enemyTouchPlayerUpdate(GameObject::GameObject* enemy, GameObject::GameObject* player, float gameDeltaTime);

	int enemyProjectileUpdate(GameObject::GameObject* enemy, GameObject::GameObject* player);

	extern int bossBulletIDBegin;
	extern int bossBulletIDEnd;
}



