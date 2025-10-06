/******************************************************************************/
/*!
\file   EasterEgg.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	SnoipahSpawner is an ECS component inheriting from Highlightable
	class, identifying a scene entity as a persistent interactable object, such
	as a lever or button.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Highlightable.h"
#include "AudioManager.h"


/*****************************************************************//*!
\class SnoipahSpawner
\brief
	ECS component.
*//******************************************************************/
class SnoipahSpawner
	: public IRegisteredComponent<SnoipahSpawner>
	, public Highlightable
{
public:
	std::string key;
	int keyIndex;
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	SnoipahSpawner();

	/*****************************************************************//*!
	\brief
		Override of Highlightable::OnUseHold(), which is called while
		the given key is pressed.
	\param use
		The key that is pressed.
	*//******************************************************************/
	void OnUseHold(KEY use) override;

	void SetIsHighlighted(bool highlighted);
private:
	property_vtable()
};
property_begin(SnoipahSpawner)
{
}
property_vend_h(SnoipahSpawner)
