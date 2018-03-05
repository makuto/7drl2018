#include "EntityCreators.hpp"

void initSkeleton(Enemy* entity)
{
	entity->SpawnStairsDown = false;
	entity->Speed = 1;
	entity->Type = SKELETON_TYPE;
	entity->Color = {ENEMY_COLOR_NORMAL};
	entity->Description = "skeleton";
	entity->Stats["HP"] = {"Health", 30, 30, 0, 0, -1};
	entity->Stats["STR"] = {"Strength", 10, 10, 0, 0, -1};
}