#pragma once

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

	Ability();
	virtual ~Ability() = default;

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