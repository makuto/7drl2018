#include "Entity.hpp"

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
			// Corpse (icky)
			if (npc->Type == CORPSE_TYPE || !npc->Stats["HP"].Value)
			{
				*npcOut = npc;
				return false;
			}
			*npcOut = npc;
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


		//LOGD << "Stat " << statName.c_str() << " Value " << stat.Value << " Restore on turn "
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
	// If we just died, turn into a corpse
	if (!Stats["HP"].Value && Type != CORPSE_TYPE)
	{
		LOGI << "A " << Description.c_str() << " died!";
		Type = CORPSE_TYPE;
		Description += " corpse";
	}
}