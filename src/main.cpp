#include <iostream>

#include <map>

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include "RLMap.hpp"

#include "Entity.hpp"
#include "Levels.hpp"

static int TurnCounter = 0;

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
		static int lastTurnLog = 0;

		std::string gameLogAction(buffer);

		// Append logs to the same line if they occur on the same turn
		if (lastTurnLog == TurnCounter && !GameLog.empty() && gameLogAction != GameLog.back())
		{
			std::string& currentTurnLog = GameLog.back();
			currentTurnLog += ". ";
			currentTurnLog += gameLogAction;
		}
		else
		{
			GameLog.push_back(gameLogAction);
			lastTurnLog = TurnCounter;
		}
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

struct GameInput
{
	inputManager& inp;
	std::map<inputCode::keyCode, bool> keyTapStates;

	GameInput(inputManager& newInput) : inp(newInput)
	{
	}

	bool Tapped(inputCode::keyCode key)
	{
		bool isPressed = inp.isPressed(key);

		std::map<inputCode::keyCode, bool>::iterator findIt = keyTapStates.find(key);
		if (findIt != keyTapStates.end())
		{
			if (isPressed)
			{
				if (findIt->second)
				{
					// Wasn't tapped this frame
					return false;
				}
				else
				{
					findIt->second = true;
					return true;
				}
			}
			else
				findIt->second = false;
		}
		else
		{
			keyTapStates[key] = true;
			return isPressed;
		}

		return false;
	}
};

void SetTextColor(text& text, RLColor& color)
{
	text.setColor(color.r, color.g, color.b, color.a);
}

int main(int argc, char const* argv[])
{
	const int WindowWidth = 2500;
	const int WindowHeight = 1600;

	const int TileTextHeight = 32;
	const int TileTextWidth = TileTextHeight / 2;

	const int SidebarTileWidth = 30;

	const int ViewTileRightMargin = SidebarTileWidth + 1;
	const int ViewTileBottomMargin = 2;
	const int ViewTileWidth = (WindowWidth / TileTextWidth) - ViewTileRightMargin;
	const int ViewTileHeight = (WindowHeight / TileTextHeight) - ViewTileBottomMargin;

	const int SidebarStartX = (ViewTileWidth + 1) * TileTextWidth;
	const int SidebarStartY = 0 * TileTextHeight;

	const int LogX = 10;
	const int LogY = ViewTileHeight * TileTextHeight;

	LOGI << "Hello 7DRL!\n";

	//
	// Window / input initialization
	//
	window win(WindowWidth, WindowHeight, "7DRL 2018 by Macoy Madson");
	initializeWindow(win);

	inputManager inp(&win);
	GameInput gameInp(inp);

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

	RLEntity lookModeCursor;

	RLMap testMap(ViewTileWidth, ViewTileHeight);
	// createTestMap(testMap);
	createTestMap2(testMap);

	RLEntity skeleton;
	skeleton.X = (ViewTileWidth / 2) + 3;
	skeleton.Y = ViewTileHeight / 2;
	skeleton.Speed = 1;
	skeleton.Type = SKELETON_TYPE;
	skeleton.Color = {ENEMY_COLOR_NORMAL};
	skeleton.Description = "a skeleton";
	skeleton.Stats["HP"] = {"Health", PLAYER_STARTING_MAX_HEALTH, PLAYER_STARTING_MAX_HEALTH};

	std::vector<RLEntity> npcs{skeleton};

	int camXOffset = 0;
	int camYOffset = 0;

	bool lookMode = false;

	while (!win.shouldClose() && !inp.isPressed(inputCode::Return))
	{
		bool playerTurnPerformed = false;

		//
		// Input
		//

		// Escape exits modes or closes the game
		if (gameInp.Tapped(inputCode::Escape))
		{
			if (lookMode)
				lookMode = false;
			else
				// Close the game
				break;
		}

		// Look mode
		if (gameInp.Tapped(inputCode::L))
		{
			// Entering look mode; set the cursor to the player's position
			if (!lookMode)
			{
				lookModeCursor.X = player.X;
				lookModeCursor.Y = player.Y;
			}
			lookMode = !lookMode;
		}

		// Directional navigation
		int deltaX = 0;
		int deltaY = 0;
		if (gameInp.Tapped(inputCode::Up))
			deltaY -= 1;
		if (gameInp.Tapped(inputCode::Down))
			deltaY += 1;
		if (gameInp.Tapped(inputCode::Left))
			deltaX -= 1;
		if (gameInp.Tapped(inputCode::Right))
			deltaX += 1;

		if (lookMode)
		{
			if (deltaX || deltaY)
			{
				lookModeCursor.X += deltaX;
				lookModeCursor.Y += deltaY;
			}
		}
		else
		{
			// Player movement
			if (canMoveTo(player, deltaX, deltaY, testMap))
			{
				player.X += deltaX;
				player.Y += deltaY;

				// Decide whether we need to make this a turn
				if (abs(deltaX) >= player.Speed || abs(deltaY) >= player.Speed)
				{
					playerTurnPerformed = true;
				}
			}
			else
			{
				LOGI << "You bump into a wall";
			}
		}

		//
		// Turn update
		//
		if (playerTurnPerformed)
		{
			TurnCounter++;
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

				bool shouldDrawTile = true;
				for (RLEntity& npc : npcs)
				{
					if (npc.X == tileX && npc.Y == tileY)
					{
						buffer += npc.Type;
						SetTextColor(displayText, npc.Color);

						shouldDrawTile = false;
						break;
					}
				}

				if (buffer.empty() && player.X == tileX && player.Y == tileY)
				{
					buffer += player.Type;
					SetTextColor(displayText, player.Color);
				}
				else if (buffer.empty())
				{
					buffer += currentTile.Type;
					SetTextColor(displayText, currentTile.Color);
				}

				displayText.setText(buffer);
				displayText.setPosition(TileTextWidth * camX, TileTextHeight * camY);
				win.draw(&displayText);
			}
		}

		// Draw look mode cursor
		if (lookMode)
		{
			static std::string lookModeCursorText(LOOK_MODE_CURSOR);
			displayText.setText(lookModeCursorText);
			displayText.setColor(LOOK_CURSOR_COLOR);
			displayText.setPosition(TileTextWidth * lookModeCursor.X,
			                        TileTextHeight * lookModeCursor.Y);
			win.draw(&displayText);
		}

		//
		// Draw sidebar
		//
		{
			int currentRowY = SidebarStartY;
			displayText.setColor(LOG_COLOR_NORMAL);
			displayText.setText("Stats");
			displayText.setPosition(SidebarStartX, SidebarStartY);
			win.draw(&displayText);
			currentRowY += TileTextHeight * 2;

			for (std::pair<const std::string, RLCombatStat>& stat : player.Stats)
			{
				std::string statName(stat.second.Name);
				std::string statValue(std::to_string(stat.second.Value));
				std::string statText(statName);
				// Add justify spaces
				for (unsigned int i = 0;
				     i < SidebarTileWidth - (statName.size() + statValue.size()) - 1; i++)
					statText += ' ';
				statText += statValue;

				displayText.setPosition(SidebarStartX, currentRowY);
				displayText.setText(statText);
				win.draw(&displayText);
				currentRowY += TileTextHeight;
			}
		}

		//
		// Draw log/status
		//
		if (lookMode)
		{
			std::string description;
			RLTile& lookTile = testMap.At(lookModeCursor.X, lookModeCursor.Y);
			std::string tileDescription = GetTileDescription(lookTile);
			std::string npcDescription;
			for (RLEntity& npc : npcs)
			{
				if (npc.X == lookModeCursor.X && npc.Y == lookModeCursor.Y)
				{
					npcDescription += npc.Description;
					break;
				}
			}

			if (!tileDescription.empty() || !npcDescription.empty())
			{
				if (player.SamePos(lookModeCursor))
					description += "You are standing on ";
				else if (description.empty())
					description += "You see ";

				if (!npcDescription.empty())
				{
					description += npcDescription;
					description += " standing on ";
					description += tileDescription;
				}
				else
					description += tileDescription;
			}

			displayText.setText(description);
			displayText.setColor(LOG_COLOR_NORMAL);
			displayText.setPosition(LogX, LogY);

			win.draw(&displayText);
		}
		else if (GameLog.size())
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