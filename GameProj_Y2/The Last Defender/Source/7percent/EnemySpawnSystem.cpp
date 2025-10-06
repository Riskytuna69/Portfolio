/******************************************************************************/
/*!
\file   EnemySpawnSystem.cpp
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
#include "EnemySpawnSystem.h"
#include "GameManager.h"
#include "PrefabManager.h"
#include "DropPodComponent.h"
#include "BossBoundedComponent.h"
EnemySpawnerComponent::EnemySpawnerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw) ,
#endif
	numLocations{ 1 }
	, alphaDecrease { true }
	, currentWave { 0 }
	, totalWaves { 3 }
	, enemiesSpawned { false }
	, numEnemiesToSpawn { 4 }
	, numEnemiesAlive { numEnemiesToSpawn }
	, numEnemyPrefabs { 1 }
	, setGraceTimer { 5.0f }
	, graceTimer { setGraceTimer }
	, objectiveAlive { true }
	, bossSpawned { false }
	, bossSpawnTimer { 10.0f }
	, enemyBaseRate { 1.0f }
	, enemyRate { enemyBaseRate }
	, enemyRateRange { 0.5f }
	, enemySpawnTimer { 0.0f }
	, numSpawned { 0 }
	, prevAmountSpawned { 0 }
	, reinforcementLevel { 0 }
	, currentLocationIndex { 0 }
	, minionsAlive { 0 }
	, noMinionsAlive { true }
	, bossAlive { true }
	, baseSpawnAmount { numEnemiesToSpawn }

{
	for (int i = 0; i < baseSpawnAmount; ++i)
	{
		gunLevels.push(reinforcementLevel);
	}
}

EnemySpawnerComponent::~EnemySpawnerComponent()
{
}

void EnemySpawnerComponent::UpdateNumEnemiesAlive()
{
	numEnemiesAlive -= 1;

	enemiesAliveUI->GetComp<TextComponent>()->SetText("Remaining Enemies: " + std::to_string(GetNumEnemiesAlive()));
	if (numEnemiesAlive <= 0)
	{
		if (currentWave == totalWaves)
		{
			Messaging::BroadcastAll("ReactorDefended");
		}
		if (objectiveAlive)
		{
			ST<GameManager>::Get()->SetGameState(GAME_STATE::GRACE);
			alphaDecrease = true;
			Vector4 color = graceTimerUI->GetComp<TextComponent>()->GetColor();
			color.w = 1;

			graceTimerUI->GetComp<TextComponent>()->SetColor(color);
			ClearWaveText();
		}
		else
		{
			ST<GameManager>::Get()->SetGameState(GAME_STATE::DEFEAT);
			ClearWaveText();

		}
	}


}

void EnemySpawnerComponent::UpdateCurrentWave()
{
	CONSOLE_LOG_EXPLICIT("UPDATING WAVE!", LogLevel::LEVEL_DEBUG);
	graceTimerUI->GetComp<TextComponent>()->SetText("");

	enemiesSpawned = false;
	numEnemiesToSpawn = numEnemiesToSpawn + 1;
	numEnemiesAlive = numEnemiesToSpawn;
	currentWave++;
	CONSOLE_LOG_EXPLICIT("NEXT WAVE! WAVE: " + std::to_string(currentWave) + "/" +std::to_string(totalWaves), LogLevel::LEVEL_DEBUG);
	if (currentWave > totalWaves)
	{
		ST<GameManager>::Get()->SetGameState(GAME_STATE::VICTORY);

	}
	else
	{
		// Update UI
		waveTextUI->GetComp<TextComponent>()->SetText("Wave: " + std::to_string(currentWave) + " / " + std::to_string(totalWaves));
		enemiesAliveUI->GetComp<TextComponent>()->SetText("Remaining Enemies: " + std::to_string(GetNumEnemiesAlive()));
		SpawnEnemies();
	}


}

void EnemySpawnerComponent::SpawnEnemies()
{
	// Update this to include more enemy types - Marc : ^)
	//std::string currentPrefab = enemyPrefabNames[0];
	CONSOLE_LOG_EXPLICIT("Spawning Enemies!", LogLevel::LEVEL_DEBUG);

	// std::string currentPrefab = "WaveEnemy";
	std::string currentPrefab = "DropPod";
	int currentSpawnLocation = 0;

	for (int i = 0; i < numEnemiesToSpawn; ++i)
	{
		ecs::EntityHandle temp = PrefabManager::LoadPrefab(currentPrefab);
		if (temp)
		{
			temp->GetTransform().SetLocalPosition(spawnLocations[currentSpawnLocation]->GetTransform().GetWorldPosition());

			currentSpawnLocation++;
			if (currentSpawnLocation >= spawnLocations.size())
			{
				currentSpawnLocation = 0;
			}
			
		}
		else
		{
			CONSOLE_LOG_EXPLICIT("No such prefab with name: " + currentPrefab + " exists!", LogLevel::LEVEL_ERROR);
			enemiesSpawned = true;
			return;
		}
	}
	enemiesSpawned = true;

}

bool EnemySpawnerComponent::HaveEnemiesSpawned() const
{
	return enemiesSpawned;
}

void EnemySpawnerComponent::UpdateGraceTimer()
{
	if (currentWave == totalWaves)
	{
		Vector4 color = graceTimerUI->GetComp<TextComponent>()->GetColor();
		color.y = 1.0f;
		color.z = 1.0f;
		color.w = 1.0f;

		graceTimerUI->GetComp<TextComponent>()->SetColor(color);

		graceTimerUI->GetComp<TextComponent>()->SetText("VICTORY!!!");

	}
	else
	{
		Vector4 color = graceTimerUI->GetComp<TextComponent>()->GetColor();

		if (alphaDecrease)
		{
			color.w -= GameTime::FixedDt();

			if (color.w <= 0.0f)
			{
				color.w = 0.0f; 
				alphaDecrease = false; 
			}
		}
		else
		{
			color.w += GameTime::FixedDt(); 

			if (color.w >= 1.0f)
			{
				color.w = 1.0f;
				alphaDecrease = true;
			}
		}

		graceTimerUI->GetComp<TextComponent>()->SetColor(color);

		graceTimerUI->GetComp<TextComponent>()->SetText("ENEMIES INCOMING!!!");
	}


	graceTimer -= GameTime::FixedDt();
	if (graceTimer <= 0.0f)
	{
		ST<GameManager>::Get()->SetGameState(GAME_STATE::DEFENDING);
		graceTimer = setGraceTimer;
		graceTimerUI->GetComp<TextComponent>()->SetText("");
		enemiesAliveUI->GetComp<TextComponent>()->SetText("Remaining Enemies: " + std::to_string(minionsAlive));

		//UpdateCurrentWave();
	}


}

int EnemySpawnerComponent::GetCurrentWave() const
{
	return currentWave;
}

int EnemySpawnerComponent::GetNumEnemiesAlive() const
{
	return numEnemiesAlive;
}

int EnemySpawnerComponent::GetNumEnemiesSpawned() const
{
	return numEnemiesToSpawn;
}

float EnemySpawnerComponent::GetGraceTimer() const
{
	return graceTimer;
}

void EnemySpawnerComponent::Restart()
{
	currentWave = 0;
	numEnemiesToSpawn = numEnemiesAlive;
	numEnemiesAlive = numEnemiesToSpawn;
	graceTimer = setGraceTimer;

	alphaDecrease = true;

	objectiveAlive = true;

	ClearWaveText();
}

void EnemySpawnerComponent::ClearWaveText()
{
	if (waveTextUI)
		waveTextUI->GetComp<TextComponent>()->SetText("");
	if (enemiesAliveUI)
		enemiesAliveUI->GetComp<TextComponent>()->SetText("");
	if (graceTimerUI)
		graceTimerUI->GetComp<TextComponent>()->SetText("");
}

void EnemySpawnerComponent::SetObjectiveAliveFlag(bool b)
{
	objectiveAlive = b;
}

void EnemySpawnerComponent::UpdateBossSpawnTimer()
{
	if (bossSpawned)
		return;

	bossSpawnTimer -= GameTime::FixedDt();
	if (bossSpawnTimer <= 0.0f && !bossSpawned)
	{
		/*ecs::EntityHandle boss = */PrefabManager::LoadPrefab("BossAnchor");
		//boss->GetTransform().SetWorldPosition(spawnLocations[0]->GetTransform().GetWorldPosition());
		bossSpawned = true;
	}
}

void EnemySpawnerComponent::Spawn()
{
	if (enemiesSpawned)
	{
		return;
	}

	enemySpawnTimer += GameTime::FixedDt();
	if (enemySpawnTimer >= enemyRate)
	{
		enemySpawnTimer = 0.0f;
		// spawn an enemy 
		ecs::EntityHandle temp = PrefabManager::LoadPrefab("DropPod");
		if (temp)
		{
			temp->GetTransform().SetLocalPosition(spawnLocations[currentLocationIndex]->GetTransform().GetWorldPosition());

			// update index
			currentLocationIndex++;
			if (currentLocationIndex >= spawnLocations.size())
			{
				currentLocationIndex = 0;
			}

		}
		// init new enemyRate 
		enemyRate = enemyBaseRate + util::RandomRangeFloat(-enemyRateRange, enemyRateRange);

		// check num spawned
		++numSpawned;
		++prevAmountSpawned;
		if (numSpawned >= numEnemiesToSpawn)
		{
			enemiesSpawned = true;
			// set up for next wave call
		}

	}
}

void EnemySpawnerComponent::ReinforcementSetup()
{
	++reinforcementLevel;

	enemiesSpawned = false;
	numEnemiesToSpawn += baseSpawnAmount + reinforcementLevel;
	prevAmountSpawned = 0;

	// numenemiestospawn is the max number
	// numspawned is the amount the system HAS spawned so far

	// add to numenemies to spawn based on reinforcementLevel
	// when drop pod is spawned it caluculates which index it is
	// that index is used to determine the gun level
	for (int i = 0; i < baseSpawnAmount + reinforcementLevel; ++i)
	{
		gunLevels.push(reinforcementLevel);
	}
}

void EnemySpawnerComponent::IncrementMinions()
{
	if (minionsAlive == 0)
	{
		noMinionsAlive = false;
	}
	minionsAlive += 1;
	enemiesAliveUI->GetComp<TextComponent>()->SetText("Remaining Enemies: " + std::to_string(minionsAlive));
}

void EnemySpawnerComponent::DecrementMinions()
{
	minionsAlive -= 1;
	if (minionsAlive == 0 && numEnemiesToSpawn == numSpawned)
	{
		noMinionsAlive = true;
	}
	enemiesAliveUI->GetComp<TextComponent>()->SetText("Remaining Enemies: " + std::to_string(minionsAlive));
}

bool EnemySpawnerComponent::GetNoMinionsAlive() const
{
	return noMinionsAlive;
}

void EnemySpawnerComponent::SetBossAlive(bool value)
{
	bossAlive = value;
	if (value == false)
	{
		// stop spawning droppods
		enemiesSpawned = true;
		// kill all drop pods
		for (auto podIte = ecs::GetCompsBegin<DropPodComponent>(), endPod = ecs::GetCompsEnd<DropPodComponent>(); podIte != endPod; ++podIte)
		{
			ecs::DeleteEntity(podIte.GetEntity());
		}
		// kill all bossminions
		for (auto minionIte = ecs::GetCompsBegin<BossBoundedComponent>(), endMinion = ecs::GetCompsEnd<BossBoundedComponent>(); minionIte != endMinion; ++minionIte)
		{
			HealthComponent* h = minionIte.GetEntity()->GetComp<HealthComponent>();
			if (h)
			{
				h->TakeDamage(9999999);
			}
		}
	}
}

bool EnemySpawnerComponent::GetBossAlive() const
{
	return bossAlive;
}

int EnemySpawnerComponent::GetGunLevel()
{
	int level = gunLevels.front();
	gunLevels.pop();
	return level;
}

bool EnemySpawnerComponent::GetHasWon() const
{
	return hasWon;
}

void EnemySpawnerComponent::SetHasWon(bool value)
{
	hasWon = value;
}

#ifdef IMGUI_ENABLED
void EnemySpawnerComponent::EditorDraw(EnemySpawnerComponent& comp)
{
	ImGui::PushID("WAVE_UI");

	ImGui::Text("Wave Counter UI");
	comp.waveTextUI.EditorDraw("");

	ImGui::PopID();
	ImGui::PushID("ENEMIESCOUNT_UI");

	//ImGui::NewLine;

	ImGui::Text("Enemy Count UI");
	comp.enemiesAliveUI.EditorDraw("");

	ImGui::PopID();
	ImGui::PushID("GRACETIMER_UI");

	//ImGui::NewLine;

	ImGui::Text("Grace Timer UI");
	comp.graceTimerUI.EditorDraw("");

	ImGui::PopID();

	//ImGui::NewLine;

	// Input field to modify numContainers
	ImGui::PushID("ENEMY_SPAWN_LOCATIONS");
	ImGui::Text("Number of Locations");
	//ImGui::NewLine;
	ImGui::InputInt("", &comp.numLocations);

	// Clamp to 1
	if (comp.numLocations < 1)
	{
		comp.numLocations = 1;
	}

	// Add button to apply changes
	if (ImGui::Button("Resize"))
	{
		comp.spawnLocations.resize(static_cast<size_t>(comp.numLocations));
	}

	if (comp.spawnLocations.size() > 0)
	{
		ImGui::Text("Locations: ");
		//ImGui::NewLine;
	}
	// Draw input fields for EntityReferences
	int i = 1;
	for (EntityReference& ref : comp.spawnLocations)
	{
		std::string label = "SpawnLocation " + std::to_string(i++);

		ImGui::PushID(label.c_str());

		ImGui::Text(label.c_str());
		//ImGui::NewLine;
		ref.EditorDraw("");

		ImGui::PopID();
	}
	ImGui::PopID();

	//ImGui::NewLine;
	//ImGui::NewLine;

	ImGui::PushID("ENEMY_PREFAB_NAMES");
	// Input field to modify numContainers
	ImGui::Text("Number of Enemy Prefabs");
	//ImGui::NewLine;
	ImGui::InputInt("", &comp.numEnemyPrefabs);

	// Clamp to 1
	if (comp.numEnemyPrefabs < 1)
	{
		comp.numEnemyPrefabs = 1;
	}

	// Add button to apply changes
	if (ImGui::Button("Resize"))
	{
		comp.enemyPrefabNames.resize(static_cast<size_t>(comp.numEnemyPrefabs));
	}

	// Draw input fields for EntityReferences
	i = 1;
	for (std::string& ref : comp.enemyPrefabNames)
	{
		std::string label = "Prefab " + std::to_string(i++);

		ImGui::PushID(label.c_str());

		ImGui::Text(label.c_str());
		//ImGui::NewLine;
		ImGui::InputText("", &ref[0], ref.capacity());

		ImGui::PopID();
	}
	ImGui::PopID();

	ImGui::PushID("ENEMYRATE");
	ImGui::Text("Enemy Spawn Rate");
	ImGui::InputFloat("", &comp.enemyBaseRate);
	ImGui::PopID();

	ImGui::PushID("ENEMYRATERANGE");
	ImGui::Text("Enemy Spawn Rate Range");
	ImGui::InputFloat("", &comp.enemyRateRange);
	ImGui::PopID();


	ImGui::PushID("BOSSSPAWNTIMERSYS");
	ImGui::Text("Boss Spawn Timer");
	ImGui::InputFloat("", &comp.bossSpawnTimer);
	ImGui::PopID();


	ImGui::PushID("NUMENEMIESTOSPAWN");
	ImGui::Text("Amount of Enemies to Spawn");
	ImGui::InputInt("", &comp.numEnemiesToSpawn);
	ImGui::PopID();

	ImGui::PushID("NUMENEMIES SPAWNED");
	ImGui::Text("Number of Enemies Spawned");
	ImGui::InputInt("", &comp.numSpawned);
	ImGui::PopID();

	ImGui::Text("~~~~~~~~DEBUG~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("MINIONSALIVE");
	std::string s = "Minions Alive: " + std::to_string(comp.minionsAlive);
	ImGui::Text(s.c_str());
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("NOMINIONSALIVE");
	ImGui::Text("No Minions Alive");
	ImGui::Checkbox("", &comp.noMinionsAlive);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("ISBOSSALIVE");
	ImGui::Text("Is Boss Alive");
	ImGui::Checkbox("", &comp.bossAlive);
	ImGui::PopID();
}
#endif
EnemySpawnSystem::EnemySpawnSystem()
	: System_Internal{ &EnemySpawnSystem::UpdateEnemySpawnerComp }
{
}

void EnemySpawnSystem::OnAdded()
{
	Messaging::Subscribe("DiedEnemyWave", EnemySpawnSystem::EnemyDiedCallBack);
	Messaging::Subscribe("GAMERESTART", EnemySpawnSystem::RestartCallBack);
	Messaging::Subscribe("CallForReinforcements", EnemySpawnSystem::ReinforcementCallback);

	Messaging::Subscribe("MinionSpawned", EnemySpawnSystem::MinionSpawnedCallback);
	Messaging::Subscribe("DiedBossMinion", EnemySpawnSystem::MinionDiedCallback);
	Messaging::Subscribe("DiedBoss", EnemySpawnSystem::BossDiedCallback);
}

void EnemySpawnSystem::OnRemoved()
{
	Messaging::Unsubscribe("DiedEnemyWave", EnemySpawnSystem::EnemyDiedCallBack);
	Messaging::Unsubscribe("GAMERESTART", EnemySpawnSystem::RestartCallBack);
	Messaging::Unsubscribe("CallForReinforcements", EnemySpawnSystem::ReinforcementCallback);

	Messaging::Unsubscribe("MinionSpawned", EnemySpawnSystem::MinionSpawnedCallback);
	Messaging::Unsubscribe("DiedBossMinion", EnemySpawnSystem::MinionDiedCallback);
	Messaging::Unsubscribe("DiedBoss", EnemySpawnSystem::BossDiedCallback);

}

void EnemySpawnSystem::UpdateEnemySpawnerComp(EnemySpawnerComponent& comp)
{
	if (ST<GameManager>::Get()->GetPaused())
		return;

	if (ST<GameManager>::Get()->GetCurrentGameState() == GAME_STATE::GRACE)
	{
		comp.UpdateGraceTimer();
	}
	//if (ST<GameManager>::Get()->GetCurrentGameState() != GAME_STATE::CRAWLING)
	if (ST<GameManager>::Get()->GetCurrentGameState() == GAME_STATE::DEFENDING)
	{
		comp.UpdateBossSpawnTimer();
		comp.Spawn();
		if (!comp.GetBossAlive() && !comp.GetHasWon())
		{
			comp.SetHasWon(true);
			CONSOLE_LOG(LEVEL_INFO) << "Won";
			//ST<Scheduler>::Get()->Add(2.0f, [&] {
			//	ST<GameManager>::Get()->SetGameState(GAME_STATE::VICTORY);
			//	});
		}
	}
	else if (ST<GameManager>::Get()->GetCurrentGameState() == GAME_STATE::CRAWLING)
	{
		comp.ClearWaveText();
	}
}

void EnemySpawnSystem::EnemyDiedCallBack()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->UpdateNumEnemiesAlive();
}

void EnemySpawnSystem::RestartCallBack()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->Restart();
}

void EnemySpawnSystem::ObjectiveDiedCallBack()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->SetObjectiveAliveFlag(false);
}

void EnemySpawnSystem::ReinforcementCallback()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->ReinforcementSetup();
}

void EnemySpawnSystem::MinionSpawnedCallback()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->IncrementMinions();
}

void EnemySpawnSystem::MinionDiedCallback()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->DecrementMinions();
}

void EnemySpawnSystem::BossDiedCallback()
{
	auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
	ite->SetBossAlive(false);
}

EnemyPatrolSpawnComponent::EnemyPatrolSpawnComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	weaponPrefabName{ "Weapon_Pistol" }
	, health{ 100 }
	, speed{ 150.0f }
{
}

void EnemyPatrolSpawnComponent::OnStart()
{
	EntityReference thisEntity = ecs::GetEntity(this);
	ST<Scheduler>::Get()->Add(0.0f, [thisEntity]() -> void {
		ecs::EntityHandle enemyEntity{ PrefabManager::LoadPrefab("PatrolEnemy") };
		enemyEntity->GetTransform().SetWorldPosition(thisEntity->GetTransform().GetWorldPosition());

		auto thisComp = thisEntity->GetComp<EnemyPatrolSpawnComponent>();

		if (auto enemyComp{ enemyEntity->GetComp<EnemyControllerComponent>() })
		{
			enemyComp->SetWeapon(thisComp->weaponPrefabName);
			enemyComp->SetSpeed(thisComp->speed);
		}
		if (auto healthComp{ enemyEntity->GetComp<HealthComponent>() })
		{
			healthComp->SetMaxHealth(thisComp->health);
			healthComp->SetHealth(thisComp->health);
		}
		ecs::DeleteEntity(thisEntity);
	});
}

void EnemyPatrolSpawnComponent::EditorDraw(EnemyPatrolSpawnComponent& comp)
{
	if (gui::Combo gunCombo{ "Gun", comp.weaponPrefabName.c_str() })
		for (const std::string& prefabName : PrefabManager::AllPrefabs())
			if (prefabName.starts_with("Weapon_"))
				if (gunCombo.Selectable(prefabName.c_str(), prefabName == comp.weaponPrefabName))
					comp.weaponPrefabName = prefabName;
	gui::VarDrag("Health", &comp.health);
	gui::VarDrag("Speed", &comp.speed);
}
