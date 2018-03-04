#include <iostream>

#include <map>

#include "graphics/graphics.hpp"
#include "input/input.hpp"

#include "Game.hpp"
#include "GameInput.hpp"
#include "RLMap.hpp"
#include "Utilities.hpp"
#include "Entity.hpp"
#include "Levels.hpp"

#include "PlayerDeadScreen.hpp"

int TurnCounter = 0;

std::vector<std::string> GameLog;
int lastTurnLog = 0;

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

window win(WindowWidth, WindowHeight, "7DRL 2018 by Makuto");
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
	win.getBase()->setFramerateLimit(60);
}

void initializeDisplayText(text& text)
{
	if (!text.loadFont("data/fonts/UbuntuMono-R.ttf"))
		LOGE << "Error: Cannot load default text font. Nothing will work\n";
}

void SetTextColor(text& text, RLColor& color)
{
	text.setColor(color.r, color.g, color.b, color.a);
}

void RecalculateDimensions()
{
	TileTextWidth = TileTextHeight / 2;

	ViewTileRightMargin = SidebarTileWidth + 1;
	ViewTileWidth = (WindowWidth / TileTextWidth) - ViewTileRightMargin;
	ViewTileHeight = (WindowHeight / TileTextHeight) - ViewTileBottomMargin;

	CamSnapLeftBounds = ViewTileWidth * .2f;
	CamSnapTopBounds = ViewTileHeight * .2f;
	CamSnapRightBounds = ViewTileWidth * .8f;
	CamSnapBottomBounds = ViewTileHeight * .8f;

	SidebarStartX = (ViewTileWidth + 1) * TileTextWidth;
	SidebarStartY = 0 * TileTextHeight;

	LogY = ViewTileHeight * TileTextHeight;
}

void UpdateCameraOffset(RLEntity* cameraTrackingEntity, int& camXOffset, int& camYOffset)
{
	if (!cameraTrackingEntity)
		return;

	if (cameraTrackingEntity->X - camXOffset < CamSnapLeftBounds ||
	    cameraTrackingEntity->Y - camYOffset < CamSnapTopBounds ||
	    cameraTrackingEntity->X - camXOffset > CamSnapRightBounds ||
	    cameraTrackingEntity->Y - camYOffset > CamSnapBottomBounds)
	{
		camXOffset = cameraTrackingEntity->X - (ViewTileWidth / 2);
		camYOffset = cameraTrackingEntity->Y - (ViewTileHeight / 2);
	}
}

bool PlayGame();
int main(int argc, char const* argv[])
{
	initializeWindow(win);
	RecalculateDimensions();

	//
	// Display initialization
	//
	initializeDisplayText(displayText);
	displayText.setSize(TileTextHeight);
	displayText.setColor(WALL_TILE_COLOR_NORMAL);

	bool shouldRestart = true;
	while (shouldRestart)
		shouldRestart = PlayGame();

	return 0;
}

bool PlayGame()
{
	RecalculateDimensions();

	LOGI << "Hello 7DRL!\n";

	//
	// Reset globals
	//
	GameLog.clear();
	TurnCounter = 0;

	//
	// Game initialization
	//
	Player player;
	player.X = ViewTileWidth / 2;
	player.Y = ViewTileHeight / 2;

	RLEntity lookModeCursor;

	RLMap testMap(ViewTileWidth, ViewTileHeight);
	// createTestMap(testMap);
	createTestMapNoise(testMap);

	RLEntity skeleton;
	skeleton.X = (ViewTileWidth / 2) + 3;
	skeleton.Y = ViewTileHeight / 2;
	skeleton.Speed = 1;
	skeleton.Type = SKELETON_TYPE;
	skeleton.Color = {ENEMY_COLOR_NORMAL};
	skeleton.Description = "a skeleton";
	skeleton.Stats["HP"] = {"Health", PLAYER_STARTING_MAX_HEALTH, PLAYER_STARTING_MAX_HEALTH};

	std::vector<RLEntity> npcs{skeleton};

	int playerMeleeAttackNpcGuid = 0;

	int camXOffset = 0;
	int camYOffset = 0;
	RLEntity* cameraTrackingEntity = nullptr;
	UpdateCameraOffset(&player, camXOffset, camYOffset);

	bool lookMode = false;
	bool playerDead = false;

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

		gameInp.GetInpDirectionalNavigation(deltaX, deltaY);

		if (lookMode)
		{
			if (deltaX || deltaY)
			{
				lookModeCursor.X += deltaX;
				lookModeCursor.Y += deltaY;
			}

			cameraTrackingEntity = &lookModeCursor;
		}
		// Player control
		else
		{
			cameraTrackingEntity = &player;

			RLEntity* npcOut = nullptr;
			if (canMeleeAttack(player, deltaX, deltaY, npcs, &npcOut))
			{
				playerMeleeAttackNpcGuid = npcOut->Guid;
				playerTurnPerformed = true;
			}
			else if (canMoveTo(player, deltaX, deltaY, testMap))
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

			// Pass turn
			if (gameInp.Tapped(inputCode::Numpad5) || gameInp.Tapped(inputCode::Period))
				playerTurnPerformed = true;
		}

		// Perform camera snapping if outside bounds (note that we can move the view around with
		// look mode as well)
		UpdateCameraOffset(cameraTrackingEntity, camXOffset, camYOffset);

		//
		// Turn update
		//
		if (playerTurnPerformed)
		{
			TurnCounter++;

			// Handle melee combat
			if (playerMeleeAttackNpcGuid)
			{
				RLEntity* attackedNpc = findEntityById(npcs, playerMeleeAttackNpcGuid);
				if (attackedNpc)
				{
					int damage = 10;
					int staminaCost = 10;
					// If out of stamina, take it out of health
					int healthCost = player.Stats["SP"].Value < staminaCost ?
					                     player.Stats["SP"].Value - staminaCost :
					                     0;
					player.Stats["SP"].Add(-10);
					player.Stats["HP"].Add(healthCost);

					attackedNpc->Stats["HP"].Value -= damage;
					LOGI << "You hit " << attackedNpc->Description.c_str() << " for " << damage
					     << " damage";
					if (healthCost)
						LOGI << "You feel weaker from exertion";
				}

				// Finished attack
				playerMeleeAttackNpcGuid = 0;
			}

			if (!player.Stats["HP"].Value)
			{
				LOGI << "You died!";
				playerDead = true;
				break;
			}
		}

		//
		// Draw map
		//
		for (int camY = 0; camY < MIN(testMap.Height, ViewTileHeight); ++camY)
		{
			for (int camX = 0; camX < MIN(testMap.Width, ViewTileWidth); ++camX)
			{
				int tileX = camX + camXOffset;
				int tileY = camY + camYOffset;
				RLTile* currentTile = testMap.At(tileX, tileY);

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
					if (currentTile)
					{
						buffer += currentTile->Type;
						SetTextColor(displayText, currentTile->Color);
					}
					else
						buffer += " ";
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
			displayText.setPosition(TileTextWidth * (lookModeCursor.X - camXOffset),
			                        TileTextHeight * (lookModeCursor.Y - camYOffset));
			win.draw(&displayText);
		}

		//
		// Draw sidebar
		//
		{
			int currentRowY = SidebarStartY;
			displayText.setColor(STATUS_COLOR_UNIMPORTANT);
			displayText.setText("Status");
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
				// If at the last 20% of the stat, emphasize it
				if (stat.second.Value <= stat.second.Max * .20f)
					displayText.setColor(STATUS_COLOR_IMPORTANT);
				else
					displayText.setColor(STATUS_COLOR_NORMAL);
				win.draw(&displayText);
				currentRowY += TileTextHeight;
			}

			currentRowY += TileTextHeight;
			displayText.setPosition(SidebarStartX, currentRowY);
			std::string turnDisplay = "Turn ";
			turnDisplay += std::to_string(TurnCounter);
			displayText.setColor(STATUS_COLOR_UNIMPORTANT);
			displayText.setText(turnDisplay);
			win.draw(&displayText);
		}

		//
		// Draw log/status
		//
		if (lookMode)
		{
			std::string description;
			RLTile* lookTile = testMap.At(lookModeCursor.X, lookModeCursor.Y);
			std::string tileDescription = lookTile ? GetTileDescription(*lookTile) : "";
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
		else if (GameLog.size() && lastTurnLog == TurnCounter)
		{
			displayText.setText(GameLog.back());
			displayText.setColor(LOG_COLOR_NORMAL);
			displayText.setPosition(LogX, LogY);
			win.draw(&displayText);
		}

		win.update();
	}

	if (playerDead)
	{
		return PlayerDeadScreen();
	}

	return false;
}