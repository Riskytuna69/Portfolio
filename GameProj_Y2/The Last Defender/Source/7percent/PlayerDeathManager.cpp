/******************************************************************************/
/*!
\file   PlayerDeathManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/04/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Interface for Player Death Manager component. Handles logic related to Player
  dying.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/#include "PlayerDeathManager.h"
#include "GameManager.h"
EntityReference PlayerDeathManagerComponent::playerReference;
PlayerDeathManagerComponent::PlayerDeathManagerComponent()
{
}

void PlayerDeathManagerComponent::OnAttached()
{
	Messaging::Subscribe("DiedPlayer",  &PlayerDeathManagerComponent::OnPlayerDied);
	Messaging::Subscribe("PlayerRespawned",  &PlayerDeathManagerComponent::OnPlayeRespawned);
}

void PlayerDeathManagerComponent::OnDetached()
{
	Messaging::Unsubscribe("DiedPlayer", &PlayerDeathManagerComponent::OnPlayerDied);
	Messaging::Unsubscribe("PlayerRespawned", &PlayerDeathManagerComponent::OnPlayeRespawned);
}

void PlayerDeathManagerComponent::OnPlayerDied()
{
	if (!playerReference)
		playerReference = ecs::GetCompsBegin<PlayerComponent>().GetEntity();
	playerReference->SetActive(false);
	ST<GameManager>::Get()->OnPlayerDied();
}

void PlayerDeathManagerComponent::OnPlayeRespawned()
{
	if (!playerReference)
		playerReference = ecs::GetCompsBegin<PlayerComponent>().GetEntity();

	playerReference->SetActive(true);
	playerReference->GetComp<PlayerComponent>()->UpdateHeldItems();
}
