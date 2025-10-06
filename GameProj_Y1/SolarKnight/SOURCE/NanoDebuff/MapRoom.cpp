/*!************************************************************************
\file       MapRoom.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "MapRoom.h"

namespace Map2 {
	// Constructor
	MapRoom::MapRoom(int* inMapDataArray, int* inMapBGArray, int inMaxRows, int inMaxCols, int inRoomID)
		: dataGrid{ inMapDataArray },
		bgGrid{ inMapBGArray },
		sizeYRows{ inMaxRows },
		sizeXCols{ inMaxCols },
		roomID{ inRoomID }
	{
	}

	// Destructor
	MapRoom::~MapRoom()
	{
		// Delete room arrays
		delete[] dataGrid;
		delete[] bgGrid;
	}

	// MapLevel and Gameplay use ===================================================================

	// Returns General data grid (collsion, enemy spawn etc.).
	int const* MapRoom::DataGrid() const
	{
		return dataGrid;
	}

	// Returns BG Grid, for displaying BG textures ONLY (non collidable).
	int const* MapRoom::BGGrid() const
	{
		return bgGrid;
	}

	// Returns number of columns (0->X).
	int const& MapRoom::SizeCols() const
	{
		return sizeXCols;
	}

	// Returns number of rows (0->Y).
	int const& MapRoom::SizeRows() const
	{
		return sizeYRows;
	}

	// Returns unique ID of room.
	int const& MapRoom::RoomID() const
	{
		return roomID;
	}
}