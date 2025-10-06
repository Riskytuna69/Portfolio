/******************************************************************************/
/*!
\file   Highlightable.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the declaration of base class Highlightable.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"

/*****************************************************************//*!
\class Highlightable
\brief
	Base class for highlightable objects.
*//******************************************************************/
class Highlightable
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	Highlightable();

	/*****************************************************************//*!
	\brief
		Overloaded constructor with use keys.
	*//******************************************************************/
	Highlightable(std::string, float, std::vector<KEY>);

	/*****************************************************************//*!
	\brief
		Sets the highlighted status of the object.
	\param highlighted
		Whether the object is highlighted.
	\param thisEntity
		The self-reference of the entity.
	*//******************************************************************/
	virtual void SetIsHighlighted(bool highlighted, EntityReference thisEntity);

	/*****************************************************************//*!
	\brief
		Sets the highlighted status of the object.
	\param highlighted
		Whether the object is highlighted.
	\param thisEntity
		The self-reference of the entity.
	*//******************************************************************/
	virtual void SetIsHighlighted(bool highlighted) = 0;

	// For Healthpack and Weapons (already held)
	/*****************************************************************//*!
	\brief
		On Use Start is called when the given key is pressed.
		(keys are in useKeys)
	\param use
		The key.
	*//******************************************************************/
	virtual void OnUseStart(KEY use);
	/*****************************************************************//*!
	\brief
		On Use Hold is called while the given key is pressed.
		(keys are in useKeys)
	\param use
		The key.
	*//******************************************************************/
	virtual void OnUseHold(KEY use);
	/*****************************************************************//*!
	\brief
		On Use End is called when the given key is released.
		(keys are in useKeys)
	\param use
		The key.
	*//******************************************************************/
	virtual void OnUseEnd(KEY use);

	// The name of the item.
	std::string name;
	// The keys that can be used to interact with this object.
	std::vector<KEY> useKeys;
	// Self entity reference to avoid having to perform GetEntity(this).
	EntityReference thisEntity;
	// Text height offset to prevent clipping
	float textHeightOffset;
	// Whether the object is interacable.
	bool interactable;
};
