/******************************************************************************/
/*!
\file   Weapon.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the definition of functions found in WeaponComponent and
	WeaponSystem.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Physics.h"
#include "Weapon.h"
#include "AudioManager.h"
#include "PrefabManager.h"
#include "ResourceManager.h"
#include "RenderComponent.h"
#include "Collision.h"
#include "MultiSpriteComponent.h"
#include "AnimatorComponent.h"
#include "ComponentLookupWorkaround.h"
#include "BombRange.h"

void WeaponComponent::ApplyStats(WeaponStats _stats)
{
	stats = _stats;
	magazineCurr = stats.magazineSize;
	thisEntity = ecs::GetEntity(this);

	ecs::CompHandle<Physics::ColliderComp> colliderComp = thisEntity->GetComp<Physics::ColliderComp>();
	ecs::CompHandle<MultiSpriteComponent> multiSprite = thisEntity->GetCompInChildren<MultiSpriteComponent>();

	// Assign the sprite ID
	multiSprite->ChangeSprite(0);
	Sprite const& sprite = multiSprite->GetSprite();
	colliderComp->SetScale({ static_cast<float>(sprite.width),static_cast<float>(sprite.height) });

	_firePointPixel = stats.firePointPixel;
	_firePointPixel.y = static_cast<float>(sprite.height) - _firePointPixel.y;
	_firePointPixel -= Vector2{ static_cast<float>(sprite.width / 2), static_cast<float>(sprite.height / 2) };

	width = sprite.width;
	height = sprite.height;

	// Set Hold Point
	holdPointPixel = stats.holdPointPixel -Vector2{ static_cast<float>(sprite.width / 2), static_cast<float>(sprite.height / 2) };

	//Delete the Muzzle Flash if it is already assigned to
	if (muzzleFlash.IsValidReference())
	{
		ecs::DeleteEntity(muzzleFlash);
		muzzleFlash = nullptr;
	}

	// Assign Muzzle Flash object reference
	std::string muzzleFlashName = "Muzzleflash_";
	switch (stats.weaponType)
	{
	default:
	case WeaponStats::WT_PISTOL:
		muzzleFlashName += "Pistol";
		break;
	case WeaponStats::WT_ASSAULT:
		muzzleFlashName += "Assault";
		break;
	case WeaponStats::WT_SHOTGUN:
		muzzleFlashName += "Shotgun";
		break;
	case WeaponStats::WT_MINIGUN:
		muzzleFlashName += "Minigun";
		break;
	case WeaponStats::WT_SNIPER:
		muzzleFlashName += "Sniper";
		break;
	case WeaponStats::WT_SMG:
		muzzleFlashName += "SMG";
		break;
	case WeaponStats::WT_RAILGUN_PIERCING:
		muzzleFlashName += "Railgun";
		break;
	}
	muzzleFlash = ST<PrefabManager>::Get()->LoadPrefab(muzzleFlashName);
	muzzleFlash->GetTransform().SetParent(thisEntity->GetTransform());
	
	muzzleFlash->GetTransform().SetLocalPosition(stats.muzzleFlashPosition);
	muzzleFlash->GetTransform().SetLocalRotation(0.0f);

	name = stats.weaponName;
}

void WeaponComponent::LoadStats(std::string statsFileName)
{
	Deserializer statsLoader{ ST<Filepaths>::Get()->assets + "/WeaponStats/" + statsFileName + ".json"};
	if (!statsLoader.IsValid())
	{
		CONSOLE_LOG_EXPLICIT("YOU ARE LOADING A NONEXISTENT GUN! NAME: " + statsFileName, LogLevel::LEVEL_ERROR);
		return;
	}
	statsLoader.Deserialize(&stats);
	ApplyStats(stats);
}


WeaponComponent::WeaponComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	stats{},
	_cycleTime{ 0.0f },
	cycleCurr{ 0.0f },
	magazineCurr{ 0 },
	ammoCurr{ 100 },
	recoilCurr{ 0.0f },
	reloadCurr{ 0.0f },
	audioReload{ "Reload" },
	audioFire{ "Shoot_Pistol" },
	audioPickup{ "Pickup_Pistol" },
	isReloading{ false },
	isTriggerHeld{ false },
	inited{ false },
	despawnCurr{ 0.0f },
	despawnTime{ 3.0f },
	isPlayerWeapon{ false },
	_firePointPixel{ 0.0f,0.0f },
	width{ 0 },
	height{ 0 },
	totalspread{ 0.0f },
	Holdable("Uninited Weapon",0.0f,{ KEY::M_LEFT }, {KEY::R})
{
}

#ifdef IMGUI_ENABLED
void WeaponComponent::EditorDraw(WeaponComponent& comp)
{
	ImGui::Checkbox("Is Player Gun", &comp.isPlayerWeapon);

	comp.audioReload.EditorDraw("Reload Audio");
	comp.audioFire.EditorDraw("Fire Audio");
	comp.audioPickup.EditorDraw("Pickup Audio");

	if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
	{
		comp.stats.weaponName.resize(MAX_NAME_LENGTH);
		ImGui::InputText("Name", comp.stats.weaponName.data(), MAX_NAME_LENGTH);
		comp.stats.weaponName = comp.stats.weaponName.substr(0, comp.stats.weaponName.find_first_of('\0'));

		ImGui::InputFloat("RPM", &comp.stats.fireRPM);

		ImGui::Combo("Fire Type", &comp.stats.fireType, "Semi Auto\0Auto\0Burst\0Railgun");
		ImGui::Combo("Weapon Type", &comp.stats.weaponType, "Pistol\0Assault\0Shotgun\0Minigun\0Sniper\0SMG\0Railgun\0Non-piercing Railgun\0Grenade\0Noisy Cricket");

		ImGui::InputInt("Burst Count", &comp.stats.burstCount);
		ImGui::InputInt("Pellet Count", &comp.stats.pelletCount);
		ImGui::InputInt("Damage", &comp.stats.damagePerBullet);
		ImGui::InputInt("Damage (Enemy)", &comp.stats.enemyDamage);
		ImGui::InputInt("Magazine Size", &comp.stats.magazineSize);
		ImGui::InputInt("Spread (Degrees)", &comp.stats.spread);

		ImGui::InputFloat("Recoil Per Shot", &comp.stats.recoilPerShot);
		ImGui::InputFloat("Recoil Wander Multiplier", &comp.stats.recoilWanderMultiplier);
		ImGui::InputFloat("Recoil Wander Max", &comp.stats.recoilWanderMax);
		ImGui::InputFloat("Recoil Recovery", &comp.stats.recoilRecovery);
		ImGui::InputFloat("Recoil Recovery Minimum", &comp.stats.recoilRecoveryMin);
		ImGui::InputFloat("Reload Time", &comp.stats.reloadTime);

		ImGui::InputFloat("Bullet Size", &comp.stats.bulletSize);
		ImGui::InputFloat("Bullet Speed", &comp.stats.bulletSpeed);
		ImGui::InputFloat("Bullet Speed Random Range", &comp.stats.bulletSpeedRange);
		ImGui::InputFloat("Bullet Lifetime", &comp.stats.bulletLifeTime);

		ImGui::InputFloat("Screen Shake/Degree", &comp.stats.screenShakePerSpreadDegree);
		ImGui::InputFloat("Screen Shake/Shot", &comp.stats.screenShakePerShot);
		ImGui::InputFloat("Max Screen Shake", &comp.stats.maximumScreenShake);

		// Helper function for vector2 controls (shamelessly copied over from Editor.cpp)
		auto DrawVec2Control = [](const char* label, Vector2& values, float columnWidth, float speed, const char* format)
			{
				bool modified = false;
				ImGui::PushID(label);
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, columnWidth);
				ImGui::Text(label);
				ImGui::NextColumn();

				// X Component
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::PushItemWidth(70);
				modified |= ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, format);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				// Y Component
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::PushItemWidth(70);
				modified |= ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, format);
				ImGui::PopItemWidth();

				ImGui::Columns(1);
				ImGui::PopID();
				return modified;
			};
		DrawVec2Control("Fire Point Pixel", comp.stats.firePointPixel, 80.0f, 1.0f, "%.1f");
		DrawVec2Control("Hold Point Pixel", comp.stats.holdPointPixel, 80.0f, 1.0f, "%.1f");
		DrawVec2Control("Muzzle Flash Position", comp.stats.muzzleFlashPosition, 80.0f, 1.0f, "%.1f");
		DrawVec2Control("Shell Ejection Position", comp.stats.shellEjectionPosition, 80.0f, 1.0f, "%.1f");

		ecs::CompHandle<MultiSpriteComponent> multiSprite = ecs::GetEntity(&comp)->GetCompInChildren<MultiSpriteComponent>();
		if (!multiSprite) return;
		Sprite const& sprite = multiSprite->GetSprite();
		comp.holdPointPixel = comp.stats.holdPointPixel - Vector2{ static_cast<float>(sprite.width / 2), static_cast<float>(sprite.height / 2) };

		ImGui::InputInt("Enemy Burst Count", &comp.stats.enemyBurstBullets);
		ImGui::InputFloat("Enemy Base Cycle Time", &comp.stats.enemyCycleTimeBase);
		ImGui::InputFloat("Enemy Burst Cycle Time", &comp.stats.enemyCycleTimeBurst);
	}
	if (ImGui::CollapsingHeader("Current values", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char currValueBuffer[128]{ "" };
		sprintf_s(currValueBuffer, "Current Cycle Time: %.3f", comp._cycleTime);
		ImGui::Text(currValueBuffer);

		ImGui::Text(comp.isReloading ? "Is Reloading: True" : "Is Reloading: False");
		ImGui::Text(comp.isTriggerHeld ? "Is Trigger Held: True" : "Is Trigger Held: False");
		ImGui::Text(comp.isHeld ? "Is Held: True" : "Is Held: False");

		sprintf_s(currValueBuffer, "Current Recoil: %.3f", comp.recoilCurr);
		ImGui::Text(currValueBuffer);

		sprintf_s(currValueBuffer, "Magazine: %d/%d", comp.magazineCurr, comp.stats.magazineSize);
		ImGui::Text(currValueBuffer);
	}
	if (ImGui::Button("Save Gun"))
	{
		Serializer statsSaver{ ST<Filepaths>::Get()->assets + "/WeaponStats/" + comp.stats.weaponName + ".json" };
		statsSaver.Serialize(comp.stats);
		statsSaver.SaveAndClose();
	}
	static char nameBuffer[32]{ "" };
	ImGui::InputTextWithHint("Gun To Load : ", "Sniper", nameBuffer, 32);
	if (ImGui::Button("Load Gun"))
	{
		comp.LoadStats(nameBuffer);
	}
}
#endif


WeaponComponent::FIRE_RESPONSE WeaponComponent::Fire()
{
	// Get this entity
	ecs::EntityHandle weaponEntity{ ecs::GetEntity(this) };

	// Separate branch of logic for grenades
	if (stats.weaponType == WeaponStats::WT_GRENADE)
	{
		isHeld = false;
		interactable = false;

		// Reuse the muzzleFlash entityReference for the explosion
		muzzleFlash = nullptr;

		// Fuse timer is handled via RPM, so 60/fireRPM = fusetime
		cycleCurr = _cycleTime = 60.0f / stats.fireRPM;


		return FIRE_RESPONSE::F_FIRED;
	}

	// Failing to fire for various reasons
	if (isTriggerHeld && stats.fireType != WeaponStats::FIRE_TYPE::FT_AUTO)
		return FIRE_RESPONSE::F_CYCLING;
	if (isReloading)
		return FIRE_RESPONSE::F_RELOADING;
	if (magazineCurr <= 0)
	{
		if (ammoCurr <= 0)
			return FIRE_RESPONSE::F_NOAMMO;
		else
		{
			Reload(1.0f);
			return FIRE_RESPONSE::F_EMPTYMAG;
		}
	}
	if (_cycleTime > 0)
		return FIRE_RESPONSE::F_CYCLING;

	//Set the cycle time based on RPM
	_cycleTime = 60.0f / stats.fireRPM;


	Transform& wTransform = ecs::GetEntity(this)->GetTransform();
	float fireangle = weaponEntity->GetTransform().GetWorldRotation();
	if (wTransform.GetWorldScale().x < 0)
		fireangle = 180.0f - fireangle;

	// Perform a raycast here to get the closer point;
	// Either firePoint or an object that may be in the way
	float fireangleRadians = math::ToRadians(fireangle);
	Vector2 aimDir(cosf(fireangleRadians), sinf(fireangleRadians));

#pragma region Bullet spawn raycast
	// Start point is from the left end of the weapon
	Vector2 fireRaycastTestStartLocal = stats.firePointPixel * wTransform.GetWorldScale().x;
	fireRaycastTestStartLocal.x = -static_cast<float>(width) / 2;

	Vector2 fireRaycastTestStartWorld = wTransform.GetWorldPosition() + fireRaycastTestStartLocal.Rotate(fireangleRadians);
	// Perform the raycast
	Physics::RaycastResult raycastResult;

	// We search the environment, and Enemy layer for player guns / Player layer for enemy guns
	EntityLayersMask layersToSearch({ ENTITY_LAYER::ENVIRONMENT , isPlayerWeapon ? ENTITY_LAYER::ENEMY : ENTITY_LAYER::PLAYER });
	Physics::Raycast(fireRaycastTestStartWorld, aimDir, layersToSearch, &raycastResult);

	// Get the distances squared
	float barrelLength = (stats.firePointPixel - fireRaycastTestStartLocal).Length();

	// The lower of the two values is where the bullet *should* come from.
	float chosenDistance = math::Min(barrelLength, raycastResult.distance);

	// Calculate the bullet origin
	Vector2 bulletOrigin = fireRaycastTestStartWorld + aimDir * chosenDistance;
	// Commented-out debugging code, we may want to use this more in the future
	//ecs::GetCompsBegin<RaycastDebug>().GetEntity()->GetTransform().SetWorldPosition(bulletOrigin);
#pragma endregion
	//float initialangle = Mathf.Atan2(aimdir.y, aimdir.x);
	recoilCurr += stats.recoilPerShot;

	// Audio Plays Here
	if (isPlayerWeapon)
	{
		ST<AudioManager>::Get()->StartSound(audioFire);
	}
	else
	{
		ST<AudioManager>::Get()->StartSound(audioFire, false, ecs::GetEntity(this)->GetTransform().GetWorldPosition());
	}
	--magazineCurr;
	// We do the same spread calculations here, but without a for loop as pelletcount now dictates the number of bounces
	float randspread = static_cast<float>(rand() % 1000) / 1000.0f;
	float finalfireangle = fireangle + (totalspread * randspread - totalspread / 2);
	switch (stats.weaponType)
	{
	case WeaponStats::WT_RAILGUN:
	case WeaponStats::WT_RAILGUN_PIERCING:
		for (int i = 0; i < stats.pelletCount; ++i)
		{

			// Create the laser
			ecs::EntityHandle newLaser = PrefabManager::LoadPrefab(isPlayerWeapon ? "Laser" : "LaserEnemy");
			ecs::CompHandle<LaserComponent> laserComp = newLaser->GetComp<LaserComponent>();

			// Get the laser's fire direction
			finalfireangle = math::ToRadians(finalfireangle);
			Vector2 fireDir{ cosf(finalfireangle), sinf(finalfireangle) };

			// Set the piercing flag to true if it's meant to pierce
			laserComp->SetPiercing(stats.weaponType == WeaponStats::WT_RAILGUN_PIERCING);

			// Make the laser fire
			laserComp->Fire(stats.bulletSpeed, bulletOrigin, fireDir, 1.0f, stats.damagePerBullet, isPlayerWeapon);
		}
		break;
	default:// This is for projectile bullets☺
#pragma region Shell casings
		// Load a shell casing from PrefabManager
		ecs::EntityHandle shellCasing = PrefabManager::LoadPrefab("ShellCasing");
		// Get the transform of the shell and set its position
		Transform& shellTransform = shellCasing->GetTransform();
		// Set shell position to the ejection point
		Vector2 shellEjectionStartLocal = stats.shellEjectionPosition * wTransform.GetWorldScale().x;
		Vector2 shellEjectionWorld = wTransform.GetWorldPosition() + shellEjectionStartLocal.Rotate(fireangleRadians);

		// Set shell position to the calculated ejection point
		shellTransform.SetWorldPosition(shellEjectionWorld);

		float perpAngle = fireangle + 90.0f;
		// Adjust based on weapon orientation
		if (wTransform.GetWorldScale().x < 0)
			perpAngle = fireangle - 90.0f;

		// Now rotate the perpendicular angle backward by 20 degrees for a backward arc
		float shellEjectionAngle = perpAngle + 30.0f;
		if (wTransform.GetWorldScale().x < 0)
			shellEjectionAngle = perpAngle - 30.0f;

		shellEjectionAngle += util::RandomRange(-10.0f, 10.0f);
		float shellEjectionRadians = math::ToRadians(shellEjectionAngle);
		Vector2 shellDirection(cosf(shellEjectionRadians), sinf(shellEjectionRadians));
		//scaled by speed of projectile
		Vector2 shellEjectionSpeed = shellDirection * util::RandomRange(8.0f, 10.0f) * stats.bulletSpeed;
		// Apply velocity to physics components
		for (auto physComp : shellCasing->GetCompInChildrenVec<Physics::PhysicsComp>())
		{
			physComp->SetVelocity(shellEjectionSpeed);
			physComp->AddAngVelocity(util::RandomRange(300.0f, 400.0f));
		}
		shellTransform.SetParent(nullptr);
#pragma endregion
		for (int i = 0; i < stats.pelletCount; ++i)
		{
			// Get a direction from the spread and reassign
			randspread = static_cast<float>(rand() % 1000) / 1000.0f;

			finalfireangle = fireangle + (totalspread * randspread - totalspread / 2);
			//CONSOLE_LOG_EXPLICIT("FIRE ANGLE: " + std::to_string(finalfireangle), LogLevel::LEVEL_DEBUG);
			// Load a bullet from PrefabManager
			ecs::EntityHandle newBullet = PrefabManager::LoadPrefab(isPlayerWeapon ? "Bullet" : "BulletEnemy");

			// Get the transform of the new bullet and the weapon
			Transform& bTransform = newBullet->GetTransform();

			// Assign it as a child of the gun to perform local position shenanigans
			bTransform.SetParent(wTransform);

			// Get the BulletComponent of the new bullet
			ecs::CompHandle<BulletComponent> bulletComp = newBullet->GetComp<BulletComponent>();

			if (stats.weaponType == WeaponStats::WT_NOISYCRICKET)
			{
				bulletComp->SetExplosive(true);
			}

			// Set the bullet's position
			//bTransform.SetLocalPosition(bulletOrigin);
			bTransform.SetWorldPosition(bulletOrigin);

			// Set the rotation of the bullet
			bTransform.SetLocalRotation(0.0f);

			// Use this angle to set bullet's direction of travel
			finalfireangle = math::ToRadians(finalfireangle);
			Vector2 bulletDirection{ cosf(finalfireangle),sinf(finalfireangle) };
			Vector2 parentVelocity{ 0.0f };

			/*auto physComp = thisEntity->GetCompInParents< Physics::PhysicsComp>();
			if (physComp)
				parentVelocity = physComp->GetVelocity();*/

			bulletComp->Fire(
				bTransform.GetWorldPosition(),
				bulletDirection,
				isPlayerWeapon ? stats.damagePerBullet : stats.enemyDamage,
				stats.bulletSpeed + ((static_cast<float>(rand() % 1000) / 1000.0f * stats.bulletSpeedRange) - (stats.bulletSpeedRange / 2)), stats.bulletLifeTime,
				parentVelocity
			);
			// Disown
			bTransform.SetParent(nullptr);
		}
		break;

	}
	// Play the muzzle flash animation
	muzzleFlash->GetComp<RenderComponent>()->SetColor(Vector4(1.0f));
	muzzleFlash->GetComp<AnimatorComponent>()->SetFrame(0);
	muzzleFlash->GetComp<AnimatorComponent>()->Play();
	// Only player's weapon should cause screen shake
	if (isPlayerWeapon)
	{
		// Shake the screen
		Messaging::BroadcastAll("DoCameraShake", stats.screenShakePerSpreadDegree * totalspread + stats.screenShakePerShot, stats.maximumScreenShake);
	}

	// Auto reload
	if (magazineCurr <= 0)
		Reload(1.0f);

	return FIRE_RESPONSE::F_FIRED;
}

bool WeaponComponent::Reload(float speed = 1.0f)
{
	// Don't reload if no ammo, mag is full, or already reloading
	if (ammoCurr <= 0 || magazineCurr >= stats.magazineSize || isReloading) return false;

	isReloading = true;
	reloadCurr = stats.reloadTime/speed;

	// Leaving the commented Unity code here since we have 
	ST<AudioManager>::Get()->StartSound(audioReload);
	return true;
}

void WeaponComponent::Init()
{
	inited = true;

	LoadStats(stats.weaponName);
}

void WeaponComponent::SetIsHighlighted(bool highlighted)
{
	Highlightable::SetIsHighlighted(highlighted, ecs::GetEntity(this));
}

void WeaponComponent::OnUseStart(KEY use)
{
	switch (use)
	{
	case KEY::R:
		Reload();
		break;
	}
}

void WeaponComponent::OnUseHold(KEY use)
{
	switch (use)
	{
	case KEY::M_LEFT:
		Fire();
		isTriggerHeld = true;
		break;
	}
}

void WeaponComponent::OnUseEnd(KEY use)
{
	switch (use)
	{
	case KEY::M_LEFT:
		isTriggerHeld = false;
		break;
	}
}

void WeaponComponent::OnSwitched(bool toThis)
{
	UNREFERENCED_PARAMETER(toThis);
	// Cancel the muzzle flash animation
	if(muzzleFlash)
	muzzleFlash->GetComp<RenderComponent>()->SetColor(Vector4(0.0f));
}

void WeaponComponent::OnDropped()
{
	Holdable::OnDropped();
	isReloading = false;
	if (!isPlayerWeapon)
		autoPickupCooldown = 0.0f;
	isPlayerWeapon = false;
}

WeaponSystem::WeaponSystem()
	: System_Internal{ &WeaponSystem::UpdateWeaponComp }
{
}

void WeaponSystem::UpdateWeaponComp(WeaponComponent& comp)
{
	// Initialisation must be done here and not OnAttached()
	if (!comp.inited)
	{
		comp.Init();
	}
	float dt = GameTime::FixedDt();

	// Firing weapon if the character's holding it
	if (comp.isHeld)
	{
		if (comp.isTriggerHeld)
		{
			comp.Fire();
		}
		float wander = math::Min(comp.stats.recoilWanderMultiplier * comp.recoilCurr, comp.stats.recoilWanderMax);

		comp.totalspread = math::Max(comp.stats.spread + wander, 0.0f);

		if (comp.isReloading)// Lower reload time if still reloading
		{
			comp.reloadCurr -= dt;// *Player.Instance.reloadSpeed;
			if (comp.reloadCurr < 0.0f)
			{
				//Stop reloading
				comp.isReloading = false;
				comp.reloadCurr = comp.stats.reloadTime;

				// Directly set mag amount since we are doing away with reserve ammo
				comp.magazineCurr = comp.stats.magazineSize;
				
				comp._cycleTime = 0;
			}
		}
		comp._cycleTime = math::MoveTowards(comp._cycleTime, 0.0f, dt);
	}
	else
	{
		// Cancel reload if the weapon is dropped
		comp.isTriggerHeld = false;
		comp.isReloading = false;

		// If the weapon is a grenade, we check if it isn't interactable
		// Non-interactable grenades are thrown after all
		if (comp.stats.weaponType == WeaponStats::WT_GRENADE && !comp.interactable)
		{
			comp._cycleTime -= dt;
			if (comp._cycleTime <= 0.0f)
			{
				if (!comp.muzzleFlash.IsValidReference())
				{
					// Load the explosion range
					comp.muzzleFlash = PrefabManager::LoadPrefab("BombRange");

					// Set its position to be same as comp entity's
					ecs::CompHandle<BombRangeComponent> bombRange = comp.muzzleFlash->GetComp<BombRangeComponent>();
					
					bombRange->Fire(
						ecs::GetEntity(&comp)->GetTransform().GetWorldPosition(),
						Vector2(0.0f),
						Vector2(250.0f),
						110,
						5000.0f, 
						0.5f);

					ecs::DeleteEntity(ecs::GetEntity(&comp));
				}
			}
		}
	
		comp.UpdatePickupCooldown(dt);
	}

	// Recoil Logic
	float recoveryThisFrame = math::Max(comp.stats.recoilRecovery * comp.recoilCurr, comp.stats.recoilRecoveryMin);
	comp.recoilCurr = math::MoveTowards(comp.recoilCurr, 0.0f, recoveryThisFrame * dt);
}
