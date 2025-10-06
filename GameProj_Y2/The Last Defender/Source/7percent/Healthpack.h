/******************************************************************************/
/*!
\file   Healthpack.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  This file contains the interface for the Health Pack Component.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Holdable.h"
#include "AudioManager.h"

class HealthpackComponent : public IRegisteredComponent<HealthpackComponent>, public Holdable
#ifdef IMGUI_ENABLED
	, IEditorComponent<HealthpackComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	HealthpackComponent();
	/*****************************************************************//*!
	\brief
		On Use Start override. This is called when the given key is pressed.
		(keys are in Highlightable::useKeys)
	\param use
		The key.
	*//******************************************************************/
	void OnUseStart(KEY use) override;
	/*****************************************************************//*!
	\brief
		Sets the highlighted status of the object.
	\param highlighted
		Whether the object is highlighted.
	*//******************************************************************/
	void SetIsHighlighted(bool highlighted);
	/*****************************************************************//*!
	\brief
		Consumes the health pack.
	*//******************************************************************/
	void Use();
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(HealthpackComponent& comp);
#endif
	//! The amount of health to heal from this pack
	int healthAmount;
	//! The sound played when using the health pack.
	AudioReference audioUse;

	property_vtable()
};
property_begin(HealthpackComponent)
{
	property_var(name),
	property_var(audioUse)
}
property_vend_h(HealthpackComponent)
