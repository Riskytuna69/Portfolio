/******************************************************************************/
/*!
\file   Bullet.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the declaration of Entity Component BulletComponent, and
	Entity System BulletSystem.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "AudioManager.h"

// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

/*****************************************************************//*!
\class BulletComponent
\brief
	Identifies an entity as a Bullet.
*//******************************************************************/
class BulletComponent : public IRegisteredComponent<BulletComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	,IEditorComponent<BulletComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	BulletComponent();

	/*****************************************************************//*!
	\brief
		Sets the direction of the Bullet's travel.
	\param const Vector2&
		The Vector2 direction to convert into the Bullet's rotation.
	*//******************************************************************/
	void SetDirection(const Vector2&);
	/*****************************************************************//*!
	\brief
		Lowers the Bullet's remaining lifetime.
	\param float
		The amount to lower lifeTime by.
	*//******************************************************************/
	void LowerLifeTime(float);
	/*****************************************************************//*!
	\brief
		Gets bullet direction.
	\return
		direction.
	*//******************************************************************/
	const Vector2& GetDirection() const;
	/*****************************************************************//*!
	\brief
		Gets bullet speed.
	\return
		speed.
	*//******************************************************************/
	float GetSpeed() const;
	/*****************************************************************//*!
	\brief
		Gets bullet speed.
	\return
		speed.
	*//******************************************************************/
	void SetSpeed(float _speed);
	/*****************************************************************//*!
	\brief
		Gets bullet lifetime.
	\return
		lifeTime.
	*//******************************************************************/
	void SetLifeTime(float _lifeTime);
	/*****************************************************************//*!
	\brief
		Gets bullet lifetime.
	\return
		lifeTime.
	*//******************************************************************/
	float GetLifeTime() const;

	/*****************************************************************//*!
	\brief
		Sets all relevant members of the bullet.
	\param origin
		The point for the bullet to fire from.
	\param direction
		The direction for the bullet to travel.
	\param damage
		The damage for the bullet to deal.
	\param speed
		The speed for the bullet to move.
	\param lifeTime
		The amount for the bullet to exist before despawning.
	*//******************************************************************/
	void Fire(Vector2 origin, Vector2 direction, int damage, float speed, float lifeTime, Vector2 parentVelocity=Vector2{ 0.0f });

private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(BulletComponent& comp);
#endif

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

	/*****************************************************************//*!
	\brief
		Sets the damage of the bullet.
	\param
		The damage to set the bullet to have.
	*//******************************************************************/
	void SetDamage(int amount);

	/*****************************************************************//*!
	\brief
		Gets the damage of the bullet.
	\return
		The damage of the bullet.
	*//******************************************************************/
	int GetDamage();

	/*****************************************************************//*!
	\brief
		Gets the gravity of the bullet.
	\return
		The gravity of the bullet.
	*//******************************************************************/
	bool GetGravity();

	/*****************************************************************//*!
	\brief
		Sets the gravity of the bullet.
	*//******************************************************************/
	void SetGravity(bool _gravity);

	/*****************************************************************//*!
	\brief
		Gets the gravity of the bullet.
	\return
		The gravity of the bullet.
	*//******************************************************************/
	bool GetExplosive();

	/*****************************************************************//*!
	\brief
		Sets the gravity of the bullet.
	*//******************************************************************/
	void SetExplosive(bool _explosive);

private:
	/*****************************************************************//*!
	\brief
		Sets the entity for deletion.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

private:

	Vector2 direction;
	int damage;
	float speed;
	float lifeTime;
	bool isPlayerBullet;
	bool gravity;
	bool explosive;

	AudioReference audioImpact;
	AudioReference audioImpactBoss;
	AudioReference audioImpactEnvironment;

	property_vtable()
};
property_begin(BulletComponent)
{
	property_var(direction),
	property_var(speed),
	property_var(lifeTime),
	property_var(isPlayerBullet),
	property_var(audioImpact),
	property_var(audioImpactBoss),
	property_var(audioImpactEnvironment),
}
property_vend_h(BulletComponent)

/*****************************************************************//*!
\class LaserComponent
\brief
	Identifies an entity as a Laser.
*//******************************************************************/
class LaserComponent : public IRegisteredComponent<LaserComponent>
#ifdef IMGUI_ENABLED
	,IEditorComponent<LaserComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	LaserComponent();

	/*****************************************************************//*!
	\brief
		Sets the Laser's piercing flag.
	\param const bool&
		Whether the laser pierces targets.
	*//******************************************************************/
	void SetPiercing(const bool&);
	/*****************************************************************//*!
	\brief
		Lowers the Bullet's remaining lifetime.
	\param float
		The amount to lower lifeTime by.
	*//******************************************************************/
	void LowerLifeTime(float);
	/*****************************************************************//*!
	\brief
		Gets laser lifetime.
	\return
		lifeTime.
	*//******************************************************************/
	float GetLifeTime() const;
	/*****************************************************************//*!
	\brief
		Gets laser max lifetime.
	\return
		lifeTime.
	*//******************************************************************/
	float GetMaxLifeTime() const;

	/*****************************************************************//*!
	\brief
		Fires the laser, performing all rebound calculations.
	\param float distance
		The remaining distance for the laser to travel.
	\param Vector2 origin
		The point the laser originates from.
	\param Vector2 direction
		The direction the Laser travels.
	\param float life
		The time the laser is rendered on screen after firing.
	\param int damage
		The damage the laser deals.
	\param bool isPlayerLaser
		Whether the laser was fired by the Player.
	\param int bounceCount
		Remaining bounces (prevents freezing by ending near-infinite reflections)
	*//******************************************************************/
	void Fire(float distance, Vector2 origin, Vector2 direction, float life, int damage, bool isPlayerLaser,int bounceCount = maxBounceCount);
private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(LaserComponent& comp);
#endif
private:
	const static float maxDistancePerSegment;
	const static int maxBounceCount;// This is here to make sure the laser doesn't travel to infinity lol

	float lifeTime;
	float maxLifeTime;
	bool isPlayerBullet;
	bool isPiercing;

	property_vtable()
};
property_begin(LaserComponent)
{
	property_var(lifeTime),
	property_var(maxLifeTime),
	property_var(isPlayerBullet),
}
property_vend_h(LaserComponent)

class BulletMovementSystem : public ecs::System<BulletMovementSystem, BulletComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	BulletMovementSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a BulletComponent.
	\param comp
		The BulletComponent to update.
	*//******************************************************************/
	void UpdateBulletComp(BulletComponent& comp);
	/*****************************************************************//*!
	\brief
		Post-update for BulletComponents.
	*//******************************************************************/
	void PostRun() override;
};

class LaserRendererSystem : public ecs::System<LaserRendererSystem, LaserComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	LaserRendererSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a BulletComponent.
	\param comp
		The BulletComponent to update.
	*//******************************************************************/
	void UpdateLaserComp(LaserComponent& comp);
};


class BossLaserComponent : public IRegisteredComponent<BossLaserComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<BossLaserComponent>
#endif
{
private:
	float maxLifeTime;
	float lifeTime;
	int damage;
	bool hitPlayer;
	bool trashLaser;

#ifdef IMGUI_ENABLED
	static void EditorDraw(BossLaserComponent& comp);
#endif

public:
	BossLaserComponent();

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

	void UpdateLifeTime();
	float GetMaxLifeTime() const;
	float GetLifeTime() const;

	void SetDamage(int value);

	int GetDamage() const;

	void SetLaserLength(float value);

	bool GetTrashLaser() const;

	property_vtable()

};
property_begin(BossLaserComponent)
{
	property_var(lifeTime),
	property_var(damage),
}
property_vend_h(BossLaserComponent)

class BossLaserRendererSystem : public ecs::System<BossLaserRendererSystem, BossLaserComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	BossLaserRendererSystem();



private:
	/*****************************************************************//*!
	\brief
		Updates a BulletComponent.
	\param comp
		The BulletComponent to update.
	*//******************************************************************/
	void UpdateLaserComp(BossLaserComponent& comp);
};