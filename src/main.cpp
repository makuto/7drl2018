#include <iostream>

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include "RLMap.hpp"

#include "Entity.hpp"
#include "Levels.hpp"

static std::vector<std::string> GameLog;

// @Callback: CustomLogOutputFunc
void ConsoleAndGameLogOutput(const gv::Logging::Record& record)
{
	bool IsWarning = (record.severity == gv::Logging::Severity::warning);
	bool IsError = (record.severity <= gv::Logging::Severity::error);
	bool IsInfo = (record.severity == gv::Logging::Severity::info);

	static char funcNameBuffer[256];
	gv::Logging::FormatFuncName(funcNameBuffer, record.Function, sizeof(funcNameBuffer));

	static char buffer[2048];
	if (IsInfo)
		snprintf(buffer, sizeof(buffer), "%s", record.OutBuffer);
	else
		snprintf(buffer, sizeof(buffer), "%s():%lu: %s", funcNameBuffer, (unsigned long)record.Line,
		         record.OutBuffer);

	if (IsError)
	{
		std::cout << "Error:   " << buffer << "\n" << std::flush;
	}
	else if (IsWarning)
	{
		std::cout << "Warning: " << buffer << "\n" << std::flush;
	}
	else
	{
		std::cout << "Log:     " << buffer << "\n" << std::flush;
	}

	if (IsInfo)
	{
		std::string gameLogAction(buffer);
		GameLog.push_back(gameLogAction);
	}
}

static gv::Logging::Logger s_GameLogger(gv::Logging::Severity::debug, &ConsoleAndGameLogOutput);

void initializeWindow(window& win)
{
	win.setBackgroundColor(WIN_BACKGROUND_COLOR);

	// shouldClose manages some state
	win.shouldClose();
	win.update();

	win.shouldClear(true);
	win.getBase()->setVerticalSyncEnabled(true);
	win.getBase()->setFramerateLimit(60);
}

void initializeDisplayText(text& text)
{
	if (!text.loadFont("data/fonts/UbuntuMono-R.ttf"))
		std::cout << "err: cannot load default text font\n";
}

int main(int argc, char const* argv[])
{
	const int WindowWidth = 2500;
	const int WindowHeight = 1600;
	const int TileTextHeight = 32;
	const int TileTextWidth = TileTextHeight / 2;
	const int ViewTileRightMargin = 2;
	const int ViewTileBottomMargin = 2;
	const int ViewTileWidth = (WindowWidth / TileTextWidth) - ViewTileRightMargin;
	const int ViewTileHeight = (WindowHeight / TileTextHeight) - ViewTileBottomMargin;
	const int LogX = 10;
	const int LogY = ViewTileHeight * TileTextHeight;

	LOGI << "Hello 7DRL!\n";

	//
	// Window / input initialization
	//
	window win(WindowWidth, WindowHeight, "7DRL 2018 by Macoy Madson");
	initializeWindow(win);

	inputManager inp(&win);

	text displayText;
	initializeDisplayText(displayText);
	displayText.setSize(TileTextHeight);
	displayText.setColor(WALL_TILE_COLOR_NORMAL);

	//
	// Game initialization
	//

	Player player;
	player.X = ViewTileWidth / 2;
	player.Y = ViewTileHeight / 2;

	RLMap testMap(ViewTileWidth, ViewTileHeight);
	createTestMap(testMap);

	int camXOffset = 0;
	int camYOffset = 0;

	while (!win.shouldClose() && !inp.isPressed(inputCode::Return) &&
	       !inp.isPressed(inputCode::Escape))
	{
		//
		// Input
		//

		// Player movement
		if (inp.isPressed(inputCode::Up))
		{
			player.Y -= 1;
		}
		if (inp.isPressed(inputCode::Down))
		{
			player.Y += 1;
		}
		if (inp.isPressed(inputCode::Left))
		{
			player.X -= 1;
		}
		if (inp.isPressed(inputCode::Right))
		{
			player.X += 1;
		}

		//
		// Draw map
		//
		for (int camY = 0; camY < MIN(testMap.Height, ViewTileHeight); ++camY)
		{
			for (int camX = 0; camX < MIN(testMap.Width, ViewTileWidth); ++camX)
			{
				int tileX = camX - camXOffset;
				int tileY = camY - camYOffset;
				RLTile& currentTile = testMap.At(tileX, tileY);

				static std::string buffer = "";
				buffer.clear();

				if (player.X == tileX && player.Y == tileY)
				{
					buffer += player.Type;
					displayText.setColor(PLAYER_COLOR_NORMAL);
				}
				else
				{
					buffer += currentTile.Type;
					displayText.setColor(currentTile.Color.r, currentTile.Color.g,
					                     currentTile.Color.b, currentTile.Color.a);
				}

				displayText.setText(buffer);
				displayText.setPosition(TileTextWidth * camX, TileTextHeight * camY);
				win.draw(&displayText);
			}
		}

		//
		// Draw log
		//
		if (GameLog.size())
		{
			displayText.setText(GameLog.back());
			displayText.setColor(LOG_COLOR_NORMAL);
			displayText.setPosition(LogX, LogY);
			win.draw(&displayText);
		}

		win.update();
	}

	return 0;
}