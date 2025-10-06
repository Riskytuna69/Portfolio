/******************************************************************************/
/*!
\file   GameManager.h
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

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"

/*****************************************************************//*!
\enum GAME_STATE
\brief
	Game states.
*//******************************************************************/
enum GAME_STATE
{
	CRAWLING,
	DEFENDING,
	GRACE,
	VICTORY,
	DEFEAT,
	DEATH
};

/*****************************************************************//*!
\class GameManager
\brief
	Singleton class.
*//******************************************************************/
class GameManager
{
public:
	friend class ST<GameManager>;

	/*****************************************************************//*!
	\brief
		Default destructor.
	*//******************************************************************/
	~GameManager();

	/*****************************************************************//*!
	\brief
		Set whether the player's jump should be enhanced.
	\param status
		True for enhanced, false for not.
	*//******************************************************************/
	void SetJumpEnhanced(bool status);

	/*****************************************************************//*!
	\brief
		Get whether the player's jump is enhanced.
	\return
		True for enhanced, false for not.
	*//******************************************************************/
	bool GetJumpEnhanced();

	/*****************************************************************//*!
	\brief
		Set whether the game should be paused.
	\param paused
		True for paused, false for unpaused.
	*//******************************************************************/
	void SetPaused(bool paused);

	/*****************************************************************//*!
	\brief
		Get whether the game is paused.
	\return
		True for paused, false for unpaused.
	*//******************************************************************/
	bool GetPaused();

	/*****************************************************************//*!
	\brief
		Update function to be called continuously.
	*//******************************************************************/
	void Update();

	/*****************************************************************//*!
	\brief
		Get the current game state.
	\return
		GAME_STATE enumerator.
	*//******************************************************************/
	GAME_STATE GetCurrentGameState() const;

	/*****************************************************************//*!
	\brief
		Set the current game state.
	\param newState
		GAME_STATE enumerator.
	*//******************************************************************/
	void SetGameState(GAME_STATE newState);

	void ResetGameVariables();

	void ResetIntel();
	void SetIntelUnlocked(int id, bool value);
	bool GetIntelUnlocked(int id);
	int GetTotalIntelUnlocked();
	int GetTotalIntel();

	void ResetEnemiesKilled();
	int GetEnemiesKilled();
	void OnEnemyKilled();

	void ResetWeaponsFound();
	int GetWeaponsFound();
	void OnWeaponFound(const std::string& weaponName);

	int GetPlayerDeaths();
	void OnPlayerDied();

	/*****************************************************************//*!
	\brief
		Set the damage shielded.
	\param value
		Integer value to set.
	*//******************************************************************/
	void SetDamageShielded(int value);

	/*****************************************************************//*!
	\brief
		Returns the damage shielded.
	\return
		Damage shielded.
	*//******************************************************************/
	int GetDamageShielded();

	/*****************************************************************//*!
	\brief
		Set the time taken.
	\param value
		Integer value to set.
	*//******************************************************************/
	void SetTimeTaken(int value);

	/*****************************************************************//*!
	\brief
		Returns the time taken.
	\return
		Time taken.
	*//******************************************************************/
	int GetTimeTaken();

	/*****************************************************************//*!
	\brief
		Called when a checkpoint is activated.
	\return
		True if the checkpoint was activated successfully.
	*//******************************************************************/
	bool OnCheckpointActivated(int ID, ecs::EntityHandle checkpointEntity, bool overrideID = false);
	ecs::EntityHandle GetCurrentCheckpoint();

	/*****************************************************************//*!
	\brief
		Get the scoreboard. Order of scores is strictly as follows:
		EnemiesKilled	0
		PlayerDeaths	1
		UnlockedIntel	2
		WeaponsFound	3
		DamageShielded	4
		TimeTaken		5
		TotalScore		6
	\return
		vector of pair of ints. The order corresponds to the order of text
		references in GameOver.cpp, vecScores. The first represents the raw
		number, the second represents the evaluated score.
	*//******************************************************************/
	std::vector<std::pair<int, int>> GetScoreboard();
private:
	// Checkpoints
	int lastCheckpointID;
	EntityReference lastCheckpoint;


	// Pause menu
	bool isGamePaused;

	// Jump system
	bool playerJumpEnhanced;

	// Wave spawning
	GAME_STATE currentState;
	float setGraceTimer;
	float graceTimer;

	// Stat tracking
	int enemiesKilled;
	int playerDeaths;
	std::vector<bool> unlockedIntel;
	std::set<std::string> weaponsFound;
	int damageShielded;
	int timeTaken;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	GameManager();
};