#pragma once

#include <vector>

#include "RLColor.hpp"

typedef char RLTileType;

struct RLTile
{
	RLTileType Type;

	// Add effects, color etc. later
	RLColor Color;
};

struct RLMap
{
	int Width;
	int Height;

	std::vector<RLTile> Tiles;

	RLMap(int width, int height);

	RLTile& At(int x, int y);
};