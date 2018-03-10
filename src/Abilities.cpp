#include "Abilities.hpp"

#include "math/math.hpp"

#include "Globals.hpp"

#include "Entity.hpp"
#include "Game.hpp"
#include "Levels.hpp"
#include "Logging.hpp"

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
	if (ActivatedOnTurn == -1)
		return false;

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
	// Don't miss the best power
	if (gameState.currentLevel == 9)
		return new FireBomb();
	// They need lightning for stage 4+
	else if (gameState.currentLevel == LEVEL_NUM_FOREST)
		return new LightningAbility();
	else if (gameState.currentLevel <= LEVEL_NUM_FOREST)
		return new PhaseDoor();
	else if (gameState.currentLevel <= LEVEL_NUM_BARREN)
	{
		int numOptions = 3;
		switch (rand() % numOptions)
		{
			case 0:
				return new LightningAbility();
			case 1:
				return new PhaseTarget();
			case 2:
				return new Restoration();
			default:
				return new PhaseDoor();
		}
	}
	else if (gameState.currentLevel <= LEVEL_NUM_HELLSCAPE)
	{
		int numOptions = 4;
		switch (rand() % numOptions)
		{
			case 0:
				return new LightningAbility();
			case 1:
				return new FireBomb();
			case 2:
				return new PhaseTarget();
			case 3:
				return new Restoration();
			default:
				return new PhaseDoor();
		}
	}

	return new PhaseDoor();
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
	// Enemies have higher cooldown, lower damange
	CooldownTime = 20;
	Damage = 15;
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

	sfxLightning.play();
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

	sfxLightning.play();
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

PhaseDoor::PhaseDoor()
{
	RequiresTarget = false;
	CooldownTime = 10;
	Damage = 0;

	if (gameState.currentLevel < 5)
	{
		Name = "Phase Door";
		Description = "Phase to a random location";
		Radius = PHASE_DOOR_SQUARE_RADIUS;
	}
	else
	{
		Name = "Teleport";
		Description = "Teleport to a random location";
		Radius = 0;
	}
}

bool PhaseDoor::CanActivateOnPlayer(Enemy* enemy)
{
	return false;
}

void PhaseDoor::EnemyActivate(Enemy* enemyActivator)
{
	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(enemyActivator->X, enemyActivator->Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_PHASE_DOOR};

		// gameState.abilitiesUpdatingFx.push_back(this);
	}

	if (Radius)
		placeEntityWithinSquareRandomSensibly(enemyActivator, enemyActivator->X, enemyActivator->Y,
		                                      Radius);
	else
		placeEntityRandomSensibly(enemyActivator);
}

void PhaseDoor::PlayerActivateWithTarget(int targetX, int targetY)
{
	LOGE << "Player somehow activated PhaseDoor despite !RequiresTarget";

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
		tileAt->Color = {FX_PHASE_DOOR};

		// gameState.abilitiesUpdatingFx.push_back(this);
	}

	if (Radius)
		placeEntityWithinSquareRandomSensibly(&gameState.player, gameState.player.X,
		                                      gameState.player.Y, Radius);
	else
		placeEntityRandomSensibly(&gameState.player);

	LOGI << "You teleport to another place";
}

void PhaseDoor::PlayerActivateNoTarget()
{
	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_PHASE_DOOR};

		// gameState.abilitiesUpdatingFx.push_back(this);
	}

	LOGD << "Phase door player " << gameState.player.X << ", " << gameState.player.Y << " radius "
	     << Radius;
	if (Radius)
		placeEntityWithinSquareRandomSensibly(&gameState.player, gameState.player.X,
		                                      gameState.player.Y, Radius);
	else
		placeEntityRandomSensibly(&gameState.player);

	LOGD << "\tresult Phase door player " << gameState.player.X << ", " << gameState.player.Y;

	LOGI << "You teleport to another place";

	sfxPhaseDoor.play();
}

// Called every frame if Active
void PhaseDoor::FxUpdate(float frameTime)
{
	// if (TotalFrameTimeAlive > 1.f)
	//	return;
}

PhaseTarget::PhaseTarget()
{
	RequiresTarget = true;
	CooldownTime = 14;

	Name = "Phase Target";
	Description = "Phase an enemy to a random location";
	Damage = 10;
}

bool PhaseTarget::CanActivateOnPlayer(Enemy* enemy)
{
	// Enemies have higher cooldown
	CooldownTime = 30;
	return IsCooldownDone() &&
	       manhattanTo(enemy->X, enemy->Y, gameState.player.X, gameState.player.Y) <
	           RANGED_ENEMY_MAX_DIST_MANHATTAN;
}

void PhaseTarget::EnemyActivate(Enemy* enemyActivator)
{
	AbilityActivate();

	// Don't damage player with phase target (it's annoying enough)
	// enemyAbilityDamagePlayer(enemyActivator, this);

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_PHASE_DOOR};

		gameState.abilitiesUpdatingFx.push_back(this);
	}

	placeEntityWithinSquareRandomSensibly(&gameState.player, enemyActivator->X, enemyActivator->Y,
	                                      PHASE_TARGET_ON_PLAYER_RADIUS);

	LOGI << "A " << enemyActivator->Description.c_str() << " force teleported you!";
}

void PhaseTarget::PlayerActivateWithTarget(int targetX, int targetY)
{
	AbilityActivate();

	std::vector<RLEntity*> damageEntities = getEntitiesAtPosition(targetX, targetY);
	for (RLEntity* entity : damageEntities)
	{
		if (!entity->IsTraversable)
		{
			playerAbilityDamageEntity(this, entity);
			placeEntityWithinSquareRandomSensibly(entity, entity->X, entity->Y,
			                                      PHASE_TARGET_ON_ENEMY_RADIUS);
		}
	}

	RLTile* tileAt = gameState.vfx.At(targetX, targetY);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_PHASE_DOOR};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

void PhaseTarget::PlayerActivateNoTarget()
{
	LOGE << "Player somehow activated PhaseTarget despite RequiresTarget";

	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_PHASE_DOOR};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

// Called every frame if Active
void PhaseTarget::FxUpdate(float frameTime)
{
	// if (TotalFrameTimeAlive > 1.f)
	//	return;
}

FireBomb::FireBomb()
{
	RequiresTarget = true;
	CooldownTime = 20;

	Name = "Fire Bomb";
	Description = "Engulf a target in a ball of fire";
	Damage = 50;
}

bool FireBomb::CanActivateOnPlayer(Enemy* enemy)
{
	return IsCooldownDone() &&
	       manhattanTo(enemy->X, enemy->Y, gameState.player.X, gameState.player.Y) <
	           RANGED_ENEMY_MAX_DIST_MANHATTAN;
}

void fireBombActivate(int X, int Y, FireBomb* bomb, bool isActivatorPlayer)
{
	for (int fireY = Y - FIREBOMB_RADIUS; fireY <= Y + FIREBOMB_RADIUS; ++fireY)
	{
		for (int fireX = X - FIREBOMB_RADIUS; fireX <= X + FIREBOMB_RADIUS; ++fireX)
		{
			std::vector<RLEntity*> damageEntities = getEntitiesAtPosition(fireX, fireY);
			for (RLEntity* entity : damageEntities)
			{
				if (!entity->IsTraversable)
				{
					if (!isActivatorPlayer)
						enemyAbilityDamageEntity(bomb, entity);
					else
						playerAbilityDamageEntity(bomb, entity);
				}
			}

			RLTile* tileAt = gameState.vfx.At(fireX, fireY);
			if (tileAt)
			{
				// tileAt->Type = '!';
				tileAt->Color = {FX_FIREBOMB};
			}
		}
	}
}

void FireBomb::EnemyActivate(Enemy* enemyActivator)
{
	AbilityActivate();

	enemyAbilityDamagePlayer(enemyActivator, this);

	// todo: should target player, not self
	// if (rand() % DRAGON_
	fireBombActivate(gameState.player.X, gameState.player.Y, this, false);

	gameState.abilitiesUpdatingFx.push_back(this);
}

void FireBomb::PlayerActivateWithTarget(int targetX, int targetY)
{
	AbilityActivate();

	std::vector<RLEntity*> damageEntities = getEntitiesAtPosition(targetX, targetY);
	for (RLEntity* entity : damageEntities)
	{
		if (!entity->IsTraversable)
			playerAbilityDamageEntity(this, entity);
	}

	fireBombActivate(targetX, targetY, this, true);

	gameState.abilitiesUpdatingFx.push_back(this);
}

void FireBomb::PlayerActivateNoTarget()
{
	LOGE << "Player somehow activated FireBomb despite RequiresTarget";

	AbilityActivate();

	RLTile* tileAt = gameState.vfx.At(gameState.player.X, gameState.player.Y);
	if (tileAt)
	{
		// tileAt->Type = '!';
		tileAt->Color = {FX_FIREBOMB};

		gameState.abilitiesUpdatingFx.push_back(this);
	}
}

// Called every frame if Active
void FireBomb::FxUpdate(float frameTime)
{
	// if (TotalFrameTimeAlive > 1.f)
	//	return;
}

Restoration::Restoration()
{
	RequiresTarget = false;
	CooldownTime = 20;
	Damage = 0;

	if (gameState.currentLevel < LEVEL_NUM_BARREN)
	{
		Name = "Restoration";
		Description = "Restore percentage of stats";
	}
	else
	{
		Name = "Full Restore";
		Description = "Restore stats";
	}
}

bool Restoration::CanActivateOnPlayer(Enemy* enemy)
{
	return IsCooldownDone() && false;
}

void Restoration::EnemyActivate(Enemy* enemyActivator)
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

void Restoration::PlayerActivateWithTarget(int targetX, int targetY)
{
	LOGE << "Player somehow activated Restoration despite RequiresTarget";
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

void Restoration::PlayerActivateNoTarget()
{
	AbilityActivate();

	for (std::pair<const std::string, RLCombatStat>& stat : gameState.player.Stats)
	{
		int restoreToValue =
		    stat.second.Max * (gameState.currentLevel < LEVEL_NUM_BARREN ? 0.75f : 1.f);
		stat.second.Value = MAX(stat.second.Value, restoreToValue);
	}
	LOGI << "You feel restored";
}

// Called every frame if Active
void Restoration::FxUpdate(float frameTime)
{
	// if (TotalFrameTimeAlive > 1.f)
	//	return;
}