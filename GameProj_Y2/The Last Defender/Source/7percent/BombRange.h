#pragma once
#include "EnemyBomb.h"

/******************************************************************************/
/*!
\file   BombRange.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   13/03/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
	Header for the bomb explosions that will be used by suicide enemies and certain weapons.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/


// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

class BombRangeComponent : public IRegisteredComponent<BombRangeComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent <BombRangeComponent>
#endif
{
public:
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

	size_t GetSpriteID();

	void SetSpriteID(size_t val);


	/*****************************************************************//*!
	\brief
		Call this function when the explosion goes off
	\param origin
		spawn loaction of the explosion
	\param startSize
		beginning size
	\param endSize
		final size
	\param damage
		maximum damage dealt
	\param knockback
		amount of knock back
	\param lifeTime
		how long the explosion should last for
	*//******************************************************************/
	void Fire(Vector2 origin, Vector2 startSize, Vector2 endSize, int damage, float knockback, float lifeTime);

	/*****************************************************************//*!
	\brief
		Constructor
	*//******************************************************************/
	BombRangeComponent();

	std::vector<ecs::EntityHandle> hitEntities;
private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(BombRangeComponent& comp);
#endif

	/*****************************************************************//*!
	\brief
		Sets the entity for deletion.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

	size_t spriteID;
	int bombDamage;
	float bombKnockback;
	float maxDistance;
	float minDistance;
	property_vtable()
};
property_begin(BombRangeComponent)
{
	property_var(spriteID)
}
property_vend_h(BombRangeComponent)


