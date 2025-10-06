/*!************************************************************************
\file       ObjectPhysics.h
\project    Solar Knight
\author(s)  Min Khant Ko (ko.m) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

#include "pch.h"
#include "GameObject.h"

namespace ObjectPhysics {

	void applyGravity(GameObject::GameObject* gameObject1, f32 gravityValue);

	void preventBelowGround(GameObject::GameObject* gameObject);

	void preventBelowGround(AEVec2 coordinatesXY);

	void updatePosition(GameObject::GameObject* gameObject, f32 gameDeltaTime);

	void applyKnockback(GameObject::GameObject* gameObject, f32 gameDeltaTime);
}