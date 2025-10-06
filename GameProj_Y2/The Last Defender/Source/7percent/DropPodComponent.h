/******************************************************************************/
/*!
\file   DropPodComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   02/13/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component of the drop pod that spawns the actual enemy


All content ? 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Collision.h"
#include "AudioManager.h"
class DropPodComponent : public IRegisteredComponent<DropPodComponent>, ecs::IComponentCallbacks
{
private:
	bool enemySpawned;
	float spawnTimer;
	
	AudioReference onCollisionSound;
public:

	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	DropPodComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~DropPodComponent();

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

	/*****************************************************************//*!
\brief
	Sets the entity for deletion.
\param collisionData
	The data of the collision.
*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

	property_vtable()
};
property_begin(DropPodComponent)
{
	property_var(onCollisionSound),
}
property_vend_h(DropPodComponent)
