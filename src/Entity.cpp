#include "Entity.hpp"

#include "Globals.hpp"
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
	IsTraversable = false;
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
	Stats["STR"] = {"Strength", PLAYER_STARTING_MAX_STRENGTH, PLAYER_STARTING_MAX_STRENGTH, 0, -1,
	                -1};

	ThisTurnAction = PlayerAction::None;

	for (Ability* ability : Abilities)
		delete ability;
	Abilities.clear();
	Abilities.push_back(new LightningAbility());
}

Player::Player()
{
	Initialize();
}

Enemy::Enemy()
{
	Stats["HP"] = {"", 100, 100, 0, 0, -1};
	Stats["SP"] = {"", 100, 100, 0, 0, -1};
	Stats["STR"] = {"", 10, 10, 0, 0, -1};
}

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map)
{
	RLTile* tile = map.At(entity.X + deltaX, entity.Y + deltaY);
	return tile && tile->Type == FLOOR_TYPE;
}

bool canMeleeAttack(RLEntity& entity, int deltaX, int deltaY, std::vector<RLEntity*>& npcs,
                    RLEntity** npcOut)
{
	*npcOut = nullptr;
	std::vector<RLEntity*> entitiesAtPosition =
	    getEntitiesAtPosition(entity.X + deltaX, entity.Y + deltaY);
	for (RLEntity* npc : entitiesAtPosition)
	{
		*npcOut = npc;
		if (!npc->IsTraversable && npc->Stats["HP"].Value)
			return true;
	}
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

std::vector<RLEntity*> getEntitiesAtPosition(int x, int y)
{
	std::vector<RLEntity*> entitiesAtPosition;
	for (RLEntity* npc : gameState.npcs)
	{
		if (npc->X == x && npc->Y == y)
			entitiesAtPosition.push_back(npc);
	}

	return entitiesAtPosition;
}

bool isNonTraversableEntityAtPosition(int x, int y)
{
	std::vector<RLEntity*> entitiesAtPosition = getEntitiesAtPosition(x, y);
	for (RLEntity* npc : entitiesAtPosition)
	{
		if (!npc->IsTraversable)
			return true;
	}
	return false;
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

	MoveTowardsPlayer();
}

void Enemy::CheckDoDeath()
{
	// If we just died, turn into a corpse
	if (!Stats["HP"].Value && ((SpawnStairsDown && Type != STAIRS_DOWN_TYPE) ||
	                           (!SpawnStairsDown && Type != CORPSE_TYPE)))
	{
		LOGI << "A " << Description.c_str() << " died!";
		IsTraversable = true;
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

void Enemy::MoveTowardsPlayer()
{
	// Shamble towards the player
	int deltaX = -(X - gameState.player.X);
	int deltaY = -(Y - gameState.player.Y);

	CLAMP(deltaX, -Speed, Speed);
	CLAMP(deltaY, -Speed, Speed);
	VelocityX = deltaX;
	VelocityY = deltaY;
}

void Enemy::RandomWalk()
{
	int deltaX = (rand() % 2) - 1;
	int deltaY = (rand() % 2) - 1;

	CLAMP(deltaX, -Speed, Speed);
	CLAMP(deltaY, -Speed, Speed);
	VelocityX = deltaX;
	VelocityY = deltaY;
}

bool sortEntitiesByAscendingDistFromPlayer(RLEntity* a, RLEntity* b)
{
	float aDist = distanceTo(a->X, a->Y, gameState.player.X, gameState.player.Y);
	float bDist = distanceTo(b->X, b->Y, gameState.player.X, gameState.player.Y);
	return aDist < bDist;
}

std::string describePosition(int x, int y)
{
	std::string description;

	RLTile* lookTile = gameState.currentMap.At(x, y);
	std::string tileDescription = lookTile ? GetTileDescription(*lookTile) : "";

	// Describe entities
	std::string npcDescription;
	std::string traversablesDescription;
	std::vector<RLEntity*> entitiesAtPosition = getEntitiesAtPosition(x, y);
	for (std::vector<RLEntity*>::iterator it = entitiesAtPosition.begin();
	     it != entitiesAtPosition.end(); ++it)
	{
		RLEntity* ent = (*it);

		if (!ent->IsTraversable)
		{
			if (ent->Stats["HP"].Value < ent->Stats["HP"].Max * .2f)
				npcDescription += "wounded ";

			npcDescription += "a ";
			npcDescription += ent->Description;
		}
		else
		{
			if (!traversablesDescription.empty())
			{
				if (ent == entitiesAtPosition.back())
					traversablesDescription += ", and a ";
				else
					traversablesDescription += ", a ";
			}
			else
				traversablesDescription += "a ";
			traversablesDescription += ent->Description;
		}
	}

	// Combine descriptions
	if (!traversablesDescription.empty() || !tileDescription.empty() || !npcDescription.empty())
	{
		if (!npcDescription.empty())
		{
			description += npcDescription;
		}

		if (!traversablesDescription.empty())
		{
			if (!npcDescription.empty())
				description += " on ";
			description += traversablesDescription;
		}

		// Only describe the ground if there's nothing on it
		if (!tileDescription.empty())
		{
			if (!npcDescription.empty() || !traversablesDescription.empty())
				description += " on ";
			description += tileDescription;
		}
	}

	return description;
}

bool playerCanUseStairsNow(std::string* stairsDescriptionOut)
{
	std::vector<RLEntity*> possibleStairs =
	    getEntitiesAtPosition(gameState.player.X, gameState.player.Y);
	for (RLEntity* ent : possibleStairs)
	{
		if (ent->Type == STAIRS_DOWN_TYPE)
		{
			if (stairsDescriptionOut)
				*stairsDescriptionOut = ent->Description;

			return true;
		}
	}

	return false;
}

void enemyMeleeAttackPlayer(RLEntity* entity)
{
	int damage = -entity->Stats["STR"].Value;

	if (damage)
	{
		gameState.player.Stats["HP"].Add(damage);
		if (damage < 0)
			LOGI << "A " << entity->Description.c_str() << " hit you for " << abs(damage)
			     << " damage!";
		if (damage > 0)
			LOGI << "A " << entity->Description.c_str() << " healed you for " << abs(damage)
			     << " health!";
	}
}

void enemyAbilityDamagePlayer(RLEntity* entity, Ability* ability)
{
	int damage = -ability->Damage;

	if (damage)
	{
		gameState.player.Stats["HP"].Add(damage);
		if (damage < 0)
			LOGI << "A " << entity->Description.c_str() << " hit you with " << ability->Name.c_str()
			     << " for " << abs(damage) << " damage!";
		if (damage > 0)
			LOGI << "A " << entity->Description.c_str() << " healed you with "
			     << ability->Name.c_str() << " for " << abs(damage) << " health!";
	}
}

void playerAbilityDamageEntity(Ability* ability, RLEntity* entity)
{
	int damage = -ability->Damage;

	if (damage)
	{
		entity->Stats["HP"].Add(damage);
		LOGI << "You hit the " << entity->Description.c_str() << " with " << ability->Name.c_str()
		     << " for " << abs(damage) << " damage";
	}
}

void playerMeleeAttackEnemy(RLEntity* entity)
{
	int damage = -gameState.player.Stats["STR"].Value;

	// Pay for it
	int staminaCost = 10;
	// If out of stamina, take it out of health
	int healthCost = gameState.player.Stats["SP"].Value < staminaCost ?
	                     gameState.player.Stats["SP"].Value - staminaCost :
	                     0;
	gameState.player.Stats["SP"].Add(-staminaCost);
	gameState.player.Stats["HP"].Add(healthCost);

	// Damage the enemy
	if (damage)
	{
		if (damage < 0)
		{
			entity->Stats["HP"].Add(damage);

			LOGI << "You hit the " << entity->Description.c_str() << " for " << abs(damage)
			     << " damage";
		}
	}

	if (healthCost)
		LOGI << "You feel weaker from overexertion";
}

RLEntity* getClosestNonTraversableEntity(int x, int y)
{
	RLEntity* closestEntity;
	float closestDist = 10000000.f;
	for (RLEntity* npc : gameState.npcs)
	{
		if (npc->IsTraversable)
			continue;

		float distTo = distanceTo(x, y, npc->X, npc->Y);
		if (distTo < closestDist)
		{
			closestDist = distTo;
			closestEntity = npc;
		}
	}

	return closestEntity;
}