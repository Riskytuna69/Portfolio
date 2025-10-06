/******************************************************************************/
/*!
\file   ReticleSpread.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  UI Reticle that reflects the current spread of the player's held weapon.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "ReticleSpread.h"
#include "Player.h"
#include "Weapon.h"
#include "percentmath.h"

ReticleSpreadComponent::ReticleSpreadComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	playerEntity{ nullptr }
{
}

#ifdef IMGUI_ENABLED
void ReticleSpreadComponent::EditorDraw(ReticleSpreadComponent& comp)
{
	comp.playerEntity.EditorDraw	("Player");
	comp.reticleTop.EditorDraw		("Reticle Top");
	comp.reticleBottom.EditorDraw	("Reticle Bottom");
	comp.reticleLeft.EditorDraw		("Reticle Left");
	comp.reticleRight.EditorDraw	("Reticle Right");
}
#endif

ReticleSpreadSystem::ReticleSpreadSystem()
	: System_Internal{ &ReticleSpreadSystem::UpdateReticleSpreadComp }
{
}

void ReticleSpreadSystem::UpdateReticleSpreadComp(ReticleSpreadComponent& comp)
{
	float spread = 0;

	// Find distance between player and reticle
	Vector2 entityPosition = ecs::GetEntity(&comp)->GetTransform().GetWorldPosition();

	// Check if player exists first
	if (!comp.playerEntity)
	{
		return;
	}

	// Get player position
	Vector2 playerPosition = comp.playerEntity->GetTransform().GetWorldPosition();
	float distance = (playerPosition - entityPosition).Length();

	// Try to get weapon component
	EntityReference currentItem = comp.playerEntity->GetComp<PlayerComponent>()->CurrentItem();
	if (currentItem != nullptr)
	{
		ecs::CompHandle<WeaponComponent> currentWeapon = currentItem->GetComp<WeaponComponent>();
		if (currentWeapon != nullptr)
		{
			spread = currentWeapon->totalspread / 2;
		}
	}

	// Calculate distance that reticle elements should be from the middle
	float value = distance * tanf(math::ToRadians(spread));

	// Set values
	comp.reticleTop->GetTransform().SetWorldPosition(Vector2(entityPosition.x, entityPosition.y + value));
	comp.reticleBottom->GetTransform().SetWorldPosition(Vector2(entityPosition.x, entityPosition.y - value));
	comp.reticleRight->GetTransform().SetWorldPosition(Vector2(entityPosition.x + value, entityPosition.y));
	comp.reticleLeft->GetTransform().SetWorldPosition(Vector2(entityPosition.x - value, entityPosition.y));
}
