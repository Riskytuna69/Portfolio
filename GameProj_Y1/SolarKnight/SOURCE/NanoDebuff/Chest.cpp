/*!************************************************************************
\file       Chest.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"
#include "GameObject.h"
#include "Chest.h"
#include "Drops.h"

namespace Chest
{
	GameObject::GameObject* interactableObj{};

	void CreateChest(AEVec2 pos, GameObject::GameObject* objList, int roomID)
	{
		GameObject::GameObject* go{};
		if ((go = GameObject::GetGameObject(GameObject::OBJ_CHEST, objList)) != nullptr)
		{
			*go = GameObject::CreateGameObject(
				/*type*/			GameObject::OBJ_CHEST,
				/*position*/		pos,
				/*width*/			50.0f,
				/*height*/			75.0f,
				/*active*/			false,
				/*Max HP*/			0,
				/*Speed*/			0,
				/*Radius*/			0.0f,
				/*Lifetime*/		10.0f,
				/*levelNumber*/		roomID
			);
		}
	}

	void CreateChestThroughSpawnPoint(AEVec2 pos, GameObject::GameObject* objList, int roomID)
	{
		GameObject::GameObject* go{};
		if ((go = GameObject::GetGameObject(GameObject::OBJ_CHEST, objList)) != nullptr)
		{
			*go = GameObject::CreateGameObject(
				/*type*/			GameObject::OBJ_CHEST,
				/*position*/		pos,
				/*width*/			50.0f,
				/*height*/			75.0f,
				/*active*/			false,
				/*Max HP*/			0,
				/*Speed*/			0,
				/*Radius*/			0.0f,
				/*Lifetime*/		10.0f,
				/*levelNumber*/		roomID
			);
		}
	}

	void OpenChest(GameObject::GameObject* obj, GameObject::GameObject* objList)
	{
		Drops::CreateBuff(obj, objList);
		obj->loots.bOpened = true;
		interactableObj = nullptr;
	}

	void SetInteractableObject(GameObject::GameObject* obj)
	{
		interactableObj = obj;
	}

	GameObject::GameObject* GetInteractableObject()
	{
		return interactableObj;
	}
}