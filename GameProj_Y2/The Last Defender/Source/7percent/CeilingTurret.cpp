#include "CeilingTurret.h"
#include "TweenManager.h"
#include "LightComponent.h"
#include "Scheduler.h"
#include "PrefabManager.h"
#include "AudioManager.h"
#include "Bullet.h"
#include "Physics.h"
#include "NameComponent.h"
#include <cmath>

CeilingTurretComponent::CeilingTurretComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	turretDetectionCollider	{ nullptr },
	turretPivot				{ nullptr },
	turretGunSprite			{ nullptr },
	turretGunFirepoint		{ nullptr },
	target					{ nullptr },
	state					{ CeilingTurretState::NEUTRAL},
	bulletDamage			{ 50 },
	bulletSpeed				{ 80.0f },
	timeUpdateInterval		{ 0.1f },
	timeBulletInterval		{ 0.2f },
	timeIntermission		{ 0.3f },
	sweepAngle				{ 70.0f },
	sweepAngleOffset		{ -90.0f },
	sweepSpeed				{ 3.14f },
	predictionMultiplier	{ 1.0f },
	updateTimer				{ 0.0f },
	gunfireTimer			{ 0.0f },
	intermissionTimer		{ 0.0f },
	sineWaveProgression		{ 0.0f },
	inited					{ false }
{
}

void CeilingTurretComponent::OnDetached()
{
	if (!turretDetectionCollider) return;

	if (auto eventsComp{ turretDetectionCollider->GetComp<EntityEventsComponent>() })
	{
		eventsComp->Unsubscribe("OnCollision", this, &CeilingTurretComponent::OnCollision);
	}
}

// Ceiling turret detection collider should only hit objects on player layer
void CeilingTurretComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	target = ecs::GetEntity(collisionData.otherComp);
}

void CeilingTurretComponent::UpdateCeilingTurret()
{
	// Safety checks
	if (!turretDetectionCollider ||
		!turretPivot ||
		!turretGunSprite ||
		!turretGunFirepoint)
	{
		return;
	}

	// Calculate values
	float turretCurrentAngle = turretPivot->GetTransform().GetWorldRotation();
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);

	// Different behaviours depending on state
	switch (state)
	{
	case CeilingTurretState::NEUTRAL:
	{
		// If target acquired, transition to intermission for tracking state
		if (target)
		{
			// Calculate angle to target
			Vector2 direction = target->GetTransform().GetWorldPosition() - thisEntity->GetTransform().GetWorldPosition();
			float targetAngle = math::ToDegrees(atan2(direction.y, direction.x));

			// Tween to target angle
			ST<TweenManager>::Get()->StartTween(
				turretPivot,
				&Transform::SetWorldRotation,
				turretCurrentAngle,
				targetAngle,
				timeIntermission,
				TT::EASE_BOTH
			);

			// Set light color to red
			turretGunSprite->GetComp<LightComponent>()->color = Vector3(1.0f, 0.0f, 0.0f);

			// Set state
			state = CeilingTurretState::INTER_TRACKING;
			break;
		}

		// Turret swing animation
		sineWaveProgression += GameTime::FixedDt() * sweepSpeed;
		turretPivot->GetTransform().SetWorldRotation(sweepAngle * sin(sineWaveProgression) + sweepAngleOffset);
		break;
	}
	case CeilingTurretState::TRACKING:
	{
		// If no target has not been restored since last update, tween to neutral position
		if (!target)
		{
			ST<TweenManager>::Get()->StartTween(
				turretPivot,
				&Transform::SetWorldRotation,
				turretCurrentAngle,
				sweepAngleOffset,
				timeIntermission,
				TT::EASE_BOTH
			);

			// Set light color to white
			turretGunSprite->GetComp<LightComponent>()->color = Vector3(1.0f);

			// Set state
			state = CeilingTurretState::INTER_NEUTRAL;
			break;
		}

		// Increment update timer
		updateTimer += GameTime::FixedDt();
		gunfireTimer += GameTime::FixedDt();

		// Update tracker at time intervals
		if (updateTimer > timeUpdateInterval)
		{
			// Reset timer
			updateTimer = 0.0f;

			// If target exists
			if (target)
			{
				// Calculate angle to target
				Vector2 targetPosition = target->GetTransform().GetWorldPosition();
				Vector2 targetVelocity = target->GetComp<Physics::PhysicsComp>()->GetVelocity();
				Vector2 predictedPosition = targetPosition + targetVelocity * predictionMultiplier;
				Vector2 direction = predictedPosition - thisEntity->GetTransform().GetWorldPosition();
				float targetAngle = math::ToDegrees(atan2(direction.y, direction.x));
				float angleDelta = targetAngle - turretCurrentAngle;
				angleDelta = fmodf(angleDelta + 180.0f, 360.0f) - 180.0f;
				float shortestTargetAngle = turretCurrentAngle + angleDelta;

				// Tween to target angle
				ST<TweenManager>::Get()->StartTween(
					turretPivot,
					&Transform::SetWorldRotation,
					turretCurrentAngle,
					shortestTargetAngle,
					timeUpdateInterval,
					TT::LINEAR
				);

				// Tracking state must constantly try to unset target
				// To know when player has stepped out of detection box
				target = nullptr;
			}
		}

		// Update gunfire
		if (gunfireTimer > timeBulletInterval)
		{
			gunfireTimer = 0.0f;

			Vector2 dir(cos(math::ToRadians(turretCurrentAngle)), sin(math::ToRadians(turretCurrentAngle)));
			Vector2 bulletPos = turretGunFirepoint->GetTransform().GetWorldPosition();

			ecs::EntityHandle temp = PrefabManager::LoadPrefab("BulletEnemy");
			temp->GetComp<BulletComponent>()->Fire(bulletPos, dir, bulletDamage, bulletSpeed, 5.0f);

			// Play sound
			ST<AudioManager>::Get()->StartSingleSound("Boss_Gunfire", false, bulletPos);
		}
		break;
	}
	case CeilingTurretState::INTER_NEUTRAL:
	{
		intermissionTimer += GameTime::FixedDt();
		if (intermissionTimer > timeIntermission)
		{
			intermissionTimer = 0.0f;
			sineWaveProgression = 0.0f;
			state = CeilingTurretState::NEUTRAL;
		}
		break;
	}
	case CeilingTurretState::INTER_TRACKING:
	{
		intermissionTimer += GameTime::FixedDt();
		if (intermissionTimer > timeIntermission)
		{
			intermissionTimer = 0.0f;
			gunfireTimer = 0.0f;
			state = CeilingTurretState::TRACKING;
			ST<AudioManager>::Get()->StartSingleSound("Turret_Activated", false, thisEntity->GetTransform().GetWorldPosition(), 10.0f);
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void CeilingTurretComponent::Init()
{
	// Find entity references
	auto children = ecs::GetEntity(this)->GetTransform().GetChildrenRecursive();

	for (auto child : children)
	{
		// Check name component
		ecs::EntityHandle childEntity = child->GetEntity();
		std::string childName = childEntity->GetComp<NameComponent>()->GetName();

		// Assign references
		if (childName == "DetectionCollider")
		{
			turretDetectionCollider = childEntity;
		}
		if (childName == "Pivot")
		{
			turretPivot = childEntity;
		}
		if (childName == "TurretGunSprite")
		{
			turretGunSprite = childEntity;
		}
		if (childName == "TurretGunFirepoint")
		{
			turretGunFirepoint = childEntity;
		}
	}

	// Check existence
	if (!turretDetectionCollider ||
		!turretPivot ||
		!turretGunSprite ||
		!turretGunFirepoint)
	{
		CONSOLE_LOG(LEVEL_WARNING) << "Ceiling Turret does not have all necessary references!";
		return;
	}

	// Subscribe to collision
	turretDetectionCollider->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &CeilingTurretComponent::OnCollision);
	inited = true;
}

#ifdef IMGUI_ENABLED
void CeilingTurretComponent::EditorDraw(CeilingTurretComponent& comp)
{
	/*comp.turretDetectionCollider.EditorDraw("Detection Collider");
	comp.turretPivot.EditorDraw("Pivot");
	comp.turretGunSprite.EditorDraw("Gun Sprite");
	comp.turretGunFirepoint.EditorDraw("Gun Firepoint");*/
	ImGui::InputInt("Bullet Damage", &comp.bulletDamage);
	ImGui::DragFloat("Bullet Speed", &comp.bulletSpeed);
	ImGui::DragFloat("Update Interval", &comp.timeUpdateInterval);
	ImGui::DragFloat("Bullet Interval", &comp.timeBulletInterval);
	ImGui::DragFloat("Intermission Interval", &comp.timeIntermission);
	ImGui::DragFloat("Sweep Angle", &comp.sweepAngle);
	ImGui::DragFloat("Sweep Offset", &comp.sweepAngleOffset);
	ImGui::DragFloat("Sweep Speed", &comp.sweepSpeed);
	ImGui::DragFloat("Prediction Multiplier", &comp.predictionMultiplier);
}
#endif

CeilingTurretSystem::CeilingTurretSystem()
	: System_Internal{ &CeilingTurretSystem::UpdateCeilingTurretComp }
{
}

void CeilingTurretSystem::UpdateCeilingTurretComp(CeilingTurretComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}
	comp.UpdateCeilingTurret();
}
