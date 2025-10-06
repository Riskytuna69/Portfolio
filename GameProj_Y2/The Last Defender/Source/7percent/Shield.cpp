/******************************************************************************/
/*!
\file   Shield.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	Shield is an ECS component-system pair which contains functionality to
	deflect enemy projectiles within the game scene.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Shield.h"
#include "RenderComponent.h"
#include "Bullet.h"
#include "EntityLayers.h"
#include "PrefabManager.h"
#include "Health.h"
#include "Messaging.h"
#include "GameManager.h"

ShieldComponent::ShieldComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	armPivotEntity{ nullptr },
	renderEntity{ nullptr },
	startMin{ 60.0f },
	endMax{ 75.0f },
	angleMax{ 45.0f },
	depletionRate{ 20.0f },
	regenRateMultiplier{ 2.5f },
	regenRateEnhancedTime{ 1.0f },
	accumulator{ 0.0f },
	reflectSound{ "Ricochet" },
	activatedSound{ "Shield_Up" },
	deactivatedSound{ "Shield_Down" },
	shieldActivated{ false },
	tryingToTurnOn{ false },
	defaultRegenRate{ 0 },
	inited{ false },
	regenEnhancedTimer{ 0.0f }
{
}

ShieldComponent::~ShieldComponent()
{
}

void ShieldComponent::Init()
{
	ecs::CompHandle<HealthRegenComponent> hrcomp = ecs::GetEntity(this)->GetComp<HealthRegenComponent>();
	if (hrcomp)
	{
		defaultRegenRate = hrcomp->GetHealAmtPerSec();
		inited = true;
	}
}

#ifdef IMGUI_ENABLED
void ShieldComponent::EditorDraw(ShieldComponent& comp)
{
	comp.armPivotEntity.EditorDraw("Arm Pivot");
	comp.renderEntity.EditorDraw("Render Child");
	ImGui::InputFloat("Start Min", &comp.startMin);
	ImGui::InputFloat("End Max", &comp.endMax);
	ImGui::InputFloat("Angle Max", &comp.angleMax);
	ImGui::InputFloat("Depletion Rate", &comp.depletionRate);
	ImGui::InputFloat("Regen Rate Multiplier", &comp.regenRateMultiplier);
	ImGui::InputFloat("Regen Rate Time", &comp.regenRateEnhancedTime);
	comp.reflectSound.EditorDraw("Reflect Sound");
	comp.activatedSound.EditorDraw("Activated Sound");
	comp.deactivatedSound.EditorDraw("Deactivated Sound");
}
#endif

ShieldSystem::ShieldSystem()
	: System_Internal{ &ShieldSystem::UpdateShieldSystem }
{
}

void ShieldSystem::OnAdded()
{
	// Uncomment to activate shield regenerating when dealing damage.
	Messaging::Subscribe("PlayerDealtDamage", ShieldSystem::EnhanceShieldRegen);
}

void ShieldSystem::OnRemoved()
{
	// Uncomment to activate shield regenerating when dealing damage.
	Messaging::Unsubscribe("PlayerDealtDamage", ShieldSystem::EnhanceShieldRegen);
}

void ShieldSystem::EnhanceShieldRegen(int value)
{
	UNREFERENCED_PARAMETER(value);

	for (auto it{ ecs::GetCompsBegin<ShieldComponent>() }, end{ ecs::GetCompsEnd<ShieldComponent>() }; it != end; ++it)
	{
		ecs::CompHandle<HealthRegenComponent> regenComp = it.GetEntity()->GetComp<HealthRegenComponent>();
		regenComp->SetHealAmtPerSec(static_cast<int>(static_cast<float>(it->defaultRegenRate) * it->regenRateMultiplier));
		it->regenEnhancedTimer = it->regenRateEnhancedTime;
	}
}

void ShieldSystem::UpdateShieldSystem(ShieldComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}

	ecs::EntityHandle shieldEntity = ecs::GetEntity(&comp);
	ecs::CompHandle<HealthComponent> healthComp = shieldEntity->GetComp<HealthComponent>();
	int currHealth = healthComp->GetCurrHealth();

	if (comp.regenEnhancedTimer > 0.0f)
	{
		comp.regenEnhancedTimer -= GameTime::FixedDt();
	}
	else
	{
		ecs::CompHandle<HealthRegenComponent> regenComp = shieldEntity->GetComp<HealthRegenComponent>();
		regenComp->SetHealAmtPerSec(comp.defaultRegenRate);
	}
	
	// Trying to activate and able to activate
	if (comp.tryingToTurnOn && currHealth > 0.0f)
	{
		// If not previously activated, play the activate sound
		if (!comp.shieldActivated)
		{
			ST<AudioManager>::Get()->StartSound(comp.activatedSound);
		}
		comp.shieldActivated = true;
		// comp.regenTimer = 0.0f;

		// Find all bullets in scene
		for (auto bullet = ecs::GetCompsBegin<BulletComponent>(); bullet != ecs::GetCompsEnd<BulletComponent>(); ++bullet)
		{
			// Get related componenents and entities
			ecs::CompHandle<BulletComponent> enemyBulletComponent = bullet.GetComp();
			ecs::EntityHandle enemyBullet = ecs::GetEntity(enemyBulletComponent);
			ecs::CompHandle<EntityLayerComponent> bulletLayer = enemyBullet->GetComp<EntityLayerComponent>();
			
			// Reject all non-enemy bullets
			if (bulletLayer->GetLayer() != ENTITY_LAYER::ENEMYBULLET)
			{
				continue;
			}

			// Calculate distance squared
			Vector2 armPosition = comp.armPivotEntity->GetTransform().GetWorldPosition();
			Vector2 enemyBulletPosition = enemyBullet->GetTransform().GetWorldPosition();
			float distanceSquared = (enemyBulletPosition - armPosition).LengthSquared();

			// Reject all bullets not within range
			if (distanceSquared > comp.endMax * comp.endMax || distanceSquared < comp.startMin * comp.startMin)
			{
				continue;
			}

			// Calculate relative vectors
			Vector2 shieldPosition = shieldEntity->GetTransform().GetWorldPosition();
			Vector2 a = shieldPosition - armPosition; // arm to shield
			Vector2 b = enemyBulletPosition - armPosition; // arm to bullet

			// Calculate angle between 2 vectors using atan2
			float angleRadians = std::atan2f(a.x * b.y - a.y * b.x, a.Dot(b));
			float angleDegrees = math::ToDegrees(angleRadians);

			// Reject all bullets going above max angle
			if (angleDegrees < -comp.angleMax || angleDegrees > comp.angleMax)
			{
				continue;
			}
			
			// Legacy code for shield reflection
			// Set player bullet characteristics
			/*
			DeferredPlayerBullet p{};
			p.position = enemyBulletPosition;
			p.direction = -enemyBulletComponent->GetDirection();
			p.speed = enemyBulletComponent->GetSpeed();
			p.damage = enemyBulletComponent->GetDamage();
			deferredPlayerBullets.push_back(p);
			*/

			// Health comp should take damage
			// healthComp->TakeDamage(enemyBulletComponent->GetDamage());
			healthComp->TakeDamage(0); // We simply take 0 damage to trigger hitflash (visual only)
			ST<GameManager>::Get()->SetDamageShielded(ST<GameManager>::Get()->GetDamageShielded() + enemyBulletComponent->GetDamage());

			// Kill the enemy bullet
			ecs::DeleteEntity(enemyBullet);

			// Play sound
			ST<AudioManager>::Get()->StartSound(comp.reflectSound);
		}

		// Spawn deferred player bullet prefabs (Legacy code for shield reflection)
		for (DeferredPlayerBullet& p : deferredPlayerBullets)
		{
			ecs::EntityHandle playerBullet = ST<PrefabManager>::Get()->LoadPrefab("Bullet");
			ecs::CompHandle<BulletComponent> playerBulletComponent = playerBullet->GetComp<BulletComponent>();

			playerBullet->GetTransform().SetWorldPosition(p.position);
			playerBulletComponent->SetDirection(p.direction);
			playerBulletComponent->SetSpeed(p.speed);
			playerBulletComponent->SetDamage(p.damage);
		}
		deferredPlayerBullets.clear();

		// Set the shield to render active
		comp.renderEntity->SetActive(true);
	}
	else // Not activated
	{
		// If comp was previously already activated, play the deactivate sound
		if (comp.shieldActivated)
		{
			ST<AudioManager>::Get()->StartSound(comp.deactivatedSound);
		}
		comp.shieldActivated = false;

		// Legacy code for passive shield regeneration
		/*
		// When not activated, regenTimer counts up to regenInterval
		if (comp.regenTimer < comp.regenInterval)
		{
			comp.regenTimer += GameTime::FixedDt();
		}
		else // If already reached regen interval, add per second health to healthcomp
		{
			healthComp->SetHealth(healthComp->GetCurrHealth() + static_cast<int>(comp.regenPerSecond * GameTime::FixedDt()));
		}
		*/
		comp.renderEntity->SetActive(false);
	}

	// New code for shield constant depletion when activated
	if (comp.shieldActivated)
	{
		comp.accumulator += comp.depletionRate * GameTime::FixedDt();
		int decrement = static_cast<int>(comp.accumulator);
		comp.accumulator -= static_cast<float>(decrement);
		healthComp->SetHealth(healthComp->GetCurrHealth() - decrement);

		// Transparency
		auto renderComp = shieldEntity->GetCompInChildren<RenderComponent>();
		Vector4 color = renderComp->GetColor();

		// We have a min opacity of 0.05 so players can still see the shield at low amounts
		color.w = healthComp->GetHealthFraction() + 0.05f;
		renderComp->SetColor(color);
	}
}

ShieldInputSystem::ShieldInputSystem()
	: System_Internal{ &ShieldInputSystem::UpdateShieldInputSystem }
{
}

void ShieldInputSystem::UpdateShieldInputSystem(ShieldComponent& comp)
{
	// This is to ensure inputs are called once per state switch
	if (!comp.tryingToTurnOn && Input::GetKeyCurr(KEY::M_RIGHT))
	{
		comp.tryingToTurnOn = true;
	}
	else if (comp.tryingToTurnOn && !Input::GetKeyCurr(KEY::M_RIGHT))
	{
		comp.tryingToTurnOn = false;
	}
}
