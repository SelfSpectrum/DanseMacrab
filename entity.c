#include "entity.h"

void LoadEnemiesFile(FILE **file, const char *enemySheet) {
	if (*file != NULL) {
		fclose(*file);
		(*file) = NULL;
	}
	if (FileExists(enemySheet)) *file = fopen(enemySheet, "r");
}
void LoadEnemiesOnCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, Combat *combat, int id) {
	if (file == NULL) return;
	char line[512];
	char *token;
	char *saveptr;
	fgets(line, sizeof(line), file);
	while (fgets(line, sizeof(line), file) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		if (atoi(token) == id) {
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[0] = LoadEnemy(enemyData, spriteData, techData, atoi(token));
			if (token[0] != '0') combat->enemy[0]->enemy.position = 0;
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[1] = LoadEnemy(enemyData, spriteData, techData, atoi(token));
			if (token[0] != '0') combat->enemy[1]->enemy.position = 1;
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[2] = LoadEnemy(enemyData, spriteData, techData, atoi(token));
			if (token[0] != '0') combat->enemy[2]->enemy.position = 2;
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[3] = LoadEnemy(enemyData, spriteData, techData, atoi(token));
			if (token[0] != '0') combat->enemy[3]->enemy.position = 3;
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[4] = LoadEnemy(enemyData, spriteData, techData, atoi(token));
			if (token[0] != '0') combat->enemy[4]->enemy.position = 4;
		}
	}
}
void *LoadEnemy(FILE *enemyData, FILE *spriteData, FILE *techData, int id) {
	if (id == 0) return NULL;
	void *enemyData = malloc(sizeof(Entity));
	Enemy *enemy = (Enemy *) enemyData;
	enemy->type = ENTITY_ENEMY;
	enemy->phyBonus = 0;
	enemy->refBonus = 0;
	enemy->lorBonus = 0;
	enemy->chaBonus = 0;
	enemy->techAmount = 0;
	char line[512];
	char *saveptr;
	char *token;
	char *tech;
	int enemyId;
	rewind(enemyData);
	fgets(line, sizeof(line), enemyData);
	while (fgets(line, sizeof(line), enemyData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		enemyId = atoi(token);
		if (enemyId == id) {
			enemy->id = enemyId;
			token = strtok_r(NULL, "	", &saveptr);
			enemy->name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->sprite = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy = (EnemyType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->size = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->maxHealth = DiceMean((DiceType) enemy->enemy.size) * atoi(token) + enemy->enemy.physique * atoi(token);
			enemy->health = enemy->enemy.maxHealth;
			token = strtok_r(NULL, "	", &saveptr);
			enemy->maxStress = atoi(token);
			enemy->stress = enemy->enemy.maxStress;
			token = strtok_r(NULL, "	", &saveptr);
			enemy->weakness[0] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->weakness[1] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->resistances[0] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->resistances[1] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->inmunities[0] = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->inmunities[1] = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->multiattack = atoi(token);
			// Loading techniques
			token = strtok_r(NULL, "	", &saveptr);
			tech = strtok_r(token, ",", &saveptr);
			while (tech != NULL) {
				enemy->tech[enemy->enemy.techAmount] = LoadTech(techData, atoi(tech));
				tech = strtok_r(NULL, ",", &saveptr);
				enemy->techAmount++;
			}
			enemy = NULL;
			return enemyData;
		}
	}
	return NULL;
}
void *LoadPlayer(FILE *characterData, FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, int id, int position) {
	if (id == 0) return NULL;
	void *playerData = malloc(sizeof(Entity));
	Player *player = (Player *) playerData;

	player->position = position;
	player->type = ENTITY_PLAYER;
	player->maxStress = 3;
	player->stress = 0;
	player->demonTally = 0;
	player->accumulatedDamage = 0;
	player->phyBonus = 0;
	player->refBonus = 0;
	player->lorBonus = 0;
	player->chaBonus = 0;
	player->featuresAmount = 0;
	player->techAmount = 0;
	player->equipedTechAmount = 0;
	char line[512];
	char *saveptr;
	char *token;
	char *feature;
	int playerId;
	int i;
	for (i = 0; i < 6; i++) {
		player->physique[i] = 0;
		player->reflex[i] = 0;
		player->lore[i] = 0;
		player->charisma[i] = 0;
	}
	for (i = 0; i < 8; i++) player->enemyBonus[i] = 0;
	rewind(characterData);
	fgets(line, sizeof(line), characterData);
	while (fgets(line, sizeof(line), characterData)) {
		token = strtok_r(line, "	", &saveptr);
		playerId = atoi(token);
		if (playerId == id) {
			player->id = playerId;
			token = strtok_r(NULL, "	", &saveptr);
			player->name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->surname = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->giftCurse = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->nomGuerre = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->sprite = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->physique[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->reflex[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->lore[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->charisma[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(playerData, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(playerData, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(playerData, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->hitDice = (DiceType) atoi(token);
			player->maxHealth = DiceMean(player->hitDice) + player->physique[0];
			player->health = player->maxHealth;
			token = strtok_r(NULL, "	", &saveptr);
			player->weakness = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->resistance = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->inmunity = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->weapon = LoadWeapon(weaponData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->armor = LoadArmor(armorData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->charm = LoadCharm(charmData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			feature = strtok_r(token, ",", &saveptr);
			while (feature != NULL) {
				SetFeature(weaponData, charmData, techData, playerData, (Feature) atoi(feature));
				feature = strtok_r(NULL, ",", &saveptr);
			}
			player = NULL;
			return playerData;
		}
	}
	return player;
}
void DamageEntity(Combat *combat, void *attacker, EntityType side, Technique tech) {
	int position = (side == ENTITY_PLAYER) ? ((Player *) attacker)->position : ((Enemy *) attacker)->position;
	int i;
	int min;
	int max;
	Player *player;
	Enemy *enemy;

	min = (int) Clamp(-2 + position, 0, 4);
	max = (int) Clamp(2 + position, 0, 4);
	for (i = min; i <= max; i++) {
		if (side == ENTITY_ENEMY) {
			combat->enemy[i]->health = (int) Clamp(combat->enemy[i]->enemy.health - i, 0, combat->enemy[i]->enemy.maxHealth);
		}
		else {
			combat->player[i]->player.health = (int) Clamp(combat->player[i]->player.health - i, 0, combat->player[i]->player.maxHealth);
		}
	}
	switch (tech.attr) {
		case DMG_SLASHING:
		case DMG_BLUDGEONING:
		case DMG_PIERCING:
		case DMG_BALLISTIC:
			break;
		case DMG_FIRE:
		case DMG_ICE:
		case DMG_ELECTRIC:
		case DMG_ACID:
			break;
		case DMG_PSYCHIC:
			break;
		case DMG_HEALNATURE:
		case DMG_HEALBLOODY:
		case DMG_HEALPSY:
		case DMG_HEALMEDICINE:
			break;
		case DMG_HEALARMOR:
			break;
		default:
			break;
	}
}
void KillEntity(Combat *combat, void *entity, EntityType type) {
	if (type == ENTITY_ENEMY) {
		Enemy *enemy = (Enemy *) entity;
		enemy = NULL;
	}
	else {
		Player *player = (Player *) entity;
		UnloadEntity(type, combat, player->position);
		player = NULL;
	}
}
void UnloadCombat(Combat *combat) {
	int i;
	for (i = 0; i < 5; i++) {
		UnloadEntity(ENTITY_ENEMY, combat, i);
		printf("INFO: COMBAT: Combat %d, enemy unloaded.\n", i);
		UnloadEntity(ENTITY_PLAYER, combat, i);
		printf("INFO: COMBAT: Combat %d, player unloaded.\n", i);
	}
	printf("INFO: COMBAT: Combat unloaded successfully.\n");
}
void UnloadEntity(EntityType type, Combat *combat, int position) {
	switch (type) {
		case ENTITY_ENEMY:
			if (combat->enemy[position] == NULL) return;
			Enemy *enemy = (Enemy *) combat->enemy[position];
			combat->enemy[position] = NULL;

			if (enemy->sprite != NULL) {
				free(enemy->sprite);
				enemy->sprite = NULL;
			}
			free(enemy);
			return;
		case ENTITY_PLAYER:
			if (combat->player[position] == NULL) return;
			Player *player = (Player *) combat->player[position];
			combat->player[position] = NULL;

			if (player->sprite != NULL) {
				free(player->sprite);
				player->sprite = NULL;
			}
			free(player);
			return;
	}
}
void DrawCombat(Combat *combat, SafeTexture *textures, Color color, bool shader, bool draw) {
	if (draw == false) return;
	int i;
	Enemy *enemy;
	Player *player;
	Sprite *sprite;
	for (i = 0; i < 5; i++) {
		if (combat->enemy[i] != NULL) {
			enemy = (Enemy *) combat->enemy[i];
			sprite = enemy->sprite;
			if (shader != sprite->shader) continue;
			DrawTexturePro(textures[sprite->textureIndex].tex,
				       sprite->origin,
				       sprite->dest,
				       (Vector2) { -48 * enemy->position - 48, -36 },
				       sprite->rotation,
				       color);
			enemy = NULL;
			sprite = NULL;
		}
		if (combat->player[i] != NULL) {
			player = (Player *) combat->player[i];
			sprite = player->sprite;
			if (shader != sprite->shader) continue;
			DrawTexturePro(textures[sprite->textureIndex].tex,
				       sprite->origin,
				       sprite->dest,
				       (Vector2) { -48 * player->position - 48, -83 },
				       sprite->rotation,
				       color);
			player = NULL;
			sprite = NULL;
		}
	}
	DrawRectangle(0, 130, 360, 1, color);
}
Technique LoadTech(FILE *techData, int id) {
	Technique tech;
	int techId;
	char line[256];
	char *token;
	char *saveptr;
	rewind(techData);
	fgets(line, sizeof(line), techData);
	while (fgets(line, sizeof(line), techData)) {
		token = strtok_r(line, "	", &saveptr);
		techId = atoi(token);
		if (techId == id) {
			tech.id = techId;
			token = strtok_r(NULL, "	", &saveptr);
			tech.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.spriteId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.animationId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.economy = (EconomyType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.roll = (DiceType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.tech = (TechniqueType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.attr = (AttributeType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.costMoney = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.costHP = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.costStress = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.diceSide = (DiceType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.applyBonuses = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.diceAmount = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.flatBonus = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.ignoreArmor = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.status = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetEnemy[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetEnemy[1] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetEnemy[2] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetEnemy[3] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetEnemy[4] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetAlly[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetAlly[1] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetAlly[2] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetAlly[3] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.targetAlly[4] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.spawnId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.rollBonus = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.damageMultiplayer = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.hurtMultiplayer = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			tech.healMultiplayer = atoi(token);
			return tech;
		}
	}
	return tech;
}
Equip LoadWeapon(FILE *weaponData, int id) {
	Equip weapon;
	weapon.weapon.type = EQUIP_WEAPON;
	int weaponId;
	char line[256];
	char *token;
	char *saveptr;
	rewind(weaponData);
	fgets(line, sizeof(line), weaponData);
	while (fgets(line, sizeof(line), weaponData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		weaponId = atoi(token);
		if (weaponId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.spriteId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.canUnequip = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.cost = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.attack = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.tech = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			weapon.weapon.hurtMultiplayer = atoi(token);
			return weapon;
		}
	}
	return weapon;
}
Equip LoadArmor(FILE *armorData, int id) {
	Equip armor;
	armor.armor.type = EQUIP_ARMOR;
	int armorId;
	char line[256];
	char *token;
	char *saveptr;
	rewind(armorData);
	fgets(line, sizeof(line), armorData);
	while (fgets(line, sizeof(line), armorData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		armorId = atoi(token);
		if (armorId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.spriteId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.canUnequip = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.cost = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.armor = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.tech = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			armor.armor.hurtMultiplayer = atoi(token);
			return armor;
		}
	}
	return armor;
}
Equip LoadCharm(FILE *charmData, int id) {
	Equip charm;
	charm.charm.type = EQUIP_CHARM;
	int charmId;
	char line[256];
	char *token;
	char *saveptr;
	rewind(charmData);
	fgets(line, sizeof(line), charmData);
	while (fgets(line, sizeof(line), charmData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		charmId = atoi(token);
		if (charmId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.spriteId = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.canUnequip = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.cost = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.health = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.armor = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.stress = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.inmunity = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.tech = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			charm.charm.hurtMultiplayer = atoi(token);
			return charm;
		}
	}
	return charm;
}
void SetProficiency(void *playerData, AttributeType attr) {
	Player *player = (Player *) playerData;
	switch (attr) {
		case ATTR_ATHLETICS:
			player->physique[1] += 2;
			break;
		case ATTR_CONSTITUTION:
			player->physique[2] += 2;
			break;
		case ATTR_MEDICINE:
			player->physique[3] += 2;
			break;
		case ATTR_MELEE:
			player->physique[4] += 2;
			break;
		case ATTR_VIBES:
			player->physique[5] += 2;
			break;
		case ATTR_ACCURACY:
			player->reflex[1] += 2;
			break;
		case ATTR_ACROBATICS:
			player->reflex[2] += 2;
			break;
		case ATTR_MISCHIEF:
			player->reflex[3] += 2;
			break;
		case ATTR_PERCEPTION:
			player->reflex[4] += 2;
			break;
		case ATTR_TOUCH:
			player->reflex[5] += 2;
			break;
		case ATTR_ARCANUM:
			player->lore[1] += 2;
			break;
		case ATTR_BEASTS:
			player->lore[2] += 2;
			break;
		case ATTR_DREAMS:
			player->lore[3] += 2;
			break;
		case ATTR_DUNGEONS:
			player->lore[4] += 2;
			break;
		case ATTR_NATURE:
			player->lore[5] += 2;
			break;
		case ATTR_ANIMA:
			player->charisma[1] += 2;
			break;
		case ATTR_AUTHORITY:
			player->charisma[2] += 2;
			break;
		case ATTR_DRAMA:
			player->charisma[3] += 2;
			break;
		case ATTR_KINSHIP:
			player->charisma[4] += 2;
			break;
		case ATTR_PASSION:
			player->charisma[5] += 2;
			break;
		default:
			break;
	}
}
void SetFeature(FILE *weaponData, FILE *charmFile, FILE *techData, void *playerData, Feature feature) {
	Player *player = (Player *) playerData;
	switch (feature) {
		case FEAT_DEMONHANDS:
			player->tech[player->techAmount] = LoadTech(techData, 81);
			player->techAmount++;
			player->tech[player->techAmount] = LoadTech(techData, 82);
			player->techAmount++;
			player->tech[player->techAmount] = LoadTech(techData, 83);
			player->techAmount++;
			player->tech[player->techAmount] = LoadTech(techData, 86);
			player->techAmount++;
			player->tech[player->techAmount] = LoadTech(techData, 87);
			player->techAmount++;
			break;
		case FEAT_PASSIONFUL:
			player->charisma[5] += 1;
			break;
		case FEAT_PASSIONNAIL:
			player->weapon = LoadWeapon(weaponData, 64);
			break;
		case FEAT_DEMONHUNTER:
			player->enemyBonus[(int) ENEMY_DEMON] += 1;
			break;
		case FEAT_BORNFORMISCHIEF:
			SetProficiency(playerData, ATTR_MISCHIEF);
			break;
		case FEAT_UNDEADHUNTER:
			player->enemyBonus[(int) ENEMY_UNDEAD] += 1;
			break;
		case FEAT_LUXURIOUSGEM:
			player->charm = LoadCharm(charmFile, 1);
		default:
			break;
	}
	player->features[player->featuresAmount] = feature;
	player->featuresAmount++;
	player = NULL;
}
int DiceRoll(DiceType dice, int *randomValue) {
	int roll;
	switch (dice) {
		case DICE_D100:
		case DICE_SAVED100:
			roll = (*randomValue % 100) +  1;
			break;
		case DICE_D20:
		case DICE_SAVED20:
			roll = (*randomValue % 20) + 1;
			break;
		case DICE_D12:
		case DICE_SAVED12:
			roll = (*randomValue % 12) + 1;
			break;
		case DICE_D10:
		case DICE_SAVED10:
			roll = (*randomValue % 10) + 1;
			break;
		case DICE_D8:
		case DICE_SAVED8:
			roll = (*randomValue % 8) + 1;
			break;
		case DICE_D6:
		case DICE_SAVED6:
			roll = (*randomValue % 6) + 1;
			break;
		case DICE_D4:
		case DICE_SAVED4:
			roll = (*randomValue % 4) + 1;
			break;
		default:
			roll = 0;
			break;
	}
	Random(randomValue);
	return roll;
}
int DiceMean(DiceType dice) {
	switch (dice) {
		case DICE_D100:
		case DICE_SAVED100:
			return 51;
		case DICE_D20:
		case DICE_SAVED20:
			return 11;
		case DICE_D12:
		case DICE_SAVED12:
			return 7;
		case DICE_D10:
		case DICE_SAVED10:
			return 6;
		case DICE_D8:
		case DICE_SAVED8:
			return 5;
		case DICE_D6:
		case DICE_SAVED6:
			return 4;
		case DICE_D4:
		case DICE_SAVED4:
			return 3;
		default:
			return 0;
	}
}
void SetSeed(int seed, int *randomValue) {
	srand(seed);
	*randomValue = rand();
}
void SetTimeSeed(int *randomValue) {
	srand(time(NULL));
	*randomValue = rand();
}
void Random(int *randomValue) {
	*randomValue = rand();
}
void RollInitiative(Combat *combat, int *randomValue) {
	int i = 0;
	int j = 0;
	Enemy *enemy;
	Player *player;

	for (i = 0; i < 5; i++) {
		if (combat->player[i] == NULL) continue;
		player = (Player *) combat->player[i];
		DiceRoll(DICE_D20, randomValue) + player->reflex[0];
		j++;
		player = NULL;
	}
	for (i = 0; i < 5; i++) {
		if (combat->enemy[i] == NULL) continue;
		enemy = (Enemy *) combat->enemy[i];
		DiceRoll(DICE_D20, randomValue) + enemy->reflex;
		j++;
		enemy = NULL;
	}
}
