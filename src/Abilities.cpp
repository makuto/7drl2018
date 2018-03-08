#include "Abilities.hpp"

#include "math/math.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "Globals.hpp"

Ability::Ability()
{
	RequiresTarget = false;
	ActivatedOnTurn = -1;
	Active = false;
	Damage = 0;
	TotalFrameTimeAlive = 0.f;
}

Ability::~Ability()
{
	for (std::vector<Ability*>::iterator it = gameState.abilitiesUpdatingFx.begin();
	     it != gameState.abilitiesUpdatingFx.end(); ++it)
	{
		if (*it == this)
		{
			gameState.abilitiesUpdatingFx.erase(it);
			break;
		}
	}
}

void Ability::AbilityActivate()
{
	Active = true;
	ActivatedOnTurn = TurnCounter;
	TotalFrameTimeAlive = 0.f;

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

Ability* getNewRandomAbility()
{
	return new LightningAbility();
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
	Damage = 30;
}

bool LightningAbility::CanActivateOnPlayer(Enemy* enemy)
{
	return IsCooldownDone() &&
	       manhattanTo(enemy->X, enemy->Y, gameState.player.X, gameState.player.Y) <
	           RANGED_ENEMY_MAX_DIST_MANHATTAN;
}

void LightningAbility::EnemyActivate(Enemy* enemyActivator)
{
	AbilityActivate();

	enemyAbilityDamagePlayer(enemyActivator, this);

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

void LightningAbility::PlayerActivateWithTarget(int targetX, int targetY)
{
	AbilityActivate();

	std::vector<RLEntity*> damageEntities = getEntitiesAtPosition(targetX, targetY);
	for (RLEntity* entity : damageEntities)
	{
		if (!entity->IsTraversable)
			playerAbilityDamageEntity(this, entity);
	}

	RLTile* tileAt = gameState.vfx.At(targetX, targetY);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

void LightningAbility::PlayerActivateNoTarget()
{
	LOGE << "Player somehow activated Lightning despite RequiresTarget";

	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_LIGHTNING};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

// Called every frame if Active
void LightningAbility::FxUpdate(float frameTime)
{
	if (TotalFrameTimeAlive > 1.f)
		return;
}