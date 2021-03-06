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
	ViewTileHeight = (WindowHeight / TileTextHeight) - ViewTileTopMargin;

	CamSnapLeftBounds = ViewTileWidth * .2f;
	CamSnapTopBounds = ViewTileHeight * .2f;
	CamSnapRightBounds = ViewTileWidth * .8f;
	CamSnapBottomBounds = ViewTileHeight * .8f;

	SidebarStartX = (ViewTileWidth + 1) * TileTextWidth;
	SidebarStartY = 0 * TileTextHeight;

	// LogY = ViewTileHeight * TileTextHeight;
	LogY = 5;

	// Ensure we have a reasonable text size
	int approxNumColumns = ViewTileWidth + SidebarTileWidth + 1;
	if (approxNumColumns < MinNumTextColumns)
	{
		LOGD << "Shrinking text height to " << TileTextHeight - 1;
		TileTextHeight--;
		RecalculateDimensions();
	}
	if (approxNumColumns > MaxNumTextColumns)
	{
		LOGD << "Growing text height to " << TileTextHeight + 1;
		TileTextHeight++;
		RecalculateDimensions();
	}

	displayText.setSize(TileTextHeight);
}

void SetTextColor(text& text, RLColor& color)
{
	text.setColor(color.r, color.g, color.b, color.a);
}

void WrapText(std::string& textToWrap, bool careAboutSidebar)
{
	unsigned long maxNumColumns = careAboutSidebar ? ViewTileWidth : (ViewTileWidth + SidebarTileWidth);
	maxNumColumns -= 10;

	// no need to wrap
	if (textToWrap.size() + 1 < maxNumColumns)
		return; 

	unsigned long lastSpaceIndex = 0;
	unsigned long numCharsInCurrentRow = 0;
	for (unsigned long i = 0; i < textToWrap.size(); ++i)
	{
		numCharsInCurrentRow++;
		if (textToWrap[i] == ' ')
			lastSpaceIndex = i;

		if (numCharsInCurrentRow > maxNumColumns && lastSpaceIndex)
		{
			textToWrap[lastSpaceIndex] = '\n';
			numCharsInCurrentRow = i - lastSpaceIndex;
		}
	}
}