#pragma once
#include "GameSettings.h"

/******************************************************************************/
/*!
\file   CheckBoxComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   13/03/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
	Header for the checkbox component to toggle fullscreen
	checkbox only toggles fullscreen for now

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

class CheckBoxComponent : public IRegisteredComponent<CheckBoxComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent <CheckBoxComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Constructor
	*//******************************************************************/
	CheckBoxComponent();

	/*****************************************************************//*!
	\brief
		Function runs when component is attached to an entity
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Function runs when component is dettached to an entity
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Returns the value of isChecked variable
	*//******************************************************************/
	bool GetIsChecked();

	size_t GetSpriteIDPressed();
	size_t GetSpriteIDUnPressed();

	/*****************************************************************//*!
	\brief
		Returns the status of the initalsprite being set first
	*//******************************************************************/
	bool GetInitialSpriteSet();

	void SetSpriteIDPressed(size_t val);
	void SetSpriteIDUnPressed(size_t val);

	/*****************************************************************//*!
	\brief
		Sets the value of isChecked variable
	*//******************************************************************/
	void SetIsChecked(bool val);

	/*****************************************************************//*!
	\brief
		Sets the status of the initalsprite being set first
	*//******************************************************************/
	void SetInitialSpriteSet(bool val);

private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(CheckBoxComponent& comp);
#endif
	bool isChecked;
	size_t	spriteID_Pressed;
	size_t	spriteID_Unpressed;
	bool setInitial;

	property_vtable()
};
property_begin(CheckBoxComponent)
{
	property_var(isChecked),
	property_var(spriteID_Pressed),
	property_var(spriteID_Unpressed)
}
property_vend_h(CheckBoxComponent)


/*****************************************************************//*!
	\brief
		CheckBoxSystem
*//******************************************************************/
class CheckBoxSystem : public ecs::System<CheckBoxSystem, CheckBoxComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	CheckBoxSystem();

private:
	void UpdateCheckBoxComp(CheckBoxComponent& comp);

	/*****************************************************************//*!
	\brief
		Function gets called when checkbox is clicked
	*//******************************************************************/
	void onCheckBoxClicked(CheckBoxComponent& comp);
};

