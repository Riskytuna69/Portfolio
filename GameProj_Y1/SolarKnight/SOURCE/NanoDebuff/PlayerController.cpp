/*!************************************************************************
\file       PlayerController.cpp
\project    Solar Knight
\author(s)  Min Khant Ko (ko.m) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once
#include "GameObject.h"
#include "PlayerController.h"

namespace PlayerController {

	GameObject::GameObject* interactableSP{}; // Store save point

	int CreateAndCheckSave(FILE** saveFile, SavePointPos* savePoint) {

		errno_t err;
		FILE* checkFile;

		err = fopen_s(&checkFile, "save_file.txt", "r");
		if (err == 0) {
			// The file opened successfully, now check if it is empty
			fseek(checkFile, 0, SEEK_END); // Move to the end of the file
			long fileSize = ftell(checkFile); // Get the size of the file
			fclose(checkFile); // Close the file

			// If the file size is greater than zero, the file is not empty
			if (fileSize > 0) {
				savePoint->currentSP = true;
				return 2; // File is not empty, do not overwrite
			}
		}

		// Attempt to open the file for writing
		if ((err = fopen_s(saveFile, "save_file.txt", "w")) != 0) {
			// File could not be opened. wFile was set to NULL
			// Error code is returned in err.
			// Error message can be retrieved with strerror(err);
			// Handle the error as needed
			return 0;
		}
		else if (*saveFile != NULL) {
			// Successfully opened the file for writing
			// Now you can write to the file

			// Example of writing text lines
			//fprintf(*saveFile, "SAVE\n");
			// Continue writing as needed...

			// Always remember to close the file when done to avoid memory leaks
			fclose(*saveFile);
			return 1;
		}
		else {
			// wFile is NULL but no error code, handle the unexpected error
			return 0;
		}
	}

	bool ClearSaveFile(FILE** saveFile) {

		errno_t err;

		// Attempt to open the file for writing
		if ((err = fopen_s(saveFile, "save_file.txt", "w")) != 0) {
			// File could not be opened. wFile was set to NULL
			// Error code is returned in err.
			// Error message can be retrieved with strerror(err);
			// Handle the error as needed
			return false;
		}
		else if (*saveFile != NULL) {
			// Successfully opened the file for writing
			// Now you can write to the file

			// Example of writing text lines
			//fprintf(*saveFile, "SAVE\n");
			// Continue writing as needed...

			// Always remember to close the file when done to avoid memory leaks
			fclose(*saveFile);
			return true;
		}
		else {
			// wFile is NULL but no error code, handle the unexpected error
			return false;
		}
	}

	void WriteToSaveFile(FILE** saveFile, GameObject::PlayerObject* playerObject, GameObject::GameObject* savePoint) {
		
		// Attempt to open the file for writing...
		if ( fopen_s(saveFile, "save_file.txt", "w") == 0) {

			fprintf(*saveFile, "SAVE\n");
			fprintf(*saveFile, "H: %d\n", playerObject->go->healthCurrent);			// HEALTH
			fprintf(*saveFile, "En: %0.2f\n", playerObject->solarEnergy);			// SOLAR ENERGY
			fprintf(*saveFile, "EnB: %0.2f\n", playerObject->buffSolarMax);			// SOLAR ENERGY MAX BASED ON BUFF
			fprintf(*saveFile, "A: %d\n", playerObject->buffAttack);				// DAMAGE BASED ON BUFF
			fprintf(*saveFile, "FR: %0.2f\n", playerObject->buffFireRate);			// FIRE RATE BASED ON BUFF

			fprintf(*saveFile, "ID: %d\n", savePoint->levelNumber);					// SAVE POINT ROOM ID
			fprintf(*saveFile, "X: %0.2f\n", savePoint->position.x);				// SAVE POINT POSITION X
			fprintf(*saveFile, "Y: %0.2f\n", savePoint->position.y);				// SAVE POINT POSITION Y
		
			fclose(*saveFile);
		}
		savePoint->knockbackState = true;
	
	}

	bool ReadFromSaveFile(const char* filePath, GameObject::PlayerObject* playerObject) {
		errno_t err;
		FILE* saveFile;
		FILE* checkFile;


		// Attempt to open the file for reading
		if (fopen_s(&saveFile, filePath, "r") != 0) {
			// If the file couldn't be opened, return false to indicate failure
			return false;
		}

		// Check if empty too
		err = fopen_s(&checkFile, filePath, "r");
		if (err == 0) {
			// The file opened successfully, now check if it is empty
			fseek(checkFile, 0, SEEK_END); // Move to the end of the file
			long fileSize = ftell(checkFile); // Get the size of the file
			fclose(checkFile); // Close the file

			if (fileSize <= 0) {
				return false; // File is empty, do no set
			}
		}

		char buffer[10]; // For reading the prefix strings like "SAVE", "H:", etc.
		unsigned bufferSize = (unsigned)_countof(buffer);
		int healthCurrent;
		float solarEnergy, buffSolarMax, buffFireRate;
		int buffAttack, levelNumber;
		float positionX, positionY;

		int tempW; // Used to suppress warnings and should be used for actual checks
		
		tempW = fscanf_s(saveFile, "%9s\n", buffer, bufferSize); // Read the "SAVE" line
		tempW = fscanf_s(saveFile, "%9s %d\n", buffer, bufferSize, &healthCurrent); // HEALTH
		tempW = fscanf_s(saveFile, "%9s %f\n", buffer, bufferSize, &solarEnergy);	// SOLAR ENERGY
		tempW = fscanf_s(saveFile, "%9s %f\n", buffer, bufferSize, &buffSolarMax);	// SOLAR ENERGY MAX BASED ON BUFF
		tempW = fscanf_s(saveFile, "%9s %d\n", buffer, bufferSize, &buffAttack);	// DAMAGE BASED ON BUFF
		tempW = fscanf_s(saveFile, "%9s %f\n", buffer, bufferSize, &buffFireRate);	// FIRE RATE BASED ON BUFF

		tempW = fscanf_s(saveFile, "%9s %d\n", buffer, bufferSize, &levelNumber);	// SAVE POINT ROOM ID
		tempW = fscanf_s(saveFile, "%9s %f\n", buffer, bufferSize, &positionX);		// SAVE POINT POSITION X
		tempW = fscanf_s(saveFile, "%9s %f\n", buffer, bufferSize, &positionY);		// SAVE POINT POSITION Y


		/*
		playerObject->go->healthCurrent = healthCurrent;
		playerObject->solarEnergy = solarEnergy;
		playerObject->buffSolarMax = buffSolarMax;
		playerObject->buffAttack = buffAttack;
		playerObject->buffFireRate = buffFireRate;
		*/

		playerObject->go->position.x = positionX;
		playerObject->go->position.y = positionY;
		playerObject->go->levelNumber = levelNumber;

		/*
		savePoint->levelNumber = levelNumber;
		savePoint->position.x = positionX;
		savePoint->position.y = positionY;
		*/

		// Close the file
		fclose(saveFile);

		// Return true to indicate success
		return true;
	}

	// For bullets / shooting
	std::vector<GameObject::GameObject*> CreateAndGetPlayerBullets(GameObject::GameObject* objList)
	{
		std::vector<GameObject::GameObject*> bulletList{};
		for (int i = GameObject::MINALLYPROJECTILE; i < GameObject::MAXALLYPROJECTILE; i++)
		{
			GameObject::GameObject* go{};
			if ((go = GameObject::GetGameObject(GameObject::OBJ_ALLY_PROJECTILE, objList)) != nullptr)
			{
				*go = GameObject::CreateGameObject(
					/*type*/			GameObject::OBJ_ALLY_PROJECTILE,
					/*position*/		AEVec2{-1.0f, -1.0f},
					/*width*/			20.0f,
					/*height*/			20.0f,
					/*active*/			true,
					/*Max HP*/			-1, // HP for bullets used to check if ACTIVE and represent damage
					/*Speed*/			500.0f,
					/*Radius*/			10.0f,
					/*Lifetime*/		1.2f,
					/*levelNumber*/		-1 //-1 for projectles
				);
				bulletList.push_back(go);
			}
		}
		return bulletList;
	
	}

	void ActivatePlayerBullet(f32 cursorPosX, f32 cursorPosY, GameObject::PlayerObject* player, std::vector<GameObject::GameObject*> bulletList)
	{
		if (player->currentBulletIndex >= (GameObject::MAXALLYPROJECTILE - GameObject::MINALLYPROJECTILE)) {
			player->currentBulletIndex = 0;
		}

		f32 directionX = cursorPosX - player->go->position.x;
		f32 directionY = cursorPosY - player->go->position.y;

		AEVec2 dirVec = { directionX , directionY };
		AEVec2 normVec{};
		AEVec2Normalize(&normVec, &dirVec);

		bulletList[player->currentBulletIndex]->healthMaximum = player->buffAttack; // Test damage value
		bulletList[player->currentBulletIndex]->knockbackType = -10; // Use to identify type of bullet
		bulletList[player->currentBulletIndex]->lifetime = 1.2f;
		bulletList[player->currentBulletIndex]->position = player->go->position;
		bulletList[player->currentBulletIndex]->velocity = normVec;
		player->currentBulletIndex++;
	}

	void ActivateChargedPlayerBullet(f32 cursorPosX, f32 cursorPosY, GameObject::PlayerObject* player, std::vector<GameObject::GameObject*> bulletList)
	{
		if (player->currentBulletIndex >= (GameObject::MAXALLYPROJECTILE - GameObject::MINALLYPROJECTILE)) {
			player->currentBulletIndex = 0;
		}

		f32 directionX = cursorPosX - player->go->position.x;
		f32 directionY = cursorPosY - player->go->position.y;

		AEVec2 dirVec = { directionX , directionY };
		AEVec2 normVec{};
		AEVec2Normalize(&normVec, &dirVec);

		bulletList[player->currentBulletIndex]->healthMaximum = player->buffAttack * 2; // Test damage value
		bulletList[player->currentBulletIndex]->knockbackType = -20; // Use to identify type of bullet
		bulletList[player->currentBulletIndex]->height = 30.0f; // Slightly bigger than normal bullet
		bulletList[player->currentBulletIndex]->width = 30.0f;
		bulletList[player->currentBulletIndex]->lifetime = 1.7f;
		bulletList[player->currentBulletIndex]->position = player->go->position;
		bulletList[player->currentBulletIndex]->velocity = normVec;
		player->currentBulletIndex++;
	}

	void SpawnSavePoint(AEVec2 pos, GameObject::GameObject* objList, int roomID, bool currentSP) {
		GameObject::GameObject* go{};

		if ((go = GameObject::GetGameObject(GameObject::OBJ_SP, objList)) != nullptr)
		{
			*go = GameObject::CreateGameObject(
				/*type*/			GameObject::OBJ_SP,
				/*position*/		pos,
				/*width*/			200.0f,
				/*height*/			200.0f,
				/*active*/			false,
				/*Max HP*/			0,
				/*Speed*/			0,
				/*Radius*/			0.0f,
				/*Lifetime*/		10.0f,
				/*levelNumber*/		roomID
			);
			go->knockbackState = currentSP;
		}
	}

	void UseSavePoint(GameObject::GameObject* obj) { // Toggle
		if (obj->knockbackState == true) {
			obj->knockbackState = false;
		}
		else {
			obj->knockbackState = true;
		}
	}

	void SetSavePoint(GameObject::GameObject* obj) {
		interactableSP = obj;
	}

	GameObject::GameObject* GetSavePoint() {
		return interactableSP;
	}

	
	// End bullets / shooting
}