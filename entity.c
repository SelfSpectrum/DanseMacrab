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
			combat->enemy[0] = LoadEnemy(enemyData, spriteData, techData, atoi(token), 0);
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[1] = LoadEnemy(enemyData, spriteData, techData, atoi(token), 1);
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[2] = LoadEnemy(enemyData, spriteData, techData, atoi(token), 2);
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[3] = LoadEnemy(enemyData, spriteData, techData, atoi(token), 3);
			token = strtok_r(NULL, "	", &saveptr);
			combat->enemy[4] = LoadEnemy(enemyData, spriteData, techData, atoi(token), 4);
		}
	}
}
void *LoadEnemy(FILE *enemyData, FILE *spriteData, FILE *techData, int id, int position) {
	if (id == 0) return NULL;

	char line[512];
	int enemyId;
	rewind(enemyData);
	fgets(line, sizeof(line), enemyData);
	while (fgets(line, sizeof(line), enemyData) != NULL) {
		sscanf(line, "%d", &enemyId);
		if (enemyId == id) return ParseEnemy(spriteData, techData, line, position);
	}
	return NULL;
}
void *ParseEnemy(FILE *spriteData, FILE *techData, char *line, int position) {
	void *enemyInfo = malloc(sizeof(Enemy));
	if (enemyInfo == NULL) {
		TraceLog(LOG_ERROR, "ERROR: ENTITY: Enemy load failed.\n");
		return NULL;
	}

	Enemy *enemy = (Enemy *) enemyInfo;
	int spriteId;
	int enemyType;
	int maxHealth;
	int stats[6];
	int i;
	char tech[256];
	char *token;
	char *saveptr;

	enemy->position = position;
	enemy->phyBonus = 0;
	enemy->refBonus = 0;
	enemy->lorBonus = 0;
	enemy->chaBonus = 0;
	enemy->techAmount = 0;

	for (i = 0; i < ENEMYTECH_SIZE; i++) enemy->tech[i] = NULL;

	sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%[^\n]",
			&enemy->id,
			&enemy->name,
			&enemy->description,
			&spriteId,
			&enemyType,
			&enemy->physique,
			&enemy->reflex,
			&enemy->lore,
			&enemy->charisma,
			&enemy->size,
			&maxHealth,
			&enemy->maxStress,
			&stats[0],
			&stats[1],
			&stats[2],
			&stats[3],
			&stats[4],
			&stats[5],
			&enemy->multiattack,
			tech);
	enemy->sprite = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, spriteId);
	enemy->enemy = (EnemyType) enemyType;
	enemy->maxHealth = DiceMean((DiceType) enemy->size) * maxHealth + enemy->physique * maxHealth;
	enemy->health = enemy->maxHealth;
	enemy->stress = enemy->maxStress;
	enemy->weakness[0] = (DamageType) stats[0];
	enemy->weakness[1] = (DamageType) stats[1];
	enemy->resistances[0] = (DamageType) stats[2];
	enemy->resistances[1] = (DamageType) stats[3];
	enemy->inmunities[0] = (StatusType) stats[4];
	enemy->inmunities[1] = (StatusType) stats[5];
	// Loading techniques
	token = strtok_r(tech, ",", &saveptr);
	while (token != NULL) {
		enemy->tech[enemy->techAmount] = LoadTech(techData, atoi(token));
		token = strtok_r(NULL, ",", &saveptr);
		enemy->techAmount++;
	}
	enemy = NULL;
	return enemyInfo;
}
void *LoadPlayer(FILE *characterData, FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, int id, int position) {
	if (id == 0) return NULL;

	char line[512];
	int playerId;
	rewind(characterData);
	fgets(line, sizeof(line), characterData);
	while (fgets(line, sizeof(line), characterData)) {
		sscanf(line, "%d", &playerId);
		if (playerId == id) return ParsePlayer(spriteData, weaponData, armorData, charmData, techData, line, position);
	}
	return NULL;
}
void *ParsePlayer(FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, char *line, int position) {
	void *playerInfo = malloc(sizeof(Player));
	if (playerInfo == NULL) {
		TraceLog(LOG_ERROR, "ERROR: ENTITY: Player load failed.\n");
		return NULL;
	}

	Player *player = (Player *) playerInfo;
	int spriteId;
	int proficiency[3];
	int hitDice;
	int stats[3];
	int equipment[3];
	int i;
	char *saveptr;
	char *token;
	char feature[128];

	player->position = position;
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
	for (i = 0; i < 6; i++) {
		player->physique[i] = 0;
		player->reflex[i] = 0;
		player->lore[i] = 0;
		player->charisma[i] = 0;
	}
	for (i = 0; i < 8; i++) player->enemyBonus[i] = 0;
	for (i = 0; i < 10; i++) {
		player->tech[i] = NULL;
		player->skill[i] = NULL;
	}
	player->weapon[0] = NULL;
	player->weapon[1] = NULL;
	player->weapon[2] = NULL;
	player->armor[0] = NULL;
	player->charm[0] = NULL;
	player->charm[1] = NULL;

	sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%[^\n]",
			&player->id,
			&player->name,
			&player->surname,
			&player->giftCurse,
			&player->nomGuerre,
			&player->description,
			&spriteId,
			&player->physique[0],
			&player->reflex[0],
			&player->lore[0],
			&player->charisma[0],
			&proficiency[0],
			&proficiency[1],
			&proficiency[2],
			&hitDice,
			&stats[0],
			&stats[1],
			&stats[2],
			&equipment[0],
			&equipment[1],
			&equipment[2],
			feature);

	player->sprite = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, spriteId);
	SetProficiency(playerData, (AttributeType) &proficiency[0]);
	SetProficiency(playerData, (AttributeType) &proficiency[1]);
	SetProficiency(playerData, (AttributeType) &proficiency[2]);
	player->hitDice = (DiceType) hitDice;
	player->maxHealth = DiceMean(player->hitDice) + player->physique[0];
	player->health = player->maxHealth;
	player->weakness = (DamageType) stats[0];
	player->resistance = (DamageType) stats[1];
	player->inmunity = (StatusType) stats[2];
	player->weapon[0] = LoadWeapon(weaponData, equipment[0]);
	player->armor = LoadArmor(armorData, equipment[1]);
	player->charm[0] = LoadCharm(charmData, equipment[2]);

	token = strtok_r(feature, ",", &saveptr);
	while (token != NULL) {
		SetFeature(weaponData, charmData, techData, playerData, (Feature) atoi(token));
		token = strtok_r(NULL, ",", &saveptr);
	}
	player = NULL;
	return playerInfo;
}
Technique *LoadTech(FILE *techData, int id) {
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
Technique *ParseTech(char *line) {
}
void PlayerLoadTech(FILE *techData, Player *player) {
}
void UseTech(Combat *combat, void *entity, EntityType side, Technique *tech, int *randomValue) {
	int position;
	int i;
	int min;
	int max;
	int damage = DiceRoll(tech->roll, randomValue);

	if (type == ENTITY_PLAYER) {
		Player *player = (Player *) entity;
		position = player->position;
		min = (int) Clamp(-2 + position, 0, 4);
		max = (int) Clamp(2 + position, 0, 4);
		for (i = min; i <= max; i++) {
			if (tech->targetEnemy[2 - position + i]) {
				DamageEntity(combat->enemy[i], ENTITY_ENEMY, damage, tech->type);
			}
			if (tech->targetAlly[2 - position + i]) {
				DamageEntity(combat->player[i], ENTITY_PLAYER, damage, tech->type);
			}
		}
	}
	else {
		Enemy *enemy = (Enemy *) entity;
		position = enemy->position;
		min = (int) Clamp(-2 + position, 0, 4);
		max = (int) Clamp(2 + position, 0, 4);
		for (i = min; i <= max; i++) {
			if (tech->targetEnemy[2 - position + i]) {
				DamageEntity(combat->player[i], ENTITY_PLAYER, damage, tech->type);
			}
			if (tech->targetAlly[2 - position + i]) {
				DamageEntity(combat->enemy[i], ENTITY_ENEMY, damage, tech->type);
			}
		}
	}
}
Weapon *LoadWeapon(FILE *weaponData, int id) {
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
Weapon *ParseWeapon(char *line) {
	Weapon *weapon;
	weapon.weapon.type = EQUIP_WEAPON;
}
Armor *LoadArmor(FILE *armorData, int id) {
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
Charm *LoadCharm(FILE *charmData, int id) {
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
void PlayerEquip(Player *player, void *equipment) { 
	Equipment *equip = (Equipment *) equipment;
	switch (equip->type) {
		case EQUIP_WEAPON:
			Weapon *weapon = (Weapon *) equipment;
			weapon = NULL;
			break;	
	}
	equip = NULL;
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
			break;
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
			enemy = NULL;
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
			player = NULL;
			return;
		default:
			printf("HOW DID THIS HAPPEN! Alongtimeagoactuallynever-\n");
			return;
	}
}
void MoveEntity(Combat *combat, EntityType type, int origin, int position) {
}
void DamageEntity(void *entity, EntityType type, int damage, DamageType damageType) {
	if (type == ENTITY_PLAYER) {
		Player *player = (Player *) entity;
		player->health = (int) Clamp(player->health - damage, 0, player->maxHealth);
		player = NULL;
	}
	else {
		Enemy *enemy = (Enemy *) combat->enemy[i];
		enemy->health = (int) Clamp(enemy->health - damage, 0, enemy->maxHealth);
		enemy = NULL;
	}
}
void KillEntity(Combat *combat, void *entity, EntityType type) {
	if (type == ENTITY_ENEMY) {
		Enemy *enemy = (Enemy *) entity;
		UnloadEntity(type, combat, enemy->position);
		enemy = NULL;
	}
	else {
		Player *player = (Player *) entity;
		UnloadEntity(type, combat, player->position);
		player = NULL;
	}
}
void RollInitiative(Combat *combat, int *randomValue) {
	int i = 0;
	Enemy *enemy;
	Player *player;

	combat->timelineAmount = 0;
	for (i = 0; i < 10; i++) {
		combat->timeline[i] = NULL;
		combat->timelineType[i] = ENTITY_NONE;
	}

	for (i = 0; i < 5; i++) {
		if (combat->player[i] == NULL) continue;
		combat->timeline[combat->timelineAmount] = combat->player[i];
		player = (Player *) combat->player[i];
		player->initiative = DiceRoll(DICE_D20, randomValue) + player->reflex[0];
		combat->timelineAmount++;
		player = NULL;
	}
	for (i = 0; i < 5; i++) {
		if (combat->enemy[i] == NULL) continue;
		combat->timeline[combat->timelineAmount] = combat->enemy[i];
		enemy = (Enemy *) combat->enemy[i];
		enemy->initiative = DiceRoll(DICE_D20, randomValue) + enemy->reflex;
		combat->timelineAmount++;
		enemy = NULL;
	}

	SortInitiative(combat->timeline, combat->timelineType, combat->timelineAmount);
}
void SortInitiative(void **timeline, EntityType *types, int timelineAmount) {
	int i;
	int j;
	void *key;
	EntityType keyType;
	for (i = 1; i < timelineAmount; i++) {
		key = timeline[i];
		keyType = types[i];
		j = i - 1;
		while (j >= 0 && GetInitiative(timeline[j], types[i]) > GetInitiative(key, keyType)) {
			timeline[j + 1] = timeline[j];
			types[j + 1] = types[j];
			j--;
		}
		timeline[j + 1] = key;
		types[j + 1] = keyType;
	}
}
int GetInitiative(void *entity, EntityType type) {
	switch (type) {
		case ENTITY_PLAYER:
			return ((Player *) entity)->initiative;
		case ENTITY_ENEMY:
			return ((Enemy *) entity)->initiative;
		default:
			return -69;
	}
}
void StartCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, int id, Combat *combat, int *randomValue);
	LoadEnemiesOnCombat(file, enemyData, spriteData, techData, combat, id);
	RollInitiative(combat, randomValue);
}
void StartTurn(Combat *combat) {
	switch (combat->timelineType[combat->timelineOrder]) {
		case ENTITY_PLAYER:
			Player *player = (Player *) combat->timeline[combat->timelineOrder];
			player->major = true;
			player->minor = true;
			player->reaction = true;
			player->movement = true;
			break;
		case ENTITY_ENEMY:
			Enemy *enemy = (Enemy *) combat->timeline[combat->timelineOrder];
			break;
		default:
			EndTurn(combat);
	}
}
void EndTurn(Combat *combat) {
	combat->timelineOrder = (combat->timelineOrder + 1) % combat->timelineAmount;
}
