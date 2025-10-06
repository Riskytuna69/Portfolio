/******************************************************************************/
/*!
\file   MenuSoundManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	MenuSoundManager is an ECS component-system pair which handles audio playing
	within the menu scene.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "MenuSoundManager.h"

MenuSoundManagerComponent::MenuSoundManagerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	backgroundMusic	{ "MenuMusic" },
	inited			{ false }
{
}

void MenuSoundManagerComponent::Init()
{
	inited = true;

	// If menu music is not playing, play it
	if(!ST<AudioManager>::Get()->IsSoundPlaying(backgroundMusic))
	{
		ST<AudioManager>::Get()->StopAllSounds();
		ST<AudioManager>::Get()->SetChannelGroup(backgroundMusic, "BGM");
		ST<AudioManager>::Get()->StartSingleSound(backgroundMusic, true);
	}
}

#ifdef IMGUI_ENABLED
void MenuSoundManagerComponent::EditorDraw(MenuSoundManagerComponent& comp)
{
	comp.backgroundMusic.EditorDraw("Background Music");
}
#endif

MenuSoundManagerSystem::MenuSoundManagerSystem()
	: System_Internal{ &MenuSoundManagerSystem::UpdateMenuSoundManager }
{
}

void MenuSoundManagerSystem::UpdateMenuSoundManager(MenuSoundManagerComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}
}
