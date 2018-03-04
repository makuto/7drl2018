#pragma once

// Global.hpp is included by all files automatically

#define CLAMP(value, min, max) if (value > max) value = max; else if (value < min) value = min; 
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
#define FLOOR_TILE_COLOR_NORMAL 46, 64, 75, 255

// Entities
#define PLAYER_COLOR_NORMAL 252, 145, 57, 255
#define ENEMY_COLOR_NORMAL 232, 30, 34, 255

//
// Tile/Entity Types
//
#define PLAYER_TYPE '@'
#define SKELETON_TYPE 's'

#define CORPSE_TYPE '%'

#define WALL_TYPE '#'
#define FLOOR_TYPE '.'

#define LOOK_MODE_CURSOR "_"

//
// Combat constants
//
#define PLAYER_STARTING_MAX_HEALTH 100
#define PLAYER_STARTING_MAX_STAMINA 100