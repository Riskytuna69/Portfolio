/******************************************************************************/
/*!
\file   CeilingTurret.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   5/4/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Component and system for ceiling turret prefabs that sweep in neutral state
  and track the player when colliding with the turret detection collider.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Collision.h"
#include "EntityUID.h"

// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

/*****************************************************************//*!
\enum CeilingTurretState
\brief
	Enumerator denoting the state of the turret.
*//******************************************************************/
enum CeilingTurretState
{
	NEUTRAL,
	TRACKING,
	INTER_NEUTRAL,
	INTER_TRACKING
};

/*****************************************************************//*!
\class CeilingTurretComponent
\brief
	ECS component.
*//******************************************************************/
class CeilingTurretComponent : public IRegisteredComponent<CeilingTurretComponent>, public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<CeilingTurretComponent>
#endif
{
public:
	EntityReference turretDetectionCollider;
	EntityReference turretPivot;
	EntityReference turretGunSprite;
	EntityReference turretGunFirepoint;
	EntityReference target;		// Not erialised, set during collision

	CeilingTurretState state;	// Not serialised, default-initialised to NEUTRAL

	int bulletDamage;			// Use your brain
	float bulletSpeed;			// Use your brain
	float timeUpdateInterval;	// Interval between tracking updates
	float timeBulletInterval;	// Interval between bullets
	float timeIntermission;		// Time between state changes
	float sweepAngle;			// Amplitude of the sine wave
	float sweepAngleOffset;		// Adds this offset just before setting pivot rotation
	float sweepSpeed;			// 6.28 == 1 oscillation/second
	float predictionMultiplier;	// How far ahead should the turret predict player movement
	bool inited;				// If this component has been initialised

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	CeilingTurretComponent();

	/*****************************************************************//*!
	\brief
		Unregisters from the collision callback on this entity.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Track player.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

	/*****************************************************************//*!
	\brief
		Called per-frame by the CeilingTurretSystem
	*//******************************************************************/
	void UpdateCeilingTurret();

	/*****************************************************************//*!
	\brief
		Initialises the component.
	*//******************************************************************/
	void Init();

private:
	float updateTimer;
	float gunfireTimer;
	float intermissionTimer;
	float sineWaveProgression;

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(CeilingTurretComponent& comp);
#endif
	property_vtable()
};
property_begin(CeilingTurretComponent)
{
	/*property_var(turretDetectionCollider),
	property_var(turretPivot),
	property_var(turretGunSprite),
	property_var(turretGunFirepoint),*/
	property_var(bulletDamage),
	property_var(bulletSpeed),
	property_var(timeUpdateInterval),
	property_var(timeBulletInterval),
	property_var(timeIntermission),
	property_var(sweepAngle),
	property_var(sweepAngleOffset),
	property_var(sweepSpeed),
	property_var(predictionMultiplier),
}
property_vend_h(CeilingTurretComponent)

class CeilingTurretSystem : public ecs::System<CeilingTurretSystem, CeilingTurretComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	CeilingTurretSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates CeilingTurretComponent.
	\param comp
		The CeilingTurretComponent to update.
	*//******************************************************************/
	void UpdateCeilingTurretComp(CeilingTurretComponent& comp);
};