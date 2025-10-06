#include "BombRange.h"
#include "TweenManager.h"

/******************************************************************************/
/*!
\file   BombRange.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   13/03/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
	Cpp for the bomb explosions that will be used by suicide enemies and certain weapons.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

BombRangeComponent::BombRangeComponent() : bombDamage(50), spriteID(0), bombKnockback{ 0.0f }, maxDistance{ 0.0f }, minDistance{ 0.0f }
#ifdef IMGUI_ENABLED
	, REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
#endif 
{


}

size_t BombRangeComponent::GetSpriteID() { return spriteID; }

void BombRangeComponent::SetSpriteID(size_t val){ spriteID = val; }

void BombRangeComponent::Fire(Vector2 origin, Vector2 startSize, Vector2 endSize, int damage,float knockback, float lifeTime)
{
	minDistance = startSize.Length();
	maxDistance = endSize.Length() - minDistance;
	bombDamage = damage;
	bombKnockback = knockback;
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	Transform& thisTransform = thisEntity->GetTransform();

	thisTransform.SetWorldPosition(origin);
	thisTransform.SetWorldScale(startSize);

	ST<TweenManager>::Get()->StartTween(thisEntity,
		&Transform::SetWorldScale,
		startSize,
		endSize,
		lifeTime,
		TT::CUBIC_EASE_OUT);
	ST<Scheduler>::Get()->Add(lifeTime + 0.1f, [thisEntity]
		{
			ecs::DeleteEntity(thisEntity);
		});
}

#ifdef IMGUI_ENABLED
void BombRangeComponent::EditorDraw( BombRangeComponent& comp)
{
	auto& spriteExplosion = ResourceManager::GetSprite(comp.GetSpriteID());
	ImGui::Text("Drag an Sprite from the browser");

	// Existing sprite handling code...
	ImGui::ImageButton("Explosion", VulkanManager::Get().VkTextureManager().getTexture(spriteExplosion.textureID).ImGui_handle, ImVec2(100, 100),
		ImVec2(spriteExplosion.texCoords.x, spriteExplosion.texCoords.y),
		ImVec2(spriteExplosion.texCoords.z, spriteExplosion.texCoords.w));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
		{
			comp.SetSpriteID(*static_cast<size_t*>(payload->Data));
			if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
				renderComp->SetSpriteID(comp.GetSpriteID());

		}
		ImGui::EndDragDropTarget();
	}
}
#endif


void BombRangeComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &BombRangeComponent::OnCollision);
}

void BombRangeComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &BombRangeComponent::OnCollision);
}

void BombRangeComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	if (collisionData.otherComp->IsTrigger())
		return;

	ecs::EntityHandle thisEntity{ ecs::GetEntity(this) };

	// Get the other entity that was hit
	ecs::EntityHandle otherEntity = ecs::GetEntity(collisionData.otherComp);

	// Check if otherEntity has been hit before
	for (ecs::EntityHandle& previouslyHitEntity : hitEntities)
	{
		// If hit before, we L E A V E
		if (otherEntity == previouslyHitEntity)
			return;
	}
	hitEntities.push_back(otherEntity);

	bool isEnemy = otherEntity->GetComp<EnemyControllerComponent>() != nullptr;
	bool isPlayer = otherEntity->GetComp<PlayerComponent>() != nullptr;

	if (isEnemy || isPlayer)
	{
		Vector2 viewDir = { otherEntity->GetTransform().GetWorldPosition() - thisEntity->GetTransform().GetWorldPosition() };

		//Raycast towards entity to see if theres a wall inbetween
		EntityLayersMask layersToSearch{ ENTITY_LAYER::ENEMY, ENTITY_LAYER::PLAYER,ENTITY_LAYER::ENVIRONMENT };
		Physics::RaycastResult raycastResultOther{};

		Physics::Raycast(thisEntity->GetTransform().GetWorldPosition()+viewDir.Normalize(), viewDir, layersToSearch, &raycastResultOther);

		if (ecs::GetEntity(raycastResultOther.hitComp) == otherEntity)
		{
			if (ecs::CompHandle<HealthComponent> healthComp = otherEntity->GetComp<HealthComponent>())
			{
				float ratio = ((maxDistance - (viewDir.Length() - minDistance)) / maxDistance);
				ratio = math::Clamp(ratio, 0.0f, 1.0f);
				int damageAmount = (int)(ratio * bombDamage);
				healthComp->TakeDamage(damageAmount);

				ecs::CompHandle<Physics::PhysicsComp> physicsComp = otherEntity->GetComp<Physics::PhysicsComp>();
				if (physicsComp != nullptr)
				{
					physicsComp->AddVelocity(viewDir.Normalize() * bombKnockback);
				}
				/* LEaving this heree incase this part is needed
				//Enemy gets hit
				if (isPlayer)
				{
					ecs::CompHandle<PlayerComponent> Comp{ otherEntity->GetComp<PlayerComponent>() };

				}*/
			}
		}
		else
		{
			//std::cout << "Raycast   " << raycastedEntity->GetTransform().GetWorldPosition() << std::endl;
			//std::cout << "player?   " << isPlayer << "      " << otherEntity->GetTransform().GetWorldPosition() << std::endl;


		}



	}
}
