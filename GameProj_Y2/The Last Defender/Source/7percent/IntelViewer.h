/******************************************************************************/
/*!
\file   IntelViewer.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  UI for viewing intel entries (short stories) collected throughout the game.
  Attached to pause scene.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "EntityUID.h"

/*****************************************************************//*!
\class IntelViewerComponent
\brief
	The primary class for intel viewer.
*//******************************************************************/
class IntelViewerComponent : public IRegisteredComponent<IntelViewerComponent>,
	public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<IntelViewerComponent>
#endif
{
public:
	// Intel Viewer Component lives on the highest hierarchy object within the pause scene.
	// It tracks both the pause UI and the intel UI to handle turning them off and on.
	EntityReference textSpawnPoint; // Spawn point for text prefab

	std::string blockerPrefabName;

	int numIntel;
	std::vector<EntityReference> vecIntel;
	std::unordered_map<int, EntityReference> mapBlockers;

	// Track the last spawned text prefab
	EntityReference textPrefab;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	IntelViewerComponent();

private:
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(IntelViewerComponent& comp);
#endif
	property_vtable()
};
property_begin(IntelViewerComponent)
{
	property_var(textSpawnPoint),
	property_var(blockerPrefabName),
	property_var(numIntel),
	property_var(vecIntel),
}
property_vend_h(IntelViewerComponent)

/*****************************************************************//*!
\class IntelViewerSystem
\brief
	Corresponding system to update IntelViewerComponent. It does not
	perform per-frame checks by itself, it exists solely to act as a
	message receiver to perform logic on the IntelViewerComponent when
	certain buttons within the pause scene are pressed.
*//******************************************************************/
class IntelViewerSystem : public ecs::System<IntelViewerSystem, IntelViewerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	IntelViewerSystem();

	/*****************************************************************//*!
	\brief
		Subscribes to relevant events.
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes from relevant events.
	*//******************************************************************/
	void OnRemoved() override;

	/*****************************************************************//*!
	\brief
		Messaging callback function when "CheckIntelUnlocks" is received.
	*//******************************************************************/
	static void CheckIntelUnlocks();

	/*****************************************************************//*!
	\brief
		Messaging callback function when "ViewIntel" is received.
	*//******************************************************************/
	static void ViewIntel(int num);

private:
	/*****************************************************************//*!
	\brief
		Updates a IntelViewerComponent. Currently doesn't do anything.
	\param comp
		The IntelViewerComponent to update.
	*//******************************************************************/
	void UpdateIntelViewerComp(IntelViewerComponent& comp);
};