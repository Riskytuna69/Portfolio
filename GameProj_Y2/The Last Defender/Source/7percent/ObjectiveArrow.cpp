/******************************************************************************/
/*!
\file   ObjectiveArrow.cpp
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
#include "ObjectiveArrow.h"

ObjectiveArrowComponent::ObjectiveArrowComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	offsetY{ 0.0f },
	activationDistance{ 0.0f },
	entityArrow{ nullptr },
	entityText{ nullptr },
	entityObjective{ nullptr }
{
}

float ObjectiveArrowComponent::GetActivationDistanceSquared()
{
	return activationDistance * activationDistance;
}

#ifdef IMGUI_ENABLED
void ObjectiveArrowComponent::EditorDraw(ObjectiveArrowComponent& comp)
{
	ImGui::InputFloat("Offset Y", &comp.offsetY);
	ImGui::InputFloat("Activation Distance", &comp.activationDistance);
	comp.entityArrow.EditorDraw("UI Arrow");
	comp.entityText.EditorDraw("UI Text");
	comp.entityObjective.EditorDraw("Objective");
}
#endif

ObjectiveArrowSystem::ObjectiveArrowSystem()
	: System_Internal{ &ObjectiveArrowSystem::UpdateObjectiveArrowComponent }
{
}

void ObjectiveArrowSystem::UpdateObjectiveArrowComponent(ObjectiveArrowComponent& comp)
{
	// Calculate direction of this entity to the objective
	if(!comp.entityArrow.IsValidReference() || !comp.entityText.IsValidReference() || !comp.entityObjective.IsValidReference())
		return;
	ecs::EntityHandle entity = ecs::GetEntity(&comp);
	Vector2 entityPosition = entity->GetTransform().GetWorldPosition();
	Vector2 objectivePosition = comp.entityObjective->GetTransform().GetWorldPosition();
	Vector2 entityToObjective = objectivePosition - entityPosition;

	// Check distance first
	float distanceSquared = entityToObjective.LengthSquared();

	// If less than required, turn the entities off and return.
	if (distanceSquared < comp.GetActivationDistanceSquared())
	{
		comp.entityArrow->SetActive(false);
		comp.entityText->SetActive(false);
		return;
	}
	else // turn it back on
	{
		comp.entityArrow->SetActive(true);
		comp.entityText->SetActive(true);
	}

	// Get direction and angle
	Vector2 dir = entityToObjective.Normalize();
	float angle = math::ToDegrees(std::atan2f(dir.y, dir.x));

	// Rotate it
	entity->GetTransform().SetLocalRotation(angle);

	// Set text position
	Vector2 arrowPosition = comp.entityArrow->GetTransform().GetWorldPosition();
	comp.entityText->GetTransform().SetWorldPosition(
		Vector2(
			arrowPosition.x, 
			arrowPosition.y + comp.offsetY
		));
}
