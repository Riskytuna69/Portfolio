/******************************************************************************/
/*!
\file   BossAIComponent.cpp
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
#include "pch.h"
#include "percentmath.h"
#include "Player.h"
#include "BossAIComponent.h"
#include "PrefabManager.h"
#include "Bullet.h"
#include "TweenManager.h"
#include "Physics.h"
#include "NameComponent.h"
#include "Messaging.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "AudioManager.h"
#include "AnimatorComponent.h"
#include "MultiSpriteComponent.h"
#include "GameManager.h"
#include "BossBoundedComponent.h"
#include "TweenManager.h"

BossAIComponent::BossAIComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	sm{ new sm::StateBossSpawning{} }, // Change to spawning 
	currentState{ BOSS_STATE::SPAWNING },
	isInit{ false },
	leftGunAlive{ true },
	rightGunAlive{ true },
	laserAlive{ true },
	hasAttacked{ false },
	attackTimerCountdown{ 3.0f },
	attackTimer{ attackTimerCountdown },
	attackRecoilTimerCountdown{ 0.5f },
	attackRecoilTimer{ attackRecoilTimerCountdown },
	attackChosen{ false },
	chosenAttack{ BOSS_ATTACKS::TOTAL },
	burstNumber{ 5 },
	numBulletsFired{ 0 },
	bulletRate{ 0.1f },
	bulletTimer{ 0.0f },
	laserTimer{ 0.0f },
	laserFireRate{ 0.3f },
	laserStartDistance{ 150.0f },
	laserDamage{ 10 },
	atLaserStartPoint{ false },
	atLaserEndPoint{ false },
	chosenLaserStart{ false },
	moveSpeedMultiplier{ 1.0f },
	distanceThreshhold{ 5.0f },
	moveSpeed{ 10.0f },
	maxVerticalDisplacement{ 0.0f },
	maxHorizontalDisplacement{ 0.0f },
	verticalDisplacement{ 0.0f },
	horizontalDisplacement{ 0.0f },
	timeProgress{ math::PI_f * 0.5f },
	goingRight{ false },
	goingUp{ false },
	finishedSpawning{ false },
	maxSpawnTime{ 2.0f },
	bossSpawnTimer{ maxSpawnTime },
	maxLightRadius{ 650.0f },
	maxLightFalloff{ 3.5f },
	maxLight{ false },
	maxShakeForce{ 3.0f },
	shakeForce{ 1.5f },
	maxDistanceRetreat{ 650.0f },
	retreatSpeed{ 5.0f },
	finishedExploding{ false },
	stayForExplosionTime{ 1.5f },
	explosionTimeCounter{ 0.0f },
	explosionRate{ 0.1f },
	explosionTimer{ 0.0f },
	explosionAudioRate{ 0.75f },
	explosionAudioTimer{ 0.0f },
	bossShakeForce{ 0.35f },
	bossCapShakeForce{ 1.0f },
	reactorDefended{ false },
	mainBodyAlive { true },
	below25 { false },
	explosion75 { false },
	explosion50 { false },
	explosion25 { false }
{
}

BossAIComponent::~BossAIComponent()
{
}

void BossAIComponent::Init()
{
	isInit = true;

	movePool.push_back(BOSS_ATTACKS::CROSS_FIRE);
	movePool.push_back(BOSS_ATTACKS::LASER);

	// ATTACH ALL REFERENCES CAUSE IT NO WORK LIKE UNITY - Marc
	auto ite = ecs::GetCompsBegin<PlayerComponent>();
	if (ite != ecs::GetCompsEnd<PlayerComponent>())
		playerRef = ite.GetEntity();

	attackTimer = attackTimerCountdown;
	attackRecoilTimer = attackRecoilTimerCountdown;

	anchorPoint = ecs::GetEntity(this)->GetTransform().GetParent()->GetEntity();

	bossSprite = ecs::GetEntity(ecs::GetEntity(this)->GetCompInChildren<RenderComponent>());

	bossSpawnTimer = maxSpawnTime;
	attackTimer = attackTimerCountdown;
	attackRecoilTimer = attackRecoilTimerCountdown;
	for (Transform* child : anchorPoint->GetTransform().GetChildren())
	{
		NameComponent* name = child->GetEntity()->GetComp<NameComponent>();
		if (name->GetName() == "Laser_Origin_Point")
		{
			laserOrigin = child->GetEntity();
		}
		else if (name->GetName() == "Boss_Left_Pivot")
		{
			leftGunPivot = child->GetEntity();
			for (Transform* grandchild : child->GetChildren())
			{
				name = grandchild->GetEntity()->GetComp<NameComponent>();
				if (name->GetName() == "LeftBulletOrigin")
				{
					leftBulletOrigin = grandchild->GetEntity();
				}
				else
				{
					leftGun = grandchild->GetEntity();
				}
			}
		}
		else if (name->GetName() == "Boss_Right_Pivot")
		{
			rightGunPivot = child->GetEntity();
			for (Transform* grandchild : child->GetChildren())
			{
				name = grandchild->GetEntity()->GetComp<NameComponent>();
				if (name->GetName() == "RightBulletOrigin")
				{
					rightBulletOrigin = grandchild->GetEntity();
				}
				else
				{
					rightGun = grandchild->GetEntity();
				}
			}
		}
		else if (name->GetName() == "BossSpawnLight")
		{
			lightSource = child->GetEntity();
		}
		else if (name->GetName() == "LaserPanelHolder")
		{
			laserPanel = child->GetEntity();
		}
		else if (name->GetName() == "LaserLight")
		{
			laserLight = child->GetEntity();
			Vector3 c = laserLight->GetComp<LightComponent>()->color;
			c.x = 0;
			c.y = 1;
			c.z = 0;
			laserLight->GetComp<LightComponent>()->color = c;

		}
		else if (name->GetName() == "LeftGunLight")
		{
			leftGunLight = child->GetEntity();
			Vector3 c = leftGunLight->GetComp<LightComponent>()->color;
			c.x = 0;
			c.y = 1;
			c.z = 0;
			leftGunLight->GetComp<LightComponent>()->color = c;

		}
		else if (name->GetName() == "RightGunLight")
		{
			rightGunLight = child->GetEntity();
			Vector3 c = rightGunLight->GetComp<LightComponent>()->color;
			c.x = 0;
			c.y = 1;
			c.z = 0;
			rightGunLight->GetComp<LightComponent>()->color = c;

		}

	}

	for (auto bound = ecs::GetCompsBegin<BossBoundedComponent>(); bound != ecs::GetCompsEnd<BossBoundedComponent>(); ++bound)
	{
		NameComponent* name = bound.GetEntity()->GetComp<NameComponent>();
		if (name->GetName() == "BossBoundLeft")
		{
			boundLeft = bound.GetEntity();
		}
		else if (name->GetName() == "BossBoundRight")
		{
			boundRight = bound.GetEntity();
		}
	}

	// init for the healthbar components
	auto camera = ecs::GetCompsBegin<CameraComponent>();
	HealthComponent* h = ecs::GetEntity(this)->GetComp<HealthComponent>();
	for (Transform* child : camera.GetEntity()->GetTransform().GetChildrenRecursive())
	{
		NameComponent* name = child->GetEntity()->GetComp<NameComponent>();
		if (name->GetName() == "BossPrimary")
		{
			h->SetPrimaryBar(child->GetEntity());
		}
		else if (name->GetName() == "BossSecondary")
		{
			h->SetSecondaryBar(child->GetEntity());
		}
		else if (name->GetName() == "BossWaveUIHolder")
		{
			mainBodyHealthbar = child->GetEntity();
		}
	}
	TurnTransparent(true);

	// Play sound - Added by Ryan Chan
	Vector2 bossPosition = bossSprite->GetTransform().GetWorldPosition();
	ST<AudioManager>::Get()->StartSingleSound("Boss_Spawn", false, bossPosition);
}
#ifdef IMGUI_ENABLED
void BossAIComponent::EditorDraw(BossAIComponent& comp)
{
	ImGui::PushID("PLAYERBOSSREF");
	ImGui::Text("PlayerRef");
	comp.playerRef.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::Text("~~~~~~~~~Guns~~~~~~~~~");

	ImGui::PushID("LEFTBOSSPIVOT");
	ImGui::Text("Left Pivot Ref");
	comp.leftGunPivot.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LEFTBOSSGUN");
	ImGui::Text("Left Gun Ref");
	comp.leftGun.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LEFTBULLETORIGIN");
	ImGui::Text("Left Bullet Origin Ref");
	comp.leftBulletOrigin.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("RIGHTBOSSPIVOT");
	ImGui::Text("Right Pivot Ref");
	comp.rightGunPivot.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("RIGHTTBOSSGUN");
	ImGui::Text("Right Gun Ref");
	comp.rightGun.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("RIGHTBULLETORIGIN");
	ImGui::Text("Right Bullet Origin Ref");
	comp.rightBulletOrigin.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();
	ImGui::NewLine();

	ImGui::Text("~~~~~~~~Movement~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("BOSSANCHORPOINT");
	ImGui::Text("Anchor Point");
	comp.anchorPoint.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSMOVESPEED");
	ImGui::Text("Move Speed");
	ImGui::InputFloat("", &comp.moveSpeed);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSMAXHOR");
	ImGui::Text("Max Horizontal Displacement");
	ImGui::InputFloat("", &comp.maxHorizontalDisplacement);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSMAXVERT");
	ImGui::Text("Max Vertical Displacement");
	ImGui::InputFloat("", &comp.maxVerticalDisplacement);
	ImGui::PopID();

	ImGui::NewLine();
	ImGui::NewLine();

	ImGui::Text("~~~~~~~~Attack~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("BOSSATTACKTIMER");
	ImGui::Text("Attack Timer Countdown");
	ImGui::InputFloat("", &comp.attackTimerCountdown);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSRECOILATTACKTIMER");
	ImGui::Text("Attack Recoil Timer Countdown");
	ImGui::InputFloat("", &comp.attackRecoilTimerCountdown);
	ImGui::PopID();

	ImGui::Text("~~~~~~~~CrossFire~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("CROSSFIREBULLETRATE");
	ImGui::Text("Turret Fire Rate");
	ImGui::InputFloat("", &comp.bulletRate);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("CROSSFIREBULLETS");
	ImGui::Text("Turret Bullets");
	ImGui::InputFloat("", &comp.burstNumber);
	ImGui::PopID();

	ImGui::Text("~~~~~~~~Laser~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("BOSSLASERPANEL");
	ImGui::Text("Laser Panel Ref");
	comp.laserPanel.EditorDraw("");
	ImGui::PopID();
	ImGui::NewLine();

	ImGui::PushID("BOSSLASERORIGIN");
	ImGui::Text("Laser Origin Ref");
	comp.laserOrigin.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSLASERDAMAGE");
	ImGui::Text("Laser Damage");
	ImGui::InputInt("", &comp.laserDamage);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LASERSTARTINGDISTANCE");
	ImGui::Text("Laser Starting Distance");
	ImGui::InputFloat("", &comp.laserStartDistance);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LASERDISTANCE");
	ImGui::Text("Laser Threshhold");
	ImGui::InputFloat("", &comp.distanceThreshhold);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LASERMOVESPEED");
	ImGui::Text("Laser Move Speed");
	ImGui::InputFloat("", &comp.moveSpeedMultiplier);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LASERFIRERATE");
	ImGui::Text("Laser Fire Rate");
	ImGui::InputFloat("", &comp.laserFireRate);
	ImGui::PopID();

	ImGui::Text("~~~~~~~~DEBUG~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("LASERENDYES");
	ImGui::Text("Laser End Point");
	ImGui::Checkbox("", &comp.atLaserEndPoint);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LASERSTARTYES");
	ImGui::Text("Laser Start Point");
	ImGui::Checkbox("", &comp.atLaserStartPoint);
	ImGui::PopID();


	ImGui::NewLine();

	ImGui::PushID("ATTACKTIMERLOL");
	ImGui::Text("AttackTimer");
	ImGui::Text(std::to_string(comp.attackTimer).c_str());
	ImGui::PopID();


	ImGui::NewLine();

	ImGui::PushID("ATTACKRECOILTIMERLOL");
	ImGui::Text("AttackRecoilTimer");
	ImGui::Text(std::to_string(comp.attackRecoilTimer).c_str());
	ImGui::PopID();


	ImGui::NewLine();

	ImGui::PushID("BOSSBOUNDLEFT");
	ImGui::Text("BossBoundLeft");
	comp.boundLeft.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSBOUNDRIGHT");
	ImGui::Text("BossBoundRight");
	comp.boundRight.EditorDraw("");
	ImGui::PopID();

	ImGui::Text("~~~~~~~~SPAWNING~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("BOSSSPAWNLIGHT");
	ImGui::Text("Spawn Light Source");
	comp.lightSource.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSSPAWNTIMER");
	ImGui::Text("Spawn Timer");
	ImGui::InputFloat("", &comp.maxSpawnTime);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSLIGHTRADIUS");
	ImGui::Text("Max Light Radius");
	ImGui::InputFloat("", &comp.maxLightRadius);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSLIGHTFALLOFF");
	ImGui::Text("Max Light Falloff");
	ImGui::InputFloat("", &comp.maxLightFalloff);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSSHAKEFORCE");
	ImGui::Text("Shake Force");
	ImGui::InputFloat("", &comp.shakeForce);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSMAXSHAKEFORCE");
	ImGui::Text("Max Shake Force");
	ImGui::InputFloat("", &comp.maxShakeForce);
	ImGui::PopID();

	ImGui::Text("~~~~~~~~RETREATING~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("BOSSRETREATRANGE");
	ImGui::Text("Retreat Distance");
	ImGui::InputFloat("", &comp.maxDistanceRetreat);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSRETREATSPEED");
	ImGui::Text("Retreat Speed");
	ImGui::InputFloat("", &comp.retreatSpeed);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSSTAYTIME");
	ImGui::Text("Explosion Stay Time");
	ImGui::InputFloat("", &comp.stayForExplosionTime);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSEXPLOSIONRATE");
	ImGui::Text("Explosion Rate");
	ImGui::InputFloat("", &comp.explosionRate);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSEXPLOSIONAUDIORATE");
	ImGui::Text("Explosion Audio Rate");
	ImGui::InputFloat("", &comp.explosionAudioRate);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSSHAKERFORCE");
	ImGui::Text("Boss Shake Force");
	ImGui::InputFloat("", &comp.bossShakeForce);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("BOSSMAXEDSHAKEFORCE");
	ImGui::Text("Boss Shake Force Cap");
	ImGui::InputFloat("", &comp.bossCapShakeForce);
	ImGui::PopID();


}
#endif

BOSS_STATE BossAIComponent::GetCurrentState() const
{
	return currentState;
}

void BossAIComponent::SetCurrentState(BOSS_STATE nextState)
{
	currentState = nextState;
}

bool BossAIComponent::GetIsInit() const
{
	return isInit;
}

float BossAIComponent::GetMoveSpeed() const
{
	return moveSpeed;
}

void BossAIComponent::SetMoveSpeed(float value)
{
	moveSpeed = std::fabs(value);
}

float BossAIComponent::GetVerticalDisplacement() const
{
	return std::fabs(verticalDisplacement);
}

void BossAIComponent::SetVerticalDisplacement(float value)
{
	verticalDisplacement = std::fabs(value);
}

float BossAIComponent::GetMaxVerticalDisplacement() const
{
	return std::fabs(maxVerticalDisplacement);
}

float BossAIComponent::GetHorizontalDisplacement() const
{
	return std::fabs(horizontalDisplacement);
}

void BossAIComponent::SetHorizontalDisplacement(float value)
{
	horizontalDisplacement = std::fabs(value);
}

float BossAIComponent::GetMaxHorizontalDisplacement() const
{
	return std::fabs(maxHorizontalDisplacement);
}

void BossAIComponent::UpdateBossStateMachine()
{
	sm.Update(ecs::GetEntity(this));
}

bool BossAIComponent::GetBossHorizontalDirection() const
{
	return goingRight;
}

void BossAIComponent::SetBossHorizontalDirection(bool value)
{
	goingRight = value;
}

void BossAIComponent::AddToTimeProgress(float value)
{
	timeProgress += value;
}

float BossAIComponent::GetTimeProgress() const
{
	return timeProgress;
}

EntityReference BossAIComponent::GetAnchorPoint()
{
	return anchorPoint;
}

void BossAIComponent::MoveBoss(float xValue, float yValue)
{
	// Move the boss
	Vector2 pos = ecs::GetEntity(this)->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	ecs::GetEntity(this)->GetTransform().SetWorldPosition(pos);
	
	// Move the turret Pivots
	pos = leftGunPivot->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	leftGunPivot->GetTransform().SetWorldPosition(pos);

	pos = rightGunPivot->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	rightGunPivot->GetTransform().SetWorldPosition(pos);

	// Move the laser origin point
	pos = laserOrigin->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	laserOrigin->GetTransform().SetWorldPosition(pos);

	// Move the laser Panel
	pos = laserPanel->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	laserPanel->GetTransform().SetWorldPosition(pos);

	pos = laserLight->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	laserLight->GetTransform().SetWorldPosition(pos);

	pos = rightGunLight->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	rightGunLight->GetTransform().SetWorldPosition(pos);

	pos = leftGunLight->GetTransform().GetWorldPosition();

	pos.x += xValue;
	pos.y += yValue;

	leftGunLight->GetTransform().SetWorldPosition(pos);
}

void BossAIComponent::SetBossPosition(Vector2 pos)
{
	Vector2 unitsMoved = pos - ecs::GetEntity(this)->GetTransform().GetWorldPosition();
	MoveBoss(unitsMoved.x, unitsMoved.y);
}

EntityReference BossAIComponent::GetLeftGunPivot() const
{
	return leftGunPivot;
}

EntityReference BossAIComponent::GetRightGunPivot() const
{
	return rightGunPivot;
}

EntityReference BossAIComponent::GetLeftGun() const
{
	return leftGun;
}

EntityReference BossAIComponent::GetRightGun() const
{
	return rightGun;
}

EntityReference BossAIComponent::GetPlayerRef() const
{
	return playerRef;
}

EntityReference BossAIComponent::GetLeftBulletOriginRef() const
{
	return leftBulletOrigin;
}

EntityReference BossAIComponent::GetRightBulletOriginRef() const
{
	return rightBulletOrigin;
}

float BossAIComponent::GetBurstNumber() const
{
	return burstNumber;
}

float BossAIComponent::GetBulletRate() const
{
	return bulletRate;
}

bool BossAIComponent::UpdateBulletTimer()
{
	bulletTimer += GameTime::FixedDt();
	if (bulletTimer >= bulletRate)
	{
		bulletTimer = 0.0f;
		return true;
	}
	return false;
}

int BossAIComponent::GetBulletCounter() const
{
	return numBulletsFired;
}

void BossAIComponent::AddToBulletsFired()
{
	numBulletsFired += 1;
	// if the number of fired bullets is the max or more its supposed to fire for the attack
	// it has finished attacking
	if (numBulletsFired >= burstNumber)
	{
		hasAttacked = true;
		numBulletsFired = 0;
	}
}

bool BossAIComponent::GetRightGunAlive() const
{
	return rightGunAlive;
}

bool BossAIComponent::GetLeftGunAlive() const
{
	return leftGunAlive;
}

void BossAIComponent::SetRightGunAlive(bool value)
{
	rightGunAlive = value;

	if (rightGunAlive)
		return;

	// if leftGun still alive when right gun dies
	if (leftGunAlive)
	{
		movePool.erase(std::remove(movePool.begin(), movePool.end(), BOSS_ATTACKS::CROSS_FIRE), movePool.end());
		movePool.push_back(BOSS_ATTACKS::LEFT_FIRE);
		Messaging::BroadcastAll("CallForReinforcements");
	}
	else
	{
		movePool.erase(std::remove(movePool.begin(), movePool.end(), BOSS_ATTACKS::RIGHT_FIRE), movePool.end());
	}

	rightGun->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
	ecs::GetEntity(rightGun->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

	ecs::EntityHandle temp = PrefabManager::LoadPrefab("BossExplosionPrefab");
	temp->GetTransform().SetWorldPosition(rightGun->GetTransform().GetWorldPosition());
	temp->GetComp<AnimatorComponent>()->Play();

	Vector3 c = rightGunLight->GetComp<LightComponent>()->color;
	c.x = 1;
	c.y = 0;
	c.z = 0;
	rightGunLight->GetComp<LightComponent>()->color = c;

	ST<AudioManager>::Get()->StartGroupedSound("Boss_Explosion", false, rightGun->GetTransform().GetWorldPosition(), 100.0f);

}

void BossAIComponent::SetLeftGunAlive(bool value)
{
	leftGunAlive = value;

	if (leftGunAlive)
		return;

	if (rightGunAlive)
	{
		movePool.erase(std::remove(movePool.begin(), movePool.end(), BOSS_ATTACKS::CROSS_FIRE), movePool.end());
		movePool.push_back(BOSS_ATTACKS::RIGHT_FIRE);
		Messaging::BroadcastAll("CallForReinforcements");
	}
	else
	{
		movePool.erase(std::remove(movePool.begin(), movePool.end(), BOSS_ATTACKS::LEFT_FIRE), movePool.end());
	}

	leftGun->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
	ecs::GetEntity(leftGun->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

	ecs::EntityHandle temp = PrefabManager::LoadPrefab("BossExplosionPrefab");
	temp->GetTransform().SetWorldPosition(leftGun->GetTransform().GetWorldPosition());
	temp->GetComp<AnimatorComponent>()->Play();

	Vector3 c = leftGunLight->GetComp<LightComponent>()->color;
	c.x = 1;
	c.y = 0;
	c.z = 0;
	leftGunLight->GetComp<LightComponent>()->color = c;

	ST<AudioManager>::Get()->StartGroupedSound("Boss_Explosion", false, leftGun->GetTransform().GetWorldPosition(), 100.0f);

}

bool BossAIComponent::GetHasAttacked() const
{
	return hasAttacked;
}

void BossAIComponent::SetHasAttacked(bool value)
{
	hasAttacked = value;
}

float BossAIComponent::GetAttackTimer() const
{
	return attackTimer;
}

bool BossAIComponent::UpdateAttackTimer()
{
	attackTimer -= GameTime::FixedDt();
	if (attackTimer <= 0.0f)
	{
		attackTimer = attackTimerCountdown;
		return true;
	}
	return false;
}

bool BossAIComponent::UpdateRecoilTimer()
{
	attackRecoilTimer -= GameTime::FixedDt();
	if (attackRecoilTimer <= 0.0f)
	{
		attackRecoilTimer = attackRecoilTimerCountdown;
		return true;
	}
	return false;
}

void BossAIComponent::ChooseAttack()
{
	// random attack
	attackChosen = true;
	if (movePool.empty())
	{
		return;
	}

	size_t attack = util::RandomRange(0, movePool.size());

	chosenAttack = movePool[attack];
}

BOSS_ATTACKS BossAIComponent::GetChosenAttack() const
{
	return chosenAttack;
}

bool BossAIComponent::GetAttackChosen() const
{
	return attackChosen;
}

void BossAIComponent::SetAttackChosen([[maybe_unused]] bool value)
{
	attackChosen = false;
}

bool BossAIComponent::MovedToStart()
{
	if (!atLaserStartPoint)
	{
		Vector2 bossPos = ecs::GetEntity(this)->GetTransform().GetWorldPosition();
		Vector2 distance = laserStartPos - bossPos;
		if (distance.LengthSquared() <= distanceThreshhold * distanceThreshhold)
		{
			atLaserStartPoint = true;
			return true;
		}
		
	}
	return atLaserStartPoint;
}

bool BossAIComponent::MovedToEnd()
{
	if (!atLaserEndPoint)
	{
		Vector2 bossPos = ecs::GetEntity(this)->GetTransform().GetWorldPosition();
		Vector2 distance = laserEndPos - bossPos;
		if (distance.LengthSquared() <= distanceThreshhold * distanceThreshhold)
		{
			atLaserEndPoint = true;
			hasAttacked = true;
			return true;
		}

	}
	return atLaserEndPoint;
}

void BossAIComponent::ResetLasers()
{
	atLaserEndPoint = false;
	atLaserStartPoint = false;
	chosenLaserStart = false;
}

float BossAIComponent::GetLaserRate() const
{
	return laserFireRate;
}

Vector2 BossAIComponent::GetLaserStartPos() const
{
	return laserStartPos;
}

Vector2 BossAIComponent::GetLaserEndPos() const
{
	return laserEndPos;
}

bool BossAIComponent::UpdateLaserTimer()
{
	laserTimer += GameTime::FixedDt();
	if (laserTimer >= laserFireRate)
	{
		laserTimer = 0.0f;
		return true;
	}
	return false;
}

bool BossAIComponent::ChooseLaserStart()
{
	if (!chosenLaserStart)
	{
		// need to add rng

		laserStartPos = anchorPoint->GetTransform().GetWorldPosition();
		laserEndPos = anchorPoint->GetTransform().GetWorldPosition();

		Vector2 diff = ecs::GetEntity(this)->GetTransform().GetWorldPosition();

		if (diff.x > laserStartPos.x)
		{
			laserStartPos.x -= laserStartDistance;

			laserEndPos.x += laserStartDistance * 0.75f;
		}
		else
		{
			laserStartPos.x += laserStartDistance;

			laserEndPos.x -= laserStartDistance * 0.75f;
		}
		chosenLaserStart = true;
	}
	return chosenLaserStart;
}

float BossAIComponent::GetLaserMoveSpeed() const
{
	return moveSpeedMultiplier;
}

int BossAIComponent::GetLaserDamage() const
{
	return laserDamage;
}

bool BossAIComponent::GetLaserAlive() const
{
	return laserAlive;
}

void BossAIComponent::SetLaserAlive(bool value)
{
	laserAlive = value;
	if (laserAlive)
		return;
	
	if (rightGunAlive || leftGunAlive)
	{
		Messaging::BroadcastAll("CallForReinforcements");
	}

	movePool.erase(std::remove(movePool.begin(), movePool.end(), BOSS_ATTACKS::LASER), movePool.end());
	laserPanel->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
	ecs::GetEntity(laserPanel->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

	ecs::EntityHandle temp = PrefabManager::LoadPrefab("BossExplosionPrefab");
	temp->GetTransform().SetWorldPosition(laserPanel->GetTransform().GetWorldPosition());
	temp->GetComp<AnimatorComponent>()->Play();

	Vector3 c = laserLight->GetComp<LightComponent>()->color;
	c.x = 1;
	c.y = 0;
	c.z = 0;
	laserLight->GetComp<LightComponent>()->color = c;

	ST<AudioManager>::Get()->StartGroupedSound("Boss_Explosion", false, laserPanel->GetTransform().GetWorldPosition(), 100.0f);

}

EntityReference BossAIComponent::GetLaserPanelRef() const
{
	return laserPanel;
}

EntityReference BossAIComponent::GetLaserOriginRef() const
{
	return laserOrigin;
}

EntityReference BossAIComponent::GetLightSource() const
{
	return lightSource;
}

bool BossAIComponent::IsFullySpawned() const
{
	return finishedSpawning;
}

float BossAIComponent::GetMaxSpawnTimer() const
{
	return maxSpawnTime;
}

float BossAIComponent::UpdateSpawnTimer()
{
	bossSpawnTimer -= GameTime::FixedDt();
	if (bossSpawnTimer <= 0.0f)
	{
		maxLight = true;
		TurnOpaque();
		Messaging::BroadcastAll("DoCameraShake", shakeForce, maxShakeForce);

	}
	return bossSpawnTimer;
}

float BossAIComponent::IncrementSpawnTimer()
{
	bossSpawnTimer += GameTime::FixedDt();
	if (bossSpawnTimer >= maxSpawnTime)
	{
		finishedSpawning = true;
		lightSource->SetActive(false);
	}
	return bossSpawnTimer;
}

float BossAIComponent::GetMaxLightRadius() const
{
	return maxLightRadius;
}

float BossAIComponent::GetMaxLightFalloff() const
{
	return maxLightFalloff;
}

bool BossAIComponent::GetMaxLight() const
{
	return maxLight;
}

void BossAIComponent::TurnTransparent(bool lightOn)
{
	bossSprite->SetActive(false);


	leftGun->SetActive(false);
	rightGun->SetActive(false);
	laserLight->SetActive(false);
	leftGunLight->SetActive(false);
	rightGunLight->SetActive(false);
	laserPanel->SetActive(false);
	mainBodyHealthbar->SetActive(false);

	// turn on the light source
	if (lightOn)
	{
		lightSource->SetActive(true);
		LightComponent* source = lightSource->GetComp<LightComponent>();
		source->falloffExponent = 5.0f;
		source->radius = 0.0f;
	}
}

void BossAIComponent::TurnOpaque()
{
	bossSprite->SetActive(true);
	leftGun->SetActive(true);
	rightGun->SetActive(true);
	laserLight->SetActive(true);
	leftGunLight->SetActive(true);
	rightGunLight->SetActive(true);
	laserPanel->SetActive(true);
	mainBodyHealthbar->SetActive(true);

	// Tween main body health bar (together with background panel)
	Vector2 bossUILocalPosition = mainBodyHealthbar->GetTransform().GetLocalPosition();
	ST<TweenManager>::Get()->StartTween(
		mainBodyHealthbar,
		&Transform::SetLocalPosition,
		bossUILocalPosition,
		Vector2(bossUILocalPosition.x, bossUILocalPosition.y - 1.0f),
		1.0f,
		TT::EASE_BOTH
	);
}

float BossAIComponent::GetRetreatSpeed() const
{
	return retreatSpeed;
}

float BossAIComponent::GetRetreatDistance() const
{
	return maxDistanceRetreat;
}

bool BossAIComponent::UpdateExplosionTimer()
{
	explosionTimer += GameTime::FixedDt();
	if (explosionTimer >= explosionRate)
	{
		explosionTimer = 0.0f;
		return true;
	}
	return false;
}

bool BossAIComponent::UpdateExplosionAudioTimer()
{
	explosionAudioTimer += GameTime::FixedDt();
	if (explosionAudioTimer >= explosionAudioRate)
	{
		explosionAudioTimer = 0.0f;
		return true;
	}
	return false;
}

bool BossAIComponent::UpdateStayForExplosion()
{
	if (!finishedExploding)
	{
		explosionTimeCounter += GameTime::FixedDt();
		if (explosionTimeCounter >= stayForExplosionTime)
		{
			finishedExploding = true;
		}		

	}
	return finishedExploding;
}

float BossAIComponent::GetShakeForce() const
{
	return bossShakeForce;
}

float BossAIComponent::GetShakeForceCap() const
{
	return bossCapShakeForce;
}

void BossAIComponent::SetReactorDefended(bool value)
{
	reactorDefended = value;
}

bool BossAIComponent::GetReactorDefended() const
{
	return reactorDefended;
}

void BossAIComponent::SetMainBodyAlive(bool value)
{
	mainBodyAlive = value;
	if (!mainBodyAlive)
	{
		ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(4);
		ecs::GetEntity(ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		laserPanel->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
		ecs::GetEntity(laserPanel->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		rightGun->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
		ecs::GetEntity(rightGun->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		leftGun->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
		ecs::GetEntity(leftGun->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });
	}
}

bool BossAIComponent::GetMainBodyAlive() const
{
	return mainBodyAlive;
}

void BossAIComponent::CheckDamageStatus()
{
	HealthComponent* h = ecs::GetEntity(this)->GetComp<HealthComponent>();
	if (!h || below25)
		return;

	int currentHealth = h->GetCurrHealth();
	int maxHealth = h->GetMaxHealth();
	double fraction = (double)currentHealth / (double)maxHealth;
	//int debugfrac = fraction * 100;
	//std::cout << "Boss HP %: " << std::to_string(debugfrac) << " / 100" << std::endl;
	// 100% - 76% -> 0
	// 75% - 51% -> 1
	// 50% - 26% -> 2
	// 25% - 1% -> 3
	// 0%(dead) -> 4
	if (fraction <= 0.25)
	{
		below25 = true;
		ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(3);
		ecs::GetEntity(ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		if (!explosion25)
		{ 
			MainBodyExplosion();
			explosion25 = true;
		}
		//CONSOLE_LOG_EXPLICIT("Changed sprite at 25%", LogLevel::LEVEL_DEBUG);
	}
	else if (fraction <= 0.5)
	{
		ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(2);
		ecs::GetEntity(ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		if (!explosion50)
		{
			MainBodyExplosion();
			explosion50 = true;
		}
		//CONSOLE_LOG_EXPLICIT("Changed sprite at 50%", LogLevel::LEVEL_DEBUG);
	}
	else if (fraction <= 0.75)
	{
		ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>()->ChangeSprite(1);
		ecs::GetEntity(ecs::GetEntity(this)->GetCompInChildren<MultiSpriteComponent>())->GetTransform().SetLocalScale({ 1.0f,1.0f });

		if (!explosion75)
		{
			MainBodyExplosion();
			explosion75 = true;
		}
		//CONSOLE_LOG_EXPLICIT("Changed sprite at 75%", LogLevel::LEVEL_DEBUG);
	}

}

void BossAIComponent::MainBodyExplosion()
{
	ecs::EntityHandle temp = PrefabManager::LoadPrefab("BossExplosionPrefab");
	temp->GetTransform().SetWorldPosition(ecs::GetEntity(this)->GetTransform().GetWorldPosition());
	temp->GetComp<AnimatorComponent>()->Play();

	ST<AudioManager>::Get()->StartGroupedSound("Boss_Explosion", false, ecs::GetEntity(this)->GetTransform().GetWorldPosition(), 100.0f);
}

bool BossAIComponent::InBounds()
{
	Vector2 pos = anchorPoint->GetTransform().GetWorldPosition();
	float boundLeftValue = boundLeft->GetTransform().GetWorldPosition().x;
	float boundRightValue = boundRight->GetTransform().GetWorldPosition().x;
	float playerXPosition = playerRef->GetTransform().GetWorldPosition().x;
	// check left
	if (pos.x <= boundLeftValue && playerXPosition <= boundLeftValue)
	{
		return false;
	}
	// check right
	if (pos.x >= boundRightValue && playerXPosition >= boundRightValue)
	{
		return false;
	}
	return true;
}

template <typename NextStateType>
sm::TransitionAttackTimer::TransitionAttackTimer(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionAttackTimer::Decide(StateMachine* sm)
{
	auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };

	if (boss->UpdateAttackTimer())
	{
		boss->SetHasAttacked(false);
		boss->SetCurrentState(BOSS_STATE::ATTACKING);
		return true;
	}

	return false;
}
template <typename NextStateType>
sm::TransitionAttackRecoilTimer::TransitionAttackRecoilTimer(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionAttackRecoilTimer::Decide(StateMachine* sm)
{
	auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };

	if (boss->GetHasAttacked() && boss->UpdateRecoilTimer())
	{
		boss->SetHasAttacked(false);
		boss->SetAttackChosen(false);
		boss->ResetLasers();
		boss->SetCurrentState(BOSS_STATE::MOVING);
		return true;
	
	}
	return false;
}

template <typename NextStateType>
sm::TransitionBossFullySpawned::TransitionBossFullySpawned(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionBossFullySpawned::Decide(StateMachine* sm)
{
	auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };

	bool hasFinished = boss->IsFullySpawned();
	if (hasFinished)
	{
		boss->SetCurrentState(BOSS_STATE::MOVING);
		return true;
	}
	return false;
}

template <typename NextStateType>
sm::TransitionMainBodyDestroyed::TransitionMainBodyDestroyed(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionMainBodyDestroyed::Decide(StateMachine* sm)
{
	auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };

	if (!boss->GetMainBodyAlive())
	{
		boss->SetCurrentState(BOSS_STATE::RETREATING);
		return true;
	}
	//if (!boss->GetLeftGunAlive() && !boss->GetRightGunAlive() && !boss->GetLaserAlive())
	//{
	//	boss->SetCurrentState(BOSS_STATE::RETREATING);
	//	return true;
	//}
	//if (boss->GetReactorDefended())
	//{
	//	boss->SetCurrentState(BOSS_STATE::RETREATING);
	//	return true;
	//}
	return false;
}

template <typename NextStateType>
sm::TransitionFullyRetreated::TransitionFullyRetreated(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionFullyRetreated::Decide(StateMachine* sm)
{
	//auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };
	// delete boss after distance passed
	UNREFERENCED_PARAMETER(sm);
	return false;
}

template <typename NextStateType>
sm::TransitionLaserDiedDuringAttack::TransitionLaserDiedDuringAttack(const NextStateTypeStruct<NextStateType>& dummy)
	: TransitionBaseTemplate{ dummy } {}

bool sm::TransitionLaserDiedDuringAttack::Decide(StateMachine* sm)
{
	auto boss{ static_cast<ECSStateMachine*>(sm)->GetEntity()->GetComp<BossAIComponent>() };

	if (boss->GetChosenAttack() == BOSS_ATTACKS::LASER)
	{
		return !boss->GetLaserAlive();
	}

	return false;
}

sm::StateBossMoving::StateBossMoving()
	:State{
		{ new BossMoving{} },
		{ new TransitionAttackTimer{ SET_NEXT_STATE(StateBossAttacking) } 
		, new TransitionMainBodyDestroyed{ SET_NEXT_STATE(StateBossRetreating)} }
	}
{
}

void sm::BossMoving::OnUpdate(StateMachine* sm)
{
	ecs::EntityHandle boss{ static_cast<ECSStateMachine*>(sm)->GetEntity() };

	actions::FollowPlayer(boss);
	actions::MoveAroundInfinity(boss);
	actions::TurretsTrackPlayer(boss);
}


sm::StateBossAttacking::StateBossAttacking()
	:State{
		{ new BossAttackPlayer{} },
		{ new TransitionAttackRecoilTimer{ SET_NEXT_STATE(StateBossMoving)}
		, new TransitionMainBodyDestroyed{ SET_NEXT_STATE(StateBossRetreating)}
		, new TransitionLaserDiedDuringAttack{ SET_NEXT_STATE(StateBossMoving)}}
	}
{
}

void sm::BossAttackPlayer::OnUpdate(StateMachine* sm)
{
	ecs::EntityHandle boss{ static_cast<ECSStateMachine*>(sm)->GetEntity() };


	if (!boss->GetComp<BossAIComponent>()->GetAttackChosen())
	{
		actions::ChooseAttack(boss);
	}
	if (boss->GetComp<BossAIComponent>()->GetChosenAttack() != BOSS_ATTACKS::LASER)
	{
		actions::FollowPlayer(boss);
		actions::MoveAroundInfinity(boss);
	}
	actions::AttackPlayer(boss);
	actions::TurretsTrackPlayer(boss);

}


sm::StateBossSpawning::StateBossSpawning()
	:State{
		{ new BossSpawning{} },
		{ new TransitionBossFullySpawned{ SET_NEXT_STATE(StateBossMoving) } }
	}
{
}

void sm::BossSpawning::OnUpdate(StateMachine* sm)
{
	ecs::EntityHandle boss{ static_cast<ECSStateMachine*>(sm)->GetEntity() };

	actions::UpdateSpawnLight(boss);
	if (boss->GetComp<BossAIComponent>()->GetMaxLight())
	{
		actions::MoveAroundInfinity(boss);
	}
}

sm::StateBossRetreating::StateBossRetreating()
	:State{
		{ new BossRetreat{} },
		{ new TransitionFullyRetreated{ SET_NEXT_STATE(StateBossRetreating) } }
	}
{
}

void sm::BossRetreat::OnUpdate(StateMachine* sm)
{
	ecs::EntityHandle boss{ static_cast<ECSStateMachine*>(sm)->GetEntity() };

	// if false stay in place
	if (boss->GetComp<BossAIComponent>()->GetReactorDefended() || boss->GetComp<BossAIComponent>()->UpdateStayForExplosion())
	{
		actions::Retreat(boss);
	}

	if (!boss->GetComp<BossAIComponent>()->GetReactorDefended())
	{
		actions::ShakeBoss(boss);
		actions::DoExplosions(boss);
	}
}

void actions::FollowPlayer(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}

	if (!boss->InBounds())
		return;

	Vector2 playerPos = boss->GetPlayerRef()->GetTransform().GetWorldPosition();

	playerPos.y = boss->GetAnchorPoint()->GetTransform().GetWorldPosition().y;

	Vector2 pos = util::Lerp(boss->GetAnchorPoint()->GetTransform().GetWorldPosition(), playerPos, 0.9f, GameTime::FixedDt());
	boss->GetAnchorPoint()->GetTransform().SetWorldPosition(pos);
}

void actions::MoveAroundInfinity(ecs::EntityHandle enemy)
{
	// move around in an infinity symbol
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}

	float timeToAdd = boss->GetMoveSpeed() * GameTime::FixedDt();
	boss->AddToTimeProgress(timeToAdd);
	float progress = boss->GetTimeProgress();
	float unitsMovedX = std::sinf(progress) * boss->GetMaxHorizontalDisplacement();
	float unitsMovedY = std::cosf(progress * 2) * boss->GetMaxVerticalDisplacement();

	// FIX THE INFINITY BOI
	// MoveBoss uses SetBossPosition
	// boss->SetBossPosition({ unitsMovedX, unitsMovedY });
	boss->MoveBoss(unitsMovedX, unitsMovedY);

}

void actions::TurretsTrackPlayer(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	EntityReference player = boss->GetPlayerRef();
	if (!player)
	{
		CONSOLE_LOG_EXPLICIT("Boss' Player Reference not found!", LogLevel::LEVEL_FATAL);
		return;
	}

	Vector2 playerPos = player->GetTransform().GetWorldPosition();
	Vector2 pivotPos = boss->GetLeftGunPivot()->GetTransform().GetWorldPosition();
	Vector2 dir = playerPos - pivotPos;
	dir.Normalize();

	float angle = std::atan2(dir.y, dir.x);

	boss->GetLeftGunPivot()->GetTransform().SetLocalRotation(math::ToDegrees(angle) + 90.0f);

	pivotPos = boss->GetRightGunPivot()->GetTransform().GetWorldPosition();
	dir = playerPos - pivotPos;
	dir.Normalize();

	angle = std::atan2(dir.y, dir.x);

	boss->GetRightGunPivot()->GetTransform().SetLocalRotation(math::ToDegrees(angle) + 90.0f); // + 90 degs cause pointing downwards

}

void actions::ChooseAttack(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	if (boss->GetHasAttacked())
	{
		return;
	}

	if (!boss->GetAttackChosen())
	{
		boss->ChooseAttack();
	}
}

void actions::AttackPlayer(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	if (boss->GetHasAttacked())
	{
		return;
	}
	EntityReference player = boss->GetPlayerRef();
	if (!player)
	{
		CONSOLE_LOG_EXPLICIT("Boss' Player Reference not found!", LogLevel::LEVEL_FATAL);
		return;
	}


	switch (boss->GetChosenAttack())
	{
	case BOSS_ATTACKS::LEFT_FIRE:
	case BOSS_ATTACKS::RIGHT_FIRE:
	case BOSS_ATTACKS::CROSS_FIRE:
	{
		actions::CrossFireAttack(enemy, player);
		break;
	}
	case BOSS_ATTACKS::LASER:
	{
		actions::LaserAttack(enemy);
		break;
	}
	default:
		break;
	}

}

void actions::CrossFireAttack(ecs::EntityHandle enemy, ecs::EntityHandle player)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	if (boss->GetHasAttacked())
	{
		return;
	}

	// Check to see if the number of shots fired is less than max
	// AND if the fire rate allows it
	if (boss->GetBulletCounter() < boss->GetBurstNumber() && boss->UpdateBulletTimer())
	{
		Vector2 playerPos = player->GetTransform().GetWorldPosition();
		// left bullet
		if (boss->GetLeftGunAlive())
		{

			Vector2 pivotPos = boss->GetLeftGunPivot()->GetTransform().GetWorldPosition();
			Vector2 dir = playerPos - pivotPos;
			dir = dir.Normalize();

			// float angle = std::atan2(dir.y, dir.x);

			Vector2 bulletPos = boss->GetLeftBulletOriginRef()->GetTransform().GetWorldPosition();

			ecs::EntityHandle temp = PrefabManager::LoadPrefab("BulletEnemy");
			temp->GetComp<BulletComponent>()->Fire(bulletPos, dir, 10, 20.0f, 5.0f);

			// Play sound - Added by Ryan Chan
			ST<AudioManager>::Get()->StartSingleSound("Boss_Gunfire", false, bulletPos);
		}
		// right bullet
		if (boss->GetRightGunAlive())
		{
			Vector2 pivotPos = boss->GetRightGunPivot()->GetTransform().GetWorldPosition();
			Vector2 dir = playerPos - pivotPos;
			dir = dir.Normalize();

			Vector2 bulletPos = boss->GetRightBulletOriginRef()->GetTransform().GetWorldPosition();

			ecs::EntityHandle temp = PrefabManager::LoadPrefab("BulletEnemy");
			temp->GetComp<BulletComponent>()->Fire(bulletPos, dir, 10, 20.0f, 5.0f);

			// Play sound - Added by Ryan Chan
			ST<AudioManager>::Get()->StartSingleSound("Boss_Gunfire", false, bulletPos);
		}

		boss->AddToBulletsFired();
	}
}

void actions::LaserAttack(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	if (boss->GetHasAttacked())
	{
		return;
	}
	// Figure out which side is the starting side
	if (boss->ChooseLaserStart())
	{
		// steps:
		// move anchor to start position
		// decrease figure 8 size
		// profit

		// move to the start location first
		if (!boss->MovedToStart())
		{
			// lerp to the start
			Vector2 laserStartPoint = boss->GetLaserStartPos();;
			Vector2 pos = util::Lerp(enemy->GetTransform().GetWorldPosition(), laserStartPoint, 0.75f, GameTime::FixedDt());
			boss->SetBossPosition(pos);
			//enemy->GetTransform().SetWorldPosition(pos);
		}
		else if (!boss->MovedToEnd())
		{
			// just move while firing
			// lerp while firing
			Vector2 laserEndPoint = boss->GetLaserEndPos();
			Vector2 pos = enemy->GetTransform().GetWorldPosition();

			Vector2 anchorPos = boss->GetAnchorPoint()->GetTransform().GetWorldPosition();
			if (laserEndPoint.x > anchorPos.x)
			{
				pos.x += boss->GetLaserMoveSpeed() * GameTime::FixedDt();
			}
			else
			{
				pos.x -= boss->GetLaserMoveSpeed() * GameTime::FixedDt();
			}

			boss->SetBossPosition(pos);

			//enemy->GetTransform().SetWorldPosition(pos);


			// Fire here
			if (boss->UpdateLaserTimer()) // update timer
			{
				// shoot raycast to get laser length
				Physics::RaycastResult raycastResult{};
				EntityLayersMask layersToSearch = EntityLayersMask({ ENTITY_LAYER::ENVIRONMENT });
				Physics::Raycast(boss->GetLaserOriginRef()->GetTransform().GetWorldPosition(), Vector2(0, -1), layersToSearch, &raycastResult);

				float laserLength = raycastResult.distance;

				ecs::EntityHandle temp = PrefabManager::LoadPrefab("LaserBoss");
				temp->GetComp<BossLaserComponent>()->SetDamage(boss->GetLaserDamage());
				temp->GetComp<BossLaserComponent>()->SetLaserLength(laserLength);
				Vector2 laserOriginPosition = boss->GetLaserOriginRef()->GetTransform().GetWorldPosition();
				temp->GetTransform().SetWorldPosition(laserOriginPosition - Vector2{ 0.0f,laserLength / 2.0f });
				Messaging::BroadcastAll("DoCameraShake", boss->GetShakeForce() * 1.5f, boss->GetShakeForceCap() * 1.5f);
				//temp->GetComp<LaserComponent>()->Fire(1000.0f, boss->GetLaserOriginRef()->GetTransform().GetWorldPosition(),
				//	Vector2(0, -1),
				//	0.1f,
				//	boss->GetLaserDamage(),
				//	false,
				//	0);

				// Play sound - Added by Ryan Chan
				ST<AudioManager>::Get()->StartSingleSound("Boss_Laserfire", false, laserOriginPosition);
			}
		}
	}

}

void actions::UpdateSpawnLight(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}

	LightComponent* source = boss->GetLightSource()->GetComp<LightComponent>();
	if (!source)
	{
		CONSOLE_LOG_EXPLICIT("Boss Light Source not found!", LogLevel::LEVEL_FATAL);
		return;
	}

	// update the radius and the cutoff as the timer counts down
	if (!boss->GetMaxLight())
	{
		float timeLeft = boss->UpdateSpawnTimer();
		float maxTime = boss->GetMaxSpawnTimer();
		float radPercent = (maxTime - timeLeft) / maxTime;
		float falloffPercent = timeLeft / maxTime;
		source->radius = boss->GetMaxLightRadius() * radPercent;
		source->falloffExponent = 5.0f * falloffPercent + boss->GetMaxLightFalloff();
	}
	else
	{
		float timeLeft = boss->IncrementSpawnTimer();
		float maxTime = boss->GetMaxSpawnTimer();
		float radPercent = (maxTime - timeLeft) / maxTime;
		float falloffPercent = timeLeft / maxTime;
		source->radius = boss->GetMaxLightRadius() * radPercent;
		source->falloffExponent = 5.0f * falloffPercent + boss->GetMaxLightFalloff();
	}


}

void actions::Retreat(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	// produce explosions for a bit
	// Retreat off the screen top right
	boss->UpdateStayForExplosion();
	float unitsMoved = boss->GetRetreatSpeed() * GameTime::FixedDt();

	boss->MoveBoss(unitsMoved, unitsMoved * 0.25f);
	// check against anchor
}

void actions::DoExplosions(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	// Shake the boss around

	// spawn explosion
	if (boss->UpdateExplosionAudioTimer())
	{
		Vector2 bossPosition = enemy->GetTransform().GetWorldPosition();
		ST<AudioManager>::Get()->StartGroupedSound("Boss_Explosion", false, bossPosition, 100.0f);
		//int sparkAmount = 10;

		//ecs::EntityHandle temp;
		//Vector2 pos = boss->GetAnchorPoint()->GetTransform().GetWorldPosition();
		//for (int i = 0; i < sparkAmount; ++i)
		//{
		//	// give a range of where to spawn
		//	temp = PrefabManager::LoadPrefab("Spark");
		//	temp->GetTransform().SetWorldPosition(pos);
		//	// random direction of force
		//	float yDir = util::RandomRangeFloat(-150.0f, 150.0f);
		//	float xDir = util::RandomRangeFloat(-150.0f, 150.0f);

		//	temp->GetComp<Physics::PhysicsComp>()->AddVelocity(Vector2(xDir, yDir));
		//}
	}
	if (boss->UpdateExplosionTimer())
	{
		// random x & y
		// explosion scale = 64 x 84
		// boss scale = 712 x 198
		// max x distance from anchor = 0.5*712 - 0.5*64
		ecs::EntityHandle temp = PrefabManager::LoadPrefab("BossExplosionPrefab");

		Vector2 bossScale = enemy->GetTransform().GetLocalScale();
		Vector2 explosionScale = temp->GetTransform().GetLocalScale();
		float xRange = 0.5f*bossScale.x - 0.5f*explosionScale.x;
		float yRange = 0.25f * bossScale.y;

		Vector2 pos(util::RandomRangeFloat(-xRange, xRange), util::RandomRangeFloat(-yRange, yRange));
		pos = pos + enemy->GetTransform().GetWorldPosition();
		temp->GetTransform().SetWorldPosition(pos);
		temp->GetComp<AnimatorComponent>()->Play();
	}
}

void actions::ShakeBoss(ecs::EntityHandle enemy)
{
	BossAIComponent* boss = enemy->GetComp<BossAIComponent>();
	if (!boss)
	{
		CONSOLE_LOG_EXPLICIT("Boss AI component not found!", LogLevel::LEVEL_FATAL);
		return;
	}
	ShakeComponent* sc = boss->GetAnchorPoint()->GetComp<ShakeComponent>();
	sc->InduceStress(boss->GetShakeForce(), boss->GetShakeForceCap());
}


BossAISystem::BossAISystem()
	: System_Internal{ &BossAISystem::UpdateBossAI }
{
}

void BossAISystem::OnAdded()
{
	// Subscribe to stuff here
	Messaging::Subscribe("DiedBossLeftGun", BossAISystem::LeftGunDiedCallback);
	Messaging::Subscribe("DiedBossRightGun", BossAISystem::RightGunDiedCallback);
	Messaging::Subscribe("DiedBossLaserPoint", BossAISystem::LaserDiedCallback);
	Messaging::Subscribe("ReactorDefended", BossAISystem::ReactorDefendedCallback);
	Messaging::Subscribe("DiedBoss", BossAISystem::BossMainBodyDiedCallback);
	Messaging::Subscribe("BossDamaged", BossAISystem::BossDamagedCallback);

}

void BossAISystem::OnRemoved()
{
	// Unsubscribe here
	Messaging::Unsubscribe("DiedBossLeftGun", BossAISystem::LeftGunDiedCallback);
	Messaging::Unsubscribe("DiedBossRightGun", BossAISystem::RightGunDiedCallback);
	Messaging::Unsubscribe("DiedBossLaserPoint", BossAISystem::LaserDiedCallback);
	Messaging::Unsubscribe("ReactorDefended", BossAISystem::ReactorDefendedCallback);
	Messaging::Unsubscribe("DiedBoss", BossAISystem::BossMainBodyDiedCallback);
	Messaging::Unsubscribe("BossDamaged", BossAISystem::BossDamagedCallback);

}

void BossAISystem::UpdateBossAI(BossAIComponent& comp)
{
	if (!comp.GetIsInit())
	{
		comp.Init();
	}
	comp.UpdateBossStateMachine();
	
	// if the boss is far away enough delete it
	if (comp.GetCurrentState() == BOSS_STATE::RETREATING)
	{
		float distance = (comp.GetAnchorPoint()->GetTransform().GetWorldPosition() - ecs::GetEntity(&comp)->GetTransform().GetWorldPosition()).LengthSquared();
		if (distance > comp.GetRetreatDistance() * comp.GetRetreatDistance())
		{
			//ST<GameManager>::Get()->SetGameState(GAME_STATE::VICTORY);
			ST<GameManager>::Get()->SetGameState(GAME_STATE::VICTORY);
			Messaging::BroadcastAll("DiedBoss");
			ecs::DeleteEntity(comp.GetAnchorPoint());

		}
	}
}

void BossAISystem::LeftGunDiedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->SetLeftGunAlive(false);
}

void BossAISystem::RightGunDiedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->SetRightGunAlive(false);
}

void BossAISystem::LaserDiedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->SetLaserAlive(false);
}

void BossAISystem::ReactorDefendedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->SetReactorDefended(true);
}

void BossAISystem::BossMainBodyDiedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->SetMainBodyAlive(false);
}

void BossAISystem::BossDamagedCallback()
{
	auto ite = ecs::GetCompsBegin<BossAIComponent>();
	ite->CheckDamageStatus();
}
