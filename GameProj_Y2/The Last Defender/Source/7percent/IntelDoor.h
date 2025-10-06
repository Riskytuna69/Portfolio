/******************************************************************************/
/*!
\file   IntelDoor.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/04/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Component for handling the intel-locked door.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"

class IntelDoorComponent : public IRegisteredComponent<IntelDoorComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<IntelDoorComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	IntelDoorComponent();
private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(IntelDoorComponent& comp);
#endif
public:
	/*****************************************************************//*!
	\brief
		Sets the unlocked status of each child indicator.
	\param index
		The index of the collected intel.
	\param unlocked
		Whether the intel has been unlocked. Default is true.
	*//******************************************************************/
	void SetUnlocked(int index, bool unlocked = true);
	/*****************************************************************//*!
	\brief
		Init function. Establishes references to all child entities.
	*//******************************************************************/
	void Init();
	EntityReference targetDoor;
	property_vtable()
};
property_begin(IntelDoorComponent)
{
	property_var(targetDoor),
}
property_vend_h(IntelDoorComponent)
