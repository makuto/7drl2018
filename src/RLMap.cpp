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

RLTile* RLMap::At(int x, int y)
{
	unsigned int index = y * Width + x;
	if (x < 0 || y < 0 || x >= Width || y >= Width || index >= Tiles.size())
		return nullptr;
	return &Tiles[index];
}