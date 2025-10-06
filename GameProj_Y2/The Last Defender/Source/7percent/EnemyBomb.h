/******************************************************************************/
/*!
\file   EnemyBomb.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   05/02/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
	Header for the Bomb that will be used by enemy type - SUICIDE

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "Collision.h"
#include "RenderComponent.h"
#include "Physics.h"
#include "EntityUID.h"
#include "EnemyStateMachine.h"
#include "Player.h"
#include "BombRange.h"



class EnemyBombComponent : public IRegisteredComponent<EnemyBombComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent <EnemyBombComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Constructor
	*//******************************************************************/
	EnemyBombComponent();

	/*****************************************************************//*!
	\brief
		Returns true when countdown reaches 0
	*//******************************************************************/
	bool BombDenotated();

	/*****************************************************************//*!
	\brief
		Count down the bomb timer with dt
	*//******************************************************************/
	void BombCountdown();

	/*****************************************************************//*!
	\brief
		Directly modifies bomb CD
	*//******************************************************************/
	void SetBombCountdown(float val);

	/*****************************************************************//*!
	\brief
		Primes the bomb to be ready to explode
	*//******************************************************************/
	void SetBombPrimed(bool val);

	void SetPrimedSpriteID(size_t val);
	void SetUnPrimedSpriteID(size_t val);

	size_t GetPrimedSpriteID();
	size_t GetUnPrimedSpriteID();
	


	/*****************************************************************//*!
	\brief
		Returns the prime status of the bomb
	*//******************************************************************/
	bool GetBombPrimed();

	float GetMaxRange();

	EntityReference& AttachedRange();

	bool spawnedExplosion;
	EntityReference bombRange;

private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(EnemyBombComponent& comp);
#endif

	
	float bombTimer;
	bool bombPrimed;
	size_t primedSpriteID;
	size_t unprimedSpriteID;

	property_vtable()
};
property_begin(EnemyBombComponent)
{
	property_var(bombTimer),
	property_var(bombRange),
	property_var(primedSpriteID),
	property_var(unprimedSpriteID)
}
property_vend_h(EnemyBombComponent)


class EnemyBombSystem : public ecs::System<EnemyBombSystem, EnemyBombComponent>
{
public:
	EnemyBombSystem();

private:
	void UpdateEnemyBomb(EnemyBombComponent& comp);

};

