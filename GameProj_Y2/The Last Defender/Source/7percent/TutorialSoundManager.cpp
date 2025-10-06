/******************************************************************************/
/*!
\file   TutorialSoundManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Manages background sounds for the tutorial scene.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "TutorialSoundManager.h"

void TutorialSoundManagerComponent::Init()
{
	inited = true;
	ST<AudioManager>::Get()->StopAllSounds();
	ST<AudioManager>::Get()->StartSound(sound, loop);
}

TutorialSoundManagerComponent::TutorialSoundManagerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	sound{ "" },
	loop{ true },
	inited{ false }
{
}

#ifdef IMGUI_ENABLED
void TutorialSoundManagerComponent::EditorDraw(TutorialSoundManagerComponent& comp)
{
	comp.sound.EditorDraw("Sound");
	ImGui::Checkbox("Loop", &comp.loop);
}
#endif

TutorialSoundManagerSystem::TutorialSoundManagerSystem() :
	System_Internal(&TutorialSoundManagerSystem::UpdateSimpleSoundPlayerComp)
{
}

void TutorialSoundManagerSystem::UpdateSimpleSoundPlayerComp(TutorialSoundManagerComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}
}
