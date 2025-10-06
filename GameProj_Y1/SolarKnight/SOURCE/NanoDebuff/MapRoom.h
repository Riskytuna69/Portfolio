/*!************************************************************************
\file       MapRoom.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once
#include "MapLevel.fwd.h"

namespace Map2 {
	class MapRoom
	{
	public:
		// Constructor
		MapRoom(int* inMapDataArray, int* inMapBGArray, int inMaxRows, int inMaxCols, int inRoomID);
		// Destructor
		~MapRoom();

		// Prevents other constructors (ONLY use defined constructor)
		MapRoom()							= delete;
		MapRoom(MapRoom const&)				= delete;
		MapRoom& operator=(MapRoom const&)	= delete;
		
		// MapLevel and Gameplay use ===================================================================
		// Arrays
		int const*	DataGrid()		const;	// Get
		int const*	BGGrid()		const;	// Get

		// Constants
		int const&	SizeCols()		const;	// Get
		int const&	SizeRows()		const;	// Get

		int const&	RoomID()		const;	// Get

	private:
		// Grid Arrays
		int const* dataGrid;		// General data (collidable tiles, enemy spawn etc.).
		int const* bgGrid;			// For displaying BG textures ONLY (non collidable).

		// Room size
		int const		sizeYRows;		// Number of rows (max Y).
		int const		sizeXCols;		// Number of columns (max X).

		// Functionality variables
		int const		roomID;			// Unique ID given to easily find this room.
	};
}
