/******************************************************************************/
/*!
\file   ObjectiveTimer.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/28/2024

\author Chua Wen Shing Bryan (30%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\author Marc Alviz Evangelista (70%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief
	This file controls the logic of the main objective timer 

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "TextSystem.h"
#include "EnemyStateMachine.h"
#include "IGameComponentCallbacks.h"

class ObjectiveTimerComponent
	: public IRegisteredComponent<ObjectiveTimerComponent>
	, public IGameComponentCallbacks<ObjectiveTimerComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<ObjectiveTimerComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	ObjectiveTimerComponent();

	/*****************************************************************//*!
	\brief
		Getter
	\return
		Returns whether the timer has started
	*//******************************************************************/
	bool GetStarted() const;

	/*****************************************************************//*!
	\brief
		Getter
	\return
		Returns the elapsed seconds
	*//******************************************************************/
	float GetElapsedSeconds() const;

	/*****************************************************************//*!
	\brief
		Increments the elpased seconds by game dt
	\return
	*//******************************************************************/
	void IncrementElapsedSeconds(float increment);
	
	/*****************************************************************//*!
	\brief
		Sets variables when play button is pressed
	\return
	*//******************************************************************/
	void OnStart() override;

private:
	bool started;
	float elapsedSeconds;

#ifdef IMGUI_ENABLED

	/*****************************************************************//*!
	\brief
		Shows variables in inspector that can be modified
	\param[in, out] comp
		Component the variables are found in
	\return
	*//******************************************************************/
	static void EditorDraw(ObjectiveTimerComponent& comp);
#endif
	property_vtable()
};
property_begin(ObjectiveTimerComponent)
{
}
property_vend_h(ObjectiveTimerComponent)

/*****************************************************************//*!
\brief
	Controls the logic for entities with "ObjectiveTimerComponent"
*//******************************************************************/
class ObjectiveTimerSystem : public ecs::System<ObjectiveTimerSystem, ObjectiveTimerComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	ObjectiveTimerSystem();

	/*****************************************************************//*!
	\brief
		Subscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes to multiple broadcasts.
	\return
	*//******************************************************************/
	void OnRemoved() override;
private:

	/*****************************************************************//*!
	\brief
		Updates the Objective Timer Component of game objects
	\param[in, out] comp
		Component to update
	\return
	*//******************************************************************/
	void UpdateObjectiveTimerComp(ObjectiveTimerComponent& comp);

	/*****************************************************************//*!
	\brief
		Callback function to restart variables.
	\return
	*//******************************************************************/
	static void RestartObjectiveTimerCallback();

	/*****************************************************************//*!
	\brief
		Callback function to stop the timer and update GameManager with
		the value.
	\return
	*//******************************************************************/
	static void StopTimerCallback();
};
