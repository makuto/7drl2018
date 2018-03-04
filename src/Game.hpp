#pragma once

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include <string>
#include <vector>

#include "GameInput.hpp"

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
extern int lastTurnLog;

//
// Display / input
//
extern window win;
extern inputManager inp;
extern text displayText;
extern GameInput gameInp;