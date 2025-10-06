/******************************************************************************/
/*!
\file   MenuSoundManager.h
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

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "AudioManager.h"

/*****************************************************************//*!
\class MenuSoundManagerComponent
\brief
	ECS component.
*//******************************************************************/
class MenuSoundManagerComponent : public IRegisteredComponent<MenuSoundManagerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<MenuSoundManagerComponent>
#endif
{
public:
	AudioReference backgroundMusic;
	bool inited;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	MenuSoundManagerComponent();

	/*****************************************************************//*!
	\brief
		Starts background music, sfx, etc.
	*//******************************************************************/
	void Init();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(MenuSoundManagerComponent& comp);
#endif
	property_vtable()
};
property_begin(MenuSoundManagerComponent)
{
	property_var(backgroundMusic)
}
property_vend_h(MenuSoundManagerComponent)

/*****************************************************************//*!
\class MenuSoundManagerSystem
\brief
	ECS system.
*//******************************************************************/
class MenuSoundManagerSystem : public ecs::System<MenuSoundManagerSystem, MenuSoundManagerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	MenuSoundManagerSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates MenuSoundManagerComponent.
	\param comp
		The MenuSoundManagerComponent to update.
	*//******************************************************************/
	void UpdateMenuSoundManager(MenuSoundManagerComponent& comp);
};