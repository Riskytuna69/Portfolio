/******************************************************************************/
/*!
\file   Confirmation.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is an interface file for a confirmation system that implements the
  confirmation dialog UI.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/


#pragma once

/*****************************************************************//*!
\class ConfirmationComponent
\brief
	Contains the function to call when the user confirms a confirmation dialog.
*//******************************************************************/
class ConfirmationComponent
	: public IRegisteredComponent<ConfirmationComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Sets the function to be called when the user confirms a confirmation dialog.
	\param inYesFunc
		The function to be called when the user confirms a confirmation dialog.
	*//******************************************************************/
	void Init(const std::function<void()>& inYesFunc);

	/*****************************************************************//*!
	\brief
		Executes the function.
	*//******************************************************************/
	void SelectYes();

private:
	//! The function to be called when the user confirms a confirmation dialog.
	std::function<void()> yesFunc;

	property_vtable()
};
property_begin(ConfirmationComponent)
{
}
property_vend_h(ConfirmationComponent)

/*****************************************************************//*!
\class ConfirmationSystem
\brief
	Listens for "ConfirmYes" event.
*//******************************************************************/
class ConfirmationSystem : public ecs::System<ConfirmationSystem>
{
public:
	/*****************************************************************//*!
	\brief
		Subscribes to the event "ConfirmYes".
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes from the event "ConfirmYes".
	*//******************************************************************/
	void OnRemoved() override;

private:
	/*****************************************************************//*!
	\brief
		Called when the event "ConfirmYes" is fired.
	*//******************************************************************/
	static void OnSelectYes();
};
