/******************************************************************************/
/*!
\file   IntelDoor.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/04/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Component for handling the intel-locked door.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "IntelDoor.h"
#include "MultiReference.h"
#include "NameComponent.h"
#include "GameManager.h"
#include "PrefabManager.h"
#include "RenderComponent.h"

IntelDoorComponent::IntelDoorComponent():
#ifdef IMGUI_ENABLED
REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
targetDoor{nullptr}
{
}

void SetIndicatorEnabled(ecs::EntityHandle indicatorEntity, bool enabled)
{
	// Do ya thang here
	ecs::CompHandle<RenderComponent> renderComp = indicatorEntity->GetComp<RenderComponent>();
	if (!enabled)
	{
		renderComp->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	}
	else
	{
		renderComp->SetColor({ 0.0f,1.0f,1.0f,1.0f });
		PrefabManager::LoadPrefab("IntelDoorLight")->GetTransform().SetWorldPosition(indicatorEntity->GetTransform().GetWorldPosition());
	}
}

#ifdef IMGUI_ENABLED
void IntelDoorComponent::EditorDraw(IntelDoorComponent& comp)
{
	comp.targetDoor.EditorDraw("Target Door");
}
#endif

void IntelDoorComponent::SetUnlocked(int index, bool unlocked)
{
	// Clamp index
	int totalIntel = ST<GameManager>::Get()->GetTotalIntel();
	index = math::Clamp(index, 0, totalIntel);

	// Get thisEntity and the multiReference
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	auto multiRef = thisEntity->GetComp<MultiReferenceComponent>()->GetReferences();

	// Attempt init if the multiRef hasn't been setup properly. 
	// We do this so the whole counter is a simple prefab drag n' drop, no scene changes from me
	if (multiRef[index] == nullptr)
	{
		Init();
	}

	// Unlocked all Intel? OPEN THE DOOR!
	if (totalIntel == ST<GameManager>::Get()->GetTotalIntelUnlocked())
	{
		// Sanity checks
		if (targetDoor)
		{
			// I, too, am extremely sanity
			if (auto eventsComp{ targetDoor->GetComp <EntityEventsComponent>() })
			{
				eventsComp->BroadcastAll("OnActivated");
			}
		}
	}

	// If the light entity is *still* null, cancel everything after this and get out of here to stop a crash
	// We're doing this after the door opening so in a worst-case scenario getting all the intel is good enough
	// to get the door open
	if (multiRef[index] == nullptr)
	{
		return;
	}

	// Change the look/active of the lights HERE
	SetIndicatorEnabled(multiRef[index], unlocked);
}

void IntelDoorComponent::Init()
{
	//Get the multiRef from thisEntity
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	auto multiRefComp = thisEntity->GetComp<MultiReferenceComponent>();
	auto multiRef = multiRefComp->GetReferences();
	multiRef.resize(ST<GameManager>::Get()->GetTotalIntel(), nullptr);


	// Acquire children
	auto children = thisEntity->GetTransform().GetChildren();

	// Assign children to vector so instantiated prefabs can keep their entity references
	for (auto child : children)
	{
		// Sanity check in case anyone wants to add children of this entity that *don't* have numbered names
		ecs::EntityHandle childEntity = child->GetEntity();
		std::string childName = childEntity->GetComp<NameComponent>()->GetName();
		bool isNumber = true;
		for (char c : childName)
		{
			if (c < '0' || c>'9')
			{
				isNumber = false;
				break;
			}
		}
		if (!isNumber)
			continue;

		// Convert name to index number
		int index = std::stoi(childName);
		multiRef[index] = childEntity;

		// Deactivate using the function so we don't have to change here if product manager no like current method
		SetIndicatorEnabled(childEntity, false);
	}
	multiRefComp->SetReferences(multiRef);
}