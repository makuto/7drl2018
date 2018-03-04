#include <iostream>

#include <map>

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "GameInput.hpp"
#include "Gameplay.hpp"
#include "Levels.hpp"
#include "RLMap.hpp"
#include "Utilities.hpp"

#include "PlayerDeadScreen.hpp"

int TurnCounter = 0;

std::vector<std::string> GameLog;
int LastTurnLog = 0;
GameState gameState;

//
// Dimensions (-1 = RecalculateDimensions() figures it out)
//
int WindowWidth = 2500;
int WindowHeight = 1600;

int TileTextHeight = 32;
int TileTextWidth = -1;

int SidebarTileWidth = 15;

int ViewTileRightMargin = -1;
int ViewTileBottomMargin = 2;
int ViewTileWidth = -1;
int ViewTileHeight = -1;

int CamSnapLeftBounds = -1;
int CamSnapTopBounds = -1;
int CamSnapRightBounds = -1;
int CamSnapBottomBounds = -1;

int SidebarStartX = -1;
int SidebarStartY = -1;

int LogX = 10;
int LogY = -1;

void windowResizeCB(float width, float height)
{
	WindowWidth = width;
	WindowHeight = height;
	RecalculateDimensions();
}

window win(WindowWidth, WindowHeight, "7DRL 2018 by Makuto", &windowResizeCB);
inputManager inp(&win);
text displayText;
GameInput gameInp(inp);

static gv::Logging::Logger s_GameLogger(gv::Logging::Severity::debug, &ConsoleAndGameLogOutput);

void initializeWindow(window& win)
{
	win.setBackgroundColor(WIN_BACKGROUND_COLOR);

	// shouldClose manages some state
	win.shouldClose();
	win.update();

	win.shouldClear(true);
	win.getBase()->setVerticalSyncEnabled(true);
	win.getBase()->setFramerateLimit(30);
}

void initializeDisplayText(text& text)
{
	if (!text.loadFont("data/fonts/UbuntuMono-R.ttf"))
		LOGE << "Error: Cannot load default text font. Nothing will work\n";
}

int main(int argc, char const* argv[])
{
	initializeWindow(win);
	RecalculateDimensions();

	initializeDisplayText(displayText);
	displayText.setSize(TileTextHeight);
	displayText.setColor(WALL_TILE_COLOR_NORMAL);

	bool shouldRestart = true;
	while (shouldRestart)
		shouldRestart = PlayGame();

	return 0;
}