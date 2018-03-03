#include "RLMap.hpp"

std::string GetTileDescription(RLTile& tile)
{
	std::string description = "";
	switch (tile.Type)
	{
		case '#':
			description += "a wall";
			break;
		case '.':
			description += "a stone tile";
			break;
		default:
			description += "something you don't understand";
			break;
	}

	return description;
}

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
	CLAMP(x, 0, Width - 1);
	CLAMP(y, 0, Height - 1);
	unsigned int index = y * Width + x;
	index = MIN(index, Tiles.size() - 1);
	return Tiles[index];
}