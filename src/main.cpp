#include <iostream>

#include <map>

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "GameInput.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Levels.hpp"
#include "RLMap.hpp"
#include "Utilities.hpp"

#include "MainMenu.hpp"
#include "PlayerDeadScreen.hpp"

int TurnCounter = 0;

std::vector<std::string> GameLog;
int LastTurnLog = 0;
GameState gameState;

//
// Dimensions (-1 = RecalculateDimensions() figures it out)
//
// int WindowWidth = 2500;
// int WindowHeight = 1600;
// Hopefully they'll resize the window if it's too small for them...
int WindowWidth = 1200;
int WindowHeight = 700;

// Don't be too narrow so as to cut off unwrapped text
int MinNumTextColumns = 100;
// Don't be so wide as to end up with tiny TileTextHeight on high DPI screens
int MaxNumTextColumns = 200;

// This will shrink if we don't have enough for MinNumTextColumns or more than MaxNumTextColumns
int TileTextHeight = 32;
int TileTextWidth = -1;

int SidebarTileWidth = 21;
int ViewTileRightMargin = -1;
int ViewTileTopMargin = 2;
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

sound sfxPlayerDead;
sound sfxPlayerVictory;
sound sfxPlayerMeleeAttack;
sound sfxPlayerMeleeMiss;
sound sfxLightning;
sound sfxPhaseDoor;
sound sfxEnemyMeleeAttack;
sound sfxNextLevel;

void loadAudio()
{
	if (!sfxPlayerDead.load("data/audio/death.ogg"))
		LOGE << "Failed to load sfxPlayerDead";
	if (!sfxPlayerVictory.load("data/audio/victory.ogg"))
		LOGE << "Failed to load sfxPlayerVictory";
	if (!sfxPlayerMeleeAttack.load("data/audio/quietShaker.ogg"))
		LOGE << "Failed to load sfxPlayerMeleeAttack";
	if (!sfxPlayerMeleeMiss.load("data/audio/quietShaker.ogg"))
		LOGE << "Failed to load sfxPlayerMeleeMiss";

	if (!sfxLightning.load("data/audio/quietShaker.ogg"))
		LOGE << "Failed to load sfxLightning";
	if (!sfxPhaseDoor.load("data/audio/quietShaker.ogg"))
		LOGE << "Failed to load sfxPhaseDoor";

	if (!sfxEnemyMeleeAttack.load("data/audio/hit2.ogg"))
		LOGE << "Failed to load sfxEnemyMeleeAttack";

	if (!sfxNextLevel.load("data/audio/level.ogg"))
		LOGE << "Failed to load sfxNextLevel";
}

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

	loadAudio();

	bool shouldPlay = MainMenu();

	if (!shouldPlay)
		return 0;

	bool shouldRestart = true;
	while (shouldRestart)
		shouldRestart = PlayGame();

	return 0;
}