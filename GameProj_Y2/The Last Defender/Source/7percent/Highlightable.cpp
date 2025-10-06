/******************************************************************************/
/*!
\file   Highlightable.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the definition for functions in base class Highlightable.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Highlightable.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "MultiSpriteComponent.h"

Highlightable::Highlightable() :
	name{ "Highlightable" },
	textHeightOffset{ 0.0f },
	useKeys{},
	thisEntity{ nullptr },
	interactable{ true }
{
}

Highlightable::Highlightable(std::string name,float heightOffset, std::vector<KEY> keys)
	:name{name},
	textHeightOffset{heightOffset},
	useKeys{std::move(keys)},
	thisEntity{nullptr},
	interactable{true}
{
}

void Highlightable::SetIsHighlighted(bool highlighted, EntityReference _thisEntity)
{
	ecs::CompHandle<MultiSpriteComponent> multiSprite = _thisEntity->GetCompInChildren<MultiSpriteComponent>();
	thisEntity = _thisEntity;

	// If no multiSprite, return
	if (!multiSprite)
	{
		return;
	}
	
	// Set sprite based on highlighted boolean
	if (highlighted)
	{
		multiSprite->ChangeSprite(1);
	}
	else
	{
		multiSprite->ChangeSprite(0);
	}
}

void Highlightable::OnUseStart(KEY)
{
}

void Highlightable::OnUseHold(KEY)
{
}

void Highlightable::OnUseEnd(KEY)
{
}
