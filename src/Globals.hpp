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
#define	HELL_TILE_COLOR_NORMAL 232, 30, 34, 255

// Entities
#define PLAYER_COLOR_NORMAL 252, 145, 57, 255
#define ENEMY_COLOR_NORMAL 232, 30, 34, 255
#define STAIRS_COLOR_NORMAL 252, 145, 57, 255

//
// Tile/Entity Types
//
#define PLAYER_TYPE '@'
#define SKELETON_TYPE 's'

#define CORPSE_TYPE '%'

#define STAIRS_DOWN_TYPE '>'

#define WALL_TYPE '#'
#define TREE_TYPE '#'
#define FLOOR_TYPE '.'

#define LOOK_MODE_CURSOR "_"

//
// Combat constants
//
#define PLAYER_STARTING_MAX_HEALTH 100
#define PLAYER_STARTING_RESTORE_HEALTH 1
#define PLAYER_DEFAULT_RESTORE_RATE_HEALTH 1

#define PLAYER_STARTING_MAX_STAMINA 100
#define PLAYER_STARTING_RESTORE_STAMINA 10
#define PLAYER_DEFAULT_RESTORE_RATE_STAMINA 5

#define PLAYER_STARTING_MAX_STRENGTH 10

// If the player rests, the number of turns to restore goes down by this number
#define PLAYER_RESTING_BONUS 1
#define PLAYER_MELEE_ATTACKING_BONUS -1

// The absolute max a single summoner can spawn
#define MAX_SINGLE_SUMMONS 100
#define SUMMONING_RADIUS 3
#define	SUMMONER_SPAWN_RATE_TURNS 20
// Each level, spawn rate turns cooldown decreases by this * level
#define SUMMONER_SPAWN_RATE_LEVEL_MULTIPLIER 0.5f

//
// Only display once in log strings
//
#define WALL_BUMP "You bump into a wall"
#define LOOK_MODE_EXIT "(Last turn)"