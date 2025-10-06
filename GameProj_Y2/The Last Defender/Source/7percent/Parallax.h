/******************************************************************************/
/*!
\file   Parallax.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   05/02/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief

	Declarations for the parallax effects in game, 
	code includes different modes for different state of the game
	as well as the option to choose the intensity and direction of the effect.


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Engine.h"
#include "Player.h"

enum ParallaxMode {
	Follow,
	Opposite
};

enum GameState {
	Menu,
	InGame
};

enum Intensity {
	FrontLayer,
	MiddleLayer,
	BackLayer
};

class ParallaxComponent : public IRegisteredComponent<ParallaxComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent <ParallaxComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Constructor
	*//******************************************************************/
	ParallaxComponent();

	/*****************************************************************//*!
	\brief
		Saves the position of the entity from the previous frame into 
		variable prevTargetPos
	*//******************************************************************/
	void SetPrevTargetPos(Vector2 val);

	/*****************************************************************//*!
	\brief
		Saves the y position of the entity in menu mode 
	*//******************************************************************/
	void SetMenuYPos(float val);

	/*****************************************************************//*!
	\brief
		Returns the y position of the entity in menu mode
	*//******************************************************************/
	float GetMenuYPos();
	
	/*****************************************************************//*!
	\brief
		Returns the position of the entity from the previous frame
	*//******************************************************************/
	Vector2 GetPrevTargetPos();

	int parallaxMode;
	int gameState;
	int currIntensity;
	bool setAnchor;

	static float constexpr intensityFrontLayer = 0.4f;
	static float constexpr intensityMidLayer = 0.2f ;
	static float constexpr intensityBackLayer = 0.025f;
private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(ParallaxComponent& comp);
#endif

	Vector2 prevTargetPos;
	float menuYPos;

	property_vtable()
};
property_begin(ParallaxComponent)
{
	property_var(parallaxMode),
	property_var(gameState),
	property_var(currIntensity),
}
property_vend_h(ParallaxComponent)

class ParallaxSystem : public ecs::System<ParallaxSystem, ParallaxComponent>
{
public:
	ParallaxSystem();
	
private:
	void UpdateParallaxComp(ParallaxComponent& comp);

};