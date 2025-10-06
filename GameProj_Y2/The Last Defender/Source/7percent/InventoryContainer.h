/******************************************************************************/
/*!
\file   InventoryContainerComponent.h
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
#pragma once
#include "EntityUID.h"
#include "CameraComponent.h"

/*****************************************************************//*!
\enum CONTAINER_STATE
\brief
	Container states.
*//******************************************************************/
enum CONTAINER_STATE : int
{
	NORMAL,
	SELECTED,
	LOCKED
};

/*****************************************************************//*!
\class InventoryContainerComponent
\brief
	ECS component.
*//******************************************************************/
class InventoryContainerComponent : public IRegisteredComponent<InventoryContainerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<InventoryContainerComponent>
#endif
{
public:
	EntityReference displayEntity;
	EntityReference containerEntity;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	InventoryContainerComponent();

	/*****************************************************************//*!
	\brief
		Set the displayed sprite inside of this container.
	\param spriteID
		spriteID of the item you want to display.
	*//******************************************************************/
	void SetDisplaySprite(int spriteID);

	/*****************************************************************//*!
	\brief
		Set the state of this container.
	\param state
		State of the container.
	*//******************************************************************/
	void SetContainerState(CONTAINER_STATE state);

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(InventoryContainerComponent& comp);
#endif
	property_vtable()
};
property_begin(InventoryContainerComponent)
{
	property_var(displayEntity),
	property_var(containerEntity)
}
property_vend_h(InventoryContainerComponent)

/*****************************************************************//*!
\class InventoryContainerReddenSystem
\brief
	Modifies the color of the background container element depending
	on its shake amount.
*//******************************************************************/
class InventoryContainerReddenSystem : public ecs::System<InventoryContainerReddenSystem, InventoryContainerComponent, ShakeComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Constructor.
	*//******************************************************************/
	InventoryContainerReddenSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates the color of the container.
	\param invenComp
		The inventory container management component.
	\param shakeComp
		The shake component that shakes the inventory container.
	*//******************************************************************/
	void UpdateComp(InventoryContainerComponent& invenComp, ShakeComponent& shakeComp);

};
