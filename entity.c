#include "entity.h"

void LoadEnemiesFile(FILE **file, const char *enemySheet) {
	if (*file != NULL) {
		fclose(*file);
		(*file) = NULL;
	}
	if (FileExists(enemySheet)) *file = fopen(enemySheet, "r");
}
void LoadEnemiesOnCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, Combat *combat, int id) {
	char line[512];
	char *token;
	char *saveptr;
	int i = 1;
	fgets(line, sizeof(line), file);
	while (fgets(line, sizeof(line), file) != NULL) {
		if (i == id) {
			token = strtok_r(line, "	", &saveptr);
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
		i++;
	}
}
Entity *LoadEnemy(FILE *enemyData, FILE *spriteData, FILE *techData, int id) {
	if (id == 0) return NULL;
	Entity *enemy = (Entity *) malloc(sizeof(Entity));
	enemy->enemy.type = ENTITY_ENEMY;
	enemy->enemy.phyBonus = 0;
	enemy->enemy.refBonus = 0;
	enemy->enemy.lorBonus = 0;
	enemy->enemy.chaBonus = 0;
	enemy->enemy.techAmount = 0;
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
			enemy->enemy.id = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.sprite = LoadSingleSprite(spriteData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.enemy = (EnemyType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.physique = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.reflex = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.lore = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.charisma = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.size = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.maxHealth = DiceMean((DiceType) enemy->enemy.size) * atoi(token) + enemy->enemy.physique * atoi(token);
			enemy->enemy.health = enemy->enemy.maxHealth;
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.maxStress = atoi(token);
			enemy->enemy.stress = enemy->enemy.maxStress;
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.weakness[0] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.weakness[1] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.resistances[0] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.resistances[1] = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			enemy->enemy.multiattack = atoi(token);
			// Loading techniques
			token = strtok_r(NULL, "	", &saveptr);
			tech = strtok_r(token, ",", &saveptr);
			while (token != NULL) {
				enemy->enemy.tech[enemy->enemy.techAmount] = LoadTech(techData, atoi(tech));
				tech = strtok_r(NULL, ",", &saveptr);
				enemy->enemy.techAmount++;
			}
			return enemy;
		}
	}
	return NULL;
}
Entity *LoadPlayer(FILE *characterData, FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, int id) {
	if (id == 0) return NULL;
	Entity *player = (Entity *) malloc(sizeof(Entity));
	player->player.type = ENTITY_PLAYER;
	player->player.maxStress = 3;
	player->player.stress = 0;
	player->player.demonTally = 0;
	player->player.accumulatedDamage = 0;
	player->player.phyBonus = 0;
	player->player.refBonus = 0;
	player->player.lorBonus = 0;
	player->player.chaBonus = 0;
	player->player.featuresAmount = 0;
	player->player.techAmount = 0;
	player->player.equipedTechAmount = 0;
	char line[512];
	char *saveptr;
	char *token;
	char *feature;
	int playerId;
	int i;
	for (i = 0; i < 6; i++) {
		player->player.physique[i] = 0;
		player->player.reflex[i] = 0;
		player->player.lore[i] = 0;
		player->player.charisma[i] = 0;
	}
	for (i = 0; i < 8; i++) player->player.enemyBonus[i] = 0;
	rewind(characterData);
	fgets(line, sizeof(line), characterData);
	while (fgets(line, sizeof(line), characterData)) {
		token = strtok_r(line, "	", &saveptr);
		playerId = atoi(token);
		if (playerId == id) {
			player->player.id = playerId;
			token = strtok_r(NULL, "	", &saveptr);
			player->player.name = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.surname = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.giftCurse = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.nomGuerre = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.description = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.sprite = LoadSingleSprite(spriteData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->player.physique[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.reflex[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.lore[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.charisma[0] = atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->player.hitDice = (DiceType) atoi(token);
			player->player.maxHealth = DiceMean(player->player.hitDice) + player->player.physique[0];
			player->player.health = player->player.maxHealth;
			token = strtok_r(NULL, "	", &saveptr);
			player->player.weakness = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.resistance = (DamageType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.inmunity = (StatusType) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			player->player.weapon = LoadWeapon(weaponData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->player.armor = LoadArmor(armorData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			player->player.charm = LoadCharm(charmData, atoi(token));
			token = strtok_r(NULL, "	", &saveptr);
			feature = strtok_r(token, ",", &saveptr);
			while (token != NULL) {
				SetFeature(weaponData, charmData, techData, player, (Feature) atoi(feature));
				feature = strtok_r(NULL, ",", &saveptr);
				player->player.featuresAmount++;
			}
			return player;
		}
	}
	return player;
}
void DamageEntity(Combat *combat, Entity attacker, Technique tech) {
	int position;
	EntityType side;
	int i;
	int min;
	int max;
	if (attacker.player.type == ENTITY_PLAYER) {
		position = attacker.player.position;
		side = attacker.player.type;
	}
	else {
		position = attacker.enemy.position;
		side = attacker.enemy.type;
	}
	min = (int) Clamp(-2 + position, 0, 4);
	max = (int) Clamp(2 + position, 0, 4);
	for (i = min; i <= max; i++) {
		if (side == ENTITY_ENEMY) {
			combat->enemy[i]->enemy.health = (int) Clamp(combat->enemy[i]->enemy.health - i, 0, combat->enemy[i]->enemy.maxHealth);
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
		case DMG_ACID:
			break;
		case DMG_PSYCHIC:
			break;
		case DMG_HEALNATURE:
		case DMG_HEALBLOODY:
		case DMG_HEALPSY:
			break;
		case DMG_HEALARMOR:
			break;
		default:
			break;
	}
}
void KillEntity(Combat *combat, Entity *entity) {
	EntityType type;
	if (entity->player.type == ENTITY_PLAYER) type = ENTITY_PLAYER;
	else type = ENTITY_ENEMY;
	UnloadEntity(type, combat, entity->player.position);
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
			if (combat->enemy[position]->enemy.sprite != NULL) {
				free(combat->enemy[position]->enemy.sprite);
				combat->enemy[position]->enemy.sprite = NULL;
			}
			free(combat->enemy[position]);
			combat->enemy[position] = NULL;
			return;
		case ENTITY_PLAYER:
			if (combat->player[position] == NULL) return;
			if (combat->player[position]->player.sprite != NULL) {
				free(combat->player[position]->player.sprite);
				combat->player[position]->player.sprite = NULL;
			}
			printf("INFO: COMBAT: Milestone.\n");
			free(combat->player[position]);
			combat->player[position] = NULL;
			return;
	}
}
void DrawCombat(Combat combat, SafeTexture *textures, Color color) {
	int i;
	Sprite *sprite;
	for (i = 0; i < 5; i++) {
		if (combat.enemy[i] != NULL) {
			sprite = combat.enemy[i]->enemy.sprite;
			DrawTexturePro(textures[sprite->textureIndex].tex, sprite->origin, sprite->dest, Vector2Add(sprite->position, (Vector2) { -64 * combat.enemy[i]->enemy.position, 0 }), sprite->rotation, color);
		}
		if (combat.player[i] != NULL) {
			sprite = combat.player[i]->player.sprite;
			DrawTexturePro(textures[sprite->textureIndex].tex, sprite->origin, sprite->dest, Vector2Add(sprite->position, (Vector2) { -64 * combat.player[i]->player.position, 0 }), sprite->rotation, color);
		}
	}
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
void SetProficiency(Entity *player, AttributeType attr) {
	switch (attr) {
		case ATTR_ATHLETICS:
			player->player.physique[1] += 2;
			break;
		case ATTR_CONSTITUTION:
			player->player.physique[2] += 2;
			break;
		case ATTR_MEDICINE:
			player->player.physique[3] += 2;
			break;
		case ATTR_MELEE:
			player->player.physique[4] += 2;
			break;
		case ATTR_VIBES:
			player->player.physique[5] += 2;
			break;
		case ATTR_ACCURACY:
			player->player.reflex[1] += 2;
			break;
		case ATTR_ACROBATICS:
			player->player.reflex[2] += 2;
			break;
		case ATTR_MISCHIEF:
			player->player.reflex[3] += 2;
			break;
		case ATTR_PERCEPTION:
			player->player.reflex[4] += 2;
			break;
		case ATTR_TOUCH:
			player->player.reflex[5] += 2;
			break;
		case ATTR_ARCANUM:
			player->player.lore[1] += 2;
			break;
		case ATTR_BEASTS:
			player->player.lore[2] += 2;
			break;
		case ATTR_DREAMS:
			player->player.lore[3] += 2;
			break;
		case ATTR_DUNGEONS:
			player->player.lore[4] += 2;
			break;
		case ATTR_NATURE:
			player->player.lore[5] += 2;
			break;
		case ATTR_ANIMA:
			player->player.charisma[1] += 2;
			break;
		case ATTR_AUTHORITY:
			player->player.charisma[2] += 2;
			break;
		case ATTR_DRAMA:
			player->player.charisma[3] += 2;
			break;
		case ATTR_KINSHIP:
			player->player.charisma[4] += 2;
			break;
		case ATTR_PASSION:
			player->player.charisma[5] += 2;
			break;
		default:
			break;
	}
}
void SetFeature(FILE *weaponData, FILE *charmFile, FILE *techData, Entity *player, Feature feature) {
	switch (feature) {
		case FEAT_DEMONHANDS:
			player->player.tech[player->player.techAmount] = LoadTech(techData, 81);
			player->player.techAmount++;
			player->player.tech[player->player.techAmount] = LoadTech(techData, 82);
			player->player.techAmount++;
			player->player.tech[player->player.techAmount] = LoadTech(techData, 83);
			player->player.techAmount++;
			player->player.tech[player->player.techAmount] = LoadTech(techData, 86);
			player->player.techAmount++;
			player->player.tech[player->player.techAmount] = LoadTech(techData, 87);
			player->player.techAmount++;
			break;
		case FEAT_PASSIONFUL:
			player->player.charisma[5] += 1;
			break;
		case FEAT_PASSIONNAIL:
			player->player.weapon = LoadWeapon(weaponData, 64);
			break;
		case FEAT_DEMONHUNTER:
			player->player.enemyBonus[(int) ENEMY_DEMON] += 1;
			break;
		case FEAT_BORNFORMISCHIEF:
			SetProficiency(player, ATTR_MISCHIEF);
			break;
		case FEAT_UNDEADHUNTER:
			player->player.enemyBonus[(int) ENEMY_UNDEAD] += 1;
			break;
		case FEAT_LUXURIOUSGEM:
			player->player.charm = LoadCharm(charmFile, 1);
		default:
			break;
	}
	player->player.features[player->player.featuresAmount] = feature;
	player->player.featuresAmount++;
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
