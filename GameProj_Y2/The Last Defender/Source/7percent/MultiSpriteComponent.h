/******************************************************************************/
/*!
\file   MultiSpriteComponent.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	MultiSpriteComponent is an ECS component that serializes a varied number of
	spriteIDs to be used for entities that need to render as different textures
	in a convenient manner.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "ResourceManager.h"

/*****************************************************************//*!
\class MultiSpriteComponent
\brief
	Requires RenderComponent on the same entity to work correctly!!!
*//******************************************************************/
class MultiSpriteComponent : public IRegisteredComponent<MultiSpriteComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<MultiSpriteComponent>
#endif
{
public:
	int numSprites;
	std::vector<size_t> spriteID_Vec;

	/*****************************************************************//*!
	\brief
		Change this entity's sprite to the one at spriteIndex of the
		serialized vector of spriteIDs.
	\param spriteIndex
		Index of vector.
	*//******************************************************************/
	void ChangeSprite(int spriteIndex);

	/*****************************************************************//*!
	\brief
		Change this entity's sprite to a specific spriteID.
	\param spriteID
		spriteID that corresponds to one in ResourceManager.
	*//******************************************************************/
	void ChangeSpriteID(int spriteID);

	/*****************************************************************//*!
	\brief
		You can attain the currently rendered sprite by calling this function.
	\return
		Const reference to sprite object.
	*//******************************************************************/
	Sprite const& GetSprite();

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	MultiSpriteComponent();

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(MultiSpriteComponent& comp);
#endif
private:
	int currentSpriteIndex;

	property_vtable()
};
property_begin(MultiSpriteComponent)
{
	property_var(numSprites),
	property_var(spriteID_Vec)
}
property_vend_h(MultiSpriteComponent)
