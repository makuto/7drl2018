#include "Levels.hpp"

#include "RLMap.hpp"

void createTestMap(RLMap& map)
{
	for (int camY = 0; camY < map.Height; ++camY)
	{
		for (int camX = 0; camX < map.Width; ++camX)
		{
			if ((camY == 0 || camY == map.Height - 1) || (camX == 0 || camX == map.Width - 1))
			{
				RLTile& currentTile = map.At(camX, camY);
				currentTile.Type = WALL_TYPE;
				currentTile.Color = {WALL_TILE_COLOR_NORMAL};
			}
		}
	}
}