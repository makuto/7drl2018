#pragma once

#include <vector>
#include <string>

// Global.hpp is included by all files automatically

#define CLAMP(value, min, max) if (value > max) value = max; else if (value < min) value = min; 
#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)

//
// Colors
//
#define WIN_BACKGROUND_COLOR 34, 34, 34, 255
#define LOG_COLOR_NORMAL 255, 255, 255, 255
#define LOG_COLOR_IMPORTANT 249, 212, 35, 255
#define LOG_COLOR_DEAD 232, 30, 34, 255

#define WALL_TILE_COLOR_NORMAL 255, 255, 255, 255
#define FLOOR_TILE_COLOR_NORMAL 46, 64, 75, 255

#define PLAYER_COLOR_NORMAL 252, 145, 57, 255

//
// Tile/Entity Types
//
#define PLAYER_TYPE '@'
#define WALL_TYPE '#'
#define FLOOR_TYPE '.'