/*!************************************************************************
\file       Chest.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef CHEST_H
#define CHEST_H
namespace Chest
{
	const f32 DEFAULT_VELOCITY = 50.0f;

	typedef struct ChestSpawnPoint
	{
		AEVec2 pos;
		int roomID;
	}ChestSpawnPoint;

	void CreateChest(AEVec2 pos, GameObject::GameObject* objList, int roomID);
	void CreateChestThroughSpawnPoint(AEVec2 pos, GameObject::GameObject* objList, int roomID);
	void OpenChest(GameObject::GameObject* obj, GameObject::GameObject* objList);
	void SetInteractableObject(GameObject::GameObject* obj);
	GameObject::GameObject* GetInteractableObject();
}
#endif