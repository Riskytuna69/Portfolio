/******************************************************************************/
/*!
\file   HitFlash.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Hitflash effect which is called (and should only be called) by HealthComponent.
  As such, ensure HitFlash component is on the same entity as HealthComponent.
  It only cares about render components within children.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "RenderSystem.h"
#include "EntityUID.h"

/*****************************************************************//*!
\class HitFlashComponent
\brief
	The primary class for hitflash.
*//******************************************************************/
class HitFlashComponent : public IRegisteredComponent<HitFlashComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<HitFlashComponent>
#endif
	, public ecs::IComponentCallbacks
{
public:
	std::string flashMaterialName;
	std::vector<EntityReference> vecRenderComps;
	bool inited;
	float timer;
	float duration;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	HitFlashComponent();

	/*****************************************************************//*!
	\brief
		Tells the component to make the child rendercomps flash white for a
		specified duration.
	\param seconds
		Duration in seconds.
	*//******************************************************************/
	void FlashForSeconds(float seconds);

	/*****************************************************************//*!
	\brief
		Resets the material of child rendercomps.
	*//******************************************************************/
	void ReturnToOriginalMaterial();

	/*****************************************************************//*!
	\brief
		Stores child rendercomps as entity references and keeps track of
		the original materials for each rendercomp.
	*//******************************************************************/
	void Init();

	/*****************************************************************//*!
	\brief
		Recursively stores child rendercomps as entity references and keeps
		track of the original materials for each rendercomp. If it encounters
		a HitFlash component, it will ignore its children.
	\param children
		The set of children to store.
	*//******************************************************************/
	void StoreRenderComps(std::set<Transform*> const& children);

	/*****************************************************************//*!
	\brief
		Resets the material of child rendercomps when removed from the entity.
	*//******************************************************************/
	void OnDetached() override;

private:
	std::vector<std::string> defaultMaterialNames;

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(HitFlashComponent& comp);
#endif
	property_vtable()
};
property_begin(HitFlashComponent)
{
	property_var(flashMaterialName)
}
property_vend_h(HitFlashComponent)

/*****************************************************************//*!
\class HitFlashSystem
\brief
	Corresponding system to update HitFlashComponent. Performs per-frame
	update to count timers. Affected by GameTime::FixedDt().
*//******************************************************************/
class HitFlashSystem : public ecs::System<HitFlashSystem, HitFlashComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	HitFlashSystem();
private:
	/*****************************************************************//*!
	\brief
		Updates a HitFlashSystem.
	\param comp
		The HitFlashComponent to update.
	*//******************************************************************/
	void UpdateHitFlashComponent(HitFlashComponent& comp);
};