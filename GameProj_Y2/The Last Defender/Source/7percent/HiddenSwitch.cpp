/******************************************************************************/
/*!
\file   HiddenSwitch.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Implementation of functions declared in SnoipahSpawner class

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "HiddenSwitch.h"
#include "PrefabManager.h"

SnoipahSpawner::SnoipahSpawner() :
	Highlightable("_______", 1.0f, { KEY::S,KEY::N,KEY::O,KEY::I,KEY::P,KEY::A,KEY::H }),
	key{ "SNOIPAH" },
	keyIndex{ 0 }
{
}

void SnoipahSpawner::OnUseHold(KEY use)
{
	if ((int)use - (int)KEY::A == key[keyIndex] - 'A')
	{
		name[keyIndex] = key[keyIndex];
		++keyIndex;
		if (keyIndex == key.length())
		{
			ecs::EntityHandle snoipah = ST<PrefabManager>::Get()->LoadPrefab("Weapon_Snoipah");
			snoipah->GetTransform().SetWorldPosition(ecs::GetEntity(this)->GetTransform().GetWorldPosition());
			ecs::DeleteEntity(ecs::GetEntity(this));
		}
	}
}

void SnoipahSpawner::SetIsHighlighted(bool highlighted)
{
	UNREFERENCED_PARAMETER(highlighted);
}
