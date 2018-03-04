#include "noise/noise.hpp"

#include "Levels.hpp"

#include "RLMap.hpp"
#include "Game.hpp"

void createTestMap(RLMap& map)
{
	for (int tileY = 0; tileY < map.Height; ++tileY)
	{
		for (int tileX = 0; tileX < map.Width; ++tileX)
		{
			if ((tileY == 0 || tileY == map.Height - 1) || (tileX == 0 || tileX == map.Width - 1))
			{
				RLTile* currentTile = map.At(tileX, tileY);
				currentTile->Type = WALL_TYPE;
				currentTile->Color = {WALL_TILE_COLOR_NORMAL};
			}
		}
	}
}

void createTestMapNoise(RLMap& map)
{
	static Noise2d noise(3158008);
	const float scale = 0.1;
	for (int tileY = 0; tileY < map.Height; ++tileY)
	{
		for (int tileX = 0; tileX < map.Width; ++tileX)
		{
			float noiseValue = noise.scaledOctaveNoise2d(tileX, tileY, 0, 255, 10, scale, 0.55, 2);
			if (noiseValue < 100.f)
			{
				RLTile* currentTile = map.At(tileX, tileY);
				currentTile->Type = WALL_TYPE;
				currentTile->Color = {WALL_TILE_COLOR_NORMAL};
			}
		}
	}
}

void LoadNextLevel()
{
	gameState.npcs.clear();
	
	gameState.currentMap.Tiles.resize(gameState.currentMap.Width * gameState.currentMap.Height);
	Noise2d noise(982347);
	const float scale = 0.1;
	for (int tileY = 0; tileY < gameState.currentMap.Height; ++tileY)
	{
		for (int tileX = 0; tileX < gameState.currentMap.Width; ++tileX)
		{
			float noiseValue = noise.scaledOctaveNoise2d(tileX, tileY, 0, 255, 10, scale, 0.55, 2);
			if (noiseValue < 100.f)
			{
				RLTile* currentTile = gameState.currentMap.At(tileX, tileY);
				currentTile->Type = WALL_TYPE;
				currentTile->Color = {WALL_TILE_COLOR_NORMAL};
			}
		}
	}
}