/******************************************************************************/
/*!
\file   SceneSoundManager.h
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

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"
#include "AudioManager.h"

/*****************************************************************//*!
\class SceneSoundManagerComponent
\brief
	ECS component.
*//******************************************************************/
class SceneSoundManagerComponent : public IRegisteredComponent<SceneSoundManagerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<SceneSoundManagerComponent>
#endif
{
public:
	EntityReference playerEntity;
	AudioReference backgroundMusic;
	AudioReference overgroundSound;
	AudioReference undergroundSound;
	float boundaryX;
	float boundaryWidth;
	bool inited;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	SceneSoundManagerComponent();

	/*****************************************************************//*!
	\brief
		Starts background music, sfx, etc.
	*//******************************************************************/
	void Init();

	/*****************************************************************//*!
	\brief
		Update to do transition between underground and overground regions.
	*//******************************************************************/
	void Update();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(SceneSoundManagerComponent& comp);
#endif
	property_vtable()
};
property_begin(SceneSoundManagerComponent)
{
	property_var(playerEntity),
	property_var(backgroundMusic),
	property_var(overgroundSound),
	property_var(undergroundSound),
	property_var(boundaryX),
	property_var(boundaryWidth),
}
property_vend_h(SceneSoundManagerComponent)

/*****************************************************************//*!
\class SceneSoundManagerSystem
\brief
	ECS system.
*//******************************************************************/
class SceneSoundManagerSystem : public ecs::System<SceneSoundManagerSystem, SceneSoundManagerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	SceneSoundManagerSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates SceneSoundManagerComponent.
	\param comp
		The SceneSoundManagerComponent to update.
	*//******************************************************************/
	void UpdateSceneSoundManager(SceneSoundManagerComponent& comp);
};