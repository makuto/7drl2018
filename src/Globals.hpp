#pragma once

// Global.hpp is included by all files automatically

#define CLAMP(value, min, max) \
	if (value > max)           \
		value = max;           \
	else if (value < min)      \
		value = min;
#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)

//
// Colors
//

// Interface
#define WIN_BACKGROUND_COLOR 34, 34, 34, 255

#define LOG_COLOR_NORMAL 255, 255, 255, 255
#define LOG_COLOR_IMPORTANT 249, 212, 35, 255
#define LOG_COLOR_DEAD 232, 30, 34, 255

#define LOOK_CURSOR_COLOR 252, 145, 57, 255

#define STATUS_COLOR_UNIMPORTANT 255, 255, 255, 255
#define STATUS_COLOR_NORMAL 252, 145, 57, 255
#define STATUS_COLOR_IMPORTANT 232, 30, 34, 255

// Tiles
#define WALL_TILE_COLOR_NORMAL 255, 255, 255, 255
#define TREE_TILE_COLOR_NORMAL 184, 247, 34, 255
#define GRASS_TILE_COLOR_NORMAL 164, 227, 14, 255
#define FLOOR_TILE_COLOR_NORMAL 46, 64, 75, 255
#define HELL_TILE_COLOR_NORMAL 232, 30, 34, 255

#define ABILITY_TILE_COLOR_NORMAL 252, 145, 57, 255

// Entities
#define PLAYER_COLOR_NORMAL 252, 145, 57, 255
#define ENEMY_COLOR_NORMAL 232, 30, 34, 255
#define STAIRS_COLOR_NORMAL 252, 145, 57, 255

// FX
#define FX_LIGHTNING 41, 94, 243, 150
#define FX_PHASE_DOOR 249, 212, 35, 150
#define FX_FIREBOMB 232, 30, 34, 150

//
// Tile/Entity Types
//
#define TYPE_NONE ' '

#define PLAYER_TYPE '@'
#define SKELETON_TYPE 's'
#define SUMMONER_TYPE 'c'
#define GOBLIN_TYPE 'g'
#define BANDIT_TYPE 'h'
#define DRAGON_TYPE 'b'
#define FIRE_DRAGON_TYPE 'd'
#define WIZARD_TYPE 'w'
#define CONTROL_WIZARD_TYPE 'm'

#define CORPSE_TYPE '%'
#define ABILITY_TYPE '!'

#define STAIRS_DOWN_TYPE '>'

#define WALL_TYPE '#'
#define TREE_TYPE '#'
#define FLOOR_TYPE '.'

#define LOOK_MODE_CURSOR "_"

//
// Combat constants
//
#define PLAYER_STARTING_MAX_HEALTH 100
#define PLAYER_STARTING_RESTORE_HEALTH 5
#define PLAYER_DEFAULT_RESTORE_RATE_HEALTH 2

#define PLAYER_STARTING_MAX_STAMINA 100
#define PLAYER_STARTING_RESTORE_STAMINA 5
#define PLAYER_DEFAULT_RESTORE_RATE_STAMINA 2

#define PLAYER_STARTING_MAX_STRENGTH 10

#define ENABLE_ONLY_HEAL_FULL_STAMINA false
#define ENABLE_OVEREXERTION false

// If the player rests, the number of turns to restore goes down by this number
#define PLAYER_RESTING_BONUS 2
#define PLAYER_MELEE_ATTACKING_BONUS 0

#define PLAYER_NUM_ABILITY_SLOTS 3

// Every level, the training stat will increase its max by this * level
#define STAT_INCREASE_LEVEL_MULTIPLIER 1.5f

// The absolute max a single summoner can spawn
#define MAX_SINGLE_SUMMONS 100
#define SUMMONING_RADIUS 5
#define SUMMONER_SPAWN_RATE_TURNS 25
// Each level, spawn rate turns cooldown decreases by this * level (0.5 = every other level lower
// spawn rate by 1)
#define SUMMONER_SPAWN_RATE_LEVEL_MULTIPLIER 0.5f

// This makes enemies slower to spawn
// TurnCounter % (SpawnRate + spawn rate modifier)
#define LIGHTNINGWIZARD_SPAWN_RATE_MODIFIER 35
#define CONTROLWIZARD_SPAWN_RATE_MODIFIER 55
#define FIREDRAGON_SPAWN_RATE_MODIFIER 50

#define RANGED_ENEMY_MAX_DIST_MANHATTAN 27

#define LEVELENEMY_PLAYER_DETECT_MANHATTAN_RADIUS 20

// Each death, chance of ability dropping = this * level
#define DEATH_ABILITY_DROP_LEVEL_MULTIPLIER 1.5f

// Dragons don't shoot fire too often
#define DRAGON_FIRE_RATE 10

// turncounter % chance < rate = random walk
#define BANDIT_CONFUSION_CHANCE 40
#define BANDIT_CONFUSION_RATE 20

//
// Abilities
//
#define PHASE_DOOR_SQUARE_RADIUS 20
#define PHASE_TARGET_ON_PLAYER_RADIUS 15
#define PHASE_TARGET_ON_ENEMY_RADIUS 15

#define FIREBOMB_RADIUS 2

//
// Levels
//
#define LEVEL_NUM_FOREST 3
#define LEVEL_NUM_BARREN 6
// lol have fun
#define LEVEL_NUM_HELLSCAPE 10000

#define NUM_LEVELS_TO_WIN 10

// Number of level enemies spawned = this * level
#define LEVELENEMY_SPAWN_NUM_MULTIPLIER_FOREST 7.f
#define LEVELENEMY_SPAWN_NUM_MULTIPLIER_BARREN 7.f
#define LEVELENEMY_SPAWN_NUM_MULTIPLIER_HELLSCAPE 3.f

// Size of one axis e.g. total tiles = FOREST_SIZE * FOREST_SIZE
#define FOREST_SIZE 80
#define BARREN_SIZE 100
#define HELLSCAPE_SIZE 80

//
// Only display once in log strings
//
#define WALL_BUMP "You bump into a wall"
#define LOOK_MODE_EXIT "(Last turn)"
#define CANCELLED_ABILITY_ACTIVATE "(Cancelled activate)"
#define ABILITY_ON_COOLDOWN "Ability on cooldown"
#define TRAINING_CHANGED "Now training "

//
// Engine tuning
//
#define MAX_PLACEMENT_ATTEMPTS 100

// How far target mode will snap to enemy
#define MAX_PLAYER_TARGET_DIST 30.f