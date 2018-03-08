#pragma once

#include "graphics/graphics.hpp"
#include "input/input.hpp"


#include <string>
#include <vector>

#include "Entity.hpp"
#include "GameInput.hpp"
#include "RLMap.hpp"

//
// Dimensions (-1 = RecalculateDimensions() figures it out)
//
extern int WindowWidth;
extern int WindowHeight;

extern int TileTextHeight;
extern int TileTextWidth;

extern int SidebarTileWidth;

extern int ViewTileRightMargin;
extern int ViewTileBottomMargin;
extern int ViewTileWidth;
extern int ViewTileHeight;

extern int CamSnapLeftBounds;
extern int CamSnapTopBounds;
extern int CamSnapRightBounds;
extern int CamSnapBottomBounds;

extern int SidebarStartX;
extern int SidebarStartY;

extern int LogX;
extern int LogY;

//
// Gameplay
//
extern int TurnCounter;

extern std::vector<std::string> GameLog;
extern int LastTurnLog;

struct GameState
{
	Player player;
	std::vector<RLEntity*> npcs;
	std::vector<RLEntity*> npcsToCreate;

	RLMap currentMap;

	RLMap vfx;
	std::vector<Ability*> abilitiesUpdatingFx;

	// Changes every play session (but not every level)
	int seed;

	int currentLevel;
	bool abilitySpawnedThisLevel;

	std::string levelName;

	bool enableCheats;

	// Remember to add fields to Reset Globals block!
};

extern GameState gameState;

//
// Display / input
//
extern window win;
extern inputManager inp;
extern text displayText;
extern GameInput gameInp;