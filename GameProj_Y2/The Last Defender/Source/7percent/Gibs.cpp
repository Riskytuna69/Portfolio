/******************************************************************************/
/*!
\file   Gibs.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/25/2025

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is a source file for a component that spawns a prefab of gibs.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "Gibs.h"
#include "game.h"
#include "PrefabManager.h"
#include "Physics.h"

GibsComponent::GibsComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	prefabName{ "Enemy Gibs" }
{
}

void GibsComponent::OnStart()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("Died", this, &GibsComponent::OnDead);
}

void GibsComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("Died", this, &GibsComponent::OnDead);
}

void GibsComponent::OnDead()
{
	ST<Scheduler>::Get()->Add(0.0f, [prefabName = prefabName, position = ecs::GetEntityTransform(this).GetWorldPosition()]() -> void {
		ecs::EntityHandle gibsEntity{ ST<PrefabManager>::Get()->LoadPrefab(prefabName) };
		gibsEntity->GetTransform().SetWorldPosition(position);
		for (auto physComp : gibsEntity->GetCompInChildrenVec<Physics::PhysicsComp>())
		{
			physComp->SetVelocity(Vector2::FromAngle(util::RandomRangeFloat(-1.0f, 1.0f) * math::PI_f * 0.7f + math::PI_f * 0.5f) * util::RandomRangeFloat(250.0f, 350.0f));
			physComp->SetAngVelocity(util::RandomRangeFloat(-1080.0f, 1080.0f));
		}
	});
}

void GibsComponent::EditorDraw(GibsComponent& comp)
{
	gui::TextBoxReadOnly("Prefab", comp.prefabName);
	gui::PayloadTarget<std::string>("PREFAB", [name = &comp.prefabName](const std::string& inName) -> void {
		*name = inName;
	});
}
