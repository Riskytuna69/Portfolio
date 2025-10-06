/******************************************************************************/
/*!
\file   ObjectiveArrow.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  UI Arrow that points to objective.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "RenderSystem.h"
#include "EntityUID.h"

/*****************************************************************//*!
\class ObjectiveArrowComponent
\brief
	The primary class for objective arrow.
*//******************************************************************/
class ObjectiveArrowComponent : public IRegisteredComponent<ObjectiveArrowComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<ObjectiveArrowComponent>
#endif
{
public:
	// How high the text should be above the arrow.
	float offsetY;

	// Minimum distance away from the reactor that activate the display.
	float activationDistance;

	// We need where the arrow is so we know where to put the text.
	EntityReference entityArrow;

	// We need to reference the text entity to set the position.
	EntityReference entityText;

	// To see where the objective is.
	EntityReference entityObjective;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ObjectiveArrowComponent();

	/*****************************************************************//*!
	\brief
		Returns activationDistance squared.
	\return
		float value.
	*//******************************************************************/
	float GetActivationDistanceSquared();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(ObjectiveArrowComponent& comp);
#endif
	property_vtable()
};
property_begin(ObjectiveArrowComponent)
{
	property_var(offsetY),
	property_var(activationDistance),
	property_var(entityArrow),
	property_var(entityText),
	property_var(entityObjective),
}
property_vend_h(ObjectiveArrowComponent)

// Objective arrow must do per-frame calculations, hence the need for accompanying system.
/*****************************************************************//*!
\class ObjectiveArrowSystem
\brief
	Corresponding system to update ObjectiveArrowComponent.
*//******************************************************************/
class ObjectiveArrowSystem : public ecs::System<ObjectiveArrowSystem, ObjectiveArrowComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ObjectiveArrowSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates a ObjectiveArrowComponent.
	\param comp
		The ObjectiveArrowComponent to update.
	*//******************************************************************/
	void UpdateObjectiveArrowComponent(ObjectiveArrowComponent& comp);
};