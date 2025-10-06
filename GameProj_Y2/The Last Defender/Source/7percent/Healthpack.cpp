/******************************************************************************/
/*!
\file   Healthpack.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  This file contains the functions for the Health Pack Component.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Healthpack.h"
#include "Health.h"
#include "AudioManager.h"

HealthpackComponent::HealthpackComponent():
#ifdef IMGUI_ENABLED
REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
healthAmount{ 100 },
audioUse{ "Fix" },
Holdable("Health Pack",0.0f,{}, {KEY::M_LEFT})
{
}

void HealthpackComponent::OnUseStart(KEY use)
{
	// We put the health pack usage function outside of here in case we want other keys to trigger this
	switch (use)
	{
	case KEY::M_LEFT:
		Use();
		break;
	}

}

void HealthpackComponent::SetIsHighlighted(bool highlighted)
{
	Highlightable::SetIsHighlighted(highlighted, ecs::GetEntity(this));
}

void HealthpackComponent::Use()
{
	// Attempt to get a health component in the parent
	ecs::CompHandle<HealthComponent> healthComp = ecs::GetEntity(this)->GetCompInParents<HealthComponent>();
	if (!healthComp)
		return;

	// Instantly add the health and delete this object
	healthComp->AddHealth(healthAmount);

	// Play sound
	ST<AudioManager>::Get()->StartSound(audioUse);

	ecs::DeleteEntity(ecs::GetEntity(this));
	isHeld = false;
}

#ifdef IMGUI_ENABLED
void HealthpackComponent::EditorDraw(HealthpackComponent& comp)
{
	// You can change how much the healthpack heals the player
	ImGui::InputInt("Health", &comp.healthAmount);

	comp.audioUse.EditorDraw("Use Sound");
}
#endif
