/*!************************************************************************
\file       PlayerController.h
\project    Solar Knight
\author(s)  Min Khant Ko (ko.m) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "GameObject.h"
#include <cstdio> // For file handling
#include <cerrno> // For error codes

namespace PlayerController {

	const float playerSpeed = 245.0f;
	const float chargeShotTime = 1.5f;
	static float chargeShotTimer = 0;

	typedef struct SavePointPos {
		AEVec2 pos;
		int roomID;
		bool currentSP;
	} SavePointPos;

	int CreateAndCheckSave(FILE** saveFile, SavePointPos* savePoint); // 1 for file created, 2 for non-empty file exists so no override, 0 for failure/error
	bool ClearSaveFile(FILE** saveFile);
	void WriteToSaveFile(FILE** saveFile, GameObject::PlayerObject* playerObject, GameObject::GameObject* savePoint);
	bool ReadFromSaveFile(const char* filePath, GameObject::PlayerObject* playerObject);

	std::vector<GameObject::GameObject*> CreateAndGetPlayerBullets(GameObject::GameObject* objList);
	void ActivatePlayerBullet(f32 cursorPosX, f32 cursorPosY, GameObject::PlayerObject* player, std::vector<GameObject::GameObject*> bulletList);
	void ActivateChargedPlayerBullet(f32 cursorPosX, f32 cursorPosY, GameObject::PlayerObject* player, std::vector<GameObject::GameObject*> bulletList);

	void SpawnSavePoint(AEVec2 pos, GameObject::GameObject* objList, int roomID, bool currentSP);
	void UseSavePoint(GameObject::GameObject* obj);
	void SetSavePoint(GameObject::GameObject* obj);
	GameObject::GameObject* GetSavePoint();

}

#endif