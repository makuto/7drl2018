#include "math/math.hpp"
#include "noise/noise.hpp"

#include "Globals.hpp"
#include "Levels.hpp"

#include "Enemies.hpp"
#include "Game.hpp"
#include "RLMap.hpp"

void placeEntityWithinSquareRandomSensibly(RLEntity* entity, int xCenter, int yCenter, int radius)
{
	for (int i = 0; i < MAX_PLACEMENT_ATTEMPTS; i++)
	{
		int randomRange = (radius * 2) + 1;
		int xPos = ((rand() % randomRange) - radius) + xCenter;
		int yPos = ((rand() % randomRange) - radius) + yCenter;

		RLTile* tileAt = gameState.currentMap.At(xPos, yPos);
		// TODO: Add check to see if player could get to them
		if (!tileAt || tileAt->Type != FLOOR_TYPE)
			continue;

		if (xPos == gameState.player.X && yPos == gameState.player.Y)
			continue;

		if (manhattanTo(xPos, yPos, gameState.player.X, gameState.player.Y) < 3.f)
			continue;

		// Met all our criteria
		entity->X = xPos;
		entity->Y = yPos;
		return;
	}

	LOGE << "Could not place entity " << entity->Description.c_str() << " within "
	     << MAX_PLACEMENT_ATTEMPTS << " attempts! Seed " << gameState.seed << " level "
	     << gameState.currentLevel;
	// We failed 1ato find a place quickly. Pick something arbitrary
	entity->X = xCenter + 1;
	entity->Y = yCenter;
}

void placeEntityRandomSensibly(RLEntity* entity)
{
	for (int i = 0; i < MAX_PLACEMENT_ATTEMPTS; i++)
	{
		int xPos = rand() % gameState.currentMap.Width;
		int yPos = rand() % gameState.currentMap.Height;

		RLTile* tileAt = gameState.currentMap.At(xPos, yPos);
		// TODO: Add check to see if player could get to them
		if (!tileAt || tileAt->Type != FLOOR_TYPE)
			continue;

		if (xPos == gameState.player.X && yPos == gameState.player.Y)
			continue;

		if (manhattanTo(xPos, yPos, gameState.player.X, gameState.player.Y) < 3.f)
			continue;

		// Met all our criteria
		entity->X = xPos;
		entity->Y = yPos;
		return;
	}

	LOGE << "Could not place entity " << entity->Description.c_str() << " within "
	     << MAX_PLACEMENT_ATTEMPTS << " attempts! Seed " << gameState.seed << " level "
	     << gameState.currentLevel;
	// We failed to find a place quickly. Pick something arbitrary
	entity->X = 1;
	entity->Y = 1;
}

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

void createHellscape()
{
	gameState.levelName = "Hellscape";
	gameState.currentMap.SetSize(500, 500);
	Noise2d noise(gameState.seed * gameState.currentLevel);
	const float scale = 0.1;
	for (int tileY = 0; tileY < gameState.currentMap.Height; ++tileY)
	{
		for (int tileX = 0; tileX < gameState.currentMap.Width; ++tileX)
		{
			RLTile* currentTile = gameState.currentMap.At(tileX, tileY);
			float noiseValue = noise.scaledOctaveNoise2d(tileX, tileY, 0, 255, 10, scale, 0.55, 2);
			// Wall
			if (noiseValue < 100.f)
			{
				currentTile->Type = WALL_TYPE;
				currentTile->Color = {WALL_TILE_COLOR_NORMAL};
			}
			// Ground
			else
			{
				currentTile->Type = FLOOR_TYPE;
				// Add some variation
				if (noiseValue > 150.f)
				{
					currentTile->Color = {FLOOR_TILE_COLOR_NORMAL};
					currentTile->DescriptionOverride.clear();
				}
				else
				{
					currentTile->Color = {HELL_TILE_COLOR_NORMAL};
					// TODO: Dumb
					currentTile->DescriptionOverride = "a patch of hot earth";
				}
			}
		}
	}
}

void createBarren()
{
	gameState.levelName = "Barren";
	gameState.currentMap.SetSize(300, 300);
	Noise2d noise(gameState.seed * gameState.currentLevel);
	const float scale = 0.1;
	for (int tileY = 0; tileY < gameState.currentMap.Height; ++tileY)
	{
		for (int tileX = 0; tileX < gameState.currentMap.Width; ++tileX)
		{
			RLTile* currentTile = gameState.currentMap.At(tileX, tileY);
			float noiseValue = noise.scaledOctaveNoise2d(tileX, tileY, 0, 255, 10, scale, 0.55, 2);
			// Wall
			if (noiseValue < 100.f)
			{
				currentTile->Type = WALL_TYPE;
				currentTile->Color = {WALL_TILE_COLOR_NORMAL};
			}
			// Ground
			else
			{
				currentTile->Type = FLOOR_TYPE;
				currentTile->Color = {FLOOR_TILE_COLOR_NORMAL};
			}
		}
	}
}

void createForest()
{
	gameState.levelName = "Forest";
	gameState.currentMap.SetSize(100, 100);
	Noise2d noise(gameState.seed * gameState.currentLevel);
	const float scale = 0.1;
	for (int tileY = 0; tileY < gameState.currentMap.Height; ++tileY)
	{
		for (int tileX = 0; tileX < gameState.currentMap.Width; ++tileX)
		{
			RLTile* currentTile = gameState.currentMap.At(tileX, tileY);
			float noiseValue = noise.scaledOctaveNoise2d(tileX, tileY, 0, 255, 10, scale, 0.55, 2);
			// Tree
			if (noiseValue < 100.f)
			{
				currentTile->Type = TREE_TYPE;
				currentTile->Color = {TREE_TILE_COLOR_NORMAL};
				currentTile->DescriptionOverride = "a tree";
			}
			// Ground
			else
			{
				currentTile->Type = FLOOR_TYPE;

				// Add some variation
				if (noiseValue > 150.f)
				{
					currentTile->Color = {FLOOR_TILE_COLOR_NORMAL};
					currentTile->DescriptionOverride.clear();
				}
				else
				{
					currentTile->Color = {GRASS_TILE_COLOR_NORMAL};
					currentTile->DescriptionOverride = "a patch of grass";
				}
			}
		}
	}
}

void ClearNpcs()
{
	for (RLEntity* entity : gameState.npcs)
		delete entity;

	gameState.npcs.clear();
}

void LoadNextLevel()
{
	ClearNpcs();

	gameState.currentLevel++;
	srand(gameState.seed * gameState.currentLevel);

	if (gameState.currentLevel <= LEVEL_NUM_FOREST)
	{
		createForest();

		int numLevelEnemiesToSpawn =
		    gameState.currentLevel * LEVELENEMY_SPAWN_NUM_MULTIPLIER_FOREST;
		for (int i = 0; i < numLevelEnemiesToSpawn; i++)
		{
			LevelEnemy* newLevelEnemy = new LevelEnemy();
			placeEntityRandomSensibly(newLevelEnemy);
			gameState.npcs.push_back(newLevelEnemy);
		}

		LOGD << "Spawned " << numLevelEnemiesToSpawn << " level enemies";
	}
	else if (gameState.currentLevel <= LEVEL_NUM_BARREN)
	{
		createBarren();

		int numLevelEnemiesToSpawn =
		    gameState.currentLevel * LEVELENEMY_SPAWN_NUM_MULTIPLIER_BARREN;
		for (int i = 0; i < numLevelEnemiesToSpawn; i++)
		{
			LevelEnemy* newLevelEnemy = new LevelEnemy();
			placeEntityRandomSensibly(newLevelEnemy);
			gameState.npcs.push_back(newLevelEnemy);
		}

		LOGD << "Spawned " << numLevelEnemiesToSpawn << " level enemies";
	}
	else if (gameState.currentLevel <= LEVEL_NUM_HELLSCAPE)
	{
		createHellscape();

		int numLevelEnemiesToSpawn =
		    gameState.currentLevel * LEVELENEMY_SPAWN_NUM_MULTIPLIER_HELLSCAPE;
		for (int i = 0; i < numLevelEnemiesToSpawn; i++)
		{
			LevelEnemy* newLevelEnemy = new LevelEnemy();
			placeEntityRandomSensibly(newLevelEnemy);
			gameState.npcs.push_back(newLevelEnemy);
		}

		LOGD << "Spawned " << numLevelEnemiesToSpawn << " level enemies";
	}

	Summoner* newSummoner = new Summoner();
	placeEntityRandomSensibly(newSummoner);
	gameState.npcs.push_back(newSummoner);

	placeEntityRandomSensibly(&gameState.player);

	// Make sure vfx layer is large enough
	gameState.vfx.SetSize(gameState.currentMap.Width, gameState.currentMap.Height);
	gameState.vfx.ResetTiles();

	gameState.abilitySpawnedThisLevel = false;
}