/******************************************************************************/
/*!
\file   CutsceneManager.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component that manages the cutscene


All content ? 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"

enum ANIM_SCENE
{
	FADEIN01,
	ANIM01,
	ANIM02, // Looping anim
	FADEOUT01,
	FADEIN02,
	ANIM03,
	ANIM04,	// Looping anim
	FADEOUT02,
	TRANSITION,
	FINISH
};

class CutsceneManagerComponent : public IRegisteredComponent<CutsceneManagerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<CutsceneManagerComponent>
#endif
{
private:
	float fadeTimer;
	float fadeTime;

	float loopingTimer;
	float loopingTime;

	bool anim02Done;
	bool transitionStarted;

	EntityReference anim01;
	EntityReference anim02;
	EntityReference anim03;
	EntityReference anim04;

	EntityReference fadeEntity;
	EntityReference skipCutsceneText;

	EntityReference currAnimPlaying;

	ANIM_SCENE currAnimScene;

	bool isInit;
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	CutsceneManagerComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~CutsceneManagerComponent();

	/*****************************************************************//*!
	\brief
		Function that inits component on start.
	\return
		None.
	*//******************************************************************/
	void Init();

	/*****************************************************************//*!
	\brief
		function that checks if the component has initialized
	\return
		isInit bool
	*//******************************************************************/
	bool GetIsInit() const;

	/*****************************************************************//*!
	\brief
		Updates the component
	\return
	*//******************************************************************/
	void Update();

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(CutsceneManagerComponent& comp);
#endif

	property_vtable()
};
property_begin(CutsceneManagerComponent)
{
	property_var(fadeTime),
	property_var(loopingTime),
	property_var(anim01),
	property_var(anim02),
	property_var(anim03),
	property_var(anim04),
	property_var(fadeEntity),
	property_var(skipCutsceneText),
}
property_vend_h(CutsceneManagerComponent)

class CutsceneSystem : public ecs::System<CutsceneSystem, CutsceneManagerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	CutsceneSystem();

private:

	/*****************************************************************//*!
	\brief
		Updates the system
	\param[in, out] comp
		Component to update
	\return
	*//******************************************************************/
	void Update(CutsceneManagerComponent& comp);
};
