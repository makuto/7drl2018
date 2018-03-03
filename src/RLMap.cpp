#include "RLMap.hpp"

RLMap::RLMap(int width, int height) : Width(width), Height(height)
{
	Tiles.resize(width * height);

	for (RLTile& tile : Tiles)
	{
		tile.Type = FLOOR_TYPE;
		tile.Color = {FLOOR_TILE_COLOR_NORMAL};
	}
}

RLTile& RLMap::At(int x, int y)
{
	return Tiles[y * Width + x];
}