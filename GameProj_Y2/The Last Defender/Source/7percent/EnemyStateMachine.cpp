/******************************************************************************/
/*!
\file   EnemyStateMachine.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   13/03/2025

\author Chua Wen Shing Bryan (50%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\author Kendrick Sim Hean Guan (50%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief

	Function definations for Enemy Ai system using the state machine interface written by
	Kendrick Sim Hean Guan.


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "EnemyStateMachine.h"
#include "ArmPivot.h"
#include "Weapon.h"
#include "AudioManager.h"
#include "AnimatorComponent.h"
#include "NameComponent.h"
#include "GameManager.h"

namespace sm {

	void ActivityEnemyPatrol::OnEnter(StateMachine* sm)
	{
		ecs::EntityHandle enemy{ CastSM(sm)->GetEntity() };
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ enemy->GetComp<EnemyControllerComponent>() };

		// Reset gun fire timing
		enemyComp->ResetFireCycle();

		// Level the pivot with the ground
		if (ecs::CompHandle<ArmPivotComponent> pivotComp{ enemy->GetCompInChildren<ArmPivotComponent>() })
			pivotComp->RotateTowards(ecs::GetEntityTransform(pivotComp).GetWorldPosition() + enemy->GetComp<EnemyControllerComponent>()->GetViewDirection());

		// Ensure sprite animation is playing
		if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
			animComp->Play();
	}

	void ActivityEnemyPatrol::OnUpdate(StateMachine* sm)
	{
		ecs::EntityHandle enemy{ CastSM(sm)->GetEntity() };
		Transform& enemyTransform{ enemy->GetTransform() };
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ enemy->GetComp<EnemyControllerComponent>() };

		// Switch direction once we've run out of our stride
		if (enemyComp->GetRemainingPatrolDistance() <= 0.0f)
			FlipDirectionAndResetDistance(*enemyComp);

		// Check if we've hit a wall. If so, switch directions
		if (enemyComp->HasReachedWall())
			FlipDirectionAndResetDistance(*enemyComp);

		// Check if there is still space in front of us to walk. If not, switch directions.
		if (enemyComp->HasReachedLedge())
			FlipDirectionAndResetDistance(*enemyComp);

		// Movement
		float moveDistance{ enemyComp->GetSpeed() * GameTime::FixedDt() };
		enemyComp->ConsumePatrolDistance(moveDistance);
		enemyTransform.AddWorldPosition(enemyComp->GetViewDirection() * moveDistance);
	}

	void ActivityEnemyPatrol::FlipDirectionAndResetDistance(EnemyControllerComponent& enemyComp)
	{
		enemyComp.ResetPatrolDistance();
		enemyComp.FlipViewDirection();
	}

	void ActivityEnemyStunned::OnUpdate(StateMachine* sm)
	{
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()};
		enemyComp->StunTick(GameTime::FixedDt());
	}

	void ActivityEnemyDetectPlayer::OnUpdate(StateMachine* sm)
	{
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>() };

		// Keep facing the player and getting close to them
		auto playerCompIter{ ecs::GetCompsActiveBeginConst<PlayerComponent>() };
		if (playerCompIter == ecs::GetCompsEndConst<PlayerComponent>())
		{
			// No player in the scene. Decrease detection level.
			enemyComp->DetectionTick(GameTime::FixedDt(), false);
			return;
		}
		Vector2 playerPos{ playerCompIter.GetEntity()->GetTransform().GetWorldPosition() };

		// Get whether the player is within LOS
		bool isInLOS{ enemyComp->IsPlayerInLOS() };

		// Look at and chase the player
		if (!enemyComp->HasReachedLedge())
			PerformChase(enemyComp, playerPos, false);
		else if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
		{
			animComp->Pause();
			animComp->Reset();
		}

		if (isInLOS)
			UpdateArmPivot(enemyComp, playerPos);

		// Update detection level
		enemyComp->DetectionTick(GameTime::FixedDt(), isInLOS);
	}
	
	void ActivityEnemyAttackPlayer::OnUpdate(StateMachine* sm)
	{
		auto playerCompIter{ ecs::GetCompsActiveBeginConst<PlayerComponent>() };
		if (playerCompIter == ecs::GetCompsEndConst<PlayerComponent>())
			return;
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>() };
		Vector2 playerPos = playerCompIter.GetEntity()->GetTransform().GetWorldPosition();

		// Check if we need to dodge.
		UpdateNeedToDodge(enemyComp, playerPos);

		// Move the enemy according to distance from player.
		if (dodgingToOtherSide)
			PerformDodging(enemyComp, playerPos);
		else
			PerformChase(enemyComp, playerPos);

		// Update arm pivot to point towards the player.
		UpdateArmPivot(enemyComp, playerPos);

		// Attempt to shoot at the player.
		enemyComp->AttemptFire(GameTime::FixedDt());
	}

	void ActivityEnemyAttackPlayer::UpdateNeedToDodge(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& playerPos)
	{
		// Don't need checking if we're already dodging.
		if (dodgingToOtherSide)
			return;

		Transform& enemyTransform{ ecs::GetEntityTransform(enemyComp) };
		float distXToPlayer{ playerPos.x - enemyTransform.GetWorldPosition().x };
		dodgingToOtherSide = std::fabsf(distXToPlayer) < EnemyControllerComponent::dodgeThreshold;

		// Divide-by-zero check
		if (std::fabsf(distXToPlayer) == 0.0f)
			return;

		// If we're initiating a dodge, set the dodge direction to the direction towards the player.
		if (dodgingToOtherSide)
			dodgeDirection = { distXToPlayer / std::fabsf(distXToPlayer), 0.0f };
	}

	void ActivityEnemyAttackPlayer::PerformDodging(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& playerPos)
	{
		// Move the enemy at an increased speed in the direction of the dodge.
		Transform& enemyTransform{ ecs::GetEntityTransform(enemyComp) };
		enemyTransform.AddWorldPosition(dodgeDirection * enemyComp->GetSpeed() * GameTime::FixedDt() * 2.0f);

		// Stop dodging if we've moved far enough from the player.
		float distXToPlayer{ playerPos.x - enemyTransform.GetWorldPosition().x };
		if (std::fabsf(distXToPlayer) > EnemyControllerComponent::dodgeThreshold)
			dodgingToOtherSide = false;

		// Force animation to play
		if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
			animComp->Play();
	}

	void ActivityEnemyAttackObjective::OnUpdate(StateMachine* sm)
	{
		auto objectiveCompIter{ ecs::GetCompsBegin<MainObjectiveComponent>() };
		if (objectiveCompIter == ecs::GetCompsEnd<MainObjectiveComponent>())
			return;

		ecs::CompHandle<EnemyControllerComponent> enemyComp{ CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>() };
		Vector2 objectivePos{ objectiveCompIter.GetEntity()->GetTransform().GetWorldPosition() };

		// Get closer to the objective if required
		PerformChase(enemyComp, objectivePos);

		// Aim and shoot at the objective
		UpdateArmPivot(enemyComp, objectivePos);
		enemyComp->AttemptFire(GameTime::FixedDt());
	}

	void ActivityEnemySuicideChase::OnUpdate(StateMachine* sm)
	{
		ecs::EntityHandle enemy{ CastSM(sm)->GetEntity() };
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ enemy->GetComp<EnemyControllerComponent>() };

		// Chase the player
		auto playerCompIter{ ecs::GetCompsActiveBeginConst<PlayerComponent>() };
		if (playerCompIter == ecs::GetCompsEndConst<PlayerComponent>())
			return;
		Vector2 playerPos{ playerCompIter.GetEntity()->GetTransform().GetWorldPosition() };
		PerformSuicideChase(enemyComp, playerPos, true);
		UpdateArmPivot(enemyComp, {playerPos.x, 1}); //Suicide enemies should stick their hands up when chasing

		// Following previous implementation: Prime the bomb
		if (ecs::CompHandle<EnemyBombComponent> bomb{ enemy->GetCompInChildren<EnemyBombComponent>() })
			if (!bomb->GetBombPrimed())
				bomb->SetBombPrimed(true);
	}

	bool TransitionIfEnemyIsType::Decide(StateMachine* sm)
	{
		return CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()->GetRole() == role;
	}

	bool TransitionEnemyPlayerInLOS::Decide(StateMachine* sm)
	{
		return CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()->IsPlayerInLOS() == checkForInLOS;
	}

	bool TransitionEnemyPlayerTooClose::Decide(StateMachine* sm)
	{
		ecs::CompHandle<EnemyControllerComponent> enemyComp{ CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>() };

		if (!enemyComp->IsPlayerInLOS())
			return false;

		if (enemyComp->GetSquaredDistanceToPlayer() < enemyComp->uniqueMinMaintainDistance * enemyComp->uniqueMinMaintainDistance)
		{
			// Let's force the enemy's detection level to max when the player gets too close, and let them shoot immediately
			enemyComp->ModifyDetection(1.0f);
			enemyComp->cycleTimeRemaining = 0.0f;
			return true;
		}
		else
			return false;
	}

	bool TransitionEnemyNoSuspicion::Decide(StateMachine* sm)
	{
		return CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()->GetDetectionLevel() <= 0.0f;
	}

	bool TransitionEnemyFullyAlerted::Decide(StateMachine* sm)
	{
		return CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()->GetDetectionLevel() >= 1.0f;
	}

	bool TransitionEnemyStunned::Decide(StateMachine* sm)
	{
		return CastSM(sm)->GetEntity()->GetComp<EnemyControllerComponent>()->GetIsStunned() == checkIsStunned;
	}


	//Start with this state
	StateDetermineRole::StateDetermineRole()
		:State{ {},
			  { new TransitionIfEnemyIsType{ ENEMY_ROLE::PATROL, SET_NEXT_STATE(StateEnemyPatrol) },
				new TransitionIfEnemyIsType{ ENEMY_ROLE::WAVE, SET_NEXT_STATE(StateEnemyAttackObjective) },
				new TransitionIfEnemyIsType{ ENEMY_ROLE::SUICIDE, SET_NEXT_STATE(StateEnemySuicidePatrol) }}
		}
	{}

	StateEnemyPatrol::StateEnemyPatrol()
		:State{ { new ActivityEnemyPatrol{} },
			  { new TransitionEnemyStunned{ true, SET_NEXT_STATE(StateEnemyStunned) },
				new TransitionEnemyPlayerInLOS{ true, SET_NEXT_STATE(StateEnemySearchPlayer) }}
		}
	{}

	StateEnemySearchPlayer::StateEnemySearchPlayer()
		:State{ { new ActivityEnemyDetectPlayer{} },
			  { new TransitionEnemyStunned{ true, SET_NEXT_STATE(StateEnemyStunned)},
				new TransitionEnemyFullyAlerted{ SET_NEXT_STATE(StateEnemyAttackPlayer) },
				new TransitionEnemyPlayerTooClose{ SET_NEXT_STATE(StateEnemyAttackPlayer) },
				new TransitionEnemyNoSuspicion{ SET_NEXT_STATE(StateEnemyPatrol) }}
		}
	{}

	StateEnemySuicideSearchPlayer::StateEnemySuicideSearchPlayer()
		:State{ { new ActivityEnemyDetectPlayer{} },
			  { new TransitionEnemyFullyAlerted{ SET_NEXT_STATE(StateEnemySuicideChase) },
				new TransitionEnemyPlayerTooClose{ SET_NEXT_STATE(StateEnemySuicideChase) },
				new TransitionEnemyNoSuspicion{ SET_NEXT_STATE(StateEnemySuicidePatrol) }}
		}
	{}

	StateEnemyAttackPlayer::StateEnemyAttackPlayer()
		:State{ { new ActivityEnemyAttackPlayer{} },
			  { //new TransitionEnemyStunned{ true, SET_NEXT_STATE(StateEnemyStunned) },
				new TransitionEnemyPlayerInLOS{ false, SET_NEXT_STATE(StateEnemySearchPlayer) }}
		}
	{}
	
	StateEnemyStunned::StateEnemyStunned()
		:State{ { new ActivityEnemyStunned{} },
			  { new TransitionEnemyStunned{ false, SET_NEXT_STATE(StateEnemyUnstunned) },
				new TransitionEnemyFullyAlerted{ SET_NEXT_STATE(StateEnemyAttackPlayer) }}
		}
	{}

	StateEnemyUnstunned::StateEnemyUnstunned()
		:State{ {},
			  {	new TransitionEnemyFullyAlerted{ SET_NEXT_STATE(StateEnemyAttackPlayer) },
				new TransitionEnemyNoSuspicion{ SET_NEXT_STATE(StateEnemyPatrol) },
				new TransitionAlways{ SET_NEXT_STATE(StateEnemySearchPlayer) }}
		}
	{}

	StateEnemyAttackObjective::StateEnemyAttackObjective()
		:State{ { new ActivityEnemyAttackObjective{} },
			  { new TransitionEnemyFullyAlerted{ SET_NEXT_STATE(StateEnemyAttackPlayer) },
				new TransitionEnemyPlayerTooClose{ SET_NEXT_STATE(StateEnemyAttackPlayer) }}
		}

	{}

	StateEnemySuicidePatrol::StateEnemySuicidePatrol()
		:State{ { new ActivityEnemyPatrol{} },
			  { new TransitionEnemyPlayerInLOS{ true, SET_NEXT_STATE(StateEnemySuicideSearchPlayer) }}
		}
	{}

	StateEnemySuicideChase::StateEnemySuicideChase()
		:State{ { new ActivityEnemySuicideChase{} },
			  {}
		}
	{}
}

//Init the starting initial state
//Enemy will start moving left when spawned
//Enemy will be set to patrol duty by default 
EnemyControllerComponent::EnemyControllerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	sm{ new sm::StateDetermineRole{} },
	isInited{},
	playerIsInLOS{},
	calculatedPlayerLOS{},
	remainingDistanceToPatrol{ distanceToPatrol },
	uniqueMinMaintainDistance{ minMaintainDistance * util::RandomRangeFloat(0.90f, 1.2f) },
	uniqueMaxMaintainDistance{ maxMaintainDistance * util::RandomRangeFloat(0.85f, 1.4f) },
	detectionLevel{},
	stunTimeRemaining{},
	entitySprite{},
	enemyRole(static_cast<int>(ENEMY_ROLE::PATROL)),
	speed{ 150.0f },
	weaponPrefabName{ "Weapon_Pistol" },
	enemyWeaponSpawned(false),
	cycleTimeBase(1.0f),
	cycleTimeBurst(0.1f),
	burstLength(5),
	cycleTimeRemaining{ cycleTimeBase },
	audioDeath{ "S" },
	burstRemaining(burstLength)
{
	
}

void EnemyControllerComponent::Init()
{
	if (isInited)
		return;
	isInited = true;

	entitySprite = util::name::GetEntityWithNameInChildren(ecs::GetEntity(this), "EnemySprite");
}

const AudioReference& EnemyControllerComponent::GetAudioDeath() const { return audioDeath; }

void EnemyControllerComponent::SetWeapon(const std::string& inWeaponPrefabName)
{
	weaponPrefabName = inWeaponPrefabName;
}

void EnemyControllerComponent::UpdateEnemyStateMachine()
{
	calculatedPlayerLOS = false;
	sm.Update(ecs::GetEntity(this));
}

float EnemyControllerComponent::GetSquaredDistanceToPlayer() const
{
	auto playerCompIter{ ecs::GetCompsActiveBegin<PlayerComponent>() };
	if (playerCompIter == ecs::GetCompsEnd<PlayerComponent>())
		return std::numeric_limits<float>::max();
	return (playerCompIter.GetEntity()->GetTransform().GetWorldPosition() - ecs::GetEntityTransform(this).GetWorldPosition()).LengthSquared();
}

bool EnemyControllerComponent::IsPlayerInLOS()
{
	if (calculatedPlayerLOS)
		return playerIsInLOS;
	calculatedPlayerLOS = true;
	playerIsInLOS = false;

	// Get the player
	auto playerCompIter{ ecs::GetCompsActiveBegin<PlayerComponent>() };
	if (playerCompIter == ecs::GetCompsEnd<PlayerComponent>())
		// There is no player in the scene.
		return false;

	Vector2 enemyPosition{ ecs::GetEntityTransform(this).GetWorldPosition() };
	Vector2 playerPosition{ playerCompIter.GetEntity()->GetTransform().GetWorldPosition() };

	// Check whether player is within LOS
	Vector2 posDiff{ playerPosition - enemyPosition };
	Vector2 positivePosDiff{ Abs(posDiff) };
	if (positivePosDiff.x > enemyLOSWidth || positivePosDiff.y > enemyLOSHeight)
		return false;
	// Check whether enemy is facing the player
	if (Dot(posDiff, GetViewDirection()) < 0.0f)
		return false;

	// Raycast to player to check for blocking obstacles
	Physics::RaycastResult raycastResult{};
	if (!Physics::Raycast(enemyPosition, posDiff, { ENTITY_LAYER::ENVIRONMENT, ENTITY_LAYER::PLAYER }, &raycastResult))
		// In theory this should never happen, we should at least hit the player... but just in case
		return false;
	playerIsInLOS = ecs::GetEntity(raycastResult.hitComp)->HasComp<PlayerComponent>();
	return playerIsInLOS;
}

Vector2 EnemyControllerComponent::GetViewDirection() const
{
	// TODO: This is slow. Should initialize once at startup.
	if (ecs::ConstEntityHandle flipper{ util::name::GetEntityWithNameInChildren(ecs::GetEntity(this), "Flipper") })
		return { flipper->GetTransform().GetLocalScale().x, 0.0f };
	else
	{
		CONSOLE_LOG(LEVEL_WARNING) << "No \"Flipper\" inside Enemy entity!";
		return { 1.0f, 0.0f };
	}
}

void EnemyControllerComponent::FlipViewDirection()
{
	// TODO: This is slow. Should initialize once at startup.
	if (ecs::EntityHandle flipper{ util::name::GetEntityWithNameInChildren(ecs::GetEntity(this), "Flipper") })
		flipper->GetTransform().SetLocalScale({ -flipper->GetTransform().GetLocalScale().x, 1.0f});
	else
		CONSOLE_LOG(LEVEL_WARNING) << "No \"Flipper\" inside Enemy entity!";
}

bool EnemyControllerComponent::HasReachedWall() const
{
	const Transform& transform{ ecs::GetEntityTransform(this) };
	Physics::RaycastResult raycastResult{};
	if (Physics::Raycast(transform.GetWorldPosition(), GetViewDirection(), { ENTITY_LAYER::ENVIRONMENT }, &raycastResult))
		return raycastResult.distance - transform.GetWorldScale().x * 0.25f < 2.0f; // tolerance
	return false;
}

bool EnemyControllerComponent::HasReachedLedge() const
{
	const Transform& transform{ ecs::GetEntityTransform(this) };
	Vector2 scale{ transform.GetWorldScale() };
	Vector2 rayOrigin{ transform.GetWorldPosition() + Vector2{ scale.x * GetViewDirection().x / 3.0f, 2.0f - scale.y * 0.5f}};
	Physics::RaycastResult raycastResult{};
	if (Physics::Raycast(rayOrigin, { 0.0f, -1.0f }, { ENTITY_LAYER::ENVIRONMENT }, &raycastResult))
		return raycastResult.distance >= 22.0f; // tolerance
	return false;
}

float EnemyControllerComponent::GetPatrolStrideLength() const
{
	return distanceToPatrol;
}
float EnemyControllerComponent::GetRemainingPatrolDistance() const
{
	return remainingDistanceToPatrol;
}
void EnemyControllerComponent::ResetPatrolDistance()
{
	remainingDistanceToPatrol = distanceToPatrol * util::RandomRangeFloat(0.85f, 1.3f);
}
bool EnemyControllerComponent::ConsumePatrolDistance(float amt)
{
	remainingDistanceToPatrol -= amt;
	return remainingDistanceToPatrol > 0.0f;
}
float EnemyControllerComponent::GetSpeed() const
{
	return speed;
}
void EnemyControllerComponent::SetSpeed(float inSpeed)
{
	speed = inSpeed;
}

void EnemyControllerComponent::DetectionTick(float dt, bool increase)
{
	ModifyDetection(dt * detectionSpeed * (increase ? 1.0f : -0.5f));
}
float EnemyControllerComponent::GetDetectionLevel() const
{
	return detectionLevel;
}

void EnemyControllerComponent::ApplyStun()
{
	stunTimeRemaining = stunLength;
}

void EnemyControllerComponent::StunTick(float dt)
{
	stunTimeRemaining -= dt;
}

bool EnemyControllerComponent::GetIsStunned() const
{
	return stunTimeRemaining > 0.0f;
}

void EnemyControllerComponent::OnHitByPlayer(bool isBackshot, float damage)
{
	ApplyStun();
	ModifyDetection(0.7f * damage / 30.0f);
	ReduceFireCycleTime(0.5f * damage / 25.0f);

	if (isBackshot)
		FlipViewDirection();
}

ENEMY_ROLE EnemyControllerComponent::GetRole() const
{
	return static_cast<ENEMY_ROLE>(enemyRole);
}

void EnemyControllerComponent::AttemptFire(float dt)
{
	// Can't fire if the cycle time isn't over yet
	cycleTimeRemaining -= dt;
	if (cycleTimeRemaining > 0.0f)
		return;
	
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	if (ecs::CompHandle<WeaponComponent> weaponComp{ thisEntity->GetCompInChildren<WeaponComponent>() })
	{
		weaponComp->ammoCurr = 500; //For testing gave enemy infinte ammmo

		// Disable weapon reload (put before firing because Fire() auto reloads...)
		if (weaponComp->magazineCurr <= 1)
			weaponComp->magazineCurr = weaponComp->stats.magazineSize + 1;

		// Every time we fire the weapon, set time time based on whether we're in a burst
		weaponComp->Fire();
		//cycleTimeRemaining += GetRandomCycleTime();
		if (--burstRemaining <= 0)
		{
			burstRemaining = burstLength;
			cycleTimeRemaining += cycleTimeBase;
		}
		else
			cycleTimeRemaining += cycleTimeBurst;
	}
}

void EnemyControllerComponent::ReduceFireCycleTime(float amt)
{
	cycleTimeRemaining -= amt;
}

void EnemyControllerComponent::ResetFireCycle()
{
	//cycleTimeRemaining = GetRandomCycleTime();
	cycleTimeRemaining = cycleTimeBase;
	burstRemaining = burstLength;
}

void EnemyControllerComponent::OnStart()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("Died", this, &EnemyControllerComponent::OnDead);
}

void EnemyControllerComponent::OnDetached()
{
	if (ecs::CompHandle<EntityEventsComponent> eventComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventComp->Unsubscribe("Died", this, &EnemyControllerComponent::OnDead);
}

void EnemyControllerComponent::OnDead()
{
	ecs::EntityHandle enemy{ ecs::GetEntity(this) };

	// Drop gun
	switch (static_cast<ENEMY_ROLE>(enemyRole))
	{
	case ENEMY_ROLE::SUICIDE:
		//DropBomb(); break; dont drop bomb for now
	default:
		DropWeapon(); break;
	}

	// Increment GameManager counter
	ST<GameManager>::Get()->OnEnemyKilled();

	// Hide pivot arm
	if (ecs::CompHandle<ArmPivotComponent> pivotComp{ enemy->GetCompInChildren<ArmPivotComponent>() })
		ecs::GetEntity(pivotComp)->SetActive(false);

	// Disable physics and collision
	enemy->RemoveComp<Physics::ColliderComp>();
	enemy->RemoveComp<Physics::PhysicsComp>();

	// Play the death sound and delete this enemy
	ST<AudioManager>::Get()->StartSound(GetAudioDeath());
	ecs::DeleteEntity(enemy);
}

#ifdef IMGUI_ENABLED
void EnemyControllerComponent::EditorDraw(EnemyControllerComponent& comp)
{
	{ // Enemy Role
#define X(enumName, strName) strName,
		static const char* enemyRoles[] = { M_ENEMY_ROLE };
#undef X
		gui::Combo roleCombo{ "Role", enemyRoles, std::size(enemyRoles), &comp.enemyRole };
	}

	// Gun
	switch (static_cast<ENEMY_ROLE>(comp.enemyRole))
	{
	case ENEMY_ROLE::PATROL:
		if (gui::Combo gunCombo{ "Gun", comp.weaponPrefabName.c_str() })
			for (const std::string& prefabName : PrefabManager::AllPrefabs())
				if (prefabName.starts_with("Weapon_"))
					if (gunCombo.Selectable(prefabName.c_str(), prefabName == comp.weaponPrefabName))
						comp.weaponPrefabName = prefabName;
		break;
	}

	gui::VarDrag("Speed", &comp.speed);

	comp.audioDeath.EditorDraw("Death Sound");
}
#endif

EntityReference& EnemyControllerComponent::CurrentItem()
{
	return heldItem;
}

float EnemyControllerComponent::GetRandomCycleTime() const
{
	return cycleTimeBase + util::RandomRangeFloat(cycleTimeBase * -rpmDeviationFactor, cycleTimeBase * rpmDeviationFactor);
}

void EnemyControllerComponent::ModifyDetection(float amt)
{
	detectionLevel = math::Clamp(detectionLevel + amt, 0.0f, 1.0f);

	if (ecs::CompHandle<DetectionIndicatorComponent> detectIndicateComp{ ecs::GetEntity(this)->GetCompInChildren<DetectionIndicatorComponent>() })
		detectIndicateComp->UpdateIndicator(detectionLevel);
}

void EnemyControllerComponent::SpawnWeapon()
{
	ecs::EntityHandle newWeapon = PrefabManager::LoadPrefab(weaponPrefabName);
	if (!newWeapon)
	{
		CONSOLE_LOG(LEVEL_ERROR) << "Enemy spawned with invalid weapon prefab: " << weaponPrefabName;
		return;
	}

	CurrentItem() = newWeapon;

	//If weapon comp exist
	if (ecs::CompHandle<WeaponComponent> weaponComp{ newWeapon->GetComp<WeaponComponent>() })
	{
		weaponComp->isHeld = true;
		weaponComp->isPlayerWeapon = false;

		//cycleTimeBase = 60.0f / weaponComp->stats.fireRPM * 7.0f;
		cycleTimeBase = weaponComp->stats.enemyCycleTimeBase;
		cycleTimeBurst = weaponComp->stats.enemyCycleTimeBurst;
		burstLength = weaponComp->stats.enemyBurstBullets;
		
		ResetFireCycle();
	}
		
}
void EnemyControllerComponent::DropWeapon()
{
	if (!CurrentItem().IsValidReference())
		return;
	ecs::EntityHandle weaponEntity{ CurrentItem() };

	// Attempt to get either a weapon or healthpack
	ecs::CompHandle<WeaponComponent> heldWeapon{ weaponEntity->GetComp<WeaponComponent>() };
	if (!heldWeapon)
		return;

	heldWeapon->OnDropped();

	heldWeapon->isPlayerWeapon = false;
	heldWeapon->isHeld = false;

	// Set layer back to WEAPON
	weaponEntity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::WEAPON);

	// Set component to dynamic
	if (ecs::CompHandle<Physics::PhysicsComp> physComp{ weaponEntity->GetComp<Physics::PhysicsComp>() })
	{
		physComp->SetFlag(Physics::PHYSICS_COMP_FLAG::IS_DYNAMIC, true);
		physComp->SetVelocity(GetViewDirection() * 60.0f + Vector2{ 0.0f, 150.0f });
	}

	// Reset transform
	weaponEntity->GetTransform().SetParent(nullptr);
	weaponEntity->GetTransform().SetWorldRotation(0.0f);

	// We aren't carrying this weapon anymore.
	CurrentItem() = nullptr;
}

void EnemyControllerComponent::DropBomb()
{
	if (ecs::EntityHandle thisEntity{ ecs::GetEntity(this) })
	{
		if (ecs::CompHandle<EnemyBombComponent> bombComp{ thisEntity->GetCompInChildren<EnemyBombComponent>() })
		{
			ecs::EntityHandle bombEntity{ ecs::GetEntity(bombComp) };
			// Set layer back to WEAPON
			bombEntity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::WEAPON);
	
			// Set component to dynamic
			if (ecs::CompHandle<Physics::PhysicsComp> physComp = bombEntity->GetComp<Physics::PhysicsComp>())
			{
				physComp->SetFlag(Physics::PHYSICS_COMP_FLAG::IS_DYNAMIC, true);
				physComp->SetIsDynamic(true);
				float scale = bombEntity->GetTransform().GetWorldScale().x;
				float rotation = bombEntity->GetTransform().GetWorldRotation();
				if (scale < 0)
					rotation = 180.0f - rotation;
				physComp->SetVelocity({ });
			
			}
			// Disown the child
			bombEntity->GetTransform().SetParent(nullptr);
		}
	}
}

EnemyControllerSystem::EnemyControllerSystem()
	: System_Internal{ &EnemyControllerSystem::UpdateEnemyControllerComp }
{
}

void EnemyControllerSystem::UpdateEnemyControllerComp(EnemyControllerComponent& comp)
{
	ecs::EntityHandle thisEntity{ ecs::GetEntity(&comp) };

	// Don't process enemies too far away from the player
	auto playerIter{ ecs::GetCompsBegin<PlayerComponent>() };
	if (playerIter != ecs::GetCompsEnd<PlayerComponent>())
	{
		Vector2 toPlayer{ playerIter.GetEntity()->GetTransform().GetWorldPosition() - thisEntity->GetTransform().GetWorldPosition() };
		if (std::fabs(toPlayer.x) > ST<GameSettings>::Get()->m_physicsSimulationSize * 0.5f || std::fabs(toPlayer.y) > ST<GameSettings>::Get()->m_physicsSimulationSize * 0.5f)
			return;
	}

	// Don't process dead enemies
	if (ecs::CompHandle<HealthComponent> healthComp{ thisEntity->GetComp<HealthComponent>() })
		if (healthComp->IsDead())
			return;

	if (!comp.isInited)
		comp.Init();

	comp.UpdateEnemyStateMachine();

	if (comp.GetRole() != ENEMY_ROLE::SUICIDE)
	{
		//Spawn random weapon once
		if (!comp.enemyWeaponSpawned)
		{
			comp.SpawnWeapon();
			ecs::EntityHandle weaponEntity{ comp.CurrentItem() };

			//Get the arm's entity for gun to attach to
			ecs::CompHandle<ArmPivotComponent> armComp{ thisEntity->GetCompInChildren<ArmPivotComponent>() };
			ecs::EntityHandle armEntity{ ecs::GetEntity(armComp) };

			//Place the weapon on the arms
			weaponEntity->GetTransform().SetParent(armEntity->GetTransform());
			comp.enemyWeaponSpawned = true;
		}

		//Weapon exists
		if (ecs::CompHandle<WeaponComponent> weapon{ thisEntity->GetCompInChildren<WeaponComponent>() })
		{

			// Held item logic
			if (comp.CurrentItem().IsValidReference())
			{
				comp.CurrentItem()->GetTransform().SetLocalPosition((comp.armHoldOffset + weapon->holdPointPixel));
				Vector2 wLocalScale = comp.CurrentItem()->GetTransform().GetLocalScale();
				if (wLocalScale.x < 0)
				{
					wLocalScale.x = -wLocalScale.x;
					comp.CurrentItem()->GetTransform().SetLocalScale(wLocalScale);
				}

				//set the weapon layer to default
				comp.CurrentItem()->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);

				if (ecs::CompHandle<Physics::PhysicsComp> physComp = comp.CurrentItem()->GetComp<Physics::PhysicsComp>())
				{
					physComp->SetFlag(Physics::PHYSICS_COMP_FLAG::IS_DYNAMIC, false);
				}

			}
		}
	}
}

void DetectionIndicatorComponent::OnStart()
{
	if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(this)->GetComp<RenderComponent>() })
		ecs::SetCompActive(renderComp, false);
}

void DetectionIndicatorComponent::UpdateIndicator(float currentDetection)
{
	ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(this)->GetComp<RenderComponent>() };
	if (!renderComp)
		return;

	if (currentDetection == 0.0f)
	{
		ecs::SetCompActive(renderComp, false);
		return;
	}
	else
		ecs::SetCompActive(renderComp, true);

	renderComp->SetSpriteID(ResourceManager::GetSpriteID(currentDetection == 1.0f ? "Exclamation_Mark" : "Question_Mark"));
}
