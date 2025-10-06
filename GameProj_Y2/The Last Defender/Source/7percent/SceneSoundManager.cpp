/******************************************************************************/
/*!
\file   SceneSoundManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	SceneSoundManager is an ECS component-system pair which contains functionality
	to do audio transition between underground and overground regions within
	the game scene, as well as handle most of the background sounds that play
	when the game scene starts.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "SceneSoundManager.h"
#include "GameCameraController.h"
#include "Console.h"

SceneSoundManagerComponent::SceneSoundManagerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	playerEntity{ nullptr },
	backgroundMusic{ "BattleMusic" },
	overgroundSound{ "Wind" },
	undergroundSound{ "RoomTone" },
	boundaryX{ 0.0f },
	boundaryWidth{ 200.0f },
	inited{ false }
{
}

void SceneSoundManagerComponent::Init()
{
	inited = true;
	ST<AudioManager>::Get()->StopAllSounds();
		
	ST<AudioManager>::Get()->InterpolateGroupVolume(ST<AudioManager>::Get()->GetGroupVolume("BGM"), 1.5f, "BGM");
	ST<AudioManager>::Get()->InterpolateGroupVolume(ST<AudioManager>::Get()->GetGroupVolume("SFX"), 1.5f, "SFX");

	ST<AudioManager>::Get()->SetChannelGroup(backgroundMusic, "BGM");
	ST<AudioManager>::Get()->SetChannelGroup(overgroundSound, "SFX");
	ST<AudioManager>::Get()->SetChannelGroup(undergroundSound, "SFX");

	// When the game starts, both underground and overground sounds play
	ST<AudioManager>::Get()->StartSingleSound(backgroundMusic, true);
	ST<AudioManager>::Get()->StartSingleSound(overgroundSound, true);
	ST<AudioManager>::Get()->StartSingleSound(undergroundSound, true);
}

void SceneSoundManagerComponent::Update()
{
	float playerYPosition = playerEntity->GetTransform().GetWorldPosition().x;
	float difference = boundaryX - playerYPosition;

	// If the difference is negative, the player is after the boundary
	if (difference < 0)
	{
		// Modify wind sound volume
		ST<AudioManager>::Get()->SetSoundVolume(overgroundSound, math::Clamp(-difference / boundaryWidth, 0.0f, 0.6f));
		ST<AudioManager>::Get()->SetSoundVolume(undergroundSound, 0.0f);
	}
	else // if the difference is positive, the player is before the boundary
	{
		// Modify room tone sound volume
		ST<AudioManager>::Get()->SetSoundVolume(undergroundSound, math::Clamp(difference / boundaryWidth, 0.0f, 2.0f));
		ST<AudioManager>::Get()->SetSoundVolume(overgroundSound, 0.0f);
	}
}

#ifdef IMGUI_ENABLED
void SceneSoundManagerComponent::EditorDraw(SceneSoundManagerComponent& comp)
{
	comp.playerEntity.EditorDraw("Player Entity");
	comp.backgroundMusic.EditorDraw("Background Music");
	comp.overgroundSound.EditorDraw("Overground Sound");
	comp.undergroundSound.EditorDraw("Underground Sound");

	ImGui::DragFloat("Boundary X", &comp.boundaryX);
	ImGui::DragFloat("Boundary Width", &comp.boundaryWidth);
}
#endif

SceneSoundManagerSystem::SceneSoundManagerSystem()
	: System_Internal{ &SceneSoundManagerSystem::UpdateSceneSoundManager }
{
}

void SceneSoundManagerSystem::UpdateSceneSoundManager(SceneSoundManagerComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}
	comp.Update();
}
