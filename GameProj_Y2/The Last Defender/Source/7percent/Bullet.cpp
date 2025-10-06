/******************************************************************************/
/*!
\file   Bullet.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the definition of functions declared in BulletComponent
	and BulletSystem.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Bullet.h"
#include "Physics.h"
#include "Collision.h"
#include "Health.h"
#include "Bullet.h"
#include "Player.h"
#include "AudioManager.h"
#include "EnemyStateMachine.h"
#include <FadeAndDie.h>

#define BULLET_SPEED_MULTIPLIER 25.0f

const float LaserComponent::maxDistancePerSegment = 256;
const int LaserComponent::maxBounceCount = 32;

BulletComponent::BulletComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	direction{ 1.0f, 0.0f }
	, damage{ 10 }
	, speed{ 1000.0f }
	, lifeTime{ 1.0f }
	, isPlayerBullet{ false }
	, audioImpact{ "Gun_Impact" }
	, audioImpactBoss{ "Gun_Impact_Boss" }
	, audioImpactEnvironment{ "Gun_Impact_Environment" }
	, gravity{false}
	, explosive{false}
{
}

void BulletComponent::SetDirection(const Vector2& newDirection)
{
	direction = newDirection;
}

void BulletComponent::LowerLifeTime(float amount)
{
	lifeTime -= amount;
}

const Vector2& BulletComponent::GetDirection() const
{
	return direction;
}

float BulletComponent::GetSpeed() const
{
	return speed;
}

void BulletComponent::SetSpeed(float _speed)
{
	speed = _speed;
}

void BulletComponent::SetLifeTime(float _lifeTime)
{
	lifeTime = _lifeTime;
}

float BulletComponent::GetLifeTime() const
{
	return lifeTime;
}

void BulletComponent::Fire(Vector2 _origin, Vector2 _direction,int _damage, float _speed, float _lifeTime, Vector2 parentVelocity)
{
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	thisEntity->GetTransform().SetWorldPosition(_origin);
	damage = _damage;
	speed = _speed;
	direction = _direction;
	lifeTime = _lifeTime;

	ecs::CompHandle<Physics::PhysicsComp> physicsComp = thisEntity->GetComp<Physics::PhysicsComp>();
	physicsComp->SetVelocity(direction * speed * BULLET_SPEED_MULTIPLIER + parentVelocity);

}

#ifdef IMGUI_ENABLED
void BulletComponent::EditorDraw(BulletComponent& comp)
{
	ImGui::DragFloat("Speed", &comp.speed);
	ImGui::Checkbox("Is Player Bullet", &comp.isPlayerBullet);

	comp.audioImpact.EditorDraw("Impact Sound");
	comp.audioImpactBoss.EditorDraw("Impact Sound Boss");
	comp.audioImpactEnvironment.EditorDraw("Impact Sound Environment");
}
#endif

void BulletComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &BulletComponent::OnCollision);
}

void BulletComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &BulletComponent::OnCollision);
}

void BulletComponent::SetDamage(int amount)
{
	damage = amount;
}

int BulletComponent::GetDamage()
{
	return damage;
}

bool BulletComponent::GetGravity()
{
	return gravity;
}

void BulletComponent::SetGravity(bool _gravity)
{
	gravity = _gravity;
}

bool BulletComponent::GetExplosive()
{
	return explosive;
}

void BulletComponent::SetExplosive(bool _explosive)
{
	explosive = _explosive;
}

void BulletComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	if (collisionData.otherComp->IsTrigger())
		return;
	ecs::EntityHandle thisEntity{ ecs::GetEntity(this) };
	ecs::EntityHandle otherEntity = ecs::GetEntity(collisionData.otherComp);

	Vector2 worldPosition = thisEntity->GetTransform().GetWorldPosition();

	// Bullets do not collide with one another
	if (ecs::CompHandle<BulletComponent> bulletComp = otherEntity->GetComp<BulletComponent>())
		return;
	
	// If other entity has a health component, damage it
	if (ecs::CompHandle<HealthComponent> healthComp{ otherEntity->GetComp<HealthComponent>() })
	{
		healthComp->TakeDamage(damage);

		// If the other entity is an enemy, inform it of the collision
		if (isPlayerBullet)
		{
			Messaging::BroadcastAll("PlayerDealtDamage", damage);
			if (ecs::CompHandle<EnemyControllerComponent> enemyComp{ otherEntity->GetComp<EnemyControllerComponent>() })
			{
				enemyComp->OnHitByPlayer(Dot(GetDirection(), enemyComp->GetViewDirection()) > 0.0f, static_cast<float>(damage));
			}
		}

		// Check healthtype
		std::string type = healthComp->GetHealthType();

		// Play different sounds depending on healthtype
		if (type == "Player" ||
			type == "EnemyPatrol" ||
			type == "EnemyWave" ||
			type == "Objective")
		{
			ST<AudioManager>::Get()->StartSound(audioImpact, false, worldPosition);
		}
		else if (
			type == "BossLeftGun" ||
			type == "BossRightGun" ||
			type == "BossLaserPoint")
		{
			ST<AudioManager>::Get()->StartSound(audioImpactBoss, false, worldPosition, 3.0f);
		}
	}
	else
	{
		ecs::CompHandle<EntityLayerComponent> layer = otherEntity->GetComp<EntityLayerComponent>();
		if (layer->GetLayer() == ENTITY_LAYER::ENVIRONMENT)
		{
			ST<AudioManager>::Get()->StartSound(audioImpactEnvironment, false, worldPosition);

			// Sprite method
			//ecs::EntityHandle bulletImpact = ecs::CreateEntity();
			//auto& transform = bulletImpact->GetTransform();
			//
			//transform.SetWorldPosition(worldPosition);
			//transform.SetLocalScale(Vector2(50.0f));
			//
			//bulletImpact->AddCompNow(RenderComponent());
			//bulletImpact->AddCompNow(AnimatorComponent{ "Boss_Explosion" });
			//bulletImpact->AddCompNow(FadeAndDieComponent());

			// Gibs method
			//ST<Scheduler>::Get()->Add([pos =ecs::GetEntityTransform(this).GetWorldPosition()] {
			//	ecs::EntityHandle gibsEntity{ ST<PrefabManager>::Get()->LoadPrefab("Environment Impact Gibs") };
			//	gibsEntity->GetTransform().SetWorldPosition(pos);
			//	for (auto physComp : gibsEntity->GetCompInChildrenVec<Physics::PhysicsComp>())
			//	{
			//		physComp->SetVelocity(Vector2::FromAngle(util::RandomRangeFloat(-70.0f, 70.0f)) * util::RandomRangeFloat(250.0f, 350.0f));
			//		physComp->SetAngVelocity(util::RandomRangeFloat(-1080.0f, 1080.0f));
			//	}
			//	});
		}
	}

	ecs::EntityHandle bulletImpact{ nullptr };
	if (isPlayerBullet)
	{
		bulletImpact = ST<PrefabManager>::Get()->LoadPrefab("BulletImpactBlue");
	}
	else
	{
		bulletImpact = ST<PrefabManager>::Get()->LoadPrefab("BulletImpactRed");
	}
	bulletImpact->GetComp<AnimatorComponent>()->Play();
	bulletImpact->GetTransform().SetWorldPosition(worldPosition);

	if(explosive)
	{
		Vector2 spawnOrigin = thisEntity->GetTransform().GetWorldPosition();
		ST<Scheduler>::Get()->Add(0.0f, [spawnOrigin]() {

			ecs::EntityHandle bombEntity = PrefabManager::LoadPrefab("BombRange");

			// Set its position to be same as comp entity's
			ecs::CompHandle<BombRangeComponent> bombRange = bombEntity->GetComp<BombRangeComponent>();

			bombRange->Fire(
				spawnOrigin,
				Vector2(100.0f),
				Vector2(100.0f),
				5,
				2000.0f,
				0.5f);
			});
	}
	ecs::DeleteEntity(thisEntity);
}

BulletMovementSystem::BulletMovementSystem()
	: System_Internal{ &BulletMovementSystem::UpdateBulletComp }
{
}

void BulletMovementSystem::UpdateBulletComp(BulletComponent& comp)
{
	ecs::EntityHandle bulletEntity{ ecs::GetEntity(&comp) };
	float dt = GameTime::FixedDt();
	// Move the bullet over time
	// Get the BulletComponent of the new bullet
	ecs::CompHandle<Physics::PhysicsComp> physicsComp = bulletEntity->GetComp<Physics::PhysicsComp>();

	physicsComp->SetFlag(Physics::PHYSICS_COMP_FLAG::ENABLE_GRAVITY, comp.GetGravity());
	//bulletEntity->GetTransform().AddLocalPosition(comp.GetDirection() * (dt * comp.GetSpeed()));
	//physicsComp->SetVelocity(comp.GetDirection() * comp.GetSpeed()*25); // To counteract gravity

	// Lifetime calculation
	comp.LowerLifeTime(dt);
	if (comp.GetLifeTime() <= 0.0f)
	{
		ecs::DeleteEntity(bulletEntity);
	}
}

void BulletMovementSystem::PostRun()
{
	ecs::FlushChanges();
}

LaserRendererSystem::LaserRendererSystem()
	: System_Internal{ &LaserRendererSystem::UpdateLaserComp }
{
}

void LaserRendererSystem::UpdateLaserComp(LaserComponent& comp)
{
	ecs::EntityHandle laserEntity{ ecs::GetEntity(&comp) };
	float dt = GameTime::FixedDt();

	Vector4 laserColor = laserEntity->GetComp<RenderComponent>()->GetColor();
	laserColor.w = comp.GetLifeTime() / comp.GetMaxLifeTime();
	laserEntity->GetComp<RenderComponent>()->SetColor(laserColor);

	comp.LowerLifeTime(dt);
	if (comp.GetLifeTime() <= 0.0f)
	{
		ecs::DeleteEntity(laserEntity);
	}

}

LaserComponent::LaserComponent():
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
 lifeTime{ 1.0f }
	, maxLifeTime{ 1.0f }
	, isPlayerBullet{ false }
	, isPiercing{ false }
	
{
}

void LaserComponent::SetPiercing(const bool& piercing)
{
	isPiercing = piercing;
}

void LaserComponent::LowerLifeTime(float amount)
{
	lifeTime -= amount;
}

float LaserComponent::GetLifeTime() const
{
	return lifeTime;
}

float LaserComponent::GetMaxLifeTime() const
{
	return maxLifeTime;
}

void LaserComponent::Fire(float distance, Vector2 origin, Vector2 direction, float life, int damage, bool isPlayerLaser, int bounceCount)
{
	// Set the values in the comp
	maxLifeTime = lifeTime = life;

	ecs::EntityHandle thisEntity{ ecs::GetEntity(this) };


	Physics::RaycastResult raycastResult{};
	// If the laser is piercing, we don't search for potential targets here
	EntityLayersMask layersToSearch;
	if (isPiercing)
	{
		layersToSearch = EntityLayersMask({ ENTITY_LAYER::ENVIRONMENT });
	}
	else
		layersToSearch = EntityLayersMask({ ENTITY_LAYER::ENVIRONMENT, isPlayerLaser ? ENTITY_LAYER::ENEMY : ENTITY_LAYER::PLAYER });
	Physics::Raycast(origin, direction, layersToSearch, &raycastResult);

	// Get the lower of the distance to the bounce or the remaining distance
	float distanceToBounce = math::Min(distance, raycastResult.distance);
	distance -= distanceToBounce;

#pragma region Segmentation
	// Iterate through the laser's travel distance and spawn segments 
	float segmentCount = distanceToBounce / maxDistancePerSegment;
	float previousSegment = 0.0f;

	// We perform the atan2 operation here because it's the same value for all segments in each bounce
	float segmentAngle = math::ToDegrees(atan2(direction.y, direction.x));
	for (float currentSegment = 0.0f; currentSegment < segmentCount + 1.0f; currentSegment += 1.0f)
	{
		// Limit currentSegment to be below segmentCount
		currentSegment = math::Min(currentSegment, segmentCount);

		// Create a new laser OBJECT for each segment past the first, but do not fire it because it do be expensive
		ecs::EntityHandle newLaser = thisEntity;
		if (currentSegment != 0.0f)
			newLaser = PrefabManager::LoadPrefab(isPlayerLaser ? "Laser" : "LaserEnemy");

		// Position is the midpoint between the previous and cuurent segment
		newLaser->GetTransform().SetWorldPosition(origin + direction * (currentSegment + previousSegment) / 2 * maxDistancePerSegment);

		// Rotation is self-explanatory
		newLaser->GetTransform().SetWorldRotation(segmentAngle);

		// Scale.x is the length of the segment
		Vector2 scale = newLaser->GetTransform().GetWorldScale();
		scale.x = (currentSegment - previousSegment) * maxDistancePerSegment;
		newLaser->GetTransform().SetWorldScale(scale);

		// Update previousSegment
		previousSegment = currentSegment;
	}
#pragma endregion

#pragma region Handle hits
	// This is where piercing and non-piercing differ, piercing performs a search through all entities within the range
	if (isPiercing)
	{
		// Perform a multiraycast along the laser from the origin to the hit point (or end)
		Physics::MultiRaycastResult multiRaycastResult{};

		// Search for enemy/player layer depending on the laser type
		EntityLayersMask multiLayersToSearch({ isPlayerLaser ? ENTITY_LAYER::ENEMY : ENTITY_LAYER::PLAYER });

		// Do the thing
		Physics::MultiRaycast(origin, direction, distanceToBounce, multiLayersToSearch, &multiRaycastResult);

		// Iterate through all the hits and damage them if needed
		for (auto& hit : multiRaycastResult.hits)
		{
			// Sanity check
			if (hit.hitComp)
			{
				ecs::EntityHandle hitEntity{ ecs::GetEntity(hit.hitComp) };

				// Damage the health component if there is one on the entity
				if (ecs::CompHandle<HealthComponent> healthComp = hitEntity->GetComp<HealthComponent>())
				{
					healthComp->TakeDamage(damage);
				}
			}
		}
	}
	// If the distance is still above 0, that means we hit something
	// Added sanity check for a hitComp
	// Added sanity check bounce count to prevent infinite loops
	if (distance > 0 && raycastResult.hitComp && --bounceCount > 0)
	{
		// Sanity check here
		if (ecs::EntityHandle hitEntity = ecs::GetEntity(raycastResult.hitComp))
		{
			// If the laser has hit a damageable target and isn't piercing, we can just damage it and move on
			ecs::CompHandle<HealthComponent> healthComp = hitEntity->GetComp<HealthComponent>();
			if (healthComp!=nullptr && !isPiercing)
			{
				healthComp->TakeDamage(damage);
			}
			else
			{
				// Create a new laser and fire
				ecs::EntityHandle newLaser = PrefabManager::LoadPrefab(isPlayerLaser ? "Laser" : "LaserEnemy");

				// Get the new laser's fire direction through reflection
				Vector2 normal = raycastResult.collisionNormal;
				float k = direction.Dot(normal);
				Vector2 newDirection = direction - 2.0f * k * normal;

				// Perform firing sequence
				ecs::CompHandle<LaserComponent> laserComp = newLaser->GetComp<LaserComponent>();
				laserComp->isPiercing = isPiercing;

				// Minus one pixel from the impact point to prevent the new raycast from colliding
				laserComp->Fire(distance, raycastResult.collisionPoint - direction, newDirection, 1.0f, damage, isPlayerLaser, bounceCount);
			}
		}
	}


#pragma endregion
	//// Translate the object (+sprite) to the midpoint and scale accordingly
	//thisEntity->GetTransform().SetWorldPosition(origin+(direction*raycastResult.distance / 2.0f));
	////thisEntity->GetTransform().SetWorldPosition((raycastResult.collisionPoint + origin) / 2.0f);
	//thisEntity->GetTransform().SetWorldRotation(math::ToDegrees(atan2(direction.y,direction.x)));
	//Vector2 scale = thisEntity->GetTransform().GetWorldScale();
	//scale.x = raycastResult.distance;
	//thisEntity->GetTransform().SetWorldScale(scale);

}
#ifdef IMGUI_ENABLED
void LaserComponent::EditorDraw(LaserComponent& comp)
{
	UNREFERENCED_PARAMETER(comp);
}

#endif

BossLaserComponent::BossLaserComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	maxLifeTime { 0.6f }
	, lifeTime { maxLifeTime }
	, damage { 10 }
	, hitPlayer { false }
	, trashLaser { false }
{
}
void BossLaserComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &BossLaserComponent::OnCollision);

}
void BossLaserComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &BossLaserComponent::OnCollision);
}
#ifdef IMGUI_ENABLED
void BossLaserComponent::EditorDraw(BossLaserComponent& comp)
{
	UNREFERENCED_PARAMETER(comp);
}
#endif

void BossLaserComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	HealthComponent* h = ecs::GetEntity(collisionData.otherComp)->GetComp<HealthComponent>();
	if (h)
	{
		if (!hitPlayer)
		{
			h->TakeDamage(damage);
			hitPlayer = true;
		}
	}
}

void BossLaserComponent::UpdateLifeTime()
{
	if (trashLaser)
		return;

	lifeTime -= GameTime::FixedDt();
	if (lifeTime <= 0.0f)
	{
		trashLaser = true;
	}
}

float BossLaserComponent::GetMaxLifeTime() const
{
	return maxLifeTime;
}

float BossLaserComponent::GetLifeTime() const
{
	return lifeTime;
}

void BossLaserComponent::SetDamage(int value)
{
	damage = value;
}

int BossLaserComponent::GetDamage() const
{
	return damage;
}

void BossLaserComponent::SetLaserLength(float value)
{
	Vector2 sca = ecs::GetEntity(this)->GetTransform().GetWorldScale();
	sca.y = value;
	ecs::GetEntity(this)->GetTransform().SetWorldScale(sca);
}

bool BossLaserComponent::GetTrashLaser() const
{
	return trashLaser;
}

BossLaserRendererSystem::BossLaserRendererSystem() :
	System_Internal{ &BossLaserRendererSystem::UpdateLaserComp }
{
}

void BossLaserRendererSystem::UpdateLaserComp(BossLaserComponent& comp)
{
	ecs::EntityHandle laserEntity{ ecs::GetEntity(&comp) };
	// float dt = GameTime::FixedDt();

	Vector4 laserColor = laserEntity->GetComp<RenderComponent>()->GetColor();
	laserColor.w = comp.GetLifeTime() / comp.GetMaxLifeTime();
	laserEntity->GetComp<RenderComponent>()->SetColor(laserColor);

	comp.UpdateLifeTime();
	if (comp.GetTrashLaser())
	{
		ecs::DeleteEntity(laserEntity);
	}
}
