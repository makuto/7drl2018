#include "Enemies.hpp"

#include "math/math.hpp"

#include "Globals.hpp"

#include "Game.hpp"
#include "Levels.hpp"

LevelEnemy::LevelEnemy()
{
	SpawnStairsDown = false;
	Speed = 1;

	if (gameState.currentLevel <= LEVEL_NUM_FOREST)
	{
		Type = GOBLIN_TYPE;
		Color = {ENEMY_COLOR_NORMAL};
		Description = "goblin";

		Stats["HP"] = {"Health", 20, 20, 0, 0, -1};
		Stats["STR"] = {"Strength", 8, 8, 0, 0, -1};
	}
	else if (gameState.currentLevel <= LEVEL_NUM_BARREN)
	{
		Type = BANDIT_TYPE;
		Color = {ENEMY_COLOR_NORMAL};
		Description = "bandit";

		Stats["HP"] = {"Health", 40, 40, 0, 0, -1};
		Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
	}
	else if (gameState.currentLevel <= LEVEL_NUM_HELLSCAPE)
	{
		Type = DRAGON_TYPE;
		Color = {ENEMY_COLOR_NORMAL};
		Description = "drake";

		Stats["HP"] = {"Health", 60, 60, 0, 0, -1};
		Stats["STR"] = {"Strength", 30, 30, 0, 0, -1};
	}
}

void LevelEnemy::DoTurn()
{
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	if (manhattanTo(X, Y, gameState.player.X, gameState.player.Y) >
	    LEVELENEMY_PLAYER_DETECT_MANHATTAN_RADIUS)
		RandomWalk();
	else
		MoveTowardsPlayer();
}

Skeleton::Skeleton()
{
	SpawnStairsDown = false;
	Speed = 1;
	Type = SKELETON_TYPE;
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
	Type = SUMMONER_TYPE;
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

	if (gameState.currentLevel > LEVEL_NUM_FOREST)
	{
		// Add offset so they aren't spawned on same turn
		if ((TurnCounter + 5) % spawnRate == 0 && NumSpawns < MAX_SINGLE_SUMMONS)
		{
			NumSpawns++;
			LightningWizard* newLightningWizard = new LightningWizard();
			placeEntityWithinSquareRandomSensibly(newLightningWizard, X, Y, SUMMONING_RADIUS);
			gameState.npcsToCreate.push_back(newLightningWizard);
			LOGD << "Spawn LightningWizard at " << newLightningWizard->X << ", "
			     << newLightningWizard->Y << " spawn rate " << spawnRate;
		}
	}
}

LightningWizard::LightningWizard()
{
	SpawnStairsDown = false;
	Speed = 1;
	Type = WIZARD_TYPE;
	Color = {ENEMY_COLOR_NORMAL};
	Description = "wizard";

	Stats["HP"] = {"Health", 30, 30, 0, 0, -1};
	Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
}

void LightningWizard::DoTurn()
{
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	MoveTowardsPlayer();

	if (lightning.CanActivateOnPlayer(this))
		lightning.EnemyActivate(this);
}