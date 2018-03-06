#include "PlayerDeadScreen.hpp"

#include "Globals.hpp"

#include "Art.hpp"
#include "Game.hpp"

// returns whether or not to restart the game
bool PlayerDeadScreen()
{
	while (!win.shouldClose() && !inp.isPressed(inputCode::Return) &&
	       !inp.isPressed(inputCode::Escape))
	{
		// Quick restart
		if (inp.isPressed(inputCode::Space))
			return true;

		// int centerOffset = 10;
		// int centerTextX = ((ViewTileWidth / 2) - centerOffset) * TileTextWidth;
		// int centerTextY = ((ViewTileHeight / 2) - centerOffset) * TileTextHeight;
		int centerOffsetPixels = 100;
		int centerTextX = centerOffsetPixels;
		int centerTextY = centerOffsetPixels;

		// displayText.setText("You have died!");
		displayText.setText(YOU_DIED);
		displayText.setColor(LOG_COLOR_DEAD);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);

		centerTextY += TileTextHeight * 13;
		std::string progressReport = "You lasted ";
		progressReport += std::to_string(TurnCounter);
		progressReport += " turns and reached level ";
		progressReport += std::to_string(gameState.currentLevel);
		progressReport += " ";
		progressReport += gameState.levelName;
		displayText.setText(progressReport);
		displayText.setColor(LOG_COLOR_NORMAL);
		displayText.setPosition(centerTextX, centerTextY);
		win.draw(&displayText);
		
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
		}
		win.update();
	}

	return false;
}
