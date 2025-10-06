/*!************************************************************************
\file       MapDoor.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once
#include "MapRoom.fwd.h"
#include "MapLevel.fwd.h"
#include "GameObject.fwd.h"

namespace Map2 {
	class MapDoor
	{
	public:
		enum class DOOR_EXIT_DIR
		{
			EXIT_UNDEFINED = 0,

			EXIT_LEFT,
			EXIT_RIGHT,
			EXIT_UP,
			EXIT_DOWN,

			EXIT_COUNT
		};


		// Constructor
		MapDoor(MapRoom* inLocatedInRoom, int inDoorLinkID, int inPosX, int inPosY, DOOR_EXIT_DIR inExitDir, int inDoorExtraHeight);

		// Prevents other constructors (ONLY use defined constructor)
		MapDoor()							= delete;
		MapDoor(MapDoor const&)				= delete;
		MapDoor& operator=(MapDoor const&)	= delete;

		// MapLevel use ================================================================================
		int const&						DoorLinkageID()		const;					// Get doorLinkageID

		void							LinkedDoor(MapDoor* doorToLink);			// Set
		MapDoor* const					LinkedDoor()		const;					// Get


		// Gameplay use ================================================================================
		DOOR_EXIT_DIR const&			ExitDirection()		const;					// Get

		int const&						DoorExtraHeight()	const;					// Get

		MapRoom* const					RoomLocatedIn()		const;					// Get

		void							GetDoorPosXY(int& posX, int& posY) const;	// Return pos in ints given


		// GameObject (Collision checking use) =========================================================
		// Used to compare to find which door was the one that got collided.
		void							AssignedGO(GameObject::GameObject* newGO);	// Set
		GameObject::GameObject const*	AssignedGO()		const;					// Get
		

	private:
		// Position variables
		int const					gridPosX;		// Position X. Starts from (0,0) at top left.
		int const					gridPosY;		// Position Y. Starts from (0,0) at top left.
		MapRoom* const				locatedInRoom;	// Room which Door is located in.

		// Functionality variables
		DOOR_EXIT_DIR const			exitDir;		// Door Exit Direction (Where player will be offset when entering room).
		int const					doorLinkageID;	// Unique ID that is used, to make different door objects "link" to each other,
													// allowing player to travel between them. [one-time-use]
		MapDoor*					linkedToDoor;	// The other door object that is linked with this

		// Collision functionality
		GameObject::GameObject*		linkedGO;		// The gameobject that is linked to this door object.


		// Extra variables
		int const					extraHeight;	// How high the hitbox will be for the door
													// Default height of 1, extraHeight up to 9
													// Totaling 10.
	};
}

