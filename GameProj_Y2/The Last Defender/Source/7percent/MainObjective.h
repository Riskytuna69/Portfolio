/******************************************************************************/
/*!
\file   MainObjective.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/28/2024

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief

	Compulsary component to attach to an entity for it to become the main objective.
	Theres is not system logic for this component

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Health.h"

class MainObjectiveComponent : public IRegisteredComponent<MainObjectiveComponent>
{
public:
	MainObjectiveComponent();

private:
	property_vtable()
};
property_begin(MainObjectiveComponent)
{
}
property_vend_h(MainObjectiveComponent)

class MainObjectiveSystem : public ecs::System<MainObjectiveSystem, MainObjectiveComponent>
{
public:
	MainObjectiveSystem();

private:
	void UpdateMainObjectiveComponent(MainObjectiveComponent& comp);
};
