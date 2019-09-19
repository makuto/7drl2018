#include "Enemies.hpp"

#include "math/math.hpp"

#include "Globals.hpp"

#include "Game.hpp"
#include "Levels.hpp"
#include "Logging.hpp"

#include "Tracy.hpp"

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
		Description = "madman";

		Stats["HP"] = {"Health", 20, 20, 0, 0, -1};
		Stats["STR"] = {"Strength", 12, 12, 0, 0, -1};
	}
	else if (gameState.currentLevel <= LEVEL_NUM_HELLSCAPE)
	{
		Type = DRAGON_TYPE;
		Color = {ENEMY_COLOR_NORMAL};
		Description = "baby drake";

		Stats["HP"] = {"Health", 40, 40, 0, 0, -1};
		Stats["STR"] = {"Strength", 15, 15, 0, 0, -1};
	}
}

void LevelEnemy::DoTurn()
{
	ZoneScoped;
	
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	if (manhattanTo(X, Y, gameState.player.X, gameState.player.Y) >
	        LEVELENEMY_PLAYER_DETECT_MANHATTAN_RADIUS ||
	    (Type == BANDIT_TYPE && TurnCounter % BANDIT_CONFUSION_CHANCE < BANDIT_CONFUSION_RATE))
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

	Stats["HP"] = {"Health", 25, 25, 0, 0, -1};
	Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
}

void Skeleton::DoTurn()
{
	ZoneScoped;
	
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
	ZoneScoped;
	
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
		if ((TurnCounter + (rand() % 5)) % (spawnRate + LIGHTNINGWIZARD_SPAWN_RATE_MODIFIER) == 0 && NumSpawns < MAX_SINGLE_SUMMONS)
		{
			NumSpawns++;
			LightningWizard* newLightningWizard = new LightningWizard();
			placeEntityWithinSquareRandomSensibly(newLightningWizard, X, Y, SUMMONING_RADIUS);
			gameState.npcsToCreate.push_back(newLightningWizard);
			LOGD << "Spawn LightningWizard at " << newLightningWizard->X << ", "
			     << newLightningWizard->Y << " spawn rate " << spawnRate;
		}

		// Add offset so they aren't spawned on same turn
		if ((TurnCounter + (rand() % 8)) % (spawnRate + CONTROLWIZARD_SPAWN_RATE_MODIFIER) == 0 && NumSpawns < MAX_SINGLE_SUMMONS)
		{
			NumSpawns++;
			ControlWizard* newControlWizard = new ControlWizard();
			placeEntityWithinSquareRandomSensibly(newControlWizard, X, Y, SUMMONING_RADIUS);
			gameState.npcsToCreate.push_back(newControlWizard);
			LOGD << "Spawn ControlWizard at " << newControlWizard->X << ", " << newControlWizard->Y
			     << " spawn rate " << spawnRate;
		}
	}

	if (gameState.currentLevel > LEVEL_NUM_BARREN)
	{
		if ((TurnCounter + (rand() % 8)) % (spawnRate + FIREDRAGON_SPAWN_RATE_MODIFIER) == 0 && NumSpawns < MAX_SINGLE_SUMMONS)
		{
			NumSpawns++;
			FireDragon* newFireDragon = new FireDragon();
			placeEntityWithinSquareRandomSensibly(newFireDragon, X, Y, SUMMONING_RADIUS);
			gameState.npcsToCreate.push_back(newFireDragon);
			LOGD << "Spawn FireDragon at " << newFireDragon->X << ", " << newFireDragon->Y
			     << " spawn rate " << spawnRate;
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
	ZoneScoped;
	
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	MoveTowardsPlayer();

	if (lightning.CanActivateOnPlayer(this))
		lightning.EnemyActivate(this);
}

ControlWizard::ControlWizard()
{
	SpawnStairsDown = false;
	Speed = 1;
	Type = CONTROL_WIZARD_TYPE;  // TODO: Replace with define?
	Color = {ENEMY_COLOR_NORMAL};
	Description = "control mage";

	Stats["HP"] = {"Health", 10, 10, 0, 0, -1};
	Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
}

void ControlWizard::DoTurn()
{
	ZoneScoped;
	
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	if (manhattanTo(X, Y, gameState.player.X, gameState.player.Y) >
	    LEVELENEMY_PLAYER_DETECT_MANHATTAN_RADIUS)
		RandomWalk();
	else
		MoveTowardsPlayer();

	if (phaseTarget.CanActivateOnPlayer(this))
		phaseTarget.EnemyActivate(this);
}

FireDragon::FireDragon()
{
	SpawnStairsDown = false;
	Speed = 1;
	Type = FIRE_DRAGON_TYPE;  // TODO: Replace with define?
	Color = {ENEMY_COLOR_NORMAL};
	Description = "dragon";

	Stats["HP"] = {"Health", 80, 80, 0, 0, -1};
	Stats["STR"] = {"Strength", 20, 20, 0, 0, -1};
}

void FireDragon::DoTurn()
{
	ZoneScoped;
	
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	MoveTowardsPlayer();

	if (fireBomb.CanActivateOnPlayer(this) && rand() % DRAGON_FIRE_RATE == 0)
		fireBomb.EnemyActivate(this);
}
