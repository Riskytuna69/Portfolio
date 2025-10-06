/******************************************************************************/
/*!
\file   GameManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (50%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\author Marc Alviz Evangelista (50%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief
	GameManager is a singleton controlling game logic and keeping track of
	parameters such as whether the game is paused, current game state, etc.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "GameManager.h"
#include "Player.h"
#include "EntityUID.h"
#include "CameraController.h"
#include "SceneManagement.h"
#include "Checkpoint.h"
#include "IntelDoor.h"

GameManager::~GameManager()
{
}

// If you really need comments to understand the following code, you should quit programming immediately.
void GameManager::SetJumpEnhanced(bool status)
{
	playerJumpEnhanced = status;
}

bool GameManager::GetJumpEnhanced()
{
	return playerJumpEnhanced;
}

void GameManager::SetPaused(bool paused)
{
	isGamePaused = paused;
}

bool GameManager::GetPaused()
{
	return isGamePaused;
}

void GameManager::Update()
{
	playerJumpEnhanced = false;
}

GAME_STATE GameManager::GetCurrentGameState() const
{
	return currentState;
}

void GameManager::SetGameState(GAME_STATE newState)
{
	currentState = newState;
	std::string ste = "GAMESTATE";
	switch (newState)
	{
	case CRAWLING:
		ste += "CRAWLING";
		break;
	case DEFENDING:
		ste += "DEFENDING";
		break;
	case GRACE:
		ste += "GRACE";
		break;
	case DEATH:
		ste += "DEATH";
		break;
	case VICTORY:
		ste += "VICTORY";
		Messaging::BroadcastAll("GameWin");

		break;
	default:
		ste += "IDK";
		break;
	}

	Messaging::BroadcastAll(ste);
	//CONSOLE_LOG_EXPLICIT("CURRENT STATE IS NOW: " + ste, LogLevel::LEVEL_DEBUG);
}

void GameManager::ResetGameVariables()
{
	ResetIntel();
	ResetEnemiesKilled();
	ResetWeaponsFound();

	playerDeaths = 0;
	lastCheckpoint = nullptr;
	lastCheckpointID = -1;
	damageShielded = 0;
	timeTaken = 0;
}

void GameManager::SetIntelUnlocked(int id, bool value)
{
	if (id < 0 || id >= unlockedIntel.size())
	{
		return;
	}
	unlockedIntel[id] = value;

	// New code here for the IntelDoor logic!
	ecs::CompHandle<IntelDoorComponent> intelDoor = nullptr;
	// Several sanity checks
	auto compIterator{ ecs::GetCompsBegin<IntelDoorComponent>() };
	if (compIterator!=ecs::GetCompsEnd< IntelDoorComponent>())
	{
		// Get the component
		intelDoor = compIterator.GetComp();

		// Call the SetUnlocked function here
		intelDoor->SetUnlocked(id, value);
	}
}

bool GameManager::GetIntelUnlocked(int id)
{
	if (id < 0 || id >= unlockedIntel.size())
	{
		return false;
	}
	return unlockedIntel[id];
}

int GameManager::GetTotalIntelUnlocked()
{
	int total = 0;
	for (bool intel : unlockedIntel)
	{
		if (intel)
		{
			++total;
		}
	}
	return total;
}

int GameManager::GetTotalIntel()
{
	return static_cast<int>(unlockedIntel.size());
}

void GameManager::ResetEnemiesKilled()
{
	enemiesKilled = 0;
}

int GameManager::GetEnemiesKilled()
{
	return enemiesKilled;
}

void GameManager::OnEnemyKilled()
{
	++enemiesKilled;
}

void GameManager::ResetWeaponsFound()
{
	weaponsFound.clear();
}

int GameManager::GetWeaponsFound()
{
	return static_cast<int>(weaponsFound.size());
}

void GameManager::OnWeaponFound(const std::string& weaponName)
{
	if (weaponsFound.find(weaponName) == weaponsFound.end())
	{
		weaponsFound.emplace(weaponName);
	}
}

int GameManager::GetPlayerDeaths()
{
	return playerDeaths;
}

void GameManager::OnPlayerDied()
{
	++playerDeaths;
}

void GameManager::SetDamageShielded(int value)
{
	damageShielded = value;
}

int GameManager::GetDamageShielded()
{
	return damageShielded;
}

void GameManager::SetTimeTaken(int value)
{
	timeTaken = value;
}

int GameManager::GetTimeTaken()
{
	return timeTaken;
}

bool GameManager::OnCheckpointActivated(int ID, ecs::EntityHandle checkpointEntity, bool overrideID)
{
	if (ID > lastCheckpointID||overrideID)
	{
		// Assign thy variables
		lastCheckpoint = checkpointEntity;
		lastCheckpointID = ID;

		// Set all checkpoint states accordingly
		for (auto checkpointItr = ecs::GetCompsBegin<CheckpointComponent>(); checkpointItr != ecs::GetCompsEnd<CheckpointComponent>(); ++checkpointItr)
		{
			if (checkpointItr->GetID() < ID)
				checkpointItr->SetState(2);
			else if (checkpointItr->GetID() > ID)
				checkpointItr->SetState(0);
			else
				checkpointItr->SetState(1);
		}
		return true;
	}
	return false;
}

ecs::EntityHandle GameManager::GetCurrentCheckpoint()
{
	return lastCheckpoint;
}

std::vector<std::pair<int, int>> GameManager::GetScoreboard()
{
	std::vector<std::pair<int, int>> scoreboard{};
	int totalIntel = GetTotalIntelUnlocked();
	int totalWeapons = GetWeaponsFound();

	// Using formula: 20 * x^0.4
	int damageShieldedScore = static_cast<int>(20.0 * std::pow(static_cast<double>(damageShielded), 0.4));
	int timeTakenScore = 90000 / (timeTaken + 100);

	scoreboard.push_back({ math::Min(enemiesKilled,		9999),	math::Min(enemiesKilled * 20,	9999) });
	scoreboard.push_back({ math::Min(playerDeaths,		9999),	math::Min(playerDeaths * -50,	9999) });
	scoreboard.push_back({ math::Min(totalIntel,		9999),	math::Min(totalIntel * 20,		9999) });
	scoreboard.push_back({ math::Min(totalWeapons,		9999),	math::Min(totalWeapons * 15,	9999) });
	scoreboard.push_back({ math::Min(damageShielded,	9999),	math::Min(damageShieldedScore,	9999) });
	scoreboard.push_back({ math::Min(timeTaken,			9999),	math::Min(timeTakenScore,		9999) });

	// Calculate total
	int total = 0;
	for (auto numScore : scoreboard)
	{
		total += numScore.second;
	}
	scoreboard.push_back({ 0, math::Clamp(total, 0, 9999) });

	return scoreboard;
}

void GameManager::ResetIntel()
{
	unlockedIntel.assign(unlockedIntel.size(), false);
	// New code here for the IntelDoor logic!
	ecs::CompHandle<IntelDoorComponent> intelDoor = nullptr;
	// Several sanity checks
	auto compIterator{ ecs::GetCompsBegin<IntelDoorComponent>() };
	if (compIterator != ecs::GetCompsEnd< IntelDoorComponent>())
	{
		// Get the component
		intelDoor = compIterator.GetComp();

		// Call the SetUnlocked function here
		for(int i = 0;i<GetTotalIntel();++i)
		{
			intelDoor->SetUnlocked(i, false);
		}
	}
}

GameManager::GameManager()
	: lastCheckpointID{ -1 }
	, lastCheckpoint{ nullptr }
	, isGamePaused{ false }
	, playerJumpEnhanced{ false }
	, currentState{ GAME_STATE::CRAWLING }
	, setGraceTimer{ 5.0f }
	, graceTimer{ setGraceTimer }
	, enemiesKilled{ 0 }
	, playerDeaths{ 0 }
	, unlockedIntel(8, false)
	, weaponsFound{ std::set<std::string>() }
	, damageShielded{ 0 }
	, timeTaken{ 0 }
{

}
