/******************************************************************************/
/*!
\file   EnemyStateMachine.h
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

	Header for Enemy Ai system using the state machine interface written by
	Kendrick Sim Hean Guan.


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "GameStateMachine.h"
#include "Player.h"
#include "ArmPivot.h"
#include "ObjectiveTimer.h"
#include "MainObjective.h"
#include "IGameComponentCallbacks.h"
#include "Collision.h"
#include "Physics.h"
#include "EnemyBomb.h"
#include "PrefabManager.h"

// Forward declarations
class EnemyControllerComponent;

#define M_ENEMY_ROLE \
X(PATROL, "Patrol") \
X(WAVE, "Wave") \
X(SUICIDE, "Suicide")

#define X(enumName, strName) enumName,
enum class ENEMY_ROLE : int
{
	M_ENEMY_ROLE
};
#undef X

// Base classes
namespace sm
{
	template <typename T>
	class ActivityEnemyAttackBase : public ECSActivityBase<T>
	{
	protected:
		/*****************************************************************//*!
		\brief
			Moves the enemy closer or further away from a location depending on distance.
		\param enemyComp
			The enemy component.
		\param targetLocation
			The target location's position.
		\param maintainMinDist
			Whether to maintain a minimum distance from the target.
		*//******************************************************************/
		void PerformChase(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation, bool maintainMinDist = true);

		void PerformSuicideChase(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation, bool maintainMinDist = true);

		/*****************************************************************//*!
		\brief
			Updates the enemy's arm pivot to point towards a target.
		\param enemyComp
			The enemy component.
		\param targetLocation
			The target's position.
		*//******************************************************************/
		void UpdateArmPivot(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation);
	};
}

namespace sm 
{
	/*****************************************************************//*!
	\class ActivityEnemyPatrol
	\brief
		Moves the enemy in a patrolling pattern.
	*//******************************************************************/
	class ActivityEnemyPatrol : public ECSActivityBase<ActivityEnemyPatrol>
	{
	public:
		/*****************************************************************//*!
		\brief
			In case we're returning from a state of alertness, reset the enemy's
			arm pivot to be level with the ground.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnEnter(StateMachine* sm) override;

		/*****************************************************************//*!
		\brief
			Controls an enemy's movement by mimicking patrolling.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;

	private:
		/*****************************************************************//*!
		\brief
			Flips an enemy's view direction and resets the remaining patrol distance.
		\param enemyComp
			The enemy component.
		*//******************************************************************/
		void FlipDirectionAndResetDistance(EnemyControllerComponent& enemyComp);

	};

	/*****************************************************************//*!
	\class ActivityEnemyAttackPlayer
	\brief
		Chases and shoots the player.
	*//******************************************************************/
	class ActivityEnemyAttackPlayer : public ActivityEnemyAttackBase<ActivityEnemyAttackPlayer>
	{
	public:
		/*****************************************************************//*!
		\brief
			Chases and shoots the player.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;

	private:
		/*****************************************************************//*!
		\brief
			Checks whether the distance to player has reached the threshold for
			dodging, and updates dodgingToOtherSide accordingly.
		\param enemyComp
			The enemy component.
		\param playerPos
			The player's position.
		*//******************************************************************/
		void UpdateNeedToDodge(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& playerPos);

		/*****************************************************************//*!
		\brief
			Moves the enemy towards the opposite side of the player at an
			increased speed. Used when the player comes too close to the enemy.
		\param enemyComp
			The enemy component.
		\param playerPos
			The player's position.
		*//******************************************************************/
		void PerformDodging(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& playerPos);

	private:
		//! Whether the enemy is maneuvering towards the player's back.
		bool dodgingToOtherSide;
		//! The dodge direction
		Vector2 dodgeDirection;

	};

	/*****************************************************************//*!
	\class ActivityEnemyAttackObjective
	\brief
		Shoots the objective.
	*//******************************************************************/
	class ActivityEnemyAttackObjective : public ActivityEnemyAttackBase<ActivityEnemyAttackObjective>
	{
	public:
		/*****************************************************************//*!
		\brief
			Shoots the objective.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;

	};

	class ActivityEnemySuicideChase : public ActivityEnemyAttackBase<ActivityEnemySuicideChase>
	{
	public:
		virtual void OnUpdate(StateMachine* sm) override;

	};

	/*****************************************************************//*!
	\class ActivityEnemyDetectPlayer
	\brief
		Increases/Decreases the detection level of the enemy.
	*//******************************************************************/
	class ActivityEnemyDetectPlayer : public ActivityEnemyAttackBase<ActivityEnemyDetectPlayer>
	{
	public:
		/*****************************************************************//*!
		\brief
			Checks whether the player is in the enemy's LOS and modifies the
			enemy's detection level accordingly.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;

	};

	/*****************************************************************//*!
	\class ActivityEnemyStunned
	\brief
		Decreases the stun timer of the enemy.
	*//******************************************************************/
	class ActivityEnemyStunned : public ECSActivityBase<ActivityEnemyStunned>
	{
	public:
		/*****************************************************************//*!
		\brief
			Decreases the stun timer of the enemy.
		\param sm
			The enemy's state machine.
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;

	};

	/*****************************************************************//*!
	\class TransitionEnemyPlayerInLOS
	\brief
		Transition - Player in LOS of enemy
	*//******************************************************************/
	class TransitionEnemyPlayerInLOS : public ECSTransitionBase<TransitionEnemyPlayerInLOS>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor.
		\param checkForInLOS
			Whether to check that the player is inside or outside LOS.
		*//******************************************************************/
		template <typename NextStateType>
		TransitionEnemyPlayerInLOS(bool checkForInLOS, const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Checks whether the player is inside or outside of LOS.
		\return
			True if the player matches the criteria as specified in the constructor.
			False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;

	private:
		//! Whether to check for whether the player is inside or outside of LOS
		bool checkForInLOS;
	};

	/*****************************************************************//*!
	\class TransitionEnemyPlayerInLOS
	\brief
		Transition - Player in LOS of enemy and is in dodging range of it
	*//******************************************************************/
	class TransitionEnemyPlayerTooClose : public ECSTransitionBase<TransitionEnemyPlayerTooClose>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor.
		*//******************************************************************/
		template <typename NextStateType>
		TransitionEnemyPlayerTooClose(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Checks whether the player is considered too close to the enemy.
		\return
			True if the player is too close to the enemy. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	/*****************************************************************//*!
	\class TransitionEnemyFullyAlerted
	\brief
		Transition - Detection level is max.
	*//******************************************************************/
	class TransitionEnemyFullyAlerted : public ECSTransitionBase<TransitionEnemyFullyAlerted>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionEnemyFullyAlerted(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Checks whether the enemy is fully alerted (detection level == 1.0f).
		\return
			True if the enemy is alerted. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	/*****************************************************************//*!
	\class TransitionEnemyNoSuspicion
	\brief
		Transition - Detection level is min.
	*//******************************************************************/
	class TransitionEnemyNoSuspicion : public ECSTransitionBase<TransitionEnemyNoSuspicion>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionEnemyNoSuspicion(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Checks whether the enemy has no detection (detection level == 0.0f).
		\return
			True if the enemy has no detection. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	/*****************************************************************//*!
	\class TransitionIfEnemyIsType
	\brief
		Transition - If an enemy is of the specified type.
	*//******************************************************************/
	class TransitionIfEnemyIsType : public ECSTransitionBase<TransitionIfEnemyIsType>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		\param role
			The role of the enemy to match.
		*//******************************************************************/
		template <typename NextStateType>
		TransitionIfEnemyIsType(ENEMY_ROLE role, const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to apply this transition.
		\return
			True when the state machine should change to this transition's stored
			state. False otherwise.
		*//******************************************************************/
		bool Decide(StateMachine* sm) override;

	private:
		//! The role that the enemy needs to be for this transition to succeed.
		ENEMY_ROLE role;
	};

	/*****************************************************************//*!
	\class TransitionEnemyStunned
	\brief
		Transition - If the enemy is/is not stunned.
	*//******************************************************************/
	class TransitionEnemyStunned : public ECSTransitionBase<TransitionEnemyStunned>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		\param checkIsStunned
			Whether to check if the enemy is or is not stunned.
		*//******************************************************************/
		template <typename NextStateType>
		TransitionEnemyStunned(bool checkIsStunned, const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to apply this transition.
		\return
			True when the state machine should change to this transition's stored
			state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;

	private:
		//! Whether to check whether the enemy is or is not stunned.
		bool checkIsStunned;
	};

	/*****************************************************************//*!
	\class StateDetermineRole
	\brief
		Switches to the appropriate state depending on the type of enemy.
	*//******************************************************************/
	class StateDetermineRole : public State
	{
	public:
		StateDetermineRole();
	};

	/*****************************************************************//*!
	\class StateEnemyAttackPlayer
	\brief
		Chases and attacks the player.
	*//******************************************************************/
	class StateEnemyAttackPlayer : public State
	{
	public:
		StateEnemyAttackPlayer();
	};

	/*****************************************************************//*!
	\brief
		When EnemyControllerSystem's "objectiveStarted" == true 
		and when player is not in LOS of enemy

	*//******************************************************************/
	class StateEnemyAttackObjective : public State
	{
	public:
		StateEnemyAttackObjective();
	};

	/*****************************************************************//*!
	\brief
		Start with this state. 
		Enemies with patrolDuty == true will NEVER transition to attack objective 

	*//******************************************************************/
	class StateEnemyPatrol : public State
	{
	public:
		StateEnemyPatrol();
	};


	/*****************************************************************//*!
	\brief
		Sucide enemies will transition into this state

	*//******************************************************************/
	class StateEnemySuicidePatrol : public State
	{
	public:
		StateEnemySuicidePatrol();
	};

	/*****************************************************************//*!
	\brief
		Enemies in this state will never transition back into patrolling

	*//******************************************************************/
	class StateEnemySuicideChase : public State
	{
	public:
		StateEnemySuicideChase();
	};

	/*****************************************************************//*!
	\brief
		Enemies in this state will enter a state where they will scan for
		player infront of them

	*//******************************************************************/
	class StateEnemySearchPlayer : public State
	{
	public:
		StateEnemySearchPlayer();
	};

	/*****************************************************************//*!
	\brief
		Enemies in this state will enter a state where they will scan for
		player infront of them

	*//******************************************************************/
	class StateEnemySuicideSearchPlayer : public State
	{
	public:
		StateEnemySuicideSearchPlayer();
	};
	
	/*****************************************************************//*!
	\brief
		Enemies in this state gets stunned temporaily

	*//******************************************************************/
	class StateEnemyStunned : public State
	{
	public:
		StateEnemyStunned();
	};

	/*****************************************************************//*!
	\brief
		Enemies in this state will prepare to transition back to their actions
		prior to being stunned

	*//******************************************************************/
	class StateEnemyUnstunned : public State
	{
	public:
		StateEnemyUnstunned();
	};
}

class EnemyControllerComponent
	: public IRegisteredComponent<EnemyControllerComponent>
	, public IGameComponentCallbacks<EnemyControllerComponent>
#ifdef IMGUI_ENABLED
	, public IEditorComponent<EnemyControllerComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Constructor

	*//******************************************************************/
	EnemyControllerComponent();

	/*****************************************************************//*!
	\brief
		Initializes the enemy component.
	*//******************************************************************/
	void Init();

	/*****************************************************************//*!
	\brief
		Updates state machine each frame
	*//******************************************************************/
	void UpdateEnemyStateMachine();

	/*****************************************************************//*!
	\brief
		Gets the squared distance to the player.
	\return
		The squared distance to the player. If the player doesn't exist,
		the max value of a float.
	*//******************************************************************/
	float GetSquaredDistanceToPlayer() const;

	/*****************************************************************//*!
	\brief
		Gets whether the player is within LOS of this enemy.
	\return
		True if the player is in LOS. False otherwise.
	*//******************************************************************/
	bool IsPlayerInLOS();

	/*****************************************************************//*!
	\brief
		Gets the direction that the enemy is facing.
	\return
		The direction that the enemy is facing.
	*//******************************************************************/
	Vector2 GetViewDirection() const;

	/*****************************************************************//*!
	\brief
		Flips the direction that the enemy is facing.
	*//******************************************************************/
	void FlipViewDirection();

	/*****************************************************************//*!
	\brief
		Gets whether this enemy has reached a wall.
	\return
		True if the enemy has reached a wall. False otherwise.
	*//******************************************************************/
	bool HasReachedWall() const;

	/*****************************************************************//*!
	\brief
		Gets whether this enemy has reached a ledge.
	\return
		True if the enemy has reached a ledge. False otherwise.
	*//******************************************************************/
	bool HasReachedLedge() const;

	/*****************************************************************//*!
	\brief
		Gets the distance that this enemy patrols in one direction.
	\return
		The distance that this enemy patrols in one direction.
	*//******************************************************************/
	float GetPatrolStrideLength() const;

	/*****************************************************************//*!
	\brief
		Gets the remaining patrol distance
	\return
		The remaining patrol distance.
	*//******************************************************************/
	float GetRemainingPatrolDistance() const;

	/*****************************************************************//*!
	\brief
		Resets the remaining patrol distance
	*//******************************************************************/
	void ResetPatrolDistance();

	/*****************************************************************//*!
	\brief
		Decreases the remaining patrol distance
	\param amt
		The amount of patrol distance remaining.
	\return
		True if there is distance still remaining. False otherwise.
	*//******************************************************************/
	bool ConsumePatrolDistance(float amt);

	/*****************************************************************//*!
	\brief
		Gets the movement speed of the enemy.
	\return
		The movement speed of the enemy.
	*//******************************************************************/
	float GetSpeed() const;

	/*****************************************************************//*!
	\brief
		Sets the movement speed of the enemy.
	\param inSpeed
		The new movement speed of the enemy.
	*//******************************************************************/
	void SetSpeed(float inSpeed);

	/*****************************************************************//*!
	\brief
		Increases/Decreases the detection level of the enemy.
	\param dt
		The delta time.
	\param increase
		Whether to increase or decrease the detection level of the enemy.
	*//******************************************************************/
	void DetectionTick(float dt, bool increase);

	/*****************************************************************//*!
	\brief
		Increases/Decreases the detection level of the enemy.
	\param amt
		The amount to increase/decrease the detection level by.
	*//******************************************************************/
	void ModifyDetection(float amt);

	/*****************************************************************//*!
	\brief
		Gets the detection level of the player.
	\return
		The current detection level of the player.
	*//******************************************************************/
	float GetDetectionLevel() const;

	/*****************************************************************//*!
	\brief
		Stuns the enemy.
	*//******************************************************************/
	void ApplyStun();

	/*****************************************************************//*!
	\brief
		Decreases the stun timer.
	*//******************************************************************/
	void StunTick(float dt);

	/*****************************************************************//*!
	\brief
		Gets whether the enemy is stunned.
	*//******************************************************************/
	bool GetIsStunned() const;

	/*****************************************************************//*!
	\brief
		Allows the enemy to react to getting hit.
	\param isBackshot
		Whether the hit came from behind the enemy.
	\param damage
		The amount of damage by this shot.
	*//******************************************************************/
	void OnHitByPlayer(bool isBackshot, float damage);

	/*****************************************************************//*!
	\brief
		Gets the role of the enemy.
	\return
		The role of the enemy
	*//******************************************************************/
	ENEMY_ROLE GetRole() const;

	/*****************************************************************//*!
	\brief
		Getter

	\return
		return Death Audio
	*//******************************************************************/
	const AudioReference& GetAudioDeath() const;

	/*****************************************************************//*!
	\brief
		Sets an enemy's weapon. Note: Only sets weapons before it spawns.
	\param inWeaponPrefabName
		The weapon prefab's name.
	*//******************************************************************/
	void SetWeapon(const std::string& inWeaponPrefabName);

	/*****************************************************************//*!
	\brief
		Spawns the weapon and sets it to enemy's hand

	\return
		void
	*//******************************************************************/
	void SpawnWeapon();

	/*****************************************************************//*!
	\brief
		Drops the weapon

	\return
		void
	*//******************************************************************/
	void DropWeapon();

	void DropBomb();

	/*****************************************************************//*!
	\brief
		Updates fire cycle time and fires a bullet if able.
	\param dt
		The delta time.
	*//******************************************************************/
	void AttemptFire(float dt);

	/*****************************************************************//*!
	\brief
		Reduces the time until the next weapon fire.
	\param amt
		The amount of time to reduce by.
	*//******************************************************************/
	void ReduceFireCycleTime(float amt);

	/*****************************************************************//*!
	\brief
		Resets the fire cycle time.
	*//******************************************************************/
	void ResetFireCycle();

	/*****************************************************************//*!
	\brief
		Randomizes cycle time and subscribes to death event.
	*//******************************************************************/
	void OnStart() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes from the death event.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Starts the enemy death process.
	*//******************************************************************/
	void OnDead();

	EntityReference& CurrentItem();
	
	//! Whether this component was initialized
	bool isInited;
	//! Is the player in view of this enemy?
	bool playerIsInLOS;
	//! Whether player LOS has been calculated this frame already.
	bool calculatedPlayerLOS;
	//! The remaining distance that this enemy can move before switching directions.
	float remainingDistanceToPatrol;
	//! This enemy's unique minimum maintain distance, so multiple enemies don't stack perfectly.
	float uniqueMinMaintainDistance;
	//! This enemy's unique maximum maintain distance, so multiple enemies don't stack perfectly.
	float uniqueMaxMaintainDistance;
	//! This enemy's current detection level
	float detectionLevel;
	//! The remaining stun time.
	float stunTimeRemaining;

	//! The maximum patrol stride per direction.
	static constexpr float distanceToPatrol{ 200.0f };
	//! The distance that the enemy can see along the X axis.
	static constexpr float enemyLOSWidth{ 450.0f };
	//! The distance that the enemy can see along the Y axis.
	static constexpr float enemyLOSHeight{ 450.0f };
	//! The speed that the detection level rises/falls
	static constexpr float detectionSpeed{ 1.0f };
	//! The factor by which gun RPM can deviate
	static constexpr float rpmDeviationFactor{ 0.2f };
	//! The length of time for which the enemy is stunned for
	static constexpr float stunLength{ 0.2f };
	//! If the enemy is closer than this distance from the player, the enemy will try to get away to the player.
	static constexpr float minMaintainDistance{ 100.0f };
	//! If the enemy is further than this distance from the player, the enemy will try to get closer to the player.
	static constexpr float maxMaintainDistance{ 150.0f };
	//! The distance to the player within which the enemy will start dodging to get behind the player.
	static constexpr float dodgeThreshold{ 50.0f };

	EntityReference armPivot; // unused for now... but prob should be used
	// Handle to the sprite entity.
	ecs::EntityHandle entitySprite;

	//! The role of the enemy.
	int enemyRole;
	//! The speed of the enemy.
	float speed;
	//! The weapon that this enemy will spawn with.
	std::string weaponPrefabName;

	float cycleTimeBase;
	float cycleTimeBurst;
	int burstLength;
	float cycleTimeRemaining;
	int burstRemaining;

	const Vector2 armHoldOffset{ 20.0f, -1.0f };
	EntityReference heldItem;

	int  currentHoldingItem = 0;
	bool fireWeapon = false;
	bool enemyWeaponSpawned;


#ifdef IMGUI_ENABLED
	static void EditorDraw(EnemyControllerComponent& comp);
#endif

private:
	/*****************************************************************//*!
	\brief
		Gets a random cycle time.
	\return
		A random fire cycle time.
	*//******************************************************************/
	float GetRandomCycleTime() const;

	sm::ECSStateMachine sm;

	AudioReference audioDeath;
	
	property_vtable()
};
property_begin(EnemyControllerComponent)
{
	property_var(enemyRole),
	property_var(speed),
	property_var(weaponPrefabName),
	property_var(audioDeath),
}
property_vend_h(EnemyControllerComponent)

/*****************************************************************//*!
\brief
	System that controls entities with EnemyControllerComponent

*//******************************************************************/
class EnemyControllerSystem : public ecs::System<EnemyControllerSystem, EnemyControllerComponent>
{
public:
	EnemyControllerSystem();
   
private:
	void UpdateEnemyControllerComp(EnemyControllerComponent& comp);
};

/*****************************************************************//*!
\class DetectionIndicatorComponent
\brief
	Updates the attached entity's sprite depending on the current
	detection level.
*//******************************************************************/
class DetectionIndicatorComponent
	: public IRegisteredComponent<DetectionIndicatorComponent>
	, public IGameComponentCallbacks<DetectionIndicatorComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Hides the detection indicator sprite.
	*//******************************************************************/
	void OnStart() override;

	/*****************************************************************//*!
	\brief
		Sets the sprite of this entity according to the specified detection
		amount.
	*//******************************************************************/
	void UpdateIndicator(float currentDetection);

	property_vtable()
};
property_begin(DetectionIndicatorComponent)
{
}
property_vend_h(DetectionIndicatorComponent)

#include "EnemyStateMachine.ipp"
