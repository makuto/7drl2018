#include "Abilities.hpp"

#include "Game.hpp"
#include "Globals.hpp"

Ability::Ability()
{
	RequiresTarget = false;
	ActivatedOnTurn = -1;
	Active = false;
}

void Ability::AbilityActivate()
{
	Active = true;
	ActivatedOnTurn = TurnCounter;

	LOGD << "Activate ability " << Name.c_str() << " turn " << TurnCounter;
}

bool Ability::IsCooldownDone()
{
	return ActivatedOnTurn == -1 || TurnCounter > ActivatedOnTurn + CooldownTime;
}

int Ability::CooldownRemaining()
{
	return -(TurnCounter - (ActivatedOnTurn + CooldownTime)) + 1;
}

bool Ability::CanActivateOnPlayer(Enemy* enemy)
{
	return IsCooldownDone() && false;
}

void Ability::EnemyActivate(Enemy* enemyActivator)
{
}

void Ability::PlayerActivateWithTarget(int targetX, int targetY)
{
	AbilityActivate();
}

void Ability::PlayerActivateNoTarget()
{
	AbilityActivate();
}

// Called every frame if Active
void Ability::FxUpdate(float frameTime)
{
}

//
// Children
//

LightningAbility::LightningAbility()
{
	RequiresTarget = true;
	CooldownTime = 10;

	Name = "Lightning";
	Description = "Fire a bolt of lightning through foes";
}

bool LightningAbility::CanActivateOnPlayer(Enemy* enemy)
{
	return IsCooldownDone();
}

void LightningAbility::EnemyActivate(Enemy* enemyActivator)
{
	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(enemyActivator->X, enemyActivator->Y);
	if (tileAt)
	{
		tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};
	}
}

void LightningAbility::PlayerActivateWithTarget(int targetX, int targetY)
{
	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(targetX, targetY);
	if (tileAt)
	{
		tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};
	}
}

void LightningAbility::PlayerActivateNoTarget()
{
	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};
	}
}

// Called every frame if Active
void LightningAbility::FxUpdate(float frameTime)
{
}