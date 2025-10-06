/******************************************************************************/
/*!
\file   InventoryUIManager.h
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
#pragma once
#include "EntityUID.h"
#include "InventoryContainer.h"
#include "Sprite.h"

/*****************************************************************//*!
\class InventoryUIManagerComponent
\brief
	ECS component.
*//******************************************************************/
class InventoryUIManagerComponent : public IRegisteredComponent<InventoryUIManagerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<InventoryUIManagerComponent>
#endif
{
public:
	int numContainers;
	std::vector<EntityReference> vecContainers;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	InventoryUIManagerComponent();

	/*****************************************************************//*!
	\brief
		Set the number of unlocked containers (inventory slots).
	\param num
		The number of unlocked containers.
	*//******************************************************************/
	void SetNumberOfUnlockedContainers(int num);

	/*****************************************************************//*!
	\brief
		Set the currently selected container (inventory slot).
	\param index
		Index of the container.
	*//******************************************************************/
	void SetCurrentlySelectedContainer(int index);

	/*****************************************************************//*!
	\brief
		Set the displayed sprite inside of the selected container.
	\param spriteID
		spriteID of the item you want to display.
	*//******************************************************************/
	void SetSelectedContainerDisplaySprite(int spriteID);

	/*****************************************************************//*!
	\brief
		Set the displayed sprite inside of the container at containerIndex.
	\param spriteID
		spriteID of the item you want to display.
	\param containerIndex
		Index of the container you want to change.
	*//******************************************************************/
	void SetContainerDisplaySprite(int spriteID, int containerIndex);

	/*****************************************************************//*!
	\brief
		Shakes a container slot.
	\param index
		Index of the container.
	*//******************************************************************/
	void ShakeContainer(int index);

	/*****************************************************************//*!
	\brief
		Sets the initial state of the UI. To be called right after Player
		is initialised.
	*//******************************************************************/
	void Initialise();

private:
	int currentContainerIndex;
	int numUnlocked;
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(InventoryUIManagerComponent& comp);
#endif
	property_vtable()
};
property_begin(InventoryUIManagerComponent)
{
	property_var(numContainers),
	property_var(vecContainers)
}
property_vend_h(InventoryUIManagerComponent)