#include "Utilities.hpp"

#include <iostream>

#include "Game.hpp"

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
		if (lastTurnLog == TurnCounter && !GameLog.empty() && gameLogAction != GameLog.back() &&
		    GameLog.back().size() < MAX_SINGLE_LOG_SIZE)
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
