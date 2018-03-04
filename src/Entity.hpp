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

	// Safely add to stat (make sure it stays within range). Returns new value. Accepts negative
	int Add(int delta);
};
typedef std::map<std::string, RLCombatStat> RLCombatStatistics;

class RLEntity
{
public:
	int Guid;

	int X;
	int Y;

	int Speed;

	RLEntityType Type;
	RLColor Color;

	std::string Description;

	RLCombatStatistics Stats;

	RLEntity();
	virtual ~RLEntity() = default;

	bool SamePos(RLEntity& otherEnt);

	virtual void DoTurn();
};

class Player : public RLEntity
{
public:
	void Initialize();
	Player();
	virtual ~Player() = default;

	virtual void DoTurn();
};

class Enemy : public RLEntity
{
public:
	Enemy() = default;
	virtual ~Enemy() = default;

	virtual void DoTurn() override;
};

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map);

bool canMeleeAttack(RLEntity& entity, int deltaX, int deltaY, std::vector<RLEntity*>& npcs,
                    RLEntity** npcOut);

RLEntity* findEntityById(std::vector<RLEntity*>& npcs, int id);

void UpdateCameraOffset(RLEntity* cameraTrackingEntity, int& camXOffset, int& camYOffset);