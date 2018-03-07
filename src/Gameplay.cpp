#include "Gameplay.hpp"

// std::sort
#include <algorithm>
// time
#include <time.h>

#include "graphics/graphics.hpp"
#include "input/input.hpp"
#include "math/math.hpp"

#include "Globals.hpp"

#include "Enemies.hpp"
#include "Entity.hpp"
#include "Game.hpp"
#include "GameInput.hpp"
#include "Gameplay.hpp"
#include "Levels.hpp"
#include "RLMap.hpp"
#include "Utilities.hpp"

#include "PlayerDeadScreen.hpp"

void UpdateCameraOffset(RLEntity* cameraTrackingEntity, int& camXOffset, int& camYOffset)
{
	if (!cameraTrackingEntity)
		return;

	if (cameraTrackingEntity->X - camXOffset < CamSnapLeftBounds ||
	    cameraTrackingEntity->X - camXOffset > CamSnapRightBounds)
		camXOffset = cameraTrackingEntity->X - (ViewTileWidth / 2);
	if (cameraTrackingEntity->Y - camYOffset < CamSnapTopBounds ||
	    cameraTrackingEntity->Y - camYOffset > CamSnapBottomBounds)
		camYOffset = cameraTrackingEntity->Y - (ViewTileHeight / 2);
}

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

	// Turn counter
	currentRowY += TileTextHeight;
	displayText.setPosition(SidebarStartX, currentRowY);
	std::string turnDisplay = "Turn ";
	turnDisplay += std::to_string(TurnCounter);
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(turnDisplay);
	win.draw(&displayText);

	currentRowY += TileTextHeight;

	// Level name
	currentRowY += TileTextHeight;
	displayText.setPosition(SidebarStartX, currentRowY);
	std::string levelNameDisplay = gameState.levelName;
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(levelNameDisplay);
	win.draw(&displayText);

	// Level
	currentRowY += TileTextHeight;
	displayText.setPosition(SidebarStartX, currentRowY);
	std::string levelDisplay = "Level ";
	levelDisplay += std::to_string(gameState.currentLevel);
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(levelDisplay);
	win.draw(&displayText);
}

// Returns whether it should restart or not
bool PlayGame()
{
	RecalculateDimensions();

	LOGI << "Hello 7DRL!\n";

	//
	// Reset globals
	//
	GameLog.clear();
	TurnCounter = 0;
	gameState.seed = time(nullptr);
	gameState.currentLevel = 0;
	gameState.levelName.clear();
	ClearNpcs();
	gameState.player.Initialize();

	//
	// Level initialization
	//
	LoadNextLevel();

	//
	// Game loop variables
	//
	RLEntity lookModeCursor;

	int playerMeleeAttackNpcGuid = 0;

	// camera
	int camXOffset = 0;
	int camYOffset = 0;
	RLEntity* cameraTrackingEntity = nullptr;
	UpdateCameraOffset(&gameState.player, camXOffset, camYOffset);

	// Target
	std::string playerAbilityActivatedName;
	int targetX = -1;
	int targetY = -1;

	// State
	bool lookMode = false;
	bool waitForTargetMode = false;
	bool playerDead = false;

	while (!win.shouldClose())
	{
		bool playerTurnPerformed = false;
		bool playerTargetAcquired = false;
		std::string standingOnDisplay;

		//
		// Input
		//

		// Return / Enter accepts target or closes the game
		if (gameInp.Tapped(inputCode::Return))
		{
			// Confirm target
			if (waitForTargetMode)
			{
				waitForTargetMode = false;
				targetX = lookModeCursor.X;
				targetY = lookModeCursor.Y;

				playerTargetAcquired = true;
			}

			if (lookMode)
			{
				lookMode = false;

				cameraTrackingEntity = &gameState.player;
				UpdateCameraOffset(cameraTrackingEntity, camXOffset, camYOffset);
			}
			else
				// Close the game
				break;
		}

		// Escape exits modes or closes the game
		if (gameInp.Tapped(inputCode::Escape))
		{
			// Reset targeting and cancel activation
			if (waitForTargetMode)
			{
				playerAbilityActivatedName.clear();
				waitForTargetMode = false;
				LOGI << CANCELLED_ABILITY_ACTIVATE;
			}

			if (lookMode)
			{
				lookMode = false;

				cameraTrackingEntity = &gameState.player;
				UpdateCameraOffset(cameraTrackingEntity, camXOffset, camYOffset);
			}
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

				LOGI << LOOK_MODE_EXIT;
			}
			// Exiting look mode; recenter on player if necessary
			else
			{
				cameraTrackingEntity = &gameState.player;
				UpdateCameraOffset(cameraTrackingEntity, camXOffset, camYOffset);
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
		else
		{
			// Player abilities
			std::vector<int> numberKeysPressed = gameInp.GetInpNumbersTapped();
			if (!numberKeysPressed.empty())
			{
				for (unsigned int i = 0; i < gameState.player.Abilities.size(); ++i)
				{
					Ability* currentAbility = gameState.player.Abilities[i];
					std::vector<int>::iterator findIt =
					    std::find(numberKeysPressed.begin(), numberKeysPressed.end(), i + 1);
					if (findIt == numberKeysPressed.end())
						continue;

					if (!currentAbility->IsCooldownDone())
					{
						LOGI << ABILITY_ON_COOLDOWN << " (ready in "
						     << currentAbility->CooldownRemaining() << " turns)";
						continue;
					}

					if (currentAbility->RequiresTarget)
					{
						waitForTargetMode = true;

						// Enter look mode
						lookMode = true;

						// Move cursor to closest enemy if possible
						RLEntity* closestEntity =
						    getClosestNonTraversableEntity(gameState.player.X, gameState.player.Y);
						if (closestEntity &&
						    distanceTo(gameState.player.X, gameState.player.Y, closestEntity->X,
						               closestEntity->Y) < MAX_PLAYER_TARGET_DIST)
						{
							lookModeCursor.X = closestEntity->X;
							lookModeCursor.Y = closestEntity->Y;
						}
						else
						{
							lookModeCursor.X = gameState.player.X;
							lookModeCursor.Y = gameState.player.Y;
						}
					}
					else
						waitForTargetMode = false;

					playerAbilityActivatedName = currentAbility->Name;

					// Only activate one ability (other keypresses are just ignored)
					break;
				}
			}
			// Player navigation control
			else if (deltaX || deltaY)
			{
				cameraTrackingEntity = &gameState.player;

				RLEntity* npcOut = nullptr;
				if (canMeleeAttack(gameState.player, deltaX, deltaY, gameState.npcs, &npcOut))
				{
					playerMeleeAttackNpcGuid = npcOut->Guid;
					gameState.player.ThisTurnAction = PlayerAction::MeleeAttacked;
					playerTurnPerformed = true;
				}
				else if (canMoveTo(gameState.player, deltaX, deltaY, gameState.currentMap))
				{
					gameState.player.X += deltaX;
					gameState.player.Y += deltaY;

					// Decide whether we need to make this a turn
					// TODO: This code will not work if speed > 1
					if (abs(deltaX) >= gameState.player.Speed ||
					    abs(deltaY) >= gameState.player.Speed)
					{
						playerTurnPerformed = true;
					}

					std::vector<RLEntity*> entitiesAtPosition =
					    getEntitiesAtPosition(gameState.player.X, gameState.player.Y);

					if (!entitiesAtPosition.empty())
					{
						standingOnDisplay += "You stomp on ";
						standingOnDisplay += describePosition(lookModeCursor.X, lookModeCursor.Y);

						std::string stairsDescription;
						if (playerCanUseStairsNow(&stairsDescription))
						{
							standingOnDisplay += " (Press > to use ";
							standingOnDisplay += stairsDescription;
							standingOnDisplay += ")";
						}
						else
						{
							standingOnDisplay += npcOut->Description;
						}
					}
				}
				else
				{
					LOGI << WALL_BUMP;
				}
			}
			// Player descend stairs
			else if ((inp.isPressed(inputCode::LShift) || inp.isPressed(inputCode::RShift)) &&
			         inp.isPressed(inputCode::Period))
			{
				if (playerCanUseStairsNow(nullptr))
				{
					LoadNextLevel();
					TurnCounter++;
					LOGI << "You step through to the next level";
					playerAbilityActivatedName = "";
					// Start the game loop over because it seems right ?
					continue;
				}
			}
			// Player pass turn
			else if (gameInp.Tapped(inputCode::Numpad5) || gameInp.Tapped(inputCode::Period))
			{
				playerTurnPerformed = true;
				gameState.player.ThisTurnAction = PlayerAction::Rested;
			}
		}

		// Perform camera snapping if outside bounds (note that we can move the view around with
		// look mode as well)
		UpdateCameraOffset(cameraTrackingEntity, camXOffset, camYOffset);

		//
		// Turn update
		//
		// Special case: handle no target or two-stage targeted abilities
		if (!playerAbilityActivatedName.empty() && !waitForTargetMode)
		{
			playerTurnPerformed = true;
		}

		if (playerTurnPerformed)
		{
			TurnCounter++;

			if (!standingOnDisplay.empty())
				LOGI << standingOnDisplay.c_str();

			// Handle player ability activation
			if (!playerAbilityActivatedName.empty())
			{
				bool foundAbility = false;
				for (Ability* currentAbility : gameState.player.Abilities)
				{
					if (currentAbility->Name != playerAbilityActivatedName)
						continue;

					foundAbility = true;

					LOGD << "Player activated ability " << currentAbility->Name.c_str() << " turn "
					     << TurnCounter;

					if (currentAbility->RequiresTarget)
					{
						LOGD << "Player targeted " << targetX << ", " << targetY;
						currentAbility->PlayerActivateWithTarget(targetX, targetY);
					}
					else
					{
						currentAbility->PlayerActivateNoTarget();
					}

					break;
				}

				if (!foundAbility)
				{
					LOGE << "Player activated ability " << playerAbilityActivatedName.c_str()
					     << " but it failed (couldn't be found)";
				}

				playerAbilityActivatedName.clear();
			}

			// Handle player's melee combat attack
			if (playerMeleeAttackNpcGuid)
			{
				RLEntity* attackedNpc = findEntityById(gameState.npcs, playerMeleeAttackNpcGuid);
				if (attackedNpc)
				{
					playerMeleeAttackEnemy(attackedNpc);
				}

				// Finished attack
				playerMeleeAttackNpcGuid = 0;
			}

			// Update player
			gameState.player.DoTurn();

			// Update NPCs
			for (RLEntity* npc : gameState.npcs)
			{
				npc->DoTurn();
			}
			gameState.npcs.insert(gameState.npcs.begin(), gameState.npcsToCreate.begin(),
			                      gameState.npcsToCreate.end());
			// I forgot this call at first lol
			gameState.npcsToCreate.clear();

			// Resolve NPC movement
			// Sort so closest move first (that way they can move tightly together)
			std::sort(gameState.npcs.begin(), gameState.npcs.end(),
			          sortEntitiesByAscendingDistFromPlayer);
			for (RLEntity* npc : gameState.npcs)
			{
				if (npc->VelocityX || npc->VelocityY)
				{
					// Gods forgive me
					if (gameState.player.X == npc->X + npc->VelocityX &&
					    gameState.player.Y == npc->Y + npc->VelocityY)
					{
						enemyMeleeAttackPlayer(npc);
					}
					else
					{
						if (abs(npc->VelocityX) && abs(npc->VelocityY))
						{
							if (canMoveTo(*npc, npc->VelocityX, npc->VelocityY,
							              gameState.currentMap) &&
							    !isNonTraversableEntityAtPosition(npc->X + npc->VelocityX,
							                                      npc->Y + npc->VelocityY))
							{
								npc->X += npc->VelocityX;
								npc->Y += npc->VelocityY;
							}
						}
						else
						{
							// Do it piecewise
							if (canMoveTo(*npc, npc->VelocityX, 0, gameState.currentMap) &&
							    !isNonTraversableEntityAtPosition(npc->X + npc->VelocityX, npc->Y))
								npc->X += npc->VelocityX;
							else if (canMoveTo(*npc, 0, npc->VelocityY, gameState.currentMap) &&
							         !isNonTraversableEntityAtPosition(npc->X,
							                                           npc->Y + npc->VelocityY))
								npc->Y += npc->VelocityY;
						}
					}

					npc->VelocityX = 0;
					npc->VelocityY = 0;
				}
			}

			if (!gameState.player.Stats["HP"].Value)
			{
				LOGI << "You died!";
				playerDead = true;
				break;
			}
		}

		//
		// Draw map, npcs, player, etc.
		//
		DrawWorld(gameState.currentMap, camXOffset, camYOffset);

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
		// Describe what the player is looking at
		if (lookMode)
		{
			std::string description;

			if (waitForTargetMode)
				description += "[TARGET MODE] ";

			if (gameState.player.SamePos(lookModeCursor))
				description += "You are standing on ";
			else
				description += "You see ";

			description += describePosition(lookModeCursor.X, lookModeCursor.Y);

			displayText.setText(description);
			displayText.setColor(LOG_COLOR_NORMAL);
			displayText.setPosition(LogX, LogY);

			win.draw(&displayText);
		}
		// Show the last turn's log
		else if (GameLog.size() && LastTurnLog == TurnCounter)
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