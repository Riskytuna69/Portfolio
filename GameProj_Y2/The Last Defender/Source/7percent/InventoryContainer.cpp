/******************************************************************************/
/*!
\file   InventoryContainerComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	InventoryContainerComponent is an ECS component which provides a convenient
	interface to modify the individual inventory slot UI boxes that exists within
	the game scene.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "InventoryContainer.h"
#include "MultiSpriteComponent.h"
#include "RenderComponent.h"

InventoryContainerComponent::InventoryContainerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	displayEntity( nullptr ),
	containerEntity( nullptr )
{
}

void InventoryContainerComponent::SetDisplaySprite(int spriteID)
{
	displayEntity->GetComp<MultiSpriteComponent>()->ChangeSpriteID(spriteID);
}

void InventoryContainerComponent::SetContainerState(CONTAINER_STATE state)
{
	containerEntity->GetComp<MultiSpriteComponent>()->ChangeSprite(static_cast<int>(state));
}

#ifdef IMGUI_ENABLED
void InventoryContainerComponent::EditorDraw(InventoryContainerComponent& comp)
{
	comp.displayEntity.EditorDraw("Display");
	comp.containerEntity.EditorDraw("Container");
}
#endif

InventoryContainerReddenSystem::InventoryContainerReddenSystem()
	: System_Internal{ &InventoryContainerReddenSystem::UpdateComp }
{
}

void InventoryContainerReddenSystem::UpdateComp(InventoryContainerComponent& invenComp, ShakeComponent& shakeComp)
{
	float intensity{ std::min(shakeComp.GetTrauma() * 1.25f, 0.8f) };
	invenComp.containerEntity->GetComp<RenderComponent>()->SetColor({
		1.0f, 1.0f - intensity, 1.0f - intensity, 1.0f
	});
}
