/******************************************************************************/
/*!
\file   Rotater.h
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
#pragma once

/*****************************************************************//*!
\class RotaterComponent
\brief
	The primary class for rotater.
*//******************************************************************/
class RotaterComponent : public IRegisteredComponent<RotaterComponent>,
	public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<RotaterComponent>
#endif
{
public:
	float rotationSpeed; // In degrees per second

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	RotaterComponent();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(RotaterComponent& comp);
#endif
	property_vtable()
};
property_begin(RotaterComponent)
{
	property_var(rotationSpeed)
}
property_vend_h(RotaterComponent)

/*****************************************************************//*!
\class RotaterSystem
\brief
	Corresponding system to update RotaterComponent.
*//******************************************************************/
class RotaterSystem : public ecs::System<RotaterSystem, RotaterComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	RotaterSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a RotaterComponent.
	\param comp
		The RotaterComponent to update.
	*//******************************************************************/
	void UpdateRotaterComp(RotaterComponent& comp);
};