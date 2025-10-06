/******************************************************************************/
/*!
\file   Rotater.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Simple component that applies constant rotation to attached entity.
  Speed is serialised.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Rotater.h"

RotaterComponent::RotaterComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	rotationSpeed(360.0f)
{
}

#ifdef IMGUI_ENABLED
void RotaterComponent::EditorDraw(RotaterComponent& comp)
{
	ImGui::DragFloat("Rotation Speed", &comp.rotationSpeed, 1.0f, 0.0f, 9999.0f);
}
#endif

RotaterSystem::RotaterSystem() :
	System_Internal(&RotaterSystem::UpdateRotaterComp)
{
}

void RotaterSystem::UpdateRotaterComp(RotaterComponent& comp)
{
	ecs::GetEntity(&comp)->GetTransform().AddWorldRotation(comp.rotationSpeed * GameTime::FixedDt());
}
