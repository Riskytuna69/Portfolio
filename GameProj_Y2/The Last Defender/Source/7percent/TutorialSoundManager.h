/******************************************************************************/
/*!
\file   TutorialSoundManager.h
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
#pragma once
#include "AudioManager.h"

/*****************************************************************//*!
\class TutorialSoundManagerComponent
\brief
	The primary class for tutorial sound manager.
*//******************************************************************/
class TutorialSoundManagerComponent : public IRegisteredComponent<TutorialSoundManagerComponent>,
	public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<TutorialSoundManagerComponent>
#endif
{
public:
	AudioReference sound; // Ambient sound to play at the beginning of the tutorial level
	bool loop;

	bool inited;
	/*****************************************************************//*!
	\brief
		Called once at the beginning of the scene. Stops all sounds then
		starts the background ambient noise.
	*//******************************************************************/
	void Init();

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	TutorialSoundManagerComponent();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(TutorialSoundManagerComponent& comp);
#endif
	property_vtable()
};
property_begin(TutorialSoundManagerComponent)
{
	property_var(sound),
	property_var(loop)
}
property_vend_h(TutorialSoundManagerComponent)

/*****************************************************************//*!
\class TutorialSoundManagerSystem
\brief
	Corresponding system to update TutorialSoundManagerComponent.
*//******************************************************************/
class TutorialSoundManagerSystem : public ecs::System<TutorialSoundManagerSystem, TutorialSoundManagerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	TutorialSoundManagerSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates a TutorialSoundManagerComponent.
	\param comp
		The TutorialSoundManagerComponent to update.
	*//******************************************************************/
	void UpdateSimpleSoundPlayerComp(TutorialSoundManagerComponent& comp);
};