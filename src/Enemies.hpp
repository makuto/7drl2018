#pragma once

#include "Entity.hpp"

class Skeleton : public Enemy
{
public:
	Skeleton();
	virtual ~Skeleton() = default;

	virtual void DoTurn() override;
};

class Summoner : public Enemy
{
private:
	int NumSpawns;
public:
	Summoner();
	virtual ~Summoner() = default;

	virtual void DoTurn() override;
};
