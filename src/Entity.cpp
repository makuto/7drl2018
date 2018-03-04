#include "Entity.hpp"

#include "math/math.hpp"

#include "Game.hpp"

int RLCombatStat::Add(int delta)
{
	Value += delta;
	CLAMP(Value, 0, Max);
	return Value;
}

RLEntity::RLEntity()
{
	static int nextGuid = 1;
	Guid = nextGuid++;
}

bool RLEntity::SamePos(RLEntity& otherEnt)
{
	return otherEnt.X == X && otherEnt.Y == Y;
}

void Player::Initialize()
{
	Type = PLAYER_TYPE;
	Color = {PLAYER_COLOR_NORMAL};
	Speed = 1;
	Description = "yourself";

	Stats["HP"] = {"Health",
	               PLAYER_STARTING_MAX_HEALTH,
	               PLAYER_STARTING_MAX_HEALTH,
	               PLAYER_STARTING_RESTORE_HEALTH,
	               PLAYER_DEFAULT_RESTORE_RATE_HEALTH,
	               -1};
	Stats["SP"] = {"Stamina",
	               PLAYER_STARTING_MAX_STAMINA,
	               PLAYER_STARTING_MAX_STAMINA,
	               PLAYER_STARTING_RESTORE_STAMINA,
	               PLAYER_DEFAULT_RESTORE_RATE_STAMINA,
	               -1};

	ThisTurnAction = PlayerAction::None;
}

Player::Player()
{
	Initialize();
}

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map)
{
	RLTile* tile = map.At(entity.X + deltaX, entity.Y + deltaY);
	return tile && tile->Type == FLOOR_TYPE;
}

bool canMeleeAttack(RLEntity& entity, int deltaX, int deltaY, std::vector<RLEntity*>& npcs,
                    RLEntity** npcOut)
{
	for (RLEntity* npc : npcs)
	{
		if (entity.X + deltaX == npc->X && entity.Y + deltaY == npc->Y)
		{
			*npcOut = npc;

			// Corpse (icky)
			if (npc->Type == CORPSE_TYPE || !npc->Stats["HP"].Value)
				return false;
			
			return true;
		}
	}
	*npcOut = nullptr;
	return false;
}

RLEntity* findEntityById(std::vector<RLEntity*>& npcs, int id)
{
	for (RLEntity* npc : npcs)
	{
		if (npc->Guid == id)
			return npc;
	}

	return nullptr;
}

RLEntity* findEntityByPosition(std::vector<RLEntity*>& npcs, int x, int y)
{
	for (RLEntity* npc : npcs)
	{
		if (npc->X == x && npc->Y == y)
			return npc;
	}

	return nullptr;
}

void RLEntity::DoTurn()
{
}

void Player::DoTurn()
{
	//
	// Update stats
	//
	// Restore faster when resting
	int restoreTurnBonus = 0;
	switch (ThisTurnAction)
	{
		case PlayerAction::None:
			restoreTurnBonus = 0;
			break;
		case PlayerAction::Rested:
			restoreTurnBonus = PLAYER_RESTING_BONUS;
			break;
		case PlayerAction::MeleeAttacked:
			restoreTurnBonus = PLAYER_MELEE_ATTACKING_BONUS;
			break;
		default:
			break;
	}

	ForStatName(stat, statName, this)
	{
		// Special case: HP doesn't restore if stamina isn't full
		if (statName == "HP" && Stats["SP"].Value != Stats["SP"].Max)
			stat.RestoreOnTurn++;
		else
			stat.RestoreOnTurn -= restoreTurnBonus;

		// LOGD << "Stat " << statName.c_str() << " Value " << stat.Value << " Restore on turn "
		//     << stat.RestoreOnTurn;
		if (TurnCounter >= stat.RestoreOnTurn)
		{
			stat.Add(stat.RestoreAmount);
			stat.RestoreOnTurn = TurnCounter + stat.RestoreRateTurns;
		}
	}
	ForStatEnd();

	// Reset turn states
	ThisTurnAction = PlayerAction::None;
}

void Enemy::DoTurn()
{
	CheckDoDeath();
	if (!Stats["HP"].Value)
		return;

	// Shamble towards the player
	int deltaX = -(X - gameState.player.X);
	int deltaY = -(Y - gameState.player.Y);

	CLAMP(deltaX, -Speed, Speed);
	CLAMP(deltaY, -Speed, Speed);
	VelocityX = deltaX;
	VelocityY = deltaY;
}

void Enemy::CheckDoDeath()
{
	// If we just died, turn into a corpse
	if (!Stats["HP"].Value && ((SpawnStairsDown && Type != STAIRS_DOWN_TYPE) ||
	                           (!SpawnStairsDown && Type != CORPSE_TYPE)))
	{
		LOGI << "A " << Description.c_str() << " died!";
		Type = CORPSE_TYPE;
		Description += " corpse";

		if (SpawnStairsDown)
		{
			Description = "portal to the next level";
			Type = STAIRS_DOWN_TYPE;
			Color = {STAIRS_COLOR_NORMAL};
		}
	}
}

bool sortEntitiesByAscendingDistFromPlayer(RLEntity* a, RLEntity* b)
{
	float aDist = distanceTo(a->X, a->Y, gameState.player.X, gameState.player.Y);
	float bDist = distanceTo(b->X, b->Y, gameState.player.X, gameState.player.Y);
	return aDist < bDist;
}