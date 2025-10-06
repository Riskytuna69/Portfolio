/******************************************************************************/
/*!
\file   DefenceStarterComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/24/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component responsible for starting the defence half of the game.


All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "ObjectiveTimer.h"
#include "MainObjective.h"
#include "AudioManager.h"
#include "Highlightable.h"

class DefenceStarterComponent 
	: public IRegisteredComponent<DefenceStarterComponent>
	, public Highlightable
	, public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, public IEditorComponent<DefenceStarterComponent>
#endif
{
public:
	AudioReference audioUse;
	EntityReference connectedEntity;
	bool isActivated;
public:
	/*****************************************************************//*!
	\brief
		Registers to the collision callback on this entity.
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Unregisters from the collision callback on this entity.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Disables the button.
	*//******************************************************************/
	static void DisableAllButtons();


	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	DefenceStarterComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~DefenceStarterComponent();

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

	void OnUseStart(KEY use) override;
private:
#ifdef IMGUI_ENABLED
		/*****************************************************************//*!
		\brief
			Editor draw function, draws the IMGui elements to allow the
			component's values to be edited. Disabled when IMGui is disabled.
		\param comp
			The component instance.
		*//******************************************************************/
		static void EditorDraw(DefenceStarterComponent& comp);
#endif
	property_vtable()
};
property_begin(DefenceStarterComponent)
{
	property_var(audioUse),
	property_var(connectedEntity),
	property_var(isActivated),
}
property_vend_h(DefenceStarterComponent)
