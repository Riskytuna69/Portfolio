/******************************************************************************/
/*!
\file   Weapon.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the delcaration of Entity Component WeaponComponent, and
	Entity System WeaponSystem.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Holdable.h"
#include "Bullet.h"
#include "AudioManager.h"

#define MAX_NAME_LENGTH 32

/*****************************************************************//*!
\struct WeaponStats
\brief
	Contains the weapon stats
*//******************************************************************/
struct WeaponStats : ISerializeable
{
	enum FIRE_TYPE : int	//Fire type, determines how the weapon behaves when fire button is held down
	{
		FT_SEMIAUTO,
		FT_AUTO,
		FT_BURST,
		FT_RAILGUN,

		FT_MELEE,//Melee changes a ton of behaviour. // 2025 Matthew here. We are *NOT* touching melee.
		FT_HEALTHPACK,//I bit the bullet and just made health packs a "weapon". fun
	};

	enum WEAPON_TYPE : int	//Weapon type, determines the muzzle flash and can be used to differentiate weapons
	{
		WT_PISTOL,
		WT_ASSAULT,
		WT_SHOTGUN,
		WT_MINIGUN,
		WT_SNIPER,
		WT_SMG,
		WT_RAILGUN_PIERCING,
		WT_RAILGUN,
		WT_GRENADE,
		WT_NOISYCRICKET,
	};

	Vector2 firePointPixel;
	Vector2 holdPointPixel;
	Vector2 muzzleFlashPosition;
	Vector2 shellEjectionPosition;

	std::string weaponName;

	float fireRPM;						//	Fire rate in Rounds/minute
	int fireType;						//	The fire type of the weapon			
	int weaponType;						//	The fire type of the weapon			

	int burstCount;						//	The number of bullets to fire when the weapon is fired. Weapon still relies on cycle time, so no double-stacked bullets.
	int pelletCount;					//	The number of bullets fired per shot from this weapon
	int damagePerBullet;				//	The amount of damage each bullet does

	int magazineSize;					//	The amount of rounds held in the weapon with each reload

	int spread;							//	The bullet spread for the weapon. Independent of Recoil, this always exists.

	float recoilPerShot;				//	How much recoil is gained per shot
	float recoilWanderMultiplier;		//	How many degrees of spread the recoil should add
	float recoilWanderMax;				//	How many degrees the recoil should reach at maximum
	float recoilRecovery;				//	How quickly the recoil should decay per second in %. 
	float recoilRecoveryMin;			//	How quickly the recoil should decay per second at minimum. This is used when recoilRecovery * recoilCurr is lower. 

	float reloadTime;					//	The time it takes to reload the weapon

	float bulletLifeTime;				//	Lifetime of each bullet
	float bulletSize;					//	Size of each bullet
	float bulletSpeed;					//	Speed the bullets travel at
	float bulletSpeedRange;				//	Random range of speed, useful for shotguns

	float screenShakePerSpreadDegree;	// Amount of screen shake per degree of spread. Makes screen shake harder when there is more spread.
	float screenShakePerShot;			// Amount of screen shake per shot. Static amount of shake added to spread-shake.
	float maximumScreenShake;			// Max force from screen shake

	// For enemies holding this weapon.
	int enemyDamage;					// The damage that enemies with this weapon deal per shot
	float enemyCycleTimeBase;			// The amount of time an enemy waits in between bursts.
	float enemyCycleTimeBurst;			// The amount of time in between each bullet within a burst.
	int enemyBurstBullets;				// The number of bullets per burst.

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	WeaponStats() :
		firePointPixel{ 0.0f,0.0f },
		holdPointPixel{ 0.0f,0.0f },
		muzzleFlashPosition{ 0.0f,0.0f },
		shellEjectionPosition{ 0.0f,0.0f },

		weaponName{ "" },

		fireRPM{ 0.0f },
		fireType{ 0 },
		weaponType{ WT_PISTOL },
		burstCount{ 0 },
		pelletCount{ 0 },
		damagePerBullet{ 0 },
		magazineSize{ 0 },
		spread{ 0 },

		recoilPerShot{ 0.0f },
		recoilWanderMultiplier{ 0.0f },
		recoilWanderMax{ 0.0f },
		recoilRecovery{ 0.0f },
		recoilRecoveryMin{ 0.0f },
		reloadTime{ 0.0f },
		bulletLifeTime{ 2.0f },
		bulletSize{ 0.0f },
		bulletSpeed{ 0.0f },
		bulletSpeedRange{ 0.0f },

		maximumScreenShake{ 0.0f },
		screenShakePerShot{ 0.0f },
		screenShakePerSpreadDegree{ 0.0f },

		enemyDamage{ 5 },
		enemyCycleTimeBase{ 1.0f },
		enemyCycleTimeBurst{ 0.2f },
		enemyBurstBullets{ 3 }
	{
	}
	property_vtable()
};
property_begin(WeaponStats)
{
	property_var(weaponName),
	property_var(fireRPM),
	property_var(fireType),
	property_var(weaponType),
	property_var(burstCount),
	property_var(pelletCount),
	property_var(damagePerBullet),
	property_var(magazineSize),
	property_var(spread),
	property_var(recoilPerShot),
	property_var(recoilWanderMultiplier),
	property_var(recoilWanderMax),
	property_var(recoilRecovery),
	property_var(recoilRecoveryMin),
	property_var(reloadTime),
	property_var(bulletLifeTime),
	property_var(bulletSize),
	property_var(bulletSpeed),
	property_var(bulletSpeedRange),
	property_var(firePointPixel),
	property_var(holdPointPixel),
	property_var(muzzleFlashPosition),
	property_var(shellEjectionPosition),
	property_var(screenShakePerSpreadDegree),
	property_var(screenShakePerShot),
	property_var(maximumScreenShake),
	property_var(enemyDamage),
	property_var(enemyCycleTimeBase),
	property_var(enemyCycleTimeBurst),
	property_var(enemyBurstBullets)
}
property_vend_h(WeaponStats)

/*****************************************************************//*!
\class WeaponComponent
\brief
	Identifies an entity as a Weapon.
*//******************************************************************/
class WeaponComponent : public IRegisteredComponent<WeaponComponent>, public Holdable
#ifdef IMGUI_ENABLED
	, IEditorComponent<WeaponComponent>
#endif
{
public:
	// Copied from Unity Project
	enum FIRE_RESPONSE : int   //Enum for fire responses
	{
		F_FIRED = 0,        //Fired
		F_RELOADING,    //Can't fire, reloading
		F_CYCLING,      //Can't fire, still waiting for fire rate
		F_NOAMMO,       //Can't fire, out of ALL ammo
		F_EMPTYMAG,     //Can't fire, magazine empty but there is still held ammo
	};

	//!The weapon's stats
	WeaponStats stats;

	//!The time to cycle a new bullet. This is calculated on weapon creation or when RPM is updated.
	float _cycleTime;
	//!The current remaining cycle time
	float cycleCurr;
	//!The amount of rounds currently held in the weapon
	int magazineCurr;
	//!The current amount of ammo remaining in this weapon
	int ammoCurr;
	//!The current amount of recoil the weapon is experiencing
	float recoilCurr;
	//!Current remaining reload time
	float reloadCurr;

	AudioReference audioReload;
	AudioReference audioFire;
	AudioReference audioPickup;

	//!Is the gun reloading?
	bool isReloading;
	//!Is the trigger being held down?
	bool isTriggerHeld;

	bool inited;

	float despawnCurr;
	float despawnTime;

	bool isPlayerWeapon;//!Marks this weapon as being held by the player.
	Vector2 _firePointPixel;

	void ApplyStats(WeaponStats _stats);
	void LoadStats(std::string statsFileName);

	uint32_t width, height;

	float totalspread;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	WeaponComponent();


	/*****************************************************************//*!
	\brief
		Fire function. Spawns a bullet prefab.
	\return
		A FIRE_RESPONSE value based on how the weapon responds.
	*//******************************************************************/
	FIRE_RESPONSE Fire();

	/*****************************************************************//*!
	\brief
		Reloads the weapon.
	\param speed
		The speed to reload with. Reload time is scaled inversely proportional
		to this value.
	\return
		Whether the weapon has started reloading.
	*//******************************************************************/
	bool Reload(float speed);
	/*****************************************************************//*!
	\brief
		Inits the Weapon.
	*//******************************************************************/
	void Init();
	/*****************************************************************//*!
	\brief
		Sets the highlighted status of the object.
	\param highlighted
		Whether the object is highlighted.
	*//******************************************************************/
	void SetIsHighlighted(bool highlighted);

	/*****************************************************************//*!
	\brief
		On Use Start override. This is called when the given key is pressed.
		(keys are in Highlightable::useKeys)
	\param use
		The key.
	*//******************************************************************/
	void OnUseStart(KEY use) override;
	/*****************************************************************//*!
	\brief
		On Use Hold override. This is called while the given key is pressed.
		(keys are in Highlightable::useKeys)
	\param use
		The key.
	*//******************************************************************/
	void OnUseHold(KEY use) override;
	/*****************************************************************//*!
	\brief
		On Use End override. This is called when the given key is released.
		(keys are in Highlightable::useKeys)
	\param use
		The key.
	*//******************************************************************/
	void OnUseEnd(KEY use) override;

	/*****************************************************************//*!
	\brief
		On Switched override. This is called when the weapon is switched 
		to or from in the inverntory.
	\param toThis
		Whether the weapon was switched to or from.
	*//******************************************************************/
	void OnSwitched(bool toThis) override;

	void OnDropped() override;

	// The muzzle flash object
	EntityReference muzzleFlash;

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(WeaponComponent& comp);
#endif
	property_vtable()
};
property_begin(WeaponComponent)
{
	property_var(name),
	property_var(isPlayerWeapon),
	property_var(stats),
	property_var(audioReload),
	property_var(audioFire),
	property_var(audioPickup),
	property_var(interactable),
}
property_vend_h(WeaponComponent)

class WeaponSystem : public ecs::System<WeaponSystem, WeaponComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	WeaponSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a WeaponComponent.
	\param comp
		The WeaponComponent to update.
	*//******************************************************************/
	void UpdateWeaponComp(WeaponComponent& comp);
};
