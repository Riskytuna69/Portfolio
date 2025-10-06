/******************************************************************************/
/*!
\file   Holdable.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the definition for functions in base class Holdable.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Holdable.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "MultiSpriteComponent.h"
#include "Physics.h"
#include "Collision.h"

#include "Weapon.h"
#include "Healthpack.h"

Holdable::Holdable()
{
	Init();
}

Holdable::Holdable(std::string name, float heightOffset, std::vector<KEY> keys, std::vector<KEY> independentKeys) :
	Highlightable(name, heightOffset, keys),
	independentUseKeys{std::move(independentKeys)}
{
	Init();
}

void Holdable::UpdatePickupCooldown(float dt)
{
	autoPickupCooldown -= dt;
}

Holdable::HOLDABLE_TYPE Holdable::GetHoldable(ecs::EntityHandle entity, Holdable** outHoldable)
{
	if ((*outHoldable = entity->GetComp<WeaponComponent>()) != nullptr)
		return HOLDABLE_TYPE::WEAPON;
	if ((*outHoldable = entity->GetComp<HealthpackComponent>()) != nullptr)
		return HOLDABLE_TYPE::HEALTHPACK;
	return HOLDABLE_TYPE::NONE;
}

void Holdable::OnSwitched(bool toThis)
{
	UNREFERENCED_PARAMETER(toThis);
}

void Holdable::OnDropped()
{
	autoPickupCooldown = AUTO_PICKUP_COOLDOWN;
	isHeld = false;
}

bool Holdable::CanAutoPickup()
{
	return autoPickupCooldown <= 0.0f;
}

void Holdable::Init()
{
	isHeld = false;
	holdPointPixel = { 0.0f,0.0f };
	autoPickupCooldown = 0.0f;
}
