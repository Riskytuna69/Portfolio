/******************************************************************************/
/*!
\file   ObjectiveTimer.cpp
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

	Function definations for ObjectiveTimer functions.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "ObjectiveTimer.h"
#include <GameManager.h>
std::stringstream ss;

//NOTE TO SELF - When constructor gets called the time should NOT start recording, maybe will change the behaviour in the future idk
ObjectiveTimerComponent::ObjectiveTimerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	started(false)
	, elapsedSeconds(0.0f)
{
	//This is to reset all objectiveStarted flags whenever the game gets reset
	Messaging::BroadcastAll("ObjectiveNotStarted");
}

bool ObjectiveTimerComponent::GetStarted() const 
{ 
	return started;
}

void  ObjectiveTimerComponent::IncrementElapsedSeconds(float increment) 
{ 
	elapsedSeconds += increment;
}

float ObjectiveTimerComponent::GetElapsedSeconds() const 
{ 
	return elapsedSeconds; 
}

void ObjectiveTimerComponent::OnStart()
{
	started = true;
	elapsedSeconds = 0.0f;
}

#ifdef IMGUI_ENABLED
void ObjectiveTimerComponent::EditorDraw(ObjectiveTimerComponent& comp)
{
	UNREFERENCED_PARAMETER(comp);
}
#endif

ObjectiveTimerSystem::ObjectiveTimerSystem()
	: System_Internal{ &ObjectiveTimerSystem::UpdateObjectiveTimerComp } 
{
}

void ObjectiveTimerSystem::OnAdded()
{
	Messaging::Subscribe("GAMERESTART", ObjectiveTimerSystem::RestartObjectiveTimerCallback);
	Messaging::Subscribe("GameWin", ObjectiveTimerSystem::StopTimerCallback);
}

void ObjectiveTimerSystem::OnRemoved()
{
	Messaging::Unsubscribe("GAMERESTART", ObjectiveTimerSystem::RestartObjectiveTimerCallback);
	Messaging::Unsubscribe("GameWin", ObjectiveTimerSystem::StopTimerCallback);
}

void ObjectiveTimerSystem::UpdateObjectiveTimerComp(ObjectiveTimerComponent& comp)
{
	if (ST<GameManager>::Get()->GetPaused() || !comp.GetStarted())
		return;

	// If text comp exists
	if (comp.GetStarted())
	{
		comp.IncrementElapsedSeconds(GameTime::FixedDt());
	}
}

void ObjectiveTimerSystem::RestartObjectiveTimerCallback()
{
	auto ite = ecs::GetCompsBegin<ObjectiveTimerComponent>();
	if (ite == ecs::GetCompsEnd<ObjectiveTimerComponent>())
		return;
	ite->OnStart();
}

void ObjectiveTimerSystem::StopTimerCallback()
{
	auto ite = ecs::GetCompsBegin<ObjectiveTimerComponent>();
	if (ite == ecs::GetCompsEnd<ObjectiveTimerComponent>())
		return;
	ST<GameManager>::Get()->SetTimeTaken(static_cast<int>(ite->GetElapsedSeconds()));
}
