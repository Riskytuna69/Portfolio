/******************************************************************************/
/*!
\file   Intel.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (10%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\author Matthew Chan Shao Jie (90%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Collectible item. Each represents a short story relating to the game world.
  Identified through integral value, id.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Highlightable.h"
#include "AudioManager.h"

/*****************************************************************//*!
\class IntelComponent
\brief
	The primary class for intel collectible.
*//******************************************************************/
class IntelComponent
	: public IRegisteredComponent<IntelComponent>
	, public Highlightable
#ifdef IMGUI_ENABLED
	, public IEditorComponent<IntelComponent>
#endif
{
public:
	int id;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	IntelComponent();

	/*****************************************************************//*!
	\brief
		Override of Highlightable::OnUseStart(), which is called once
		when the given key is pressed.
	\param use
		The key that is pressed.
	*//******************************************************************/
	void OnUseStart(KEY use) override;

	/*****************************************************************//*!
	\brief
		Sets the highlighted status of the object.
	\param highlighted
		True for highlighted, false for not.
	*//******************************************************************/
	void SetIsHighlighted(bool highlighted);
private:
	AudioReference audioUse;
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(IntelComponent& comp);
#endif
	property_vtable()
};
property_begin(IntelComponent)
{
	property_var(id),
}
property_vend_h(IntelComponent)