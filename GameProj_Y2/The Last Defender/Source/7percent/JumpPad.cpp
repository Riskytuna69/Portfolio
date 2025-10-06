/******************************************************************************/
/*!
\file   JumpPad.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  JumpPad is an ECS component-system pair contains functionality for Jump Pads
  within the game scene.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "JumpPad.h"
#include "Physics.h"
#include "Collision.h"
#include "EntityLayers.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "Player.h"
#include "GameManager.h"
#include "MultiSpriteComponent.h"

JumpPadComponent::JumpPadComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	isActivated{ false }
{
}

void JumpPadComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &JumpPadComponent::OnCollision);
}

void JumpPadComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &JumpPadComponent::OnCollision);
}

void JumpPadComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	// Get the other entity that was hit
	ecs::EntityHandle otherEntity = ecs::GetEntity(collisionData.otherComp);

	// Make sure we are colliding with player
	ecs::CompHandle<PlayerComponent> playerComponent = otherEntity->GetComp<PlayerComponent>();

	// If colliding with Player, set bool to true
	if (playerComponent != nullptr)
	{
		isActivated = true;
	}
}

#ifdef IMGUI_ENABLED
void JumpPadComponent::EditorDraw(JumpPadComponent& comp)
{
	UNREFERENCED_PARAMETER(comp);
	// To use for future purposes...
}
#endif

JumpPadSystem::JumpPadSystem()
	: System_Internal{ &JumpPadSystem::UpdateJumpPadComp }
{
}

void JumpPadSystem::UpdateJumpPadComp(JumpPadComponent& comp)
{
	ecs::EntityHandle jumpPadEntity{ ecs::GetEntity(&comp) };
	ecs::CompHandle<MultiSpriteComponent> multiSprite = jumpPadEntity->GetComp<MultiSpriteComponent>();
	
	if (multiSprite == nullptr)
	{
		CONSOLE_LOG_EXPLICIT("JumpPadComponent requires MultiSpriteComponent!", LogLevel::LEVEL_WARNING);
		return;
	}

	if (comp.isActivated)
	{
		multiSprite->ChangeSprite(0);
		ST<GameManager>::Get()->SetJumpEnhanced(true);
		comp.isActivated = false;
	}
	else
	{
		multiSprite->ChangeSprite(1);
	}
}
