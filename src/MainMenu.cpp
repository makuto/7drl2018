#include "MainMenu.hpp"

#include "Globals.hpp"

#include "Art.hpp"
#include "Game.hpp"

// Doesn't take control of window / input
void DrawHelpScreen()
{
	int centerOffsetPixels = 50;
	int centerTextX = centerOffsetPixels;
	int centerTextY = centerOffsetPixels;

	// displayText.setText("You have died!");
	std::string story =
	    "You are Deathcaller, a warrior tasked with destroying the Callers in the "
	    "world.\nCallers summon unnatural horrors which must be eradicated!\n\n1) Killing "
	    "[c]allers will create a portal [>] to the next caller.\n\n2) Kill enough enemies in a "
	    "level to get an Ability (up to one per level).";
	story += "\n\n3) Kill all ";
	story += std::to_string(NUM_LEVELS_TO_WIN);
	story +=
	    " to complete your task!\n\nControls\nNumpad5 or . : Pass turn (stats regenerate faster "
	    "when waiting)\nArrow keys or Numpad keys: Move up, down, left, right, or diagonally "
	    "(numpad only)\nL: Toggle look mode. Move to inspect positions\n1 through 3: Activate "
	    "respective ability\n\t(trained stat will "
	    "level after stepping through portal; change at any time)\nShift + 1 through 3: Swap "
	    "abilities for one on ground \n\t(use when standing on ability scroll [!])\nCtrl + 1 through "
	    "3: Train respective stat\n\nEscape: Close look/target mode (if applicable) or close the game";

	displayText.setText(story);
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
		int centerOffsetPixels = 100;
		int centerTextX = centerOffsetPixels;
		int centerTextY = centerOffsetPixels;

		// displayText.setText("You have died!");
		std::string story =
		    "You are Deathcaller, a warrior tasked with destroying the Callers in the "
		    "world.\nCallers summon unnatural horrors which must be eradicated!\n\n1) Killing "
		    "[c]allers will create a portal [>] to the next caller.\n\n2) Kill enough enemies in a "
		    "level to get an Ability (up to one per level).";
		story += "\n\n3) Kill all ";
		story += std::to_string(NUM_LEVELS_TO_WIN);
		story += " to complete your task!\n\n(Hold [h] in game to display help/controls)";

		displayText.setText(story);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 11;
		std::string exitGame = "[Space] Enter the first portal...";
		displayText.setText(exitGame);
		displayText.setColor(LOG_COLOR_IMPORTANT);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);
		/*
		displayText.setColor(LOG_COLOR_NORMAL);
		centerTextY += TileTextHeight * 2;
		displayText.setPosition(centerTextX, centerTextY);
		displayText.setText("Press space to restart.");
		win.draw(&displayText);

		if (GameLog.size())
		{
		    centerTextY += TileTextHeight * 4;
		    displayText.setText("Log (last ten entries):");
		    displayText.setColor(LOG_COLOR_NORMAL);
		    displayText.setPosition(centerTextX, centerTextY);
		    win.draw(&displayText);
		    centerTextY += TileTextHeight;
		    centerTextX += 48;
		    int numLogEntriesDisplayed = 0;
		    for (std::vector<std::string>::reverse_iterator i = GameLog.rbegin();
		         i != GameLog.rend(); ++i)
		    {
		        displayText.setText((*i));

		        // The last entry was the thing which killed us
		        if (!numLogEntriesDisplayed)
		            displayText.setColor(LOG_COLOR_DEAD);
		        else
		            displayText.setColor(LOG_COLOR_NORMAL);
		        displayText.setPosition(centerTextX, centerTextY);
		        centerTextY += TileTextHeight;
		        win.draw(&displayText);

		        if (++numLogEntriesDisplayed > 10)
		            break;
		    }
		}*/
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
		int centerOffsetPixels = 100;
		int centerTextX = centerOffsetPixels;
		int centerTextY = centerOffsetPixels;

		// displayText.setText("You have died!");
		displayText.setText(TITLE);
		displayText.setColor(LOG_COLOR_IMPORTANT);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 13;
		std::string startGame = "[Space] Start Game";
		displayText.setText(startGame);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 3;
		std::string exitGame = "[Escape] Close Game";
		displayText.setText(exitGame);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);
		/*
		displayText.setColor(LOG_COLOR_NORMAL);
		centerTextY += TileTextHeight * 2;
		displayText.setPosition(centerTextX, centerTextY);
		displayText.setText("Press space to restart.");
		win.draw(&displayText);

		if (GameLog.size())
		{
		    centerTextY += TileTextHeight * 4;
		    displayText.setText("Log (last ten entries):");
		    displayText.setColor(LOG_COLOR_NORMAL);
		    displayText.setPosition(centerTextX, centerTextY);
		    win.draw(&displayText);
		    centerTextY += TileTextHeight;
		    centerTextX += 48;
		    int numLogEntriesDisplayed = 0;
		    for (std::vector<std::string>::reverse_iterator i = GameLog.rbegin();
		         i != GameLog.rend(); ++i)
		    {
		        displayText.setText((*i));

		        // The last entry was the thing which killed us
		        if (!numLogEntriesDisplayed)
		            displayText.setColor(LOG_COLOR_DEAD);
		        else
		            displayText.setColor(LOG_COLOR_NORMAL);
		        displayText.setPosition(centerTextX, centerTextY);
		        centerTextY += TileTextHeight;
		        win.draw(&displayText);

		        if (++numLogEntriesDisplayed > 10)
		            break;
		    }
		}*/
		win.update();
	}

	return false;
}
