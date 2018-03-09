#include "Gameplay.hpp"

// std::sort
#include <algorithm>
// time
#include <time.h>

#include "graphics/graphics.hpp"
#include "input/input.hpp"
#include "math/math.hpp"

#include "Globals.hpp"

#include "Abilities.hpp"
#include "Enemies.hpp"
#include "Entity.hpp"
#include "Game.hpp"
#include "GameInput.hpp"
#include "Gameplay.hpp"
#include "Levels.hpp"
#include "MainMenu.hpp"
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

	int statCounter = 0;
	for (std::pair<const std::string, RLCombatStat>& stat : gameState.player.Stats)
	{
		std::string statName(std::to_string(statCounter + 1) + ":" + stat.second.Name);
		std::string statValue(std::to_string(stat.second.Value));
		std::string statText(statName);
		// Add justify spaces
		for (unsigned int i = 0; i < SidebarTileWidth - (statName.size() + statValue.size()) - 2;
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

		statCounter++;
	}

	//
	// Leveling
	//
	currentRowY += TileTextHeight;

	displayText.setPosition(SidebarStartX, currentRowY);
	std::string levelingDisplay = "Training ";
	levelingDisplay += gameState.player.Stats[gameState.player.TrainingStatIndex].Name;
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(levelingDisplay);
	win.draw(&displayText);

	currentRowY += TileTextHeight;

	displayText.setPosition(SidebarStartX, currentRowY);
	std::string levelingHelpDisplay = "ctrl+num to change";
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(levelingHelpDisplay);
	win.draw(&displayText);

	currentRowY += TileTextHeight;

	//
	// Abilities
	//
	currentRowY += TileTextHeight;

	displayText.setPosition(SidebarStartX, currentRowY);
	std::string abilityDisplay = "Abilities";
	displayText.setColor(STATUS_COLOR_UNIMPORTANT);
	displayText.setText(abilityDisplay);
	win.draw(&displayText);

	currentRowY += TileTextHeight;

	for (unsigned int i = 0; i < PLAYER_NUM_ABILITY_SLOTS; ++i)
	{
		Ability* currentAbility = nullptr;
		if (i < gameState.player.Abilities.size())
			currentAbility = gameState.player.Abilities[i];

		displayText.setPosition(SidebarStartX, currentRowY);
		std::string abilityNameDisplay;
		abilityNameDisplay += std::to_string(i + 1);
		abilityNameDisplay += ":";
		abilityNameDisplay += currentAbility ? currentAbility->Name : "Empty";
		displayText.setColor(STATUS_COLOR_NORMAL);
		displayText.setText(abilityNameDisplay);
		win.draw(&displayText);

		currentRowY += TileTextHeight;

		displayText.setPosition(SidebarStartX, currentRowY);
		displayText.setColor(STATUS_COLOR_UNIMPORTANT);
		std::string abilityStatusDisplay;
		if (!currentAbility)
			abilityStatusDisplay = "  Empty";
		else if (currentAbility->IsCooldownDone())
			abilityStatusDisplay = "  Ready";
		else
		{
			abilityStatusDisplay += "  Ready in ";
			abilityStatusDisplay += std::to_string(currentAbility->CooldownRemaining());
			displayText.setColor(STATUS_COLOR_IMPORTANT);
		}

		displayText.setText(abilityStatusDisplay);
		win.draw(&displayText);

		currentRowY += TileTextHeight;
	}

	//
	// Progression
	//

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
	gameState.abilitiesUpdatingFx.clear();

	// TODO: Remove before ship!
	gameState.enableCheats = true;

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
	int playerAbilityActivatedIndex = -1;
	int targetX = -1;
	int targetY = -1;

	// State
	bool lookMode = false;
	bool waitForTargetMode = false;
	bool playerDead = false;

	timer frameTimer;
	frameTimer.start();
	float lastFrameTime = 0.032f;

	while (!win.shouldClose())
	{
		bool playerTurnPerformed = false;
		bool playerTargetAcquired = false;
		std::string standingOnDisplay;

		//
		// Input
		//

		// Help screen
		if (inp.isPressed(inputCode::H))
		{
			DrawHelpScreen();
			win.update();
			continue;
		}

		// Player dead continue
		if (playerDead && gameInp.Tapped(inputCode::Space))
			break;

		// TODO: Reset before ship!
		if (!playerDead && gameState.enableCheats)
		{
			// Set health to high number
			if (gameInp.Tapped(inputCode::F1))
			{
				RLCombatStat& health = gameState.player.Stats["HP"];
				health.Max = 10000;
				health.Value = 10000;
				LOGD << "Cheat: health up";
			}
			// Reset cooldowns
			if (gameInp.Tapped(inputCode::F2))
			{
				for (Ability* ability : gameState.player.Abilities)
				{
					if (!ability)
						continue;
					ability->ActivatedOnTurn = -1;
				}
				LOGD << "Cheat: skip cooldowns";
			}
			// Level up
			if (gameInp.Tapped(inputCode::F3))
			{
				gameState.player.LevelUp();
				LOGD << "Cheat: Level up";
			}
			// Next level
			if (gameInp.Tapped(inputCode::F4))
			{
				LoadNextLevel();
				TurnCounter++;
				LOGI << "You step through to the next level";

				LOGD << "Cheat: Next Level";

				continue;
			}
			// Randomize abilities
			if (gameInp.Tapped(inputCode::F6))
			{
				int counter = 0;
				for (Ability* ability : gameState.player.Abilities)
				{
					if (ability)
						delete ability;

					gameState.player.Abilities[counter] = getNewRandomAbility();
					counter++;
				}
				LOGD << "Cheat: Randomize abilities";
			}
			if (gameInp.Tapped(inputCode::F7))
			{
				RLCombatStat& health = gameState.player.Stats["HP"];
				health.Value = 0;
				playerDead = true;
				playerTurnPerformed = true;
				LOGD << "Cheat: suicide";
			}
		}

		// Return / Enter accepts target // (no longer) or closes the game
		if (!playerDead && gameInp.Tapped(inputCode::Return))
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
			// else
			// 	// Close the game
			// 	break;
		}

		// Escape exits modes or closes the game
		if (gameInp.Tapped(inputCode::Escape))
		{
			// Reset targeting and cancel activation
			if (waitForTargetMode)
			{
				playerAbilityActivatedIndex = -1;
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
			}
			// Exiting look mode; recenter on player if necessary
			else
			{
				LOGI << LOOK_MODE_EXIT;
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
		else if (!playerDead)
		{
			// Player training and abilities via number keys
			std::vector<int> numberKeysPressed = gameInp.GetInpNumbersTapped();
			if (!numberKeysPressed.empty())
			{
				// Swap ability
				if (inp.isPressed(inputCode::LShift) || inp.isPressed(inputCode::RShift))
				{
					std::vector<RLEntity*> entitiesAtPosition =
					    getEntitiesAtPosition(gameState.player.X, gameState.player.Y);

					if (!entitiesAtPosition.empty())
					{
						for (RLEntity* ent : entitiesAtPosition)
						{
							if (ent->Type == ABILITY_TYPE)
							{
								int abilityIndex = numberKeysPressed[0] - 1;
								if (abilityIndex >= PLAYER_NUM_ABILITY_SLOTS)
									break;

								Enemy* enemy = static_cast<Enemy*>(ent);
								Ability* slottedAbility = gameState.player.Abilities[abilityIndex];
								std::string lastAbilityName =
								    slottedAbility ? slottedAbility->Name : "empty slot";
								gameState.player.Abilities[abilityIndex] = enemy->DroppedAbility;
								enemy->DroppedAbility = slottedAbility;
								LOGI << "Swapped " << lastAbilityName.c_str() << " for "
								     << gameState.player.Abilities[abilityIndex]->Name.c_str();

								if (!enemy->DroppedAbility)
								{
									enemy->Type = CORPSE_TYPE;
									enemy->Color = {ENEMY_COLOR_NORMAL};
									enemy->Description = "ash";
								}
								else
								{
									// This isn't the only place where they're called scroll of, fyi
									// if you want to change it
									enemy->Description = "scroll of ";
									enemy->Description += enemy->DroppedAbility->Name;
								}
							}
						}
					}
				}
				// Change training
				else if (inp.isPressed(inputCode::LControl) || inp.isPressed(inputCode::RControl))
				{
					int statIndex = numberKeysPressed[0] - 1;
					int currentStatIndex = 0;
					for (std::pair<const std::string, RLCombatStat>& stat : gameState.player.Stats)
					{
						if (currentStatIndex != statIndex)
						{
							++currentStatIndex;
							continue;
						}

						if (gameState.player.TrainingStatIndex != stat.first)
							LOGI << TRAINING_CHANGED << stat.second.Name.c_str();

						gameState.player.TrainingStatIndex = stat.first;

						break;
					}
				}
				// Activate ability
				else
				{
					for (unsigned int i = 0; i < gameState.player.Abilities.size(); ++i)
					{
						Ability* currentAbility = gameState.player.Abilities[i];

						if (!currentAbility)
							continue;

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
							RLEntity* closestEntity = getClosestNonTraversableEntity(
							    gameState.player.X, gameState.player.Y);
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

						playerAbilityActivatedIndex = i;

						// Only activate one ability (other keypresses are just ignored)
						break;
					}
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

						std::string abilityDescription;
						if (playerCanSwapAbilityNow(&abilityDescription))
						{
							standingOnDisplay += " (Press shift+[1-3] to swap ability for ";
							standingOnDisplay += abilityDescription;
							standingOnDisplay += ")";
						}
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

					gameState.player.LevelUp();

					// This is probably unnecessary
					playerAbilityActivatedIndex = -1;

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
		// If we activated an ability and we're done waiting for target, count the turn (and process
		// the ability)
		if (playerAbilityActivatedIndex != -1 && !waitForTargetMode && !playerDead)
		{
			playerTurnPerformed = true;
		}

		if (!playerDead && playerTurnPerformed)
		{
			TurnCounter++;

			if (!standingOnDisplay.empty())
			{
				LOGI << standingOnDisplay.c_str();
				standingOnDisplay.clear();  // probably pointless
			}

			// Reset FX layer every turn so new fx can do their thing
			gameState.vfx.ResetTiles();
			gameState.abilitiesUpdatingFx.clear();

			// Handle player ability activation
			if (playerAbilityActivatedIndex != -1)
			{
				bool foundAbility = false;
				Ability* activateAbility = gameState.player.Abilities[playerAbilityActivatedIndex];
				if (activateAbility)
				{
					foundAbility = true;

					LOGD << "Player activated ability " << activateAbility->Name.c_str() << " turn "
					     << TurnCounter;

					if (activateAbility->RequiresTarget)
					{
						LOGD << "Player targeted " << targetX << ", " << targetY;
						activateAbility->PlayerActivateWithTarget(targetX, targetY);
					}
					else
						activateAbility->PlayerActivateNoTarget();
				}

				if (!foundAbility)
				{
					LOGE << "Player activated ability " << playerAbilityActivatedIndex
					     << " but it failed (couldn't be found)";
				}

				playerAbilityActivatedIndex = -1;
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
				gameState.player.Type = CORPSE_TYPE;
				LOGI << "You died!";
				playerDead = true;
			}
		}

		//
		// Update ability FX
		//
		for (Ability* ability : gameState.abilitiesUpdatingFx)
		{
			ability->FxUpdate(lastFrameTime);
			ability->TotalFrameTimeAlive += lastFrameTime;
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
			                        TileTextHeight * ((lookModeCursor.Y - camYOffset) + ViewTileTopMargin));
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
			std::string wrappedLogText = GameLog.back();

			if (playerDead)
			{
				displayText.setColor(LOG_COLOR_DEAD);
				wrappedLogText += " Press [space] to continue...";
			}
			else
				displayText.setColor(LOG_COLOR_NORMAL);

			WrapText(wrappedLogText, true);
			displayText.setText(wrappedLogText);
			displayText.setPosition(LogX, LogY);
			win.draw(&displayText);
		}

		win.update();
		lastFrameTime = frameTimer.getTime();
		frameTimer.start();
	}

	if (playerDead)
	{
		return PlayerDeadScreen();
	}

	return false;
}