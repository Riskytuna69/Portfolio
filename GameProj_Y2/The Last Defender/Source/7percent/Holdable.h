/******************************************************************************/
/*!
\file   Holdable.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the declaration of base class Holdable.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Highlightable.h"
#define AUTO_PICKUP_COOLDOWN 2.0f

// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

/*****************************************************************//*!
\class Holdable
\brief
	Identifies an entity as a Bullet.
*//******************************************************************/
class Holdable: public Highlightable
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	Holdable();
	/*****************************************************************//*!
	\brief
		Constructor with use keys.
	*//******************************************************************/
	Holdable(std::string,float, std::vector<KEY>,std::vector<KEY>);

	// Hold point pixel, used for offset.
	Vector2 holdPointPixel;
	// Whether this item is being held.
	bool isHeld;
	
	// Use Keys that are independent of the shield. 
	// (Personally feels better to do it here than through
	std::vector<KEY> independentUseKeys;

	void UpdatePickupCooldown(float);
	/*****************************************************************//*!
	\enum class HOLDABLE_TYPE
	\brief
		Identifies the kind of holdable.
	*//******************************************************************/
	enum class HOLDABLE_TYPE
	{
		NONE,
		WEAPON,
		HEALTHPACK //(UNSUPPORTED, we're removing it as of now) // Matthew here to perform a resurrection // Matthew here again to assassinate this feature
	};

	/*****************************************************************//*!
	\brief
		Gets a holdable from an entity.
	\param entity
		The entity to get a holdable from.
	\param outHoldable
		Value will be set to a pointer to the holdable component. If none exists, nullptr.
	\return
		What kind of holdable is it.
	*//******************************************************************/
	static HOLDABLE_TYPE GetHoldable(ecs::EntityHandle entity, Holdable** outHoldable);

	/*****************************************************************//*!
	\brief
		Called when the item is switched to or from.
	\param toThis
		Whether the item is switched to, or from.
	*//******************************************************************/
	virtual void OnSwitched(bool toThis);

	virtual void OnDropped();

	bool CanAutoPickup();
protected:
	float autoPickupCooldown;
	void Init();
};


