#ifndef ENTITY_H
#define ENTITY_H

#include "graphic.h"

typedef struct Player Player;
typedef struct Enemy Enemy;
typedef struct Weapon Weapon;
typedef struct Armor Armor;
typedef struct Charm Charm;
typedef struct Technique Technique;
typedef struct Combat Combat;
typedef enum DamageType DamageType;
typedef enum EntityType EntityType;
typedef enum EnemyType EnemyType;
typedef enum EquipType EquipType;
typedef enum DiceType DiceType;
typedef enum TechniqueType TechniqueType;
typedef enum AttributeType AttributeType;
typedef enum EconomyType EconomyType;
typedef enum StatusType StatusType;
typedef enum Feature Feature;
typedef union Entity Entity;
typedef union Equip Equip;

enum DamageType {
	DMG_NONE = 0,
	DMG_SLASHING = 1,
	DMG_BLUDGEONING = 2,
	DMG_PIERCING = 3,
	DMG_BALLISTIC = 4,
	DMG_FIRE = 5,
	DMG_ICE = 6,
	DMG_ELECTRIC = 7,
	DMG_ACID = 8,
	DMG_PSYCHIC = 9,
	DMG_HEALNATURE = 10,
	DMG_HEALBLOODY = 11,
	DMG_HEALPSY = 12,
	DMG_HEALARMOR = 13,
	DMG_HEALMEDICINE = 14,
	DMG_TRUE = 15
};
enum EntityType {
	ENTITY_ENEMY = 0,
	ENTITY_PLAYER = 1
};
enum EnemyType {
	ENEMY_BEAST = 0,
	ENEMY_DEMON = 1,
	ENEMY_DRAGON = 2,
	ENEMY_FASCIST = 3,
	ENEMY_GRUDGE = 4,
	ENEMY_MUTANT = 5,
	ENEMY_NIGHTMARE = 6,
	ENEMY_UNDEAD = 7
};
enum EquipType {
	EQUIP_WEAPON = 0,
	EQUIP_ARMOR = 1,
	EQUIP_CHARM = 2
};
enum DiceType {
	DICE_NONE = 0,
	DICE_D100 = 7,
	DICE_D20 = 6,
	DICE_D12 = 5,
	DICE_D10 = 4,
	DICE_D8 = 3,
	DICE_D6 = 2,
	DICE_D4 = 1,
	DICE_SAVED100 = 14,
	DICE_SAVED20 = 13,
	DICE_SAVED12 = 12,
	DICE_SAVED10 = 11,
	DICE_SAVED8 = 10,
	DICE_SAVED6 = 9,
	DICE_SAVED4 = 8
};
enum TechniqueType {
	TECH_ATTACK = 0,
	TECH_FEATURE = 1,
	TECH_CHECK = 2,
	TECH_TOME = 3,
	TECH_SONG = 4,
	TECH_DRACONIC = 5,
	TECH_GOREART = 6,
	TECH_DEMONHAND = 7,
	TECH_SUMMON = 8,
	TECH_SORCERY = 9
};
enum AttributeType {	// ATTR % 6 for index
	ATTR_PHYSIQUE = 0,
	ATTR_ATHLETICS = 1,
	ATTR_CONSTITUTION = 2,
	ATTR_MEDICINE = 3,
	ATTR_MELEE = 4,
	ATTR_VIBES = 5,
	ATTR_REFLEX = 6,
	ATTR_ACCURACY = 7,
	ATTR_ACROBATICS = 8,
	ATTR_MISCHIEF = 9,
	ATTR_PERCEPTION = 10,
	ATTR_TOUCH = 11,
	ATTR_LORE = 12,
	ATTR_ARCANUM = 13,
	ATTR_BEASTS = 14,
	ATTR_DREAMS = 15,
	ATTR_DUNGEONS = 16,
	ATTR_NATURE = 17,
	ATTR_CHARISMA = 18,
	ATTR_ANIMA = 19,
	ATTR_AUTHORITY = 20,
	ATTR_DRAMA = 21,
	ATTR_KINSHIP = 22,
	ATTR_PASSION = 23
};
enum StatusType {
	STATUS_NONE = 0, // No effect
	STATUS_BLEED = 1, // Take damage everytime the entity moves
	STATUS_BLINDED = 2, // Must succeed on 12 reflex check before attacks or checks
	STATUS_BRUISED = 3, // Take extra bludgeoning damage
	STATUS_BURNING = 4, // 1d10 fire damage, might destroy items
	STATUS_CONFUSED = 5, // If a tech don't hit, counts as critical fumble
	STATUS_CURSED = 6, //
	STATUS_DEAFENED = 7, // Can't communicate
	STATUS_DEATH = 8, // Die after some turns
	STATUS_DEVOURED = 9, // 1d10 acid damage, deal stress
	STATUS_DROWNING = 10, // Deal stress
	STATUS_GRAPPLED = 11, // Can't move or dodge
	STATUS_HORRIFIED = 12, // Can't be in front of an enemy
	STATUS_INVISIBLE = 13, // Must succeed a 12 perception check before roll or attack
	STATUS_LINKED = 14, // Two or more creatures link, what happens to one, happens to all
	STATUS_MOURNFUL = 15, // Can't do anything
	STATUS_OILED = 16, // Recieve extra fire damage
	STATUS_PARALYZED = 17, // Can't move or take actions
	STATUS_PASSENGER = 18, // Carry an entity
	STATUS_POISONED = 19, // Disadvantage on all rolls
	STATUS_POSSESSION = 20, // Another creature controls actions
	STATUS_PRONE = 21, // Melee are crits
	STATUS_RAGE = 22, // Attack the closest creature
	STATUS_SCARRED = 23, // Recieve extra slashing damage
	STATUS_SLEEPING = 24, // Attacks autohit
	STATUS_SUFFOCATING = 25 // Deal stress
};
enum EconomyType {
	ECO_ACTION = 0,
	ECO_BONUS = 1,
	ECO_REACTION = 2,
	ECO_OFF = 3
};
enum Feature {
	// Berserk
	FEAT_SELFSTEEM = 0,
	FEAT_BEASTHORNS = 1,
	FEAT_EMOTIONSCENT = 2,
	FEAT_DARKVISION = 3,
	// Blue Mage
	FEAT_TOME = 16,
	FEAT_DECODER = 17,
	FEAT_DETECTMAGIC = 18,
	FEAT_PATHFINDER = 19,
	// Dragoon
	FEAT_DRAGONSPEECH = 32,
	FEAT_DRACONIC = 33,
	FEAT_SNAKEBLOOD = 34,
	FEAT_DETECTSUPERNATURAL = 35,
	// Fang
	FEAT_STRESSRITUAL = 48,
	FEAT_STRESSMARKS = 49,
	FEAT_SCARRED = 50,
	FEAT_SCARSOFEXPERIENCE = 51,
	// Moon Child
	FEAT_LUCKY = 64,
	FEAT_FAVOROFLUCK = 65,
	FEAT_DETECTDANGER = 66,
	FEAT_SECRETDOOR = 67,
	// Red Mage
	FEAT_BLODDYLEARNING = 80,
	FEAT_LIPREADING = 81,
	FEAT_LIFEABSORTION = 82,
	FEAT_BLOODUNLEASHED = 83,
	// Songbird Maiden
	FEAT_SONGS = 96,
	FEAT_EARTHLYSONG = 97,
	FEAT_BLESS = 98,
	FEAT_FAREWELL = 99,
	// Sorcerer
	FEAT_WISH = 112,
	// Volfe Knight
	FEAT_WOLFCOMPANION = 128,
	FEAT_WOLFSENSE = 129,
	FEAT_DREAMWALKER = 130,
	FEAT_RANGEATTACKER = 131,
	// Unseelie
	FEAT_ANIMALSHAPE = 144,
	FEAT_TRUENAME = 145,
	FEAT_ANIMALFEELING = 146,
	FEAT_DRUIDSPEECH = 147,
	// Black Dog
	FEAT_BLACKDOG = 160,
	FEAT_UNDYING = 161,
	FEAT_SHADOWFREEDOM = 162,
	FEAT_SHADOWTELEPORT = 163,
	// Black Mage
	FEAT_DEMONHANDS = 176,
	FEAT_LABERYNTHIASPEECH = 177,
	FEAT_FORKEDTONGE = 178,
	FEAT_PASSIONFUL = 179,
	// Familiar
	FEAT_FAMILIARBOND = 192,
	FEAT_HELPBOND = 193,
	FEAT_FREEFLY = 194,
	FEAT_BOOKOFOOL = 195,
	// Guardian Nail
	FEAT_PASSIONNAIL = 208,
	FEAT_SPECTRALHAND = 209,
	FEAT_PRINCESSLAMENT = 210,
	FEAT_DEMONHUNTER = 211,
	// Tiefling
	FEAT_DEMONHEARING = 224,
	FEAT_TAIL = 225,
	FEAT_DEMONVIAL = 226,
	FEAT_BORNFORMISCHIEF = 227,
	// Necromancer
	FEAT_RAISESKELETON = 240,
	FEAT_MINIONCONTROL = 241,
	FEAT_SKELLYDOMINION = 242,
	FEAT_UNDEADHUNTER = 243,
	// Princess
	FEAT_ANIMALCOMUNICATION = 256,
	FEAT_ROYALWEAPON = 257,
	FEAT_LOYALSQUIRE = 258,
	FEAT_LUXURIOUSGEM = 259,
	// Experiment X09
	FEAT_PSYCHIC = 272,
	// Jinx
	FEAT_JINXIMP = 288,
	FEAT_BANE = 289,
	FEAT_BANEFULHEALTH = 290,
	FEAT_JINXEDKNOWLEDGE = 291,
	// White Mage
	FEAT_GOREARTS = 304,
	FEAT_HEARTBEAT = 305,
	FEAT_HEALTHYINSIDES = 306,
	FEAT_DESPERATESTRIKES = 307
};
struct Technique {
	int id;
	int name;
	int description;
	// For when rolling and stuff (?
	EconomyType economy;
	DiceType roll;
	TechniqueType tech;
	AttributeType attr;		// Used for bonusses on the roll, save or difficulty
	// Costs, money and HP are the commonest
	int costMoney;
	int costHP;
	int costStress;
	// Value pool, could be used for damage or healing
	DiceType diceSide;
	int diceAmount;
	bool applyBonuses;
	int flatBonus;
	bool ignoreArmor;		// If the damage should ignore armor, or if healing should heal HP
	StatusType status;		// Apply status effect to apply
	bool targetEnemy[5];
	bool targetAlly[5];
	// Spawn entity?
	int spawnId;
	// Modify stats
	int physique;
	int reflex;
	int lore;
	int charisma;
	int rollBonus;
	float damageMultiplayer;
	float hurtMultiplayer;
	float healMultiplayer;
	// Graphical stuff?
	int spriteId;
	int animationId;
};
struct Weapon {
	EquipType type;
	// Important stuff, what really makes the weapon unique
	int name;
	int description;
	int cost;
	int attack;
	int tech;
	// Stats modifier
	int physique;
	int reflex;
	int lore;
	int charisma;
	int hurtMultiplayer;
	bool canUnequip;		// Useful for cursed weapons
	// Graphics
	int spriteId;
};
struct Armor {
	EquipType type;
	// Important stuff, what really makes the armor unique
	int name;
	int description;
	int cost;
	int armor;
	int tech;
	// Stats modifier
	int physique;
	int reflex;
	int lore;
	int charisma;
	int hurtMultiplayer;
	bool canUnequip;		// Useful for cursed armors
	// Graphics
	int spriteId;
};
struct Charm {
	EquipType type;
	// Important stuff, what really makes the charm unique
	int name;
	int description;
	int cost;
	int health;
	int armor;
	int stress;
	StatusType inmunity;		// To what status effect the charm grants inmunity
	int tech;
	// Stat modifier
	int physique;
	int reflex;
	int lore;
	int charisma;
	int hurtMultiplayer;
	bool canUnequip;		// Useful for cursed charms
	// Graphics
	int spriteId;
};
union Equip {
	Weapon weapon;
	Armor armor;
	Charm charm;
};
struct Player {
	EntityType type;
	int id;
	int position;
	// INFO: VITALS
	DiceType hitDice;
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	int demonTally;
	int accumulatedDamage;
	// INFO: ATTRIBUTES
	int physique[6];		// Physique + Athletics, Constitution, Medicine, Melee, Vibes
	int reflex[6];			// Reflex + Accuracy, Acrobatics, Mischief, Perception, Touch
	int lore[6];			// Lore + Arcanum, Beasts, Dream, Dungeons, Nature
	int charisma[6];		// Charisma + Anima, Authority, Drama, Kinship, Passion
	int phyBonus;
	int refBonus;
	int lorBonus;
	int chaBonus;
	int enemyBonus[8];
	// INFO: OTHER STUFF
	int name;
	int surname;
	int giftCurse;
	int nomGuerre;
	int description;
	Equip weapon;
	Equip armor;
	Equip charm;
	DamageType weakness;		// Weakness against certain damage types, x2 damage
	DamageType resistance;		// Resistance against certain damage types, x0.5 damage
	StatusType inmunity;		// Inmunity against status effects
	Feature features[16];
	int featuresAmount;
	Technique tech[20];
	int techAmount;
	Technique equipedTech[10];
	int equipedTechAmount;
	Sprite *sprite;
};
struct Enemy {
	EntityType type;
	int id;
	int position;
	int size;
	// INFO: VITALS
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	// INFO: ATTRIBUTES
	int physique;
	int reflex;
	int lore;
	int charisma;
	int phyBonus;
	int refBonus;
	int lorBonus;
	int chaBonus;
	// INFO: OTHER STUFF
	int name;
	int description;
	EnemyType enemy;
	DamageType weakness[2];		// Weakness against certain damage types, x2 damage
	DamageType resistances[2];	// Resistance against certain damage types, x0.5 damage
	StatusType inmunities[2];	// Inmunity against status effects
	Technique tech[10];
	int techAmount;
	int multiattack;
	Sprite *sprite;
};
struct Combat {
	void *enemy[5];
	void *player[5];
	Equip inventory[20];
	int inventoryAmount;
	int turn;
};

// Function Declaration

void LoadEnemiesFile(FILE **file, const char *enemySheet);
void LoadEnemiesOnCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, Combat *combat, int id);
void *LoadEnemy(FILE *enemyData, FILE *spriteData, FILE *techData, int id);
void *LoadPlayer(FILE *characterData, FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, int id, int position);
void RollInitiative(Combat *combat, int *randomValue);
void MoveEntity(Combat *combat, int position);
void DamageEntity(Combat *combat, void attacker, Technique *tech);
void KillEntity(Combat *combat, void *entity); //TODO
void UnloadCombat(Combat *combat);
void UnloadEntity(EntityType type, Combat *combat, int position);
void DrawCombat(Combat *combat, SafeTexture *textures, Color color, bool shader, bool draw);
// Techniques
Technique LoadTech(FILE *techData, int id);
void PlayerLoadTech(FILE *techData, void *player); //TODO
// Equipment
Equip LoadWeapon(FILE *weaponData, int id);
Equip LoadArmor(FILE *armorData, int id);
Equip LoadCharm(FILE *charmData, int id);
void SetProficiency(void *player, AttributeType attr);
void SetFeature(FILE *weaponData, FILE *charmData, FILE *tech, void *player, Feature feature);
// Dice related
int DiceMean(DiceType dice);
int DiceRoll(DiceType dice, int *randomValue);
void SetSeed(int seed, int *randomValue);
void SetTimeSeed(int *randomValue);
void Random(int *randomValue);

#endif		// ENTITY_H
