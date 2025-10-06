/******************************************************************************/
/*!
\file   ReticleSpread.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  UI Reticle that reflects the current spread of the player's held weapon.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "RenderSystem.h"
#include "EntityUID.h"

/*****************************************************************//*!
\class ReticleSpreadComponent
\brief
	The primary class for reticle.
*//******************************************************************/
class ReticleSpreadComponent : public IRegisteredComponent<ReticleSpreadComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<ReticleSpreadComponent>
#endif
{
public:
	EntityReference playerEntity;
	EntityReference reticleTop;
	EntityReference reticleBottom;
	EntityReference reticleLeft;
	EntityReference reticleRight;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ReticleSpreadComponent();
private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(ReticleSpreadComponent& comp);
#endif
	property_vtable()
};
property_begin(ReticleSpreadComponent)
{
	property_var(playerEntity),
	property_var(reticleTop),
	property_var(reticleBottom),
	property_var(reticleLeft),
	property_var(reticleRight)
}
property_vend_h(ReticleSpreadComponent)

/*****************************************************************//*!
\class ReticleSpreadSystem
\brief
	Corresponding system to update ReticleSpreadComponent.
*//******************************************************************/
class ReticleSpreadSystem : public ecs::System<ReticleSpreadSystem, ReticleSpreadComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	ReticleSpreadSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a ReticleSpreadComponent.
	\param comp
		The ReticleSpreadComponent to update.
	*//******************************************************************/
	void UpdateReticleSpreadComp(ReticleSpreadComponent& comp);
};
