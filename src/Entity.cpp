#include "Entity.hpp"

bool RLEntity::SamePos(RLEntity& otherEnt)
{
	return otherEnt.X == X && otherEnt.Y == Y;
}

Player::Player()
{
	Type = PLAYER_TYPE;
	Color = {PLAYER_COLOR_NORMAL};
	Speed = 1;
	Description = "yourself";

	Stats["HP"] = {"Health", PLAYER_STARTING_MAX_HEALTH, PLAYER_STARTING_MAX_HEALTH};
}

bool canMoveTo(RLEntity& entity, int deltaX, int deltaY, RLMap& map)
{
	return map.At(entity.X + deltaX, entity.Y + deltaY).Type == FLOOR_TYPE;
}