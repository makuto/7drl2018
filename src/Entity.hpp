#pragma once

#include <map>
#include <vector>

#include "Abilities.hpp"
#include "RLColor.hpp"
#include "RLMap.hpp"

typedef char RLEntityType;

struct RLCombatStat
{
	std::string Name;

	int Max;
	int Value;

	int RestoreAmount;
	// 0 = never
	int RestoreRateTurns;
	// -1 = never
	int RestoreOnTurn;

	// Safely add to stat (make sure it stays within range). Returns new value. Accepts negative
	int Add(int delta);
};
typedef std::map<std::string, RLCombatStat> RLCombatStatistics;

#define ForStat(varName, entityPtr)                                                \
	for (std::pair<const std::string, RLCombatStat> & statPair : entityPtr->Stats) \
	{                                                                              \
		RLCombatStat& varName = statPair.second;
#define ForStatName(varName, statVarName, entityPtr)                               \
	for (std::pair<const std::string, RLCombatStat> & statPair : entityPtr->Stats) \
	{                                                                              \
		RLCombatStat& varName = statPair.second;                                   \
		const std::string& statVarName = statPair.first;
#define ForStatEnd() }

class RLEntity
{
public:
	int Guid;

	int X;
	int Y;

	int VelocityX;
	int VelocityY;

	int Speed;

	RLEntityType Type;
	RLColor Color;

	std::string Description;

	RLCombatStatistics Stats;

	bool IsTraversable;

	RLEntity();
	virtual ~RLEntity() = default;

	bool SamePos(RLEntity& otherEnt);

	virtual void DoTurn();
};

enum class PlayerAction : int
{
	None = 0,

	Rested,
	MeleeAttacked
};

class Player : public RLEntity
{
public:
	std::vector<Ability*> Abilities;
	PlayerAction ThisTurnAction;

	void Initialize();
	Player();
	virtual ~Player() = default;

	virtual void DoTurn();
};

class Enemy : public RLEntity
{
protected:
	void CheckDoDeath();

	// Movement types
	void MoveTowardsPlayer();
	void RandomWalk();

public:
	bool SpawnStairsDown;
	Enemy();
	virtual ~Enemy() = default;

	virtual void DoTurn() override;
};

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map);

bool canMeleeAttack(RLEntity& entity, int deltaX, int deltaY, std::vector<RLEntity*>& npcs,
                    RLEntity** npcOut);

RLEntity* findEntityById(std::vector<RLEntity*>& npcs, int id);

std::vector<RLEntity*> getEntitiesAtPosition(int x, int y);
bool isNonTraversableEntityAtPosition(int x, int y);

bool sortEntitiesByAscendingDistFromPlayer(RLEntity* a, RLEntity* b);

std::string describePosition(int x, int y);

bool playerCanUseStairsNow(std::string* stairsDescriptionOut);

void enemyAbilityDamagePlayer(RLEntity* entity, Ability* ability);
void enemyMeleeAttackPlayer(RLEntity* entity);
void playerAbilityDamageEntity(Ability* ability, RLEntity* entity);
void playerMeleeAttackEnemy(RLEntity* entity);

RLEntity* getClosestNonTraversableEntity(int x, int y);