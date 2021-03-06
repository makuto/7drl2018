#pragma once

#include <string>

class Enemy;

struct Ability
{
	bool Active;

	// -1 = not activated
	int ActivatedOnTurn;

	// Once TurnCounter > ActivatedOnTurn + CooldownTime, it's ready to use
	int CooldownTime;

	std::string Name;
	std::string Description;

	int Damage;

	// Incremented after FxUpdate. Use to e.g. time out your Fx Update
	float TotalFrameTimeAlive;

	Ability();
	virtual ~Ability();

	// Helper functions for managing cooldown mostly
	bool IsCooldownDone();
	int CooldownRemaining();
	void AbilityActivate();

	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	bool RequiresTarget;
	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};

Ability* getNewRandomAbility();

//
// Children
// 

struct LightningAbility : public Ability
{
	LightningAbility();
	virtual ~LightningAbility() = default;
	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};

struct PhaseDoor : public Ability
{
	int Radius;
	
	PhaseDoor();
	virtual ~PhaseDoor() = default;
	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};

struct PhaseTarget : public Ability
{
	PhaseTarget();
	virtual ~PhaseTarget() = default;
	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};

struct FireBomb : public Ability
{
	FireBomb();
	virtual ~FireBomb() = default;
	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};

struct Restoration : public Ability
{
	Restoration();
	virtual ~Restoration() = default;
	virtual bool CanActivateOnPlayer(Enemy* enemy);

	virtual void EnemyActivate(Enemy* enemyActivator);

	virtual void PlayerActivateWithTarget(int targetX, int targetY);
	virtual void PlayerActivateNoTarget();

	// Called every frame if Active
	virtual void FxUpdate(float frameTime);
};