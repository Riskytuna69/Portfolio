/******************************************************************************/
/*!
\file   GameOver.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   27/11/2024

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Component and system to manage game win/loss conditions as well as giving the
  option to restart the level when the session has ended.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "RenderSystem.h"
#include "IGameComponentCallbacks.h"
#include "EntityUID.h"

/*****************************************************************//*!
\enum PLAYSTATE
\brief
	Enumerator denoting the state of the current play session.
*//******************************************************************/
enum class PLAYSTATE : int
{
	PLAY,				// Default state
	END_ANIM_LOSE_1,	// Animation State
	END_ANIM_LOSE_2,	// Animation State
	END_ANIM_WIN_1,		// Animation State
	END_ANIM_WIN_2,		// Animation State
	END_ANIM_WIN_3,		// Animation State
	PENDING_RESTART		// Pending a keypress from the player to restart the game
};

/*****************************************************************//*!
\class GameOverComponent
\brief
	Component to attach to GameOver entity in scene.

	In order for the current implementation of this component to work,
	ensure that there is a RenderComponent attached to this entity and
	there are two child entities with TextComponent attached.
*//******************************************************************/
class GameOverComponent
	: public IRegisteredComponent<GameOverComponent>
	, public IGameComponentCallbacks<GameOverComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<GameOverComponent>
#endif
{
public:
	float duration_FadeIn;
	float duration_FadeOut;
	float duration_LoseWait;
	float duration_WinTextMovingApart;
	float duration_WinScoreboardAppearing;
	float duration_PerScore; // Not serialised

	// WIN_ANIM_2
	float gameStatus_YOffset;
	float gameStatus_ScaleMultiplier;
	float spacebar_YOffset;
	float spacebar_ScaleMultiplier;

	// WIN_ANIM_3
	float score_YOffset;
	float score_YInterval;
	EntityReference scoreboardPosition;

	// Standard anim text
	EntityReference text_Spacebar;
	EntityReference text_GameStatus;

	// Scoreboard
	std::vector<EntityReference> vecScores;
	int currentScoreID;

	float internalTimer;
	float scoreTimer;
	Vector2 text_Spacebar_DefaultLocalPosition;
	Vector2 text_GameStatus_DefaultLocalPosition;
	bool inited;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	GameOverComponent();

	/*****************************************************************//*!
	\brief
		Get the stored playState.
	\return
		PLAYSTATE enumerator.
	*//******************************************************************/
	PLAYSTATE GetPlayState();

	/*****************************************************************//*!
	\brief
		Set the stored playState.
	\param state
		PLAYSTATE enumerator.
	*//******************************************************************/
	void SetPlayState(PLAYSTATE state);

	/*****************************************************************//*!
	\brief
		Gets default positions of restartable tween text.
	*//******************************************************************/
	void Init();

private:
	PLAYSTATE playState;

	/*****************************************************************//*!
	\brief
		Draws this component to the inspector window.
	\param comp
		The component to draw.
	*//******************************************************************/
#ifdef IMGUI_ENABLED
	static void EditorDraw(GameOverComponent& comp);
#endif
	property_vtable()
};
property_begin(GameOverComponent)
{
	property_var(duration_FadeIn),
	property_var(duration_FadeOut),
	property_var(duration_LoseWait),
	property_var(duration_WinTextMovingApart),
	property_var(duration_WinScoreboardAppearing),

	property_var(gameStatus_YOffset),
	property_var(gameStatus_ScaleMultiplier),
	property_var(spacebar_YOffset),
	property_var(spacebar_ScaleMultiplier),

	property_var(score_YOffset),
	property_var(score_YInterval),
	property_var(scoreboardPosition),

	property_var(text_Spacebar),
	property_var(text_GameStatus)
}
property_vend_h(GameOverComponent)

/*****************************************************************//*!
\class GameOverSystem
\brief
	System that operates on the GameOverComponent.
*//******************************************************************/
class GameOverSystem : public ecs::System<GameOverSystem, GameOverComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	GameOverSystem();

	/*****************************************************************//*!
	\brief
		Subscribes the system to relevant messages.
	*//******************************************************************/
	void OnAdded() override;

	/*****************************************************************//*!
	\brief
		Unsubscribes the system from relevant messages.
	*//******************************************************************/
	void OnRemoved() override;

	/*****************************************************************//*!
	\brief
		Callback function for when the game ends in a loss.
	*//******************************************************************/
	static void OnDeath();

	/*****************************************************************//*!
	\brief
		Callback function for when the game ends in a win.
	*//******************************************************************/
	static void OnGameWin();

private:
	/*****************************************************************//*!
	\brief
		Required to update anim timer and check for keypress to restart
		the game.
	\param comp
		Component to update.
	*//******************************************************************/
	void UpdateGameOver(GameOverComponent& comp);
};
