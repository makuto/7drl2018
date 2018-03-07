#pragma once

#include <string>
#include <vector>

#include "RLColor.hpp"

typedef char RLTileType;

struct RLTile
{
	RLTileType Type;

	// Add effects, color etc. later
	RLColor Color;

	std::string DescriptionOverride;
};

std::string GetTileDescription(RLTile& tile);

struct RLMap
{
	int Width;
	int Height;

	std::vector<RLTile> Tiles;

	RLMap();
	RLMap(int width, int height);

	void ResetTiles();

	void SetSize(int width, int height);

	RLTile* At(int x, int y);
};

void DrawWorld(RLMap& map, int camXOffset, int camYOffset);