/******************************************************************************/
/*!
\file   Shield.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	Shield is an ECS component-system pair which contains functionality to
	deflect enemy projectiles within the game scene.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"
#include "AudioManager.h"

/*****************************************************************//*!
\class ShieldComponent
\brief
	ECS component. Requires HealthComponent on same entity to work!!!
*//******************************************************************/
class ShieldComponent : public IRegisteredComponent<ShieldComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<ShieldComponent>
#endif
{
public:
	EntityReference armPivotEntity;		// Reference to the arm pivot entity
	EntityReference renderEntity;		// Entity that renders the shield - must be a child
	float startMin;						// Distance at which the shield begins - the minimum
	float endMax;						// Distance at which the shield ends - the maximum
	float angleMax;						// Any bullets going above this angle (in degrees) are ignored
	float depletionRate;				// How much shield is lost per second when activated?
	float regenRateMultiplier;			// Multiplier for regeneration rate when dealing damage
	float regenRateEnhancedTime;				// Time that regen rate is enhanced in seconds
	float accumulator;					// Workaround for integer health values (not to be serialised)

	AudioReference reflectSound;		// Sound to play when blocking a projectile
	AudioReference activatedSound;		// Sound to play when activating the shield successfully
	AudioReference deactivatedSound;	// Sound to play when shield goes down
	
	bool shieldActivated;				// Is the shield already turned on?
	bool tryingToTurnOn;				// Is the shield trying to turn on?
	int defaultRegenRate;				// Store the default regeneration rate when init (not serialised)
	bool inited;						// If it is initialised
	float regenEnhancedTimer;			// Timer to count regen rate enhanced time

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ShieldComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	*//******************************************************************/
	~ShieldComponent();

	/*****************************************************************//*!
	\brief
		Stores the default regen rate for the shield.
	*//******************************************************************/
	void Init();

private:

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(ShieldComponent& comp);
#endif
	property_vtable()
};
property_begin(ShieldComponent)
{
	property_var(armPivotEntity),
	property_var(renderEntity),
	property_var(startMin),
	property_var(endMax),
	property_var(angleMax),
	property_var(depletionRate),
	property_var(regenRateMultiplier),
	property_var(regenRateEnhancedTime),
	property_var(reflectSound),
	property_var(activatedSound),
	property_var(deactivatedSound)
}
property_vend_h(ShieldComponent)

/*****************************************************************//*!
\class DeferredPlayerBullet
\brief
	Holds player bullet spawning parameters
*//******************************************************************/
struct DeferredPlayerBullet
{
	Vector2 position;
	Vector2 direction;
	float speed;
	int damage;
};

/*****************************************************************//*!
\class ShieldSystem
\brief
	ECS system. Handles projectile deflection logic.
*//******************************************************************/
class ShieldSystem : public ecs::System<ShieldSystem, ShieldComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ShieldSystem();

	/*****************************************************************//*!
	\brief
		Subscribes to relevant events.
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes from relevant events.
	*//******************************************************************/
	void OnRemoved() override;

	/*****************************************************************//*!
	\brief
		Enhances the shield regeneration rate for a predefined time.
	\param value
		Unused parameter
	*//******************************************************************/
	static void EnhanceShieldRegen(int value);

private:
	// Deferred player bullet prefab spawning to work around a crash
	std::vector<DeferredPlayerBullet> deferredPlayerBullets;

	/*****************************************************************//*!
	\brief
		Updates ShieldComponent.
	\param comp
		The ShieldComponent to update.
	*//******************************************************************/
	void UpdateShieldSystem(ShieldComponent& comp);
};

/*****************************************************************//*!
\class ShieldInputSystem
\brief
	ECS system. Handles input detection. Exists on a separate Gamesystem layer.
*//******************************************************************/
class ShieldInputSystem : public ecs::System<ShieldInputSystem, ShieldComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ShieldInputSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates ShieldComponent.
	\param comp
		The ShieldComponent to update.
	*//******************************************************************/
	void UpdateShieldInputSystem(ShieldComponent& comp);
};
