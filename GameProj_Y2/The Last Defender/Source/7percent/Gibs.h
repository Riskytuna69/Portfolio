/******************************************************************************/
/*!
\file   Gibs.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/25/2025

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is an interface file for a component that spawns a prefab of gibs.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "IGameComponentCallbacks.h"

/*****************************************************************//*!
\class GibsComponent
\brief
	Spawns a prefab of gibs when the attached entity runs out of health.
*//******************************************************************/
class GibsComponent
	: public IRegisteredComponent<GibsComponent>
#ifdef IMGUI_ENABLED
	, public IEditorComponent<GibsComponent>
#endif
	, public IGameComponentCallbacks<GibsComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Constructor.
	*//******************************************************************/
	GibsComponent();

	/*****************************************************************//*!
	\brief
		Subscribes to "Died" event.
	*//******************************************************************/
	void OnStart() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes from "Died" event.
	*//******************************************************************/
	void OnDetached() override;

private:
	/*****************************************************************//*!
	\brief
		Spawns gibs.
	*//******************************************************************/
	void OnDead();

	/*****************************************************************//*!
	\brief
		Draws this component to the editor.
	*//******************************************************************/
	static void EditorDraw(GibsComponent& comp);

private:
	//! The name of the prefab to spawn.
	std::string prefabName;

public:
	property_vtable()
};
property_begin(GibsComponent)
{
	property_var(prefabName)
}
property_vend_h(GibsComponent)

