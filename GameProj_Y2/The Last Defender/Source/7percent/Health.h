/******************************************************************************/
/*!
\file   Health.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   27/11/2024

\author Matthew Chan Shao Jie (50%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\author Chan Kuan Fu Ryan (50%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	Health component which stores a health type and broadcasts a message that it
	has died if the health goes below 0.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "RenderSystem.h"
#include "EntityUID.h"

/*****************************************************************//*!
\class HealthComponent
\brief
	To attach to entities which need to keep track of health.
*//******************************************************************/
class HealthComponent : public IRegisteredComponent<HealthComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<HealthComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	HealthComponent();

	/*****************************************************************//*!
	\brief
		Gets the current health.
	\return
		The current health.
	*//******************************************************************/
	int GetCurrHealth() const;

	/*****************************************************************//*!
	\brief
		Gets whether this entity is dead based on their current health.
	\return
		True if the entity is dead. False otherwise.
	*//******************************************************************/
	bool IsDead() const;

	/*****************************************************************//*!
	\brief
		Gets the max health.
	\return
		The max health.
	*//******************************************************************/
	int GetMaxHealth() const;

	/*****************************************************************//*!
	\brief
		Gets the health type based on the entity this comp is attached to.
	*//******************************************************************/
	const std::string GetHealthType();

	/*****************************************************************//*!
	\brief
		Adds a certain amount of health.
	\param amount
		The amount of health to add.
	*//******************************************************************/
	void AddHealth(int amount);

	/*****************************************************************//*!
	\brief
		Subtracts a certain amount of health.
	\param amount
		The amount of damage to deal.
	*//******************************************************************/
	void TakeDamage(int amount);

	/*****************************************************************//*!
	\brief
		Sets the current health. Cannot exceed max health.
	\param newAmount
		The amount of health to set to.
	*//******************************************************************/
	void SetHealth(int newAmount);

	/*****************************************************************//*!
	\brief
		Sets a new max health.
	\param newMaxAmount
		The new max health.
	*//******************************************************************/
	void SetMaxHealth(int newMaxAmount);

	/*****************************************************************//*!
	\brief
		Sets this health component to be invulnerable.
	\param length
		The amount of time that this health component cannot take damage.
	*//******************************************************************/
	void SetInvulnerable(float length);

	/*****************************************************************//*!
	\brief
		Updates the invulnerability timer.
	\param dt
		Delta time.
	*//******************************************************************/
	void UpdateInvulnerable(float dt);

	/*****************************************************************//*!
	\brief
		Sets the health type based on the entity this comp is attached to.
	\param hpType
		Type to set.
	*//******************************************************************/
	void SetHealthType(const std::string& hpType);

	/*****************************************************************//*!
	\brief
		Updates the attached secondary BarComponent (if it exists) to
		catch up to the primary health bar.
	\param dt
		Delta time.
	*//******************************************************************/
	void UpdateSecondaryHealthBar(float dt);

	/*****************************************************************//*!
	\brief
		Updates the attached secondary BarComponent (if it exists) to
		catch up to the primary health bar.
	\param dt
		Delta time.
	*//******************************************************************/
	void UpdateIsDeadBroadcasted(bool broadcasted);

	/*****************************************************************//*!
	\brief
		Initialise the component.
	*//******************************************************************/
	void Init();

	void SetPrimaryBar(EntityReference handle);
	void SetSecondaryBar(EntityReference handle);

	float GetHealthFraction();

	bool inited;
private:
	/*****************************************************************//*!
	\brief
		Draws this component to the inspector window.
	\param comp
		The health component to draw.
	*//******************************************************************/
#ifdef IMGUI_ENABLED
	static void EditorDraw(HealthComponent& comp);
#endif

private:
	/*****************************************************************//*!
	\brief
		Updates attached BarComponent (if it exists) and modifies the
		color if the health goes below a certain threshold.
	*//******************************************************************/
	void UpdateHealthBar();

	/*****************************************************************//*!
	\brief
		Gets the percentage width of a bar that should be displayed for a certain
		health value against a certain max health value. Use this to lie about
		the true amount of health that the player still has.
	\param healthVar
		The health value to be checked.
	\param maxHealth
		The max health.
	\return
		The percentage width of the bar.
	*//******************************************************************/
	float GetBarLength(int healthVal, int maximumHealth);

	// To reference the primary health bar component
	EntityReference barEntity;
	// To reference the secondary health bar component
	EntityReference secondaryBarEntity;
	// To reference the camera that should shake when this health component takes damage.
	EntityReference camEntity;

	bool isDeadBroadcasted;
	int maxHealth;
	int currHealth;
	//! Tracks for how long this health component cannot take damage.
	float invulnerableTime;
	std::string healthType;
	//! The amount of time until the secondary health bar starts moving to catch up to the primary health bar.
	float secondaryHealthBarDelay;
	//! Whether to display bar health linearly or logarithmically
	bool barShowLogarithmic;

	static constexpr int defaultMax{ 100 };
	static constexpr std::array<const char*, 10> types{ 
		"Player", 
		"EnemyPatrol", 
		"EnemyWave", 
		"Objective", // No longer used, but deleting will mess with the array order! DO NOT TOUCH
		"Shield", 
		"BossLeftGun", 
		"BossRightGun", 
		"BossLaserPoint",
		"BossMinion",
		"Boss"
	};
	int selectedIndex;
	property_vtable()
};
property_begin(HealthComponent)
{
	property_var(barEntity),
	property_var(secondaryBarEntity),
	property_var(camEntity),
	property_var(maxHealth),
	property_var(currHealth),
	property_var(healthType),
	property_var(barShowLogarithmic),
	property_var(selectedIndex)
}
property_vend_h(HealthComponent)

/*****************************************************************//*!
\class HealthRegenComponent
\brief
	Regenerates the health component after some time.
*//******************************************************************/
class HealthRegenComponent
	: public IRegisteredComponent<HealthRegenComponent>
	, public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, public IEditorComponent<HealthRegenComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Constructor.
	*//******************************************************************/
	HealthRegenComponent();

	/*****************************************************************//*!
	\brief
		Subscribes to "Damaged" event from health component.
	*//******************************************************************/
	void OnAttached() override;
	/*****************************************************************//*!
	\brief
		Unsubscribes to "Damaged" event from health component.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Updates internal variables to calculate the amount of health recovered
		this frame.
	\param dt
		Delta time
	\return
		The amount of health recovered this frame.
	*//******************************************************************/
	int UpdateRecharge(float dt);

	/*****************************************************************//*!
	\brief
		Getter function.
	\return
		The amount of health recovered per second.
	*//******************************************************************/
	int GetHealAmtPerSec();

	/*****************************************************************//*!
	\brief
		Setter function.
	\param amt
		The amount of health recovered per second.
	*//******************************************************************/
	void SetHealAmtPerSec(int amt);

private:
	/*****************************************************************//*!
	\brief
		Resets the recharge delay when taking damage.
	*//******************************************************************/
	void OnDamaged();

	static void EditorDraw(HealthRegenComponent& comp);

private:
	//! The amount of health healed per second.
	int healAmtPerSec;
	//! The delay after taking damage before starting to heal.
	float rechargeDelay;
	//! Internal variable to track heal delay
	float healDelay;
	//! Internal variable counting the amount of health healed per frame.
	float healAccumulation;

public:
	property_vtable()
};
property_begin(HealthRegenComponent)
{
	property_var(healAmtPerSec),
	property_var(rechargeDelay)
}
property_vend_h(HealthRegenComponent)

class HealthVignetteComponent
	: public IRegisteredComponent<HealthVignetteComponent>
#ifdef IMGUI_ENABLED
	, public IEditorComponent<HealthVignetteComponent>
#endif
	, public ecs::IComponentCallbacks
{
public:
	HealthVignetteComponent();

	void OnAttached() override;
	void OnDetached() override;

	void Init();
	void UpdateIntensity(float dt);
	float GetIntensity() const;
	float GetMinVignetteRadius() const;
	float GetMaxVignetteRadius() const;

private:
	void OnDamaged();

public:
	bool inited;
	float initialVignetteRadius;

	float intensity;

	float maxVignetteRadius;
	float decaySpeed;
	float lowHealthPercentThreshold;
	float lowHealthIntensityModifier;

#ifdef IMGUI_ENABLED
public:
	static void EditorDraw(HealthVignetteComponent& comp);
#endif

public:
	property_vtable()
};
property_begin(HealthVignetteComponent)
{
	property_var(maxVignetteRadius),
	property_var(decaySpeed),
	property_var(lowHealthPercentThreshold),
	property_var(lowHealthIntensityModifier)
}
property_vend_h(HealthVignetteComponent)

/*****************************************************************//*!
\class HealthSystem
\brief
	System to operate on HealthComponent.
*//******************************************************************/
class HealthSystem : public ecs::System<HealthSystem, HealthComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	HealthSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a PlayerHealthSystem.
	\param comp
		The PlayerHealthSystem to update.
	*//******************************************************************/
	void UpdateHealthComp(HealthComponent& comp);
};

/*****************************************************************//*!
\class HealthRegenSystem
\brief
	Regenerates the health component after some time.
*//******************************************************************/
class HealthRegenSystem : public ecs::System<HealthRegenComponent, HealthComponent, HealthRegenComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Constructor.
	*//******************************************************************/
	HealthRegenSystem();

private:
	/*****************************************************************//*!
	\brief
		Regenerates the health component after some time.
	*//******************************************************************/
	void UpdateComp(HealthComponent& healthComp, HealthRegenComponent& regenComp);
};

class HealthVignetteSystem : public ecs::System<HealthVignetteSystem, HealthVignetteComponent>
{
public:
	HealthVignetteSystem();

private:
	void UpdateComp(HealthVignetteComponent& comp);

};
