/******************************************************************************/
/*!
\file   PersistentInteractableComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	PersistentInteractableComponent is an ECS component inheriting from Highlightable
	class, identifying a scene entity as a persistent interactable object, such
	as a lever or button.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Highlightable.h"
#include "AudioManager.h"

/*****************************************************************//*!
\enum INTERACTABLE_TYPE
\brief
	Various types of interactables.
*//******************************************************************/
enum class INTERACTABLE_TYPE : size_t
{
	SINGLE_USE = 0,
	TOGGLEABLE,
	LEVER_2,
	SECRET_BUTTON
};

/*****************************************************************//*!
\class PersistentInteractableComponent
\brief
	ECS component.
*//******************************************************************/
class PersistentInteractableComponent
	: public IRegisteredComponent<PersistentInteractableComponent>
	, public Highlightable
#ifdef IMGUI_ENABLED
	, public IEditorComponent<PersistentInteractableComponent>
#endif
{
public:
	INTERACTABLE_TYPE type;

	EntityReference connectedEntity;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	PersistentInteractableComponent();

	/*****************************************************************//*!
	\brief
		Override of Highlightable::OnUseStart(), which is called once
		when the given key is pressed.
	\param use
		The key that is pressed.
	*//******************************************************************/
	void OnUseStart(KEY use) override;

	/*****************************************************************//*!
	\brief
		Serializes various types that aren't supported by default.
	\param writer
		Serializer reference.
	*//******************************************************************/
	void Serialize(Serializer& writer) const override;

	/*****************************************************************//*!
	\brief
		Deserializes various types that aren't supported by default.
	\param writer
		Deserializer reference.
	*//******************************************************************/
	void Deserialize(Deserializer& reader) override;

	/*****************************************************************//*!
	\brief
		Does nothing if this interactable has already been used once.
		Otherwise, calls the base class function in Highlightable.
	\param highlighted
		True for highlighted, false for not.
	*//******************************************************************/
	void SetIsHighlighted(bool highlighted);

private:
	bool activated;
	AudioReference audioUse;

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(PersistentInteractableComponent& comp);
#endif
	property_vtable()
};
property_begin(PersistentInteractableComponent)
{
	property_var(name),
	property_var(connectedEntity),
}
property_vend_h(PersistentInteractableComponent)
