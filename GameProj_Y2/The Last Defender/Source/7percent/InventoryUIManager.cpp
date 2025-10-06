/******************************************************************************/
/*!
\file   InventoryUIManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	InventoryUIManager is an ECS component which provides a convenient interface
	to modify the inventory UI that exists within the game scene.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "InventoryUIManager.h"
#include "CameraComponent.h"

InventoryUIManagerComponent::InventoryUIManagerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	numContainers{ 1 },
	vecContainers{ std::vector<EntityReference>(numContainers, nullptr) },
	currentContainerIndex{ 0 },
	numUnlocked{ 0 }
{
}

void InventoryUIManagerComponent::SetNumberOfUnlockedContainers(int num)
{
	numUnlocked = num;

	for (int index = 0; index < numContainers; ++index)
	{
		// Get multisprite component
		ecs::CompHandle<InventoryContainerComponent> container = vecContainers[index]->GetComp<InventoryContainerComponent>();

		if (num > 0) // Set unlocked
		{
			// If selected, set as selected
			if (index == currentContainerIndex)
			{
				//CONSOLE_LOG_EXPLICIT("Container " + std::to_string(index) + " set to SELECTED", LogLevel::LEVEL_WARNING);
				container->SetContainerState(CONTAINER_STATE::SELECTED);
			}
			else
			{
				//CONSOLE_LOG_EXPLICIT("Container " + std::to_string(index) + " set to NORMAL", LogLevel::LEVEL_WARNING);
				container->SetContainerState(CONTAINER_STATE::NORMAL);
			}
		}
		else // Set locked
		{
			//CONSOLE_LOG_EXPLICIT("Container " + std::to_string(index) + " set to LOCKED", LogLevel::LEVEL_WARNING);
			container->SetContainerState(CONTAINER_STATE::LOCKED);
		}
		--num; // Decrement
	}
}

void InventoryUIManagerComponent::SetCurrentlySelectedContainer(int index)
{
	// Reject invalid index
	if (index < 0 || index >= numUnlocked)
	{
		return;
	}

	// Set current container to normal state first
	vecContainers[currentContainerIndex]->GetComp<InventoryContainerComponent>()->SetContainerState(CONTAINER_STATE::NORMAL);

	// Change the current container index
	currentContainerIndex = index;

	// Set new selected
	vecContainers[currentContainerIndex]->GetComp<InventoryContainerComponent>()->SetContainerState(CONTAINER_STATE::SELECTED);
}

void InventoryUIManagerComponent::SetSelectedContainerDisplaySprite(int spriteID)
{
	SetContainerDisplaySprite(spriteID, currentContainerIndex);
}

void InventoryUIManagerComponent::SetContainerDisplaySprite(int spriteID, int containerIndex)
{
	vecContainers[containerIndex]->GetComp<InventoryContainerComponent>()->SetDisplaySprite(spriteID);
}

void InventoryUIManagerComponent::ShakeContainer(int index)
{
	if (ecs::CompHandle<ShakeComponent> shakeComp{ vecContainers[index]->GetComp<ShakeComponent>() })
		shakeComp->InduceStress(1.0f);
}

void InventoryUIManagerComponent::Initialise()
{
	for (int index = 0; index < numContainers; ++index)
	{
		// Get multisprite component
		ecs::CompHandle<InventoryContainerComponent> container = vecContainers[index]->GetComp<InventoryContainerComponent>();
		container->SetContainerState(CONTAINER_STATE::LOCKED);
		container->SetDisplaySprite(-1);
	}
}

#ifdef IMGUI_ENABLED
void InventoryUIManagerComponent::EditorDraw(InventoryUIManagerComponent& comp)
{
	// Input field to modify numContainers
	ImGui::InputInt("Number of Containers", &comp.numContainers);

	// Clamp to 1
	if (comp.numContainers < 1)
	{
		comp.numContainers = 1;
	}

	// Add button to apply changes
	if (ImGui::Button("Resize"))
	{
		comp.vecContainers.resize(static_cast<size_t>(comp.numContainers));
	}

	// Draw input fields for EntityReferences
	int i = 0;
	for (EntityReference& ref : comp.vecContainers)
	{
		ref.EditorDraw(std::to_string(i++).c_str());
	}
}
#endif
