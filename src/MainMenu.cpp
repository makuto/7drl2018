#include "MainMenu.hpp"

#include "Globals.hpp"

#include "Art.hpp"
#include "Game.hpp"

std::string story =
    "You are Deathcaller, a warrior tasked with destroying the Callers in the "
    "world.\nCallers summon unnatural horrors which must be eradicated!\n\n1) Killing "
    "[c]allers will create a portal [>] to the next caller.\n\n2) Kill enough enemies in a "
    "level to get an Ability (up to one per level).\n\n3) Try to kill Callers quickly so they cannot spawn more enemies";

// Doesn't take control of window / input
void DrawGameCompletedScreen()
{
	int centerTextX = 15;
	int centerTextY = ViewTileHeight;

	std::string completedText = VICTORY;
	displayText.setText(completedText);
	displayText.setColor(LOG_COLOR_IMPORTANT);
	displayText.setPosition(centerTextX, centerTextY);
	win.draw(&displayText);

	centerTextY += TileTextHeight * 11;
	std::string storyText = "You have destroyed the Callers and vanquished the horrors!";
	displayText.setText(storyText);
	displayText.setColor(LOG_COLOR_NORMAL);
	displayText.setPosition(centerTextX, centerTextY);
	win.draw(&displayText);

	centerTextY += TileTextHeight * 2;
	std::string turnsText = "You took ";
	turnsText += std::to_string(TurnCounter);
	turnsText += " turns to slay all ";
	turnsText += std::to_string(NUM_LEVELS_TO_WIN);
	turnsText += " callers";
	displayText.setText(turnsText);
	displayText.setColor(LOG_COLOR_NORMAL);
	displayText.setPosition(centerTextX, centerTextY);
	win.draw(&displayText);

	centerTextY += TileTextHeight * 2;
	std::string detailsText =
	    "Great job! Thank you for playing Deathcall\n\nPress [Space] to continue in Infinite "
	    "Mode\nPress [Enter] to quit game";

	displayText.setText(detailsText);
	displayText.setColor(LOG_COLOR_NORMAL);
	displayText.setPosition(centerTextX, centerTextY);
	win.draw(&displayText);
}

// Doesn't take control of window / input
void DrawHelpScreen()
{
	int centerTextX = 15;
	int centerTextY = ViewTileHeight;

	std::string helpText = story;
	helpText += "\n\n4) Kill all ";
	helpText += std::to_string(NUM_LEVELS_TO_WIN);
	helpText +=
	    " to complete your task!\n\nControls\nNumpad5 or . : Pass turn (stats regenerate faster "
	    "when waiting)\nArrow keys or Numpad keys: Move up, down, left, right, or diagonally "
	    "(numpad only)\nL: Toggle look mode. Move to inspect positions\n1 through 3: Activate "
	    "respective ability\nShift + 1 through 3: Swap "
	    "abilities for one on ground \n\t(use when standing on ability scroll [!])\nCtrl + 1 "
	    "through 3: Train respective stat\n\t(trained stat will "
	    "level after stepping through portal; change at any time)\nEnter/Return: Confirm target "
	    "(if in target "
	    "mode)\nEscape: Close look/target mode (if applicable) or close the game";

	displayText.setText(helpText);
	displayText.setColor(LOG_COLOR_NORMAL);
	displayText.setPosition(centerTextX, centerTextY);
	win.draw(&displayText);
}

void IntroScreen()
{
	while (!win.shouldClose() && !inp.isPressed(inputCode::Escape))
	{
		// Play
		if (gameInp.Tapped(inputCode::Space))
			return;

		// int centerOffset = 10;
		// int centerTextX = ((ViewTileWidth / 2) - centerOffset) * TileTextWidth;
		// int centerTextY = ((ViewTileHeight / 2) - centerOffset) * TileTextHeight;
		int centerTextX = 15;
		int centerTextY = ViewTileHeight;

		std::string introStory = story;
		introStory += "\n\n4) Kill all ";
		introStory += std::to_string(NUM_LEVELS_TO_WIN);
		introStory += " to complete your task!\n\n(Hold [h] in game to display help/controls)";

		displayText.setText(introStory);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 13;
		std::string exitGame = "[Space] Enter the first portal...";
		displayText.setText(exitGame);
		displayText.setColor(LOG_COLOR_IMPORTANT);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		win.update();
	}
}

// returns whether or not to start the game
bool MainMenu()
{
	while (!win.shouldClose() && !inp.isPressed(inputCode::Escape))
	{
		// Quick restart
		if (gameInp.Tapped(inputCode::Space))
		{
			IntroScreen();
			return true;
		}

		// int centerOffset = 10;
		// int centerTextX = ((ViewTileWidth / 2) - centerOffset) * TileTextWidth;
		// int centerTextY = ((ViewTileHeight / 2) - centerOffset) * TileTextHeight;
		int centerTextX = 15;
		int centerTextY = ViewTileHeight;

		displayText.setText(TITLE);
		displayText.setColor(LOG_COLOR_IMPORTANT);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 10;
		std::string startGame = "A 7 Day Roguelike by Macoy Madson.\n\n[Space] Start Game";
		displayText.setText(startGame);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight *3;
		std::string exitGame = "[Escape] Close Game";
		displayText.setText(exitGame);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		win.update();
	}

	return false;
}
