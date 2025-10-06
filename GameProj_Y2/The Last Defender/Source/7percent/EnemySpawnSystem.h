
/******************************************************************************/
/*!
\file   EnemySpawnSystem.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/24/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Funciton definations for system that controls where, how and when enemy spawns.


All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "ObjectiveTimer.h"
#include "MainObjective.h"
#include "PrefabManager.h"
#include "Health.h"
#include "Player.h"
#include "EnemyStateMachine.h"

enum ENEMY_TYPE
{

};

class EnemySpawnerComponent : public IRegisteredComponent<EnemySpawnerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<EnemySpawnerComponent>
#endif
{
private:

public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	EnemySpawnerComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~EnemySpawnerComponent();

	/*****************************************************************//*!
	\brief
		Updates the number of enemies alive in the current wave.
	\return
	*//******************************************************************/
	void UpdateNumEnemiesAlive();

	/*****************************************************************//*!
	\brief
		Updates the current wave to the next wave alongside 
		respective varaibles to init the next wave.
	\return
	*//******************************************************************/
	void UpdateCurrentWave();

	/*****************************************************************//*!
	\brief
		Spawns enemies of the current wave.
	\return
	*//******************************************************************/
	void SpawnEnemies();

	/*****************************************************************//*!
	\brief
		Checks if the current wave has spawned its enemies
	\return
		bool if the current wave enemies have been spawned
	*//******************************************************************/
	bool HaveEnemiesSpawned() const;

	/*****************************************************************//*!
	\brief
		Updates the timer of the grace period before the next wave.
	\return
	*//******************************************************************/
	void UpdateGraceTimer();
	
	// For UI
	/*****************************************************************//*!
	\brief
		Gets the current wave
	\return
		int of wave number
	*//******************************************************************/
	int GetCurrentWave() const;

	/*****************************************************************//*!
	\brief
		Gets the number of wave enemies alive
	\return
		int of alive enemy count
	*//******************************************************************/
	int GetNumEnemiesAlive() const;

	/*****************************************************************//*!
	\brief
		Gets the number of wave enemies spawned
	\return
		int of enemy count
	*//******************************************************************/
	int GetNumEnemiesSpawned() const;

	/*****************************************************************//*!
	\brief
		Gets the grace period timer
	\return
		float of timer
	*//******************************************************************/
	float GetGraceTimer() const;

	/*****************************************************************//*!
	\brief
		Restarts system values to initial wave 1 values
	\return
	*//******************************************************************/
	void Restart();

	/*****************************************************************//*!
	\brief
		Clears the text of the wave ui
	\return
	*//******************************************************************/
	void ClearWaveText();

	/*****************************************************************//*!
	\brief
		Sets the ObjectiveAlive bool
	\param[in] b
		new bool value of the variable
	\return
	*//******************************************************************/
	void SetObjectiveAliveFlag(bool b);

	/*****************************************************************//*!
	\brief
		Updates the timer for spawning the boss
	\return
	*//******************************************************************/
	void UpdateBossSpawnTimer();

	/*****************************************************************//*!
	\brief
		Spawns enemies based on boss weapon status
	\return
	*//******************************************************************/
	void Spawn();

	/*****************************************************************//*!
	\brief
		Sets the variables ready for the next wave of enemies to be spawned
	\return
	*//******************************************************************/
	void ReinforcementSetup();

	/*****************************************************************//*!
	\brief
		Increments the number of minions spawned by the boss
	\return
	*//******************************************************************/
	void IncrementMinions();

	/*****************************************************************//*!
	\brief
		Decrements the number of minions when one dies
	\return
	*//******************************************************************/
	void DecrementMinions();

	/*****************************************************************//*!
	\brief
		Gets the bool for if all minions have died
	\return
		noMinionsAlive bool
	*//******************************************************************/
	bool GetNoMinionsAlive() const;

	/*****************************************************************//*!
	\brief
		Setter for the bossAlive variable
	\param[in] value
		New value for the bossAlive variable
	\return
	*//******************************************************************/
	void SetBossAlive(bool value);

	/*****************************************************************//*!
	\brief
		Getter for the bossAlive variable
	\return
		bossAlive bool
	*//******************************************************************/
	bool GetBossAlive() const;

	/*****************************************************************//*!
	\brief
		Gets a gun level from the vector to spawn an enemy with
	\return
	*//******************************************************************/
	int GetGunLevel();

	bool GetHasWon() const;
	void SetHasWon(bool value);

private:
	int numLocations;
	std::vector<EntityReference> spawnLocations;

	EntityReference waveTextUI;
	EntityReference enemiesAliveUI;
	EntityReference graceTimerUI;

	bool alphaDecrease;

	int currentWave;
	int totalWaves;

	bool enemiesSpawned;
	int numEnemiesToSpawn;
	int numEnemiesAlive;

	int numEnemyPrefabs;
	std::vector<std::string> enemyPrefabNames;

	const float setGraceTimer;
	float graceTimer;

	bool objectiveAlive;



	float enemyBaseRate;
	float enemyRate;
	float enemySpawnTimer;
	float enemyRateRange;
	int numSpawned;
	int prevAmountSpawned;
	float bossSpawnTimer;
	bool bossSpawned;
	int currentLocationIndex;

	int reinforcementLevel;
	int baseSpawnAmount;

	int minionsAlive;
	bool noMinionsAlive;
	bool bossAlive;
	bool hasWon;

	std::queue<int> gunLevels;

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Shows variables in inspector that can be modified
	\param[in, out] comp
		Component the variables are found in
	\return
	*//******************************************************************/
	static void EditorDraw(EnemySpawnerComponent& comp);
#endif
	property_vtable()
};
property_begin(EnemySpawnerComponent)
{
	property_var(numLocations),
	property_var(spawnLocations),
	property_var(waveTextUI),
	property_var(enemiesAliveUI),
	property_var(graceTimerUI),
	property_var(numEnemyPrefabs),
	property_var(enemyPrefabNames),
	property_var(numEnemiesToSpawn),
	property_var(enemyBaseRate),
	property_var(enemyRateRange),
	property_var(bossSpawnTimer),
}
property_vend_h(EnemySpawnerComponent)
#pragma endregion

class EnemySpawnSystem : public ecs::System<EnemySpawnSystem, EnemySpawnerComponent>
{
public:

	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	EnemySpawnSystem();

	/*****************************************************************//*!
	\brief
		Subscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnRemoved() override;

private:

	/*****************************************************************//*!
	\brief
		Updates the Enemey Spawner Component of game objects
	\param[in, out] comp
		Component to update
	\return
	*//******************************************************************/
	void UpdateEnemySpawnerComp(EnemySpawnerComponent& comp);

	/*****************************************************************//*!
	\brief
		Callback function to Update the number of enemies alive in the 
		wave.
	\return
	*//******************************************************************/
	static void EnemyDiedCallBack();

	/*****************************************************************//*!
	\brief
		Callback function to restart the spawner system
	\return
	*//******************************************************************/
	static void RestartCallBack();

	/*****************************************************************//*!
	\brief
		Callback function to set the objective alive bool to false
	\return
	*//******************************************************************/
	static void ObjectiveDiedCallBack();

	/*****************************************************************//*!
	\brief
		Callback function for when the boss is calling for a wave
		of enemies to be spawned
	\return
	*//******************************************************************/
	static void ReinforcementCallback();

	/*****************************************************************//*!
	\brief
		Callback function for when a minion has fully spawned
	\return
	*//******************************************************************/
	static void MinionSpawnedCallback();

	/*****************************************************************//*!
	\brief
		Callback function for when a minion has died
	\return
	*//******************************************************************/
	static void MinionDiedCallback();

	/*****************************************************************//*!
	\brief
		Callback function for when the boss dies
	\return
	*//******************************************************************/
	static void BossDiedCallback();
private:
};

/*****************************************************************//*!
\class EnemyPatrolSpawnComponent
\brief
	Spawns a patrol enemy on the first frame of the simulation.
*//******************************************************************/
class EnemyPatrolSpawnComponent
	: public IRegisteredComponent<EnemyPatrolSpawnComponent>
#ifdef IMGUI_ENABLED
	, public IEditorComponent<EnemyPatrolSpawnComponent>
#endif
	, public IGameComponentCallbacks<EnemyPatrolSpawnComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Constructor.
	*//******************************************************************/
	EnemyPatrolSpawnComponent();

	/*****************************************************************//*!
	\brief
		Spawns a patrol enemy and deletes itself.
	*//******************************************************************/
	void OnStart() override;

private:
	/*****************************************************************//*!
	\brief
		Draws this component to inspector.
	*//******************************************************************/
	static void EditorDraw(EnemyPatrolSpawnComponent& comp);

private:
	//! The name of the weapon to spawn with.
	std::string weaponPrefabName;
	//! The enemy health.
	int health;
	//! The enemy speed.
	float speed;

public:
	property_vtable()

};
property_begin(EnemyPatrolSpawnComponent)
{
	property_var(weaponPrefabName),
	property_var(health),
	property_var(speed)
}
property_vend_h(EnemyPatrolSpawnComponent)
