#include "Gameplay.hpp"

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

void DrawSidebar()
{
	int currentRowY = SidebarStartY;
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText("Status");
	displayText.setPosition(SidebarStartX, SidebarStartY);
	win.draw(&displayText);
	currentRowY += TileTextHeight * 2;

	for (std::pair<const std::string, RLCombatStat>& stat : gameState.player.Stats)
	{
		std::string statName(stat.second.Name);
		std::string statValue(std::to_string(stat.second.Value));
		std::string statText(statName);
		// Add justify spaces
		for (unsigned int i = 0; i < SidebarTileWidth - (statName.size() + statValue.size()) - 1;
		     i++)
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

bool PlayGame()
{
	RecalculateDimensions();

	LOGI << "Hello 7DRL!\n";

	//
	// Reset globals
	//
	GameLog.clear();
	TurnCounter = 0;
	gameState.npcs.clear();
	gameState.player.Initialize();

	//
	// Game initialization
	//
	gameState.player.X = ViewTileWidth / 2;
	gameState.player.Y = ViewTileHeight / 2;

	RLEntity lookModeCursor;

	RLMap testMap(ViewTileWidth, ViewTileHeight);
	// createTestMap(testMap);
	createTestMapNoise(testMap);

	Enemy skeleton[3];
	for (int i = 0; i < 3; ++i)
	{
		skeleton[i].X = (ViewTileWidth / 2) + 3 + i;
		skeleton[i].Y = ViewTileHeight / 2;
		skeleton[i].Speed = 1;
		skeleton[i].Type = SKELETON_TYPE;
		skeleton[i].Color = {ENEMY_COLOR_NORMAL};
		skeleton[i].Description = "skeleton";
		skeleton[i].Stats["HP"] = {"Health", PLAYER_STARTING_MAX_HEALTH,
		                           PLAYER_STARTING_MAX_HEALTH};
		gameState.npcs.push_back(&skeleton[i]);
	}

	int playerMeleeAttackNpcGuid = 0;

	int camXOffset = 0;
	int camYOffset = 0;
	RLEntity* cameraTrackingEntity = nullptr;
	UpdateCameraOffset(&gameState.player, camXOffset, camYOffset);

	bool lookMode = false;
	bool playerDead = false;

	while (!win.shouldClose() && !inp.isPressed(inputCode::Return))
	{
		bool playerTurnPerformed = false;
		std::string standingOnDisplay;

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
				lookModeCursor.X = gameState.player.X;
				lookModeCursor.Y = gameState.player.Y;
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
		else if (deltaX || deltaY)
		{
			cameraTrackingEntity = &gameState.player;

			RLEntity* npcOut = nullptr;
			if (canMeleeAttack(gameState.player, deltaX, deltaY, gameState.npcs, &npcOut))
			{
				playerMeleeAttackNpcGuid = npcOut->Guid;
				playerTurnPerformed = true;
			}
			else if (canMoveTo(gameState.player, deltaX, deltaY, testMap))
			{
				gameState.player.X += deltaX;
				gameState.player.Y += deltaY;

				// Decide whether we need to make this a turn
				if (abs(deltaX) >= gameState.player.Speed || abs(deltaY) >= gameState.player.Speed)
				{
					playerTurnPerformed = true;
				}

				if (npcOut)
				{
					standingOnDisplay += "You stomp on a ";
					standingOnDisplay += npcOut->Description;
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

			if (!standingOnDisplay.empty())
				LOGI << standingOnDisplay.c_str();

			// Update NPCs
			for (RLEntity* npc : gameState.npcs)
			{
				npc->DoTurn();
			}

			// Handle melee combat
			if (playerMeleeAttackNpcGuid)
			{
				RLEntity* attackedNpc = findEntityById(gameState.npcs, playerMeleeAttackNpcGuid);
				if (attackedNpc)
				{
					int damage = 10;
					int staminaCost = 10;
					// If out of stamina, take it out of health
					int healthCost = gameState.player.Stats["SP"].Value < staminaCost ?
					                     gameState.player.Stats["SP"].Value - staminaCost :
					                     0;
					gameState.player.Stats["SP"].Add(-staminaCost);
					gameState.player.Stats["HP"].Add(healthCost);

					attackedNpc->Stats["HP"].Add(-damage);
					LOGI << "You hit the " << attackedNpc->Description.c_str() << " for " << damage
					     << " damage";
					if (healthCost)
						LOGI << "You feel weaker from overexertion";
				}

				// Finished attack
				playerMeleeAttackNpcGuid = 0;
			}

			if (!gameState.player.Stats["HP"].Value)
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
				for (RLEntity* npc : gameState.npcs)
				{
					if (npc->X == tileX && npc->Y == tileY && !gameState.player.SamePos(*npc))
					{
						buffer += npc->Type;
						SetTextColor(displayText, npc->Color);

						shouldDrawTile = false;
						break;
					}
				}

				if (buffer.empty() && gameState.player.X == tileX && gameState.player.Y == tileY)
				{
					buffer += gameState.player.Type;
					SetTextColor(displayText, gameState.player.Color);
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
		DrawSidebar();

		//
		// Draw log/status
		//
		if (lookMode)
		{
			std::string description;
			RLTile* lookTile = testMap.At(lookModeCursor.X, lookModeCursor.Y);
			std::string tileDescription = lookTile ? GetTileDescription(*lookTile) : "";
			std::string npcDescription;
			for (RLEntity* npc : gameState.npcs)
			{
				if (npc->X == lookModeCursor.X && npc->Y == lookModeCursor.Y)
				{
					if (npc->Type != CORPSE_TYPE &&
					    npc->Stats["HP"].Value < npc->Stats["HP"].Max * .2f)
						npcDescription += "wounded ";
					npcDescription += npc->Description;
					if (npc->Type == CORPSE_TYPE)
						npcDescription += " lying on ";
					else
						npcDescription += " standing on ";

					break;
				}
			}

			if (!tileDescription.empty() || !npcDescription.empty())
			{
				if (gameState.player.SamePos(lookModeCursor))
					description += "You are standing on ";
				else if (description.empty())
					description += "You see ";

				if (!npcDescription.empty())
				{
					description += "a ";
					description += npcDescription;
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