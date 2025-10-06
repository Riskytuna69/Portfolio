/******************************************************************************/
/*!
\file   BossAIComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   02/15/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component that controls the boss AI.


All content ? 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Collision.h"
#include "EntityUID.h"
#include "IGameComponentCallbacks.h"
#include "EntityStateMachine.h"
#include "Health.h"

enum BOSS_STATE
{
	// Add more phases as neccessary
	SPAWNING,
	MOVING,
	ATTACKING,
	RETREATING
};

enum BOSS_ATTACKS
{
	// ADD more attacks here :>
	LEFT_FIRE,
	RIGHT_FIRE,
	CROSS_FIRE,
	LASER,
	TOTAL
};

namespace sm
{
	class BossAttackPlayer : public ActivityBaseTemplate<BossAttackPlayer>
	{
	public:
		/*****************************************************************//*!
		\brief
			Updates the attack activity of the statemachine while in attack state
		\param[in] sm
			The enemy's state machine.
		\return
			None
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;
	};

	class BossMoving : public ActivityBaseTemplate<BossMoving>
	{
	public:
		/*****************************************************************//*!
		\brief
			Updates the move activity of the statemachine while in moving state
		\param[in] sm
			The enemy's state machine.
		\return
			None
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;
	};

	class BossSpawning : public ActivityBaseTemplate<BossSpawning>
	{
	public:
		/*****************************************************************//*!
		\brief
			Updates the spawn activity of the statemachine while in moving state
		\param[in] sm
			The enemy's state machine.
		\return
			None
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;
	};

	class BossRetreat : public ActivityBaseTemplate<BossRetreat>
	{
	public:
		/*****************************************************************//*!
		\brief
			Updates the retreat activity of the statemachine while in moving state
		\param[in] sm
			The enemy's state machine.
		\return
			None
		*//******************************************************************/
		virtual void OnUpdate(StateMachine* sm) override;
	};

	class TransitionAttackTimer : public TransitionBaseTemplate<TransitionAttackTimer>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionAttackTimer(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;

	private:

	};

	class TransitionAttackRecoilTimer : public TransitionBaseTemplate<TransitionAttackRecoilTimer>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionAttackRecoilTimer(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;

	private:

	};

	class TransitionBossFullySpawned : public TransitionBaseTemplate<TransitionBossFullySpawned>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionBossFullySpawned(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	class TransitionMainBodyDestroyed : public TransitionBaseTemplate<TransitionMainBodyDestroyed>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionMainBodyDestroyed(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	class TransitionLaserDiedDuringAttack : public TransitionBaseTemplate<TransitionLaserDiedDuringAttack>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionLaserDiedDuringAttack(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;

	private:

	};

	class TransitionFullyRetreated : public TransitionBaseTemplate<TransitionFullyRetreated>
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor
		*//******************************************************************/
		template <typename NextStateType>
		TransitionFullyRetreated(const NextStateTypeStruct<NextStateType>& dummy);

		/*****************************************************************//*!
		\brief
			Tests whether to change state.

		\return
			True when it's time to change state. False otherwise.
		*//******************************************************************/
		virtual bool Decide(StateMachine* sm) override;
	};

	class StateBossMoving : public State
	{
	public:
		StateBossMoving();
	};

	class StateBossAttacking : public State
	{
	public:
		StateBossAttacking();
	};

	class StateBossSpawning : public State
	{
	public:
		StateBossSpawning();
	};

	class StateBossRetreating : public State
	{
	public:
		StateBossRetreating();
	};
}

namespace actions
{
	/*****************************************************************//*!
	\brief
		Moves the bossAnchor of the boss and make it follow the player's
		x axis position
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void FollowPlayer(ecs::EntityHandle enemy);

	/*****************************************************************//*!
	\brief
		Moves the boss sprite and weapons and lights in an infinity symbol
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void MoveAroundInfinity(ecs::EntityHandle enemy);

	/*****************************************************************//*!
	\brief
		Rotates the guns of the boss to point towards the player
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void TurretsTrackPlayer(ecs::EntityHandle enemy);

	// Attacks

	/*****************************************************************//*!
	\brief
		Chooses the attack the boss will do
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void ChooseAttack(ecs::EntityHandle enemy);

	/*****************************************************************//*!
	\brief
		The boss attacks the player with the corresponding attack chosen
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void AttackPlayer(ecs::EntityHandle enemy);

	// Different Attacks

	/*****************************************************************//*!
	\brief
		Fires bullets from the turrets that are alive towards the player
	\param[in,out] enemy
		reference to the boss entity
	\param[in] player
		reference to the player
	\return
		None
	*//******************************************************************/
	void CrossFireAttack(ecs::EntityHandle enemy, ecs::EntityHandle player);

	/*****************************************************************//*!
	\brief
		Goes to a starting location to either side of the player.
		continuously fires lasers downwards while moving to the opposite end
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void LaserAttack(ecs::EntityHandle enemy);

	// Spawning

	/*****************************************************************//*!
	\brief
		Updates the light source used for the spawning of the boss
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void UpdateSpawnLight(ecs::EntityHandle enemy);

	// Retreating

	/*****************************************************************//*!
	\brief
		Moves the boss away in a top right direction when defeated
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void Retreat(ecs::EntityHandle enemy);

	/*****************************************************************//*!
	\brief
		periodically spawn explosions and plays sfx during the retreat state
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void DoExplosions(ecs::EntityHandle enemy);

	/*****************************************************************//*!
	\brief
		Shakes the boss around when the boss is defeated
	\param[in,out] enemy
		reference to the boss entity
	\return
		None
	*//******************************************************************/
	void ShakeBoss(ecs::EntityHandle enemy);

}


// Controller
class BossAIComponent : public IRegisteredComponent<BossAIComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<BossAIComponent>
#endif
{
private:
	sm::ECSStateMachine sm;
	BOSS_STATE currentState;

	bool isInit;

	bool leftGunAlive;
	bool rightGunAlive;

	EntityReference playerRef;
	EntityReference leftGun;
	EntityReference rightGun;
	EntityReference laserPanel;
	EntityReference leftGunPivot;
	EntityReference rightGunPivot;

	EntityReference leftBulletOrigin;
	EntityReference rightBulletOrigin;

	EntityReference laserOrigin;

	// Lights
	EntityReference laserLight;
	EntityReference leftGunLight;
	EntityReference rightGunLight;

	// remove this later
	HealthComponent healthComp;

	// Healthbar
	EntityReference mainBodyHealthbar;

	// For Attacking
	bool hasAttacked;
	float attackTimerCountdown; //  the set timer
	float attackTimer; // timer to go into attack state
	float attackRecoilTimerCountdown; // Set the timer
	float attackRecoilTimer; // timer that counts down after attacking before moving around again


	// The Turret Fire Attack
	float burstNumber; // number of bullets per burst
	int numBulletsFired; // Check against burstNumber;
	float bulletRate; // Time between each bullet
	float bulletTimer; // To check against the bulletRate to spawn bullets

	// The Laser attack
	bool laserAlive;
	bool atLaserStartPoint;
	bool atLaserEndPoint;
	float moveSpeedMultiplier;
	float laserFireRate;
	float laserTimer;
	float distanceThreshhold;
	float laserStartDistance;

	Vector2 laserStartPos;
	Vector2 laserEndPos;
	bool chosenLaserStart;
	int laserDamage;

	//static constexpr std::array<BOSS_ATTACKS, 3> phase01MovePool{ LEFT_FIRE, RIGHT_FIRE, CROSS_FIRE };
	std::vector<BOSS_ATTACKS> movePool;

	BOSS_ATTACKS chosenAttack;
	bool attackChosen;

	// For Movement
	float moveSpeed;
	float verticalDisplacement;
	float maxVerticalDisplacement;
	float horizontalDisplacement;
	float maxHorizontalDisplacement;
	float timeProgress;

	bool goingRight;
	bool goingUp;

	// For Spawning
	// Spawn above the reactor
	EntityReference reactorRef;
	EntityReference lightSource;
	EntityReference bossSprite;
	bool finishedSpawning;
	float maxSpawnTime;
	float bossSpawnTimer;
	float maxLightRadius;
	float maxLightFalloff;
	bool maxLight;

	float shakeForce;
	float maxShakeForce;

	// For Retreating
	float stayForExplosionTime;
	float explosionTimeCounter; // check against stay for explosionTimer
	bool finishedExploding;
	float explosionAudioRate;
	float explosionAudioTimer;
	float explosionRate; 
	float explosionTimer; // check against explosionRate
	float maxDistanceRetreat;
	float retreatSpeed;

	float bossShakeForce;
	float bossCapShakeForce;

	bool reactorDefended;

	bool mainBodyAlive;
	bool below25;

	bool explosion75;
	bool explosion50;
	bool explosion25;

	EntityReference anchorPoint; // <- for displacement calculation
	EntityReference boundLeft;
	EntityReference boundRight;
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	BossAIComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~BossAIComponent();

	/*****************************************************************//*!
	\brief
		Sets the necssessary Refences on first frame 
	*//******************************************************************/
	void Init();

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(BossAIComponent& comp);
#endif

	/*****************************************************************//*!
	\brief
		Gets the boss current state
	\return
		enum BOSS_STATE
	*//******************************************************************/
	BOSS_STATE GetCurrentState() const;
	/*****************************************************************//*!
	\brief
		Sets the boss state
	\return
		None
	*//******************************************************************/
	void SetCurrentState(BOSS_STATE nextState);
	/*****************************************************************//*!
	\brief
		Gets the boss init bool
	\return
		isInit bool
	*//******************************************************************/
	bool GetIsInit() const;

	/*****************************************************************//*!
	\brief
		Gets the boss move speed
	\return 
		moveSpeed float
	*//******************************************************************/
	float GetMoveSpeed() const;

	/*****************************************************************//*!
	\brief
		Sets Boss move speed
	\param[in] value
		New move speed value
	*//******************************************************************/
	void SetMoveSpeed(float value);

	/*****************************************************************//*!
	\brief
		Gets the vertical displacement of the boss
	\return 
		The vertical discplament of the boss
	*//******************************************************************/
	float GetVerticalDisplacement() const;

	/*****************************************************************//*!
	\brief
		Sets Boss vertical displacement
	\param[in] value
		New vertical displacement value
	*//******************************************************************/
	void SetVerticalDisplacement(float value);

	/*****************************************************************//*!
	\brief
		Gets the max vertical displacement of the boss
	\return
		The max vertical discplament of the boss
	*//******************************************************************/
	float GetMaxVerticalDisplacement() const;

	/*****************************************************************//*!
	\brief
		Gets the horizontal displacement of the boss
	\return
		The horizontal discplament of the boss
	*//******************************************************************/
	float GetHorizontalDisplacement() const;

	/*****************************************************************//*!
	\brief
		Sets the horizontal displacement of the boss
	\param[in] value
		The new horizontal discplament of the boss
	*//******************************************************************/
	void SetHorizontalDisplacement(float value);

	/*****************************************************************//*!
	\brief
		Gets the max horizontal displacement of the boss
	\return
		The max horizontal discplament of the boss
	*//******************************************************************/
	float GetMaxHorizontalDisplacement() const;

	/*****************************************************************//*!
	\brief
		Updates the state machine of the boss
	*//******************************************************************/
	void UpdateBossStateMachine();

	/*****************************************************************//*!
	\brief
		Gets the horizontal direction of the boss
	\return
		The horizontal direction of the boss
	*//******************************************************************/
	bool GetBossHorizontalDirection() const;

	/*****************************************************************//*!
	\brief
		Sets the horizontal direction of the boss
	\param[in] value
		The new horizontal direction of the boss
	*//******************************************************************/
	void SetBossHorizontalDirection(bool value);

	/*****************************************************************//*!
	\brief
		Adds to TimeProgress for infinity symbol movement
	\param[in] value
		Value to add to timeProgress
	*//******************************************************************/
	void AddToTimeProgress(float value);

	/*****************************************************************//*!
	\brief
		Gets the timeProgress of the boss
	\return
		The timeProgress of the boss
	*//******************************************************************/
	float GetTimeProgress() const;

	/*****************************************************************//*!
	\brief
		Gets the anchor point of the boss
	\return
		The anchor point of the boss
	*//******************************************************************/
	EntityReference GetAnchorPoint();

	/*****************************************************************//*!
	\brief
		Moves the boss
	\param[in] xValue
		how much to move the boss in x axis
	\param[in] yValue
		how much to move the boss in y axis
	*//******************************************************************/
	void MoveBoss(float xValue, float yValue);

	/*****************************************************************//*!
	\brief
		Sets Boss Position
	\param[in] pos
		new position of the boss
	*//******************************************************************/
	void SetBossPosition(Vector2 pos);

	/*****************************************************************//*!
	\brief
		Gets the reference of the left gun pivot of the boss
	\return
		The left gun pivot of the boss
	*//******************************************************************/
	EntityReference GetLeftGunPivot() const;

	/*****************************************************************//*!
	\brief
		Gets the reference of the right gun pivot of the boss
	\return
		The right gun pivot of the boss
	*//******************************************************************/
	EntityReference GetRightGunPivot() const;

	/*****************************************************************//*!
	\brief
		Gets the reference of the left gun of the boss
	\return
		The left gun of the boss
	*//******************************************************************/
	EntityReference GetLeftGun() const;

	/*****************************************************************//*!
	\brief
		Gets the reference of the right gun of the boss
	\return
		The right gun of the boss
	*//******************************************************************/
	EntityReference GetRightGun() const;

	/*****************************************************************//*!
	\brief
		Gets the reference to the player
	\return
		The player reference
	*//******************************************************************/
	EntityReference GetPlayerRef() const;

	/*****************************************************************//*!
	\brief
		Gets the reference to the left origin point for bullets
	\return
		The left bullet origin point
	*//******************************************************************/
	EntityReference GetLeftBulletOriginRef() const;

	/*****************************************************************//*!
	\brief
		Gets the reference to the right origin point for bullets
	\return
		The right bullet origin point
	*//******************************************************************/
	EntityReference GetRightBulletOriginRef() const;

	/*****************************************************************//*!
	\brief
		Gets the number of bullets to be fired
	\return
		float of number of bullets
	*//******************************************************************/
	float GetBurstNumber() const;

	/*****************************************************************//*!
	\brief
		Gets the fire rate of the guns
	\return
		float of fire rate
	*//******************************************************************/
	float GetBulletRate() const;

	/*****************************************************************//*!
	\brief
		Updates the timer for the bullets to be fired
	\return
		bool if a bullet can be fired 
	*//******************************************************************/
	bool UpdateBulletTimer();

	/*****************************************************************//*!
	\brief
		Gets the number of bullets that has been fired
	\return
		int of number of bullets fired
	*//******************************************************************/
	int GetBulletCounter() const;

	/*****************************************************************//*!
	\brief
		Increments to the number of bullets fired and checks if it meets
		the burst amount
	\return
		None
	*//******************************************************************/
	void AddToBulletsFired();

	/*****************************************************************//*!
	\brief
		Gets the bool of if the right gun is alive
	\return
		rightGunAlive bool
	*//******************************************************************/
	bool GetRightGunAlive() const;

	/*****************************************************************//*!
	\brief
		Gets the bool of if the left gun is alive
	\return
		leftGun alive bool 
	*//******************************************************************/
	bool GetLeftGunAlive() const;

	/*****************************************************************//*!
	\brief
		Setter for the rightGunAlive bool variable. also changes sprite
		to damaged version if value is false
	\param[in] value
		new value of rightGunAlive
	\return
		None
	*//******************************************************************/
	void SetRightGunAlive(bool value);

	/*****************************************************************//*!
	\brief
		Setter for the leftGunAlive bool variable. also changes sprite
		to damaged version if value is false
	\param[in] value
		new value of leftGunAlive
	\return
		None
	*//******************************************************************/
	void SetLeftGunAlive(bool value);


	/*****************************************************************//*!
	\brief
		Setter for the leftGunAlive bool variable
	\param[in] value
		new value of leftGunAlive
	\return
		None
	*//******************************************************************/
	bool GetHasAttacked() const;

	/*****************************************************************//*!
	\brief
		Setter for the leftGunAlive bool variable
	\param[in] value
		new value of leftGunAlive
	\return
		None
	*//******************************************************************/
	void SetHasAttacked(bool value);


	/*****************************************************************//*!
	\brief
		Gets the float attackTimer
	\return
		attackTimer float
	*//******************************************************************/
	float GetAttackTimer() const;

	/*****************************************************************//*!
	\brief
		Updates the attackTimer by decrementing w fixedDt. Used for the 
		transition to attacking state by the statemaching
	\return
		bool if the boss should transition to attack state
	*//******************************************************************/
	bool UpdateAttackTimer();

	/*****************************************************************//*!
	\brief
		Updates the attackrecoilTimer by decrementing w fixedDt. Used for the
		transition to moving state by the statemaching
	\return
		bool if the boss should transition to move state
	*//******************************************************************/
	bool UpdateRecoilTimer();


	/*****************************************************************//*!
	\brief
		Chooses what attack the boss will do during its attack state
	\return
		None
	*//******************************************************************/
	void ChooseAttack();

	/*****************************************************************//*!
	\brief
		Gets the enum of the chosen attack for the attack state
	\return
		ChosenAttack enum of BOSS_ATTACKS
	*//******************************************************************/
	BOSS_ATTACKS GetChosenAttack() const;

	/*****************************************************************//*!
	\brief
		Gets the bool attackChosen to see if the boss has already chosen
		an attack for the attack state
	\return
		attackChosen bool
	*//******************************************************************/
	bool GetAttackChosen() const;

	/*****************************************************************//*!
	\brief
		Setter for the attackChosen bool
	\param[in] value
		new value for the attackChosen bool
	\return
		None
	*//******************************************************************/
	void SetAttackChosen(bool value);


	/*****************************************************************//*!
	\brief
		Checks the distance between the boss and the starting position
		for the laser attack to initiate the attack
	\return
		atLaserStartPoint bool
	*//******************************************************************/
	bool MovedToStart();

	/*****************************************************************//*!
	\brief
		Checks the distance between the boss and the end position
		for the laser attack while firing lasers
	\return
		atLaserEndPoint bool
	*//******************************************************************/
	bool MovedToEnd();

	/*****************************************************************//*!
	\brief
		Resets the two above bools after the attack has concluded
	\return
		None
	*//******************************************************************/
	void ResetLasers();

	/*****************************************************************//*!
	\brief
		Gets the fire rate of the laser attack
	\return
		laserFireRate float
	*//******************************************************************/
	float GetLaserRate() const;

	/*****************************************************************//*!
	\brief
		Gets the position of the starting position of the laser attack
	\return
		laserStartPos Vector2
	*//******************************************************************/
	Vector2 GetLaserStartPos() const;

	/*****************************************************************//*!
	\brief
		Gets the position of the ending position of the laser attack
	\return
		laserEndPos Vector2
	*//******************************************************************/
	Vector2 GetLaserEndPos() const;

	/*****************************************************************//*!
	\brief
		Updates the laserTimer to decrement w fixedDt to check if the
		boss should fire a laser in accordance with the firerate
	\return
		bool if the boss can fire a laser
	*//******************************************************************/
	bool UpdateLaserTimer();

	/*****************************************************************//*!
	\brief
		Chooses left or right position as the starting position of the 
		laser attack and sets the chosenLaserStart bool
	\return
		chosenLaserStart bool
	*//******************************************************************/
	bool ChooseLaserStart();

	/*****************************************************************//*!
	\brief
		Gets the move speed of the boss during the laser attack while its
		firing lasers
	\return
		moveSpeedMultiplier float
	*//******************************************************************/
	float GetLaserMoveSpeed() const;

	/*****************************************************************//*!
	\brief
		Gets damage of the laser
	\return
		laserDamage int
	*//******************************************************************/
	int GetLaserDamage() const;

	/*****************************************************************//*!
	\brief
		Gets the state of the laser panel
	\return
		laserAlive bool
	*//******************************************************************/
	bool GetLaserAlive() const;

	/*****************************************************************//*!
	\brief
		Setter for the laserAlive bool. also changes sprite
		to damaged version if value is false
	\param[in] value
		new value of laserAlive
	\return
		None
	*//******************************************************************/
	void SetLaserAlive(bool value);

	/*****************************************************************//*!
	\brief
		Gets the EntityReference laserPanel
	\return
		laserPanel EntityReference
	*//******************************************************************/
	EntityReference GetLaserPanelRef() const;

	/*****************************************************************//*!
	\brief
		Gets the EntityReference laserOrigin
	\return
		laserOrigin EntityReference
	*//******************************************************************/
	EntityReference GetLaserOriginRef() const;


	/*****************************************************************//*!
	\brief
		Gets the EntityReference lightSource
	\return
		laserSource EntityReference
	*//******************************************************************/
	EntityReference GetLightSource() const;

	/*****************************************************************//*!
	\brief
		Gets the finishedSpawning bool
	\return
		finishedSpawning bool
	*//******************************************************************/
	bool IsFullySpawned() const;

	/*****************************************************************//*!
	\brief
		Gets the maxSpawnTime float
	\return
		maxSpawnTime float
	*//******************************************************************/
	float GetMaxSpawnTimer() const;

	/*****************************************************************//*!
	\brief
		Updates the bossSpawnTimer by decrementing it w fixedDt which
		is used for updating the spawning lights radius
	\return
		bossSpawnTimer float
	*//******************************************************************/
	float UpdateSpawnTimer();

	/*****************************************************************//*!
	\brief
		Updates the bossSpawnTimer by incrementing it w fixedDt which
		is used for updating the spawning lights radius after the boss
		has spawned (dimming the light)
	\return
		bossSpawnTimer float
	*//******************************************************************/
	float IncrementSpawnTimer();

	/*****************************************************************//*!
	\brief
		Gets the max radius of the light during spawning
	\return
		maxLightRadius float
	*//******************************************************************/
	float GetMaxLightRadius() const;

	/*****************************************************************//*!
	\brief
		Gets the max falloff of the light during spawning
	\return
		maxLightFalloff float
	*//******************************************************************/
	float GetMaxLightFalloff() const;

	/*****************************************************************//*!
	\brief
		Gets the maxLight bool, bool if the radius has reached max
	\return
		maxLight float
	*//******************************************************************/
	bool GetMaxLight() const;

	/*****************************************************************//*!
	\brief
		Turns the boss transparent by setting the sprites inactive
	\return
		None
	*//******************************************************************/
	void TurnTransparent(bool lightOn = false);

	/*****************************************************************//*!
	\brief
		Turns the boss opaque by setting the sprites active
	\return
		None
	*//******************************************************************/
	void TurnOpaque();


	/*****************************************************************//*!
	\brief
		Gets the speed of the boss while retreating
	\return
		retreatSpeed float
	*//******************************************************************/
	float GetRetreatSpeed() const;

	/*****************************************************************//*!
	\brief
		Gets the maxDistanceRetreat
	\return
		maxDistanceRetreat float
	*//******************************************************************/
	float GetRetreatDistance() const;

	/*****************************************************************//*!
	\brief
		Updates the explosionTimer by incrementing w fixedDt, to see if
		an explosion animation should occur based on rate of explosions
	\return
		bool if explosion animation should be loaded
	*//******************************************************************/
	bool UpdateExplosionTimer();

	/*****************************************************************//*!
	\brief
		Updates the explosionAudioTimer by incrementing w fixedDt, to see if
		an explosion sfx should be played, based on explosionAudioRate
	\return
		bool if explosion sfx should be played
	*//******************************************************************/
	bool UpdateExplosionAudioTimer();

	/*****************************************************************//*!
	\brief
		Updates the explosionTime by incrementing w fixedDt, to see if
		the boss should start moving.
	\return
		bool if boss has finished exploding on spot
	*//******************************************************************/
	bool UpdateStayForExplosion();

	/*****************************************************************//*!
	\brief
		Gets the shake force of the boss while exploding.
	\return
		bossShakeForce float
	*//******************************************************************/
	float GetShakeForce() const;

	/*****************************************************************//*!
	\brief
		Gets the max shake force of the boss while exploding.
	\return
		bossCapShakeForce float
	*//******************************************************************/
	float GetShakeForceCap() const;

	/*****************************************************************//*!
	\brief
		Setter for the reactorDefended bool
	\param[in] value
		new value for reactorDefended
	\return
		None
	*//******************************************************************/
	void SetReactorDefended(bool value);

	/*****************************************************************//*!
	\brief
		Gets the reactorDefended bool
	\return
		reactorDefended bool
	*//******************************************************************/
	bool GetReactorDefended() const;

	/*****************************************************************//*!
	\brief
		Setter for the mainBodyAlive bool. also changes sprite
		to damaged version if value is false
	\param[in] value
		new value of mainBodyAlive
	\return
		None
	*//******************************************************************/
	void SetMainBodyAlive(bool value);

	/*****************************************************************//*!
	\brief
		Gets the mainBodyAlive bool
	\return
		mainBodyAlive bool
	*//******************************************************************/
	bool GetMainBodyAlive() const;

	/*****************************************************************//*!
	\brief
		Checks the health of the boss. Updates sprite based on health percentages.
	\return
		None
	*//******************************************************************/
	void CheckDamageStatus();

	/*****************************************************************//*!
	\brief
		Called when health reaches certain percentages. Spawns explosion
		prefab ontop of main body
	\return
		None
	*//******************************************************************/
	void MainBodyExplosion();

	/*****************************************************************//*!
	\brief
		Checks if the boss is within bounds of the arena
	\return
		bool if in bounds or not
	*//******************************************************************/
	bool InBounds();
	property_vtable()
};
property_begin(BossAIComponent)
{
	property_var(playerRef),
	property_var(leftGun),
	property_var(rightGun),
	property_var(leftGunPivot),
	property_var(rightGunPivot),
	property_var(attackTimerCountdown),
	property_var(attackTimer),
	property_var(attackRecoilTimerCountdown),
	property_var(attackRecoilTimer),
	property_var(moveSpeed),
	property_var(maxVerticalDisplacement),
	property_var(maxHorizontalDisplacement),
	property_var(anchorPoint),
	property_var(leftBulletOrigin),
	property_var(rightBulletOrigin),
	property_var(burstNumber),
	property_var(bulletRate),
	property_var(moveSpeedMultiplier),
	property_var(laserStartDistance),
	property_var(laserFireRate),
	property_var(laserOrigin),
	property_var(laserDamage),
	property_var(distanceThreshhold),
	property_var(maxSpawnTime),
	property_var(maxLightRadius),
	property_var(maxLightFalloff),
	property_var(shakeForce),
	property_var(maxShakeForce),
	property_var(maxDistanceRetreat),
	property_var(retreatSpeed),
	property_var(stayForExplosionTime),
	property_var(explosionRate),
	property_var(explosionAudioRate),
	property_var(bossShakeForce),
	property_var(bossCapShakeForce),

}
property_vend_h(BossAIComponent)

class BossAISystem : public ecs::System<BossAISystem, BossAIComponent>
{
public:

	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	BossAISystem();

	/*****************************************************************//*!
	\brief
		Subscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnRemoved() override;

private:

	/*****************************************************************//*!
	\brief
		Updates the Boss AI
	\param[in, out] comp
		Component to update
	\return
	*//******************************************************************/
	void UpdateBossAI(BossAIComponent& comp);


	/*****************************************************************//*!
	\brief
		Callback for the messaging system to update the leftGunAlive bool
		of the boss to false
	\return
		None
	*//******************************************************************/
	static void LeftGunDiedCallback();

	/*****************************************************************//*!
	\brief
		Callback for the messaging system to update the rightGunAlive bool
		of the boss to false
	\return
		None
	*//******************************************************************/
	static void RightGunDiedCallback();

	/*****************************************************************//*!
	\brief
		Callback for the messaging system to update the laserAlive bool
		of the boss to false
	\return
		None
	*//******************************************************************/
	static void LaserDiedCallback();

	/*****************************************************************//*!
	\brief
		Callback for the messaging system to update the reactorDefended bool
		of the boss to true
	\return
		None
	*//******************************************************************/
	static void ReactorDefendedCallback();

	/*****************************************************************//*!
	\brief
		Callback for the messaging system when the main body of the boss
		dies
	\return
		None
	*//******************************************************************/
	static void BossMainBodyDiedCallback();

	/*****************************************************************//*!
	\brief
		Callback for the messaging system when the main body takes damage
	\return
		None
	*//******************************************************************/
	static void BossDamagedCallback();

private:
};