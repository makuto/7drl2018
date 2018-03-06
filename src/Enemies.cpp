#include "Enemies.hpp"

#include "Game.hpp"
#include "Levels.hpp"

Skeleton::Skeleton()
{
	SpawnStairsDown = false;
	Speed = 1;
	Type = SKELETON_TYPE;  // TODO: Replace with define?
	Color = {ENEMY_COLOR_NORMAL};
	Description = "skeleton";

	Stats["HP"] = {"Health", 30, 30, 0, 0, -1};
	Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
}

void Skeleton::DoTurn()
{
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	MoveTowardsPlayer();
}

Summoner::Summoner()
{
	NumSpawns = 0;
	SpawnStairsDown = true;
	Speed = 1;
	Type = 'c';  // TODO: Replace with define?
	Color = {ENEMY_COLOR_NORMAL};
	Description = "caller";

	Stats["HP"] = {"Health", 30, 30, 0, 0, -1};
	Stats["STR"] = {"Strength", 0, 0, 0, 0, -1};
}

void Summoner::DoTurn()
{
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	// MoveTowardsPlayer();

	int spawnRate = (int)(SUMMONER_SPAWN_RATE_TURNS -
	                      (gameState.currentLevel * SUMMONER_SPAWN_RATE_LEVEL_MULTIPLIER));
	if (TurnCounter % spawnRate == 0 && NumSpawns < MAX_SINGLE_SUMMONS)
	{
		NumSpawns++;
		Skeleton* newSkeleton = new Skeleton();
		placeEntityWithinSquareRandomSensibly(newSkeleton, X, Y, SUMMONING_RADIUS);
		gameState.npcsToCreate.push_back(newSkeleton);
		LOGD << "Spawn skeleton at " << newSkeleton->X << ", " << newSkeleton->Y << " spawn rate "
		     << spawnRate;
	}
}