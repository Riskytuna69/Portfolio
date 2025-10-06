/*!************************************************************************
\file       Drops.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef DROPS_H
#define DROPS_H

#include "ParticleSystem.h"
namespace Drops
{
	const f32 DEFAULT_VELOCITY = 50.0f;
	void CreateDrops(AEVec2 pos, GameObject::GameObject* obj);

	void CreateBuff(GameObject::GameObject* obj, GameObject::GameObject* objList);

	void LootLifeTimeUpdate(GameObject::GameObject* lootObj, f32 gameDT);

	void NoCollisionTime(GameObject::GameObject* lootObj, f32 actualDT);

	void PickUpDrops(GameObject::GameObject* drops, GameObject::PlayerObject* player, ParticleSystem& m_particleSystem);

	void DropsMovementUpdate(GameObject::GameObject& obj, f32 gameDT);

	void LootCollisionToGridCheck(GameObject::GameObject* lootObj, GameObject::GameObject* gridObj);

	void MaxBuffCheats(GameObject::PlayerObject* player);

}
#endif

