/******************************************************************************/
/*!
\file   Confirmation.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is the source file for a confirmation system that implements the
  confirmation dialog UI.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "Confirmation.h"
#include "UIScreenManager.h"

void ConfirmationComponent::Init(const std::function<void()>& inYesFunc)
{
	yesFunc = inYesFunc;
}

void ConfirmationComponent::SelectYes()
{
	yesFunc();
}

void ConfirmationSystem::OnAdded()
{
	Messaging::Subscribe("ConfirmYes", OnSelectYes);
}

void ConfirmationSystem::OnRemoved()
{
	Messaging::Unsubscribe("ConfirmYes", OnSelectYes);
}

void ConfirmationSystem::OnSelectYes()
{
	for (auto confirmIter{ ecs::GetCompsBegin<ConfirmationComponent>() }, endIter{ ecs::GetCompsEnd<ConfirmationComponent>() }; confirmIter != endIter; ++confirmIter)
		confirmIter->SelectYes();
}
