/*!************************************************************************
\file       Collision.h
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#pragma once

#ifndef COLLISION_H
#define COLLISION_H

#include "pch.h"
#include "GameObject.h"

namespace Collision {

	typedef enum Collision_Flag
	{
		FLAG_RIGHT = 1,
		FLAG_LEFT,
		FLAG_BOTTOM,
		FLAG_TOP

	} Collision_Flag;

	//Collision flags
	const unsigned int	COLLISION_LEFT	 = 0x00000001;	//0001
	const unsigned int	COLLISION_RIGHT	 = 0x00000002;	//0010
	const unsigned int	COLLISION_TOP	 = 0x00000004;	//0100
	const unsigned int	COLLISION_BOTTOM = 0x00000008;	//1000

	bool checkCirclesIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2);

	bool checkSquaresIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2);

	int checkBinarySquaresIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2);

	int checkSquaresIntersectionLeftRight(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2);

	int checkSquaresIntersectionUpDown(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2);

	void snapToPosition(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2, int flag);

	void snapToPositionPlayer(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2, Collision_Flag flag);
}


#endif // COLLISION_H
