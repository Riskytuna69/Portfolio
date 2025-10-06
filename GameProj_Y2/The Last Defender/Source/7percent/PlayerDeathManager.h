/******************************************************************************/
/*!
\file   PlayerDeathManager.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/04/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Interface for Player Death Manager component. Handles logic related to Player
  dying.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Player.h"

class PlayerDeathManagerComponent : public IRegisteredComponent<PlayerDeathManagerComponent>, ecs::IComponentCallbacks
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	PlayerDeathManagerComponent();
	/*****************************************************************//*!
	\brief
		Registers to the collision callback on this entity.
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Unregisters from the collision callback on this entity.
	*//******************************************************************/
	void OnDetached() override;
private:
	/*****************************************************************//*!
	\brief
		Called when the player dies.
	*//******************************************************************/
	static void OnPlayerDied();
	/*****************************************************************//*!
	\brief
		Called when the player respawns.
	*//******************************************************************/
	static void OnPlayeRespawned();

	static EntityReference playerReference;
	property_vtable()

};
property_begin(PlayerDeathManagerComponent)
{
}
property_vend_h(PlayerDeathManagerComponent)
