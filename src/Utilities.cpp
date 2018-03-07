#include "Utilities.hpp"

#include <iostream>
// isalnum
#include <cctype>

#include "Game.hpp"
#include "Globals.hpp"

#define MAX_SINGLE_LOG_SIZE 300

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

		// Append logs to the same line if they occur on the same turn
		if (LastTurnLog == TurnCounter && !GameLog.empty() &&
		    GameLog.back().size() < MAX_SINGLE_LOG_SIZE)
		{
			std::string& currentTurnLog = GameLog.back();

			// Yes, this is terrible. Don't display wall bump message more than once
			if (currentTurnLog.find(WALL_BUMP) != std::string::npos && gameLogAction == WALL_BUMP)
				return;
			if (currentTurnLog.find(LOOK_MODE_EXIT) != std::string::npos &&
			    gameLogAction == LOOK_MODE_EXIT)
				return;
			if (currentTurnLog.find(CANCELLED_ABILITY_ACTIVATE) != std::string::npos &&
			    gameLogAction == CANCELLED_ABILITY_ACTIVATE)
				return;
			// What if they change it back on the same turn? Can't suppress that
			/*if (currentTurnLog.find(TRAINING_CHANGED) != std::string::npos &&
			    gameLogAction == TRAINING_CHANGED)
			    return;*/

			if (currentTurnLog.find(ABILITY_ON_COOLDOWN) != std::string::npos &&
			    gameLogAction.find(ABILITY_ON_COOLDOWN) != std::string::npos)
				return;

			// Add divider
			if (std::isalnum(currentTurnLog[currentTurnLog.size() - 1]))
				currentTurnLog += ". ";
			else
				currentTurnLog += " ";

			currentTurnLog += gameLogAction;
		}
		else
		{
			// Don't show mode exit if no status
			if (gameLogAction == LOOK_MODE_EXIT)
				return;

			GameLog.push_back(gameLogAction);
			LastTurnLog = TurnCounter;
		}
	}
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

void SetTextColor(text& text, RLColor& color)
{
	text.setColor(color.r, color.g, color.b, color.a);
}
