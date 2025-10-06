/******************************************************************************/
/*!
\file   Health.cpp
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

#include "Health.h"
#include "Bar.h"
#include "CameraComponent.h"
#include "EnemySpawnSystem.h"
#include "HitFlash.h"
#include "PostProcessingComponent.h"

int cheatState = 0;
bool cheatActive = false; ///THis is so the healthbar colour wont keep updating

HealthComponent::HealthComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	barEntity(nullptr)
	, inited(false)
	, isDeadBroadcasted(false)
	, maxHealth(defaultMax)
	, currHealth(maxHealth)
	, invulnerableTime{ 0.0f }
	, barShowLogarithmic{ true }
	, healthType("")
	, secondaryHealthBarDelay{}
	, selectedIndex{ 0 }
{
}

int HealthComponent::GetCurrHealth() const
{
	return currHealth;
}

bool HealthComponent::IsDead() const
{
	return currHealth <= 0;
}

int HealthComponent::GetMaxHealth() const
{
	return maxHealth;
}

const std::string HealthComponent::GetHealthType()
{
	return healthType;
}

void HealthComponent::AddHealth(int amount)
{
	currHealth += amount;
	if (currHealth > maxHealth)
		currHealth = maxHealth;
	UpdateHealthBar();
}

void HealthComponent::TakeDamage(int amount)
{
	// We don't need to flash if the entity is already dead,
	// or this health component is invulnerable.
	if (IsDead() || invulnerableTime > 0.0f)
		return;

	// Try to get hitflash component (must be done first)
	if (ecs::CompHandle<HitFlashComponent> hitFlash{ ecs::GetEntity(this)->GetComp<HitFlashComponent>() })
		hitFlash->FlashForSeconds(0.1f);

	currHealth -= amount;
	
	// Broadcast damage
	ecs::CompHandle<EntityEventsComponent> eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() };
	eventsComp->BroadcastAll("Damaged");
	if (healthType == "Boss")
	{
		Messaging::BroadcastAll("BossDamaged");
	}
	if (currHealth < 0)
	{
		currHealth = 0;
	}
	if (IsDead() && !isDeadBroadcasted)
	{
		isDeadBroadcasted = true;
		eventsComp->BroadcastAll("Died");
		Messaging::BroadcastAll("Died" + healthType);
	}

	if (barEntity)
		if (ecs::CompHandle<ShakeComponent> shakeComp{ barEntity->GetCompInParents<ShakeComponent>(1) })
			shakeComp->InduceStress(1.0f);
	if (secondaryBarEntity)
		secondaryHealthBarDelay = 1.0f;
	if (camEntity)
		if (ecs::CompHandle<ShakeComponent> shakeComp{ camEntity->GetComp<ShakeComponent>() })
		{
			// Intensity curve: y = (-1/x^0.01 + 1) * 22
			float y{ shakeComp->GetTrauma() };
			float x{ std::powf(-22.0f / (y - 22.0f), 100.0f) };
			x += amount;
			y = (-1.0f / std::powf(x, 0.01f) + 1.0f) * 22.0f;
			shakeComp->InduceStress(y - shakeComp->GetTrauma(), 0.75f);
		}

	UpdateHealthBar();
}

void HealthComponent::SetHealth(int newAmount)
{
	int newValue = math::Clamp(newAmount, 0, maxHealth);

	// If the value we are trying to set is lower than the current,
	// Broadcast the Damaged message. For health regen component.
	if (newValue < currHealth)
	{
		ecs::CompHandle<EntityEventsComponent> eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() };
		eventsComp->BroadcastAll("Damaged");
	}

	// Set and update values
	currHealth = newValue;
	UpdateHealthBar();
}

void HealthComponent::SetHealthType(const std::string& hpType)
{
	healthType = hpType;
}

void HealthComponent::SetMaxHealth(int newMaxAmount)
{
	maxHealth = newMaxAmount;
	if (currHealth > maxHealth)
		currHealth = maxHealth;
	UpdateHealthBar();
}

void HealthComponent::SetInvulnerable(float length)
{
	invulnerableTime = length;
}

void HealthComponent::UpdateInvulnerable(float dt)
{
	invulnerableTime -= dt;
}

void HealthComponent::UpdateSecondaryHealthBar(float dt)
{
	if (!secondaryBarEntity)
		return;
	secondaryHealthBarDelay -= dt;
	if (secondaryHealthBarDelay > 0.0f)
		return;

	ecs::CompHandle<BarComponent> barComp{ secondaryBarEntity->GetComp<BarComponent>() };
	if (!barComp)
		return;

	barComp->SetCurr(std::max(barComp->GetCurr() + secondaryHealthBarDelay * 0.5f, GetBarLength(currHealth, maxHealth)));
	secondaryHealthBarDelay = 0.0f;
}

void HealthComponent::UpdateIsDeadBroadcasted(bool broadcasted)
{
	isDeadBroadcasted = broadcasted;
}

void HealthComponent::Init()
{
	inited = true;

	//! Let's fix the bar's max value to 1.0f
	if (barEntity)
		if (ecs::CompHandle<BarComponent> barComp{ barEntity->GetComp<BarComponent>() })
		{
			barComp->SetMax(1.0f);
			barComp->SetCurr(GetBarLength(currHealth, maxHealth));
		}
	if (secondaryBarEntity)
		if (ecs::CompHandle<BarComponent> barComp{ secondaryBarEntity->GetComp<BarComponent>() })
		{
			barComp->SetMax(1.0f);
			barComp->SetCurr(GetBarLength(currHealth, maxHealth));
		}
}

void HealthComponent::SetPrimaryBar(EntityReference handle)
{
	barEntity = handle;
}

void HealthComponent::SetSecondaryBar(EntityReference handle)
{
	secondaryBarEntity = handle;
}

float HealthComponent::GetHealthFraction()
{
	return (float)currHealth/(float)maxHealth;
}

#ifdef IMGUI_ENABLED
void HealthComponent::EditorDraw(HealthComponent& comp)
{
	gui::Checkbox("Logarithmic Display", &comp.barShowLogarithmic);
	comp.barEntity.EditorDraw("Primary Bar Entity");
	comp.secondaryBarEntity.EditorDraw("Damage Bar Entity");
	comp.camEntity.EditorDraw("Camera Entity");

	gui::VarDrag("Current HP", &comp.currHealth);
	if (gui::VarDrag("Max HP", &comp.maxHealth) && comp.GetCurrHealth() > comp.GetMaxHealth())
		comp.SetHealth(comp.GetMaxHealth());

	//int selectedIndex{};
	if (ImGui::Combo("Health Type", &comp.selectedIndex, comp.types.data(), 10))
	{
		comp.healthType = types[comp.selectedIndex];
	}
}
#endif
void HealthComponent::UpdateHealthBar()
{
	// If bar entity was not assigned, there is no health bar to update!
	if (!barEntity)
		return;

	// Update the length of the bar entity
	if (ecs::CompHandle<BarComponent> barComp{ barEntity->GetComp<BarComponent>() })
		barComp->SetCurr(GetBarLength(currHealth, maxHealth));
}

float HealthComponent::GetBarLength(int healthVal, int maximumHealth)
{
	float fraction{ static_cast<float>(healthVal) / static_cast<float>(maximumHealth) };
	if (barShowLogarithmic)
		// y = x^1.65
		return std::powf(fraction, 1.65f);
	else
		// y = x
		return fraction;
}

HealthRegenComponent::HealthRegenComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(HealthRegenComponent::EditorDraw),
#endif
	healAmtPerSec{ 30 }
	, rechargeDelay{ 3.0f }
	, healDelay{}
	, healAccumulation{}
{
}

void HealthRegenComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("Damaged", this, &HealthRegenComponent::OnDamaged);
}

void HealthRegenComponent::OnDetached()
{
	if (ecs::CompHandle<EntityEventsComponent> eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("Damaged", this, &HealthRegenComponent::OnDamaged);
}

int HealthRegenComponent::UpdateRecharge(float dt)
{
	healDelay -= dt;
	if (healDelay > 0.0f)
		return 0;

	dt = -healDelay;
	healDelay = 0.0f;

	healAccumulation -= static_cast<float>(static_cast<int>(healAccumulation));
	healAccumulation += static_cast<float>(healAmtPerSec) * dt;
	return static_cast<int>(healAccumulation);
}

int HealthRegenComponent::GetHealAmtPerSec()
{
	return healAmtPerSec;
}

void HealthRegenComponent::SetHealAmtPerSec(int amt)
{
	healAmtPerSec = amt;
}

void HealthRegenComponent::OnDamaged()
{
	healDelay = rechargeDelay;
	healAccumulation = 0.0f;
}

void HealthRegenComponent::EditorDraw(HealthRegenComponent& comp)
{
	gui::VarDrag("Heal Per Sec", &comp.healAmtPerSec);
	gui::VarDrag("Heal Delay", &comp.rechargeDelay);
}

HealthVignetteComponent::HealthVignetteComponent():
#ifdef IMGUI_ENABLED
	 REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	inited{}
	, initialVignetteRadius{}
	, intensity{ 0.0f }
	, maxVignetteRadius{ 0.7f }
	, decaySpeed{ 1.0f }
	, lowHealthPercentThreshold{ 0.4f }
	, lowHealthIntensityModifier{ 0.4f }
{
}

void HealthVignetteComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("Damaged", this, &HealthVignetteComponent::OnDamaged);
}

void HealthVignetteComponent::OnDetached()
{
	if (ecs::CompHandle<EntityEventsComponent> eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("Damaged", this, &HealthVignetteComponent::OnDamaged);
}

void HealthVignetteComponent::Init()
{
	if (inited)
		return;

	auto postProcessIter{ ecs::GetCompsBegin<PostProcessingComponent>() };
	if (postProcessIter == ecs::GetCompsEnd<PostProcessingComponent>())
		return;

	initialVignetteRadius = postProcessIter->vignetteRadius;
	inited = true;
}

void HealthVignetteComponent::UpdateIntensity(float dt)
{
	intensity -= dt * decaySpeed;
	if (intensity < 0.0f)
		intensity = 0.0f;
}

float HealthVignetteComponent::GetIntensity() const
{
	ecs::ConstCompHandle<HealthComponent> healthComp{ ecs::GetEntity(this)->GetComp<HealthComponent>() };
	if (!healthComp)
		return intensity;

	float healthPercentage{ static_cast<float>(healthComp->GetCurrHealth()) / healthComp->GetMaxHealth() };
	if (healthPercentage >= lowHealthPercentThreshold)
		return intensity;

	return intensity + lowHealthIntensityModifier * (1.0f - healthPercentage / lowHealthPercentThreshold);
}

float HealthVignetteComponent::GetMinVignetteRadius() const
{
	return initialVignetteRadius;
}

float HealthVignetteComponent::GetMaxVignetteRadius() const
{
	return maxVignetteRadius;
}

void HealthVignetteComponent::OnDamaged()
{
	intensity = 1.2f;
}

#ifdef IMGUI_ENABLED
void HealthVignetteComponent::EditorDraw(HealthVignetteComponent& comp)
{
	gui::VarDrag("Max Vignette Radius", &comp.maxVignetteRadius, 0.01f, 0.0f, 1.0f);
	gui::VarDrag("Decay Speed", &comp.decaySpeed, 0.02f);
	gui::VarDrag("Low Health Threshold", &comp.lowHealthPercentThreshold, 0.01f, 0.0f, 1.0f);
	gui::VarDrag("Low Health Intensity", &comp.lowHealthIntensityModifier, 0.01f, 0.0f, 1.0f);
}
#endif

HealthSystem::HealthSystem()
	: System_Internal{ &HealthSystem::UpdateHealthComp }
{
}

void HealthSystem::UpdateHealthComp(HealthComponent& comp)
{
	if (!comp.inited)
		comp.Init();

	comp.UpdateInvulnerable(GameTime::FixedDt());
	comp.UpdateSecondaryHealthBar(GameTime::FixedDt());
}

HealthRegenSystem::HealthRegenSystem()
	: System_Internal{ &HealthRegenSystem::UpdateComp }
{
}

void HealthRegenSystem::UpdateComp(HealthComponent& healthComp, HealthRegenComponent& regenComp)
{
	int regenAmt{ regenComp.UpdateRecharge(GameTime::FixedDt()) };
	if (regenAmt)
		healthComp.AddHealth(regenAmt);
}

HealthVignetteSystem::HealthVignetteSystem()
	: System_Internal{ &HealthVignetteSystem::UpdateComp }
{
}

void HealthVignetteSystem::UpdateComp(HealthVignetteComponent& comp)
{
	comp.Init();
	comp.UpdateIntensity(GameTime::FixedDt());

	auto postProcessIter{ ecs::GetCompsBegin<PostProcessingComponent>() };
	if (postProcessIter == ecs::GetCompsEnd<PostProcessingComponent>())
		return;

	float intensity{ std::clamp(comp.GetIntensity(), 0.0f, 1.0f) };
	postProcessIter->vignetteColor = util::Lerp(
		Vector4{ 0.0f, 0.0f, 0.0f, 1.0f },
		Vector4{ 1.0f, 0.0f, 0.0f, 1.0f },
		intensity
	);
	postProcessIter->vignetteRadius = util::Lerp(comp.GetMinVignetteRadius(), comp.GetMaxVignetteRadius(), intensity);
}
