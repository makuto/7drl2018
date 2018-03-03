#pragma once

#include <map>

#include "RLColor.hpp"
#include "RLMap.hpp"

typedef char RLEntityType;

struct RLCombatStat
{
	std::string Name;

	int Max;
	int Value;
};
typedef std::map<std::string, RLCombatStat> RLCombatStatistics;

struct RLEntity
{
	int X;
	int Y;

	int Speed;

	RLEntityType Type;
	RLColor Color;

	std::string Description;

	RLCombatStatistics Stats;

	bool SamePos(RLEntity& otherEnt);
};

struct Player : RLEntity
{
	Player();
};

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map);