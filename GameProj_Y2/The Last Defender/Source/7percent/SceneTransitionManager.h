/******************************************************************************/
/*!
\file   SceneTransitionManager.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Interface for scene transition functions.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#pragma once

class SceneTransitionManager
{
	friend class ST<SceneTransitionManager>;
public:
	/*****************************************************************//*!
	\brief
		Checks if there is a transition programmed for the given pair of scenes.
	\param currSceneName
		The current scene.
	\param nextSceneName
		The next scene.
	*//******************************************************************/
	void TransitionScene(std::string currSceneName, std::string nextSceneName);
};