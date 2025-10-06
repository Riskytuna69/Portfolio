/******************************************************************************/
/*!
\file   CheatCodes.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/28/2024

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief

	Defininations for CheatCodes function


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "CheatCodes.h"
#include "GameManager.h"
#include "Checkpoint.h"

Time CheatCodes::start = std::chrono::steady_clock::now();
Time CheatCodes::end = std::chrono::steady_clock::now();


void CheatCodes::ClearCheats()
{
	if (godMode)
	{
		godMode = false;
		ClearGodModeCheat();
	}
	if (slowMotion)
	{
		slowMotion = false;
		GameTime::SetTimeScale(1.0f);
		ST<AudioManager>::Get()->SetGroupPitch(1.0f, "BGM");
		ST<AudioManager>::Get()->SetGroupVolume(1.0f, "BGM");
		ST<AudioManager>::Get()->SetGroupPitch(1.0f, "SFX");
		ST<AudioManager>::Get()->SetGroupVolume(1.0f, "SFX");
	}
	
}

void GodModeCheat()
{
	auto playerCompIt{ ecs::GetCompsBegin<PlayerComponent>() };
	auto objectiveCompIt{ ecs::GetCompsBegin<MainObjectiveComponent>() };

	if (playerCompIt != ecs::GetCompsEnd<PlayerComponent>())
	{
		playerCompIt.GetEntity()->GetComp<HealthComponent>()->SetMaxHealth(99999);
		playerCompIt.GetEntity()->GetComp<HealthComponent>()->SetHealth(99999);
	}

	if (objectiveCompIt != ecs::GetCompsEnd<MainObjectiveComponent>())
	{
		objectiveCompIt.GetEntity()->GetComp<HealthComponent>()->SetMaxHealth(99999);
		objectiveCompIt.GetEntity()->GetComp<HealthComponent>()->SetHealth(99999);
	}

	//TODO add like a text box that says  god more or something
}

void ClearGodModeCheat() 
{
	auto playerCompIt{ ecs::GetCompsBegin<PlayerComponent>() };
	auto objectiveCompIt{ ecs::GetCompsBegin<MainObjectiveComponent>() };

	if (playerCompIt != ecs::GetCompsEnd<PlayerComponent>())
	{
		playerCompIt.GetEntity()->GetComp<HealthComponent>()->SetMaxHealth(100);
		playerCompIt.GetEntity()->GetComp<HealthComponent>()->SetHealth(100);	
	}

	if (objectiveCompIt != ecs::GetCompsEnd<MainObjectiveComponent>())
	{
		objectiveCompIt.GetEntity()->GetComp<HealthComponent>()->SetMaxHealth(500);
		objectiveCompIt.GetEntity()->GetComp<HealthComponent>()->SetHealth(500);
	}
	//May need to change how the values are set back in future D:
	//TODO add like a text box that says clear or something
}

void CheckpointTeleportCheat(bool next)
{
	// Get current checkpoint
	int currentIndex = -1;
	
	if(ST<GameManager>::Get()->GetCurrentCheckpoint())
	currentIndex = ST<GameManager>::Get()->GetCurrentCheckpoint()->GetComp<CheckpointComponent>()->GetID();
	
	// Get the max checkpoint index (to be safe)
	int maxIndex = -1;
	for (auto checkpoint = ecs::GetCompsBegin<CheckpointComponent>(); checkpoint != ecs::GetCompsEnd<CheckpointComponent>(); ++checkpoint)
	{
		if (checkpoint->GetID() > maxIndex)
			maxIndex = checkpoint->GetID();
	}

	// Sanity check
	if (maxIndex == -1)
		return;

	if (next)
		++currentIndex;
	else
		--currentIndex;

	currentIndex = math::Clamp(currentIndex, 0, maxIndex);

	for (auto checkpoint = ecs::GetCompsBegin<CheckpointComponent>(); checkpoint != ecs::GetCompsEnd<CheckpointComponent>(); ++checkpoint)
	{
		if (checkpoint->GetID() == currentIndex)
		{
			ST<GameManager>::Get()->OnCheckpointActivated(currentIndex, checkpoint.GetEntity(), true);
			// New respawn here
			ecs::CompHandle<PlayerComponent> playerComp = ecs::GetCompsBegin<PlayerComponent>().GetComp();
			ecs::EntityHandle playerEntity = ecs::GetEntity(playerComp);
			playerEntity->GetTransform().SetWorldPosition(checkpoint.GetEntity()->GetTransform().GetWorldPosition());
			break;
		}
	}
}
void SpawnLaserRifleCheat()
{
	auto playerCompIt{ ecs::GetCompsBegin<PlayerComponent>() };

	if (playerCompIt != ecs::GetCompsEnd<PlayerComponent>())
	{
		ecs::EntityHandle laserGun = ST<PrefabManager>::Get()->LoadPrefab("Weapon_Railgun");
		laserGun->GetTransform().SetWorldPosition(playerCompIt.GetEntity()->GetTransform().GetWorldPosition());
	}
}

bool CheatCodes::PreRun()
{
	/* 
	 * UP   : 0001
	 * Down : 0010
	 * Left : 0100
	 * Right: 1000
	*/

	end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

	//Press C to clear cheats
	if (Input::GetKeyCurr(KEY::C))
	{
		ClearCheats();
	}

	//Clear all bits if more that 1 sec have passed since pressing
	if (elapsed.count() >= 1)
	{
		inputCmds.reset();
		bitPos = 0;
	}
		
	if (Input::GetKeyPressed(KEY::UP) || Input::GetKeyPressed(KEY::DOWN) ||
		Input::GetKeyPressed(KEY::LEFT) || Input::GetKeyPressed(KEY::RIGHT))
	{
		start = std::chrono::steady_clock::now();

		//An extra command is inputted and matches none of the cheats
		if (bitPos >= 16)
		{
			inputCmds.reset();
			bitPos = 0;
		}

		mask.reset();
		//Inputs here
		if (Input::GetKeyPressed(KEY::UP))
		{
			mask.set(0 + bitPos);
			bitPos += 4;
		}
		else if (Input::GetKeyPressed(KEY::DOWN))
		{
			mask.set(1 + bitPos);
			bitPos += 4;
		}
		else if (Input::GetKeyPressed(KEY::LEFT))
		{
			mask.set(2 + bitPos);
			bitPos += 4;
		}
		else if (Input::GetKeyPressed(KEY::RIGHT))
		{
			mask.set(3 + bitPos);
			bitPos += 4;
		}
		else
		{
			//Nothing
		}
		inputCmds |= mask;


		//Checks here
		if (inputCmds == Cheats::GodMode_PlayerAndObjective)
		{
			godMode = true;
			GodModeCheat();
			inputCmds.reset();
			bitPos = 0;
		}
		if (inputCmds == Cheats::SlowMotion)
		{
			slowMotion = !slowMotion;
			GameTime::SetTimeScale(slowMotion ? 0.1f : 1.0f);
			ST<AudioManager>::Get()->SetGroupPitch(slowMotion ? 0.1f : 1.0f, "BGM");
			ST<AudioManager>::Get()->SetGroupVolume(slowMotion ? 10.0f : 1.0f, "BGM");
			ST<AudioManager>::Get()->SetGroupPitch(slowMotion ? 0.1f : 1.0f, "SFX");
			ST<AudioManager>::Get()->SetGroupVolume(slowMotion ? 10.0f : 1.0f, "SFX");

			inputCmds.reset();
			bitPos = 0;
		}
		if (inputCmds == Cheats::LaserRifle)
		{
			SpawnLaserRifleCheat();
		}
		//else if (inputCmds == Cheats::Cheat2){}

	}

	// Checkpoint Teleport cheat
	if (Input::GetKeyPressed(KEY::NUM_9))
	{
		CheckpointTeleportCheat(false);
	}
	else if (Input::GetKeyPressed(KEY::NUM_0))
	{
		CheckpointTeleportCheat(true);
	}


	return false;
}

