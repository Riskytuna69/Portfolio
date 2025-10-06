/******************************************************************************/
/*!
\file   PauseSystem.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	PauseSystem is an ECS system that does controls logic for pause screen
	while the default scene is loaded (game scene).

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once

/*****************************************************************//*!
\class PauseSystem
\brief
	ECS System.
*//******************************************************************/
class PauseSystem : public ecs::System<PauseSystem>
{
public:
	/*****************************************************************//*!
	\brief
		Update function that is called continuously.
	*//******************************************************************/
	bool PreRun() override;

private:

};