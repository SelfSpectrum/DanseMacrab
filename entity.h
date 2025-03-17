#ifndef ENTITY_H
#define ENTITY_H

#include "graphic.h"
#define FXSET_SIZE 8
#define ENEMYTECH_SIZE 10

typedef struct Player Player; // Contiene toda la información relevante para un personaje jugable
typedef struct Enemy Enemy; // Idem ac player, pero pa' los enemigos
typedef struct Equipment Equipment; // Estructura base para el equipo
typedef struct Weapon Weapon; // Armas
typedef struct Armor Armor; // Armaduras
typedef struct Charm Charm; // Accesorios

typedef struct Effect Effect; // Estructura base para los efectos
typedef struct FXSet FXSet; // Conjunto: Para agrupar múltiples efectos
typedef struct FXDmg FXDmg; // Daño: Para alterar las variables vitales de las entidades
typedef struct FXSpawn FXSpawn; // Generar: Para generar entidades en el campo
typedef struct FXStatus FXStatus; // Estados: Para aplicar efectos de estados a las entidades
typedef struct Technique Technique; // Técnicas, estas aplican estados durante el combate

typedef struct Combat Combat; // Contiene toda la información relevante para los combates
typedef enum EntityType EntityType; // Tipo de entidad, pueden haber jugadores o enemigos, pero esto podría cambiar en el futuro
typedef enum DamageType DamageType; // Tipo de daño, existe para añadirle más sabor a las armas dentro y fuera del combate
typedef enum MaterialType MaterialType; // Tipo de material, para reparar armaduras
typedef enum EnemyType EnemyType; // jiji, diferentes tipos de enemigos para darle más sabor al juego en general u know?
typedef enum EquipType EquipType;
typedef enum DiceType DiceType;
typedef enum TechniqueType TechniqueType;
typedef enum EffectType EffectType;
typedef enum AttributeType AttributeType;
typedef enum EconomyType EconomyType;
typedef enum StatusType StatusType;
typedef enum Feature Feature;

enum EntityType {
	ENTITY_NONE = 0,
	ENTITY_PLAYER = 1,
	ENTITY_ENEMY = 2,
};
enum DamageType {
	DMG_NONE = 0, // Ningún daño, no debería utilizarse, creo que voy a programar las cosas para que si tienen este daño, no hagan nada
	DMG_SLASHING = 1, // Daño cortante, ideal para espadas, cuchillos y garras
	DMG_BLUDGEONING = 2, // Daño contundente, ideal para martillos, puñetazos y placajes
	DMG_PIERCING = 3, // Daño perforante, ideal para lanzas, estoques y mordidas
	DMG_BALLISTIC = 4, // Daño balístico, ideal para pistolas, mosquetes y cañones
	DMG_FIRE = 5, // Daño de fuego, ideal para llamas, quemaduras e incendios
	DMG_ICE = 6, // Daño de hielo, ideal para congelamientos, maldiciones y frío extremo
	DMG_ELECTRIC = 7, // Daño de electricidad, ideal para rayos, centellas y descargas
	DMG_ACID = 8, // Daño de ácido, ideal para vómitos, gases y devoramientos
	DMG_PSYCHIC = 9, // Daño psíquico, ideal para quebrar voluntades, herir el alma y desgarrar mentes
	DMG_HEALNATURE = 10, // Curación natural, ideal para ungüentos, vahos y grageas
	DMG_HEALBLOODY = 11, // Curación sangrienta, ideal para curaciones relacionadas a las artes sangrientas
	DMG_HEALPSY = 12, // Curación psíquica, ideal para curaciones relacionadas a los poderes psiónicos
	DMG_HEALARMOR = 13, // Curación de armadura, ideal para pulidores de armaduras, servicios de reparación y herrería
	DMG_HEALMEDICINE = 14, // Curación médica, ideal para cirugías, primeros auxilios y medicamentos
	DMG_HEALLOVE = 15, // Curación amorosa, ideal para orgías, intimidad y vampirismo
	DMG_HEALREST = 16, // Curación de descanso, ideal para banquetes, acampar de manera exitosa y comida de calidad
	DMG_TRUE = 17 // Daño real, ignora todas las resistencias
};
enum EnemyType {
	ENEMY_NONE = 0, // Ninguno, por si acaso
	ENEMY_BEAST = 1, // Bestias: Criaturas de la naturaleza que intentan vivir sus vidas en esta nueva normalidad
	ENEMY_DEMON = 2, // Demonios: 
	ENEMY_DRAGON = 3, // Dragones:
	ENEMY_FASCIST = 4, // Fascistas:
	ENEMY_GRUDGE = 5, // Rencor: Espíritu que se niega a descansar y que habita ligado a algún objeto físico inanimado
	ENEMY_MUTANT = 6, // Mutante: Criaturas cuyos espírirus han sido profanados, mutando más allá de lo natural
	ENEMY_NIGHTMARE = 7, // Pesadilla:
	ENEMY_UNDEAD = 8 // No-Muerto:
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
	TECH_TOME = 3, // Magia escrita en libros
	TECH_SONG = 4, // Canciones o bailes
	TECH_DRACONIC = 5, // Palabras de dragones que cambian el mundo
	TECH_GOREART = 6, // Deja que tus entrañas pinten un hermoso cuadro
	TECH_DEMONHAND = 7, // Resulta que la lengua de señas sí era útil
	TECH_SUMMON = 8, // Llama a un ser paranormal para que te asista
	TECH_SORCERY = 9
};
enum EffectType { // Diferentes efectos para que las técnicas sean más completas
	EFFECT_NONE = 0, // Ninguno, ignorar
	EFFECT_SET = 1, // Conjunto, para crear técnicas complejas
	EFFECT_DMG = 2, // Daño, para disminuir o aumentar vitales
	EFFECT_STATUS = 3, // Estado, para aplicar buffs y debuffs
	EFFECT_SPAWN = 4, // Engendrar, para crear entidades en el campo de combate
};
enum AttributeType {	// ATTR % 6 for index
	ATTR_PHYSIQUE = 0, // Físico
	ATTR_ATHLETICS = 1, // Atletismo
	ATTR_CONSTITUTION = 2, // Constitución
	ATTR_MEDICINE = 3, // Medicina
	ATTR_MELEE = 4, // Combate cuerpo a cuerpo
	ATTR_VIBES = 5, // Vibras
	ATTR_REFLEX = 6, // Reflejos
	ATTR_ACCURACY = 7, // Precisión
	ATTR_ACROBATICS = 8, // Acrobacias
	ATTR_MISCHIEF = 9, // Travesuras
	ATTR_PERCEPTION = 10, // Percepción
	ATTR_TOUCH = 11, // Tacto
	ATTR_LORE = 12, // Conocimiento
	ATTR_ARCANUM = 13, // Arcano
	ATTR_BEASTS = 14, // Bestias
	ATTR_DREAMS = 15, // Sueños
	ATTR_DUNGEONS = 16, // Mazmorras
	ATTR_NATURE = 17, // Naturaleza
	ATTR_CHARISMA = 18, // Carisma
	ATTR_ANIMA = 19, // Alma
	ATTR_AUTHORITY = 20, // Autoridad
	ATTR_DRAMA = 21, // Drama
	ATTR_KINSHIP = 22, // Afinidad
	ATTR_PASSION = 23 // Pasión
};
enum StatusType {
	STATUS_NONE = 0, // Sin efecto
	STATUS_BLEED = 1, // Recibe daño cada vez que una entidad se mueve
	STATUS_BLINDED = 2, // Debe superar un 12 en reflejos antes de atacar o hacer pruebas
	STATUS_BRUISED = 3, // Recibe daño contundente adicional
	STATUS_BURNING = 4, // Recibe 1d10 de daño de fuego, puede destruir objetos
	STATUS_CONFUSED = 5, // Si un ataque no impacta, entonces cuenta como un fallo crítico
	STATUS_CURSED = 6, // No puede recibir sanación natural
	STATUS_DEAFENED = 7, // No se puede beneficiar de las canciones
	STATUS_DEATH = 8, // Muere luego de algunos turnos
	STATUS_DEVOURED = 9, // Recibe 1d10 de daño de ácido, puede dar estrés
	STATUS_DROWNING = 10, // Recibe 1d10 de daño de frío, puede dar estrés
	STATUS_GRAPPLED = 11, // No puede moverse o esquivar
	STATUS_HORRIFIED = 12, // No puede estar frente a un enemigo
	STATUS_INVISIBLE = 13, // Debe superar un 12 en una prueba de percepción antes de realizar tiradas o ataques
	STATUS_LINKED = 14, // Dos o más criaturas enlazadas, lo que le ocurra a una, le ocurre a todas
	STATUS_MOURNFUL = 15, // No puede hacer nada
	STATUS_OILED = 16, // Recibe daño de fuego adicional
	STATUS_PARALYZED = 17, // No se puede mover o tomar acciones
	STATUS_PASSENGER = 18, // Carga con una entidad
	STATUS_POISONED = 19, // Desventaja en todas las tiradas
	STATUS_POSSESSION = 20, // Otra criatura toma control sobre las acciones
	STATUS_PRONE = 21, // Los ataques cuerpo a cuerpo contra esta entidad son críticos automáticos
	STATUS_RAGE = 22, // Durante sus turnos, sólo puede utilizar ataques básicos
	STATUS_SCARRED = 23, // Recibe daño cortante adicional
	STATUS_SLEEPING = 24, // Los ataques le impactan de manera automática, no puede tomar acciones
	STATUS_SUFFOCATING = 25 // Recibe estrés
};
enum EconomyType {
	ECO_ACTION = 0,
	ECO_BONUS = 1,
	ECO_REACTION = 2,
	ECO_OFF = 3
};
enum Feature {
	// Berserker
	FEAT_SELFSTEEM = 0,
	FEAT_BEASTHORNS = 1,
	FEAT_EMOTIONSCENT = 2,
	FEAT_DARKVISION = 3,
	// Mago Azul
	FEAT_TOME = 16,
	FEAT_DECODER = 17,
	FEAT_DETECTMAGIC = 18,
	FEAT_PATHFINDER = 19,
	// Dragón
	FEAT_DRAGONSPEECH = 32,
	FEAT_DRACONIC = 33,
	FEAT_SNAKEBLOOD = 34,
	FEAT_DETECTSUPERNATURAL = 35,
	// Colmillo
	FEAT_STRESSRITUAL = 48,
	FEAT_STRESSMARKS = 49,
	FEAT_SCARRED = 50,
	FEAT_SCARSOFEXPERIENCE = 51,
	// Hijo de la Luna
	FEAT_LUCKY = 64,
	FEAT_FAVOROFLUCK = 65,
	FEAT_DETECTDANGER = 66,
	FEAT_SECRETDOOR = 67,
	// Mago Rojo
	FEAT_BLODDYLEARNING = 80,
	FEAT_LIPREADING = 81,
	FEAT_LIFEABSORTION = 82,
	FEAT_BLOODUNLEASHED = 83,
	// Doncella Songbird
	FEAT_SONGS = 96,
	FEAT_EARTHLYSONG = 97,
	FEAT_BLESS = 98,
	FEAT_FAREWELL = 99,
	// Hechicero
	FEAT_WISH = 112,
	// Caballero de Volfe
	FEAT_WOLFCOMPANION = 128,
	FEAT_WOLFSENSE = 129,
	FEAT_DREAMWALKER = 130,
	FEAT_RANGEATTACKER = 131,
	// Ocultista
	FEAT_ANIMALSHAPE = 144,
	FEAT_TRUENAME = 145,
	FEAT_ANIMALFEELING = 146,
	FEAT_DRUIDSPEECH = 147,
	// Perro Negro
	FEAT_BLACKDOG = 160,
	FEAT_UNDYING = 161,
	FEAT_SHADOWFREEDOM = 162,
	FEAT_SHADOWTELEPORT = 163,
	// Mago Azul
	FEAT_DEMONHANDS = 176,
	FEAT_LABERYNTHIASPEECH = 177,
	FEAT_FORKEDTONGE = 178,
	FEAT_PASSIONFUL = 179,
	// Familiar
	FEAT_FAMILIARBOND = 192,
	FEAT_HELPBOND = 193,
	FEAT_FREEFLY = 194,
	FEAT_BOOKOFOOL = 195,
	// Guardian del Clavo
	FEAT_PASSIONNAIL = 208,
	FEAT_SPECTRALHAND = 209,
	FEAT_PRINCESSLAMENT = 210,
	FEAT_DEMONHUNTER = 211,
	// Tiflin
	FEAT_DEMONHEARING = 224,
	FEAT_TAIL = 225,
	FEAT_DEMONVIAL = 226,
	FEAT_BORNFORMISCHIEF = 227,
	// Nigromante
	FEAT_RAISESKELETON = 240,
	FEAT_MINIONCONTROL = 241,
	FEAT_SKELLYDOMINION = 242,
	FEAT_UNDEADHUNTER = 243,
	// Princesa
	FEAT_ANIMALCOMUNICATION = 256,
	FEAT_ROYALWEAPON = 257,
	FEAT_LOYALSQUIRE = 258,
	FEAT_LUXURIOUSGEM = 259,
	// Experimento X09
	FEAT_PSYCHIC = 272,
	// Gafe
	FEAT_JINXIMP = 288,
	FEAT_BANE = 289,
	FEAT_BANEFULHEALTH = 290,
	FEAT_JINXEDKNOWLEDGE = 291,
	// Mago Blanco
	FEAT_GOREARTS = 304,
	FEAT_HEARTBEAT = 305,
	FEAT_HEALTHYINSIDES = 306,
	FEAT_DESPERATESTRIKES = 307
};
struct Effect {
	EffectType type;
	int id;
};
struct FXSet {
	Effect base;
	void *fxs[FXSET_SIZE];
};
struct FXDmg {
	Effect base;
	DamageType type; // Esto puede determinar el elemento del daño o la naturaleza de la curación
	DiceType diceSide; // Cantidad de caras que puede tener el dado con el que se aplique el daño o sanación (D4, D6, D8, D10, D12, D20, D100)
	int diceAmount; // Incrementa el número de tiradas que se realiza con los dados previamente mencionados
	bool applyBonuses; // Si es que la técnica puede recibir bonos de atributos o bonos planos
	int flatBonus; // Un bono plano al ataque o sanación
	bool ignoreArmor; // Si es que el daño debe ignorar armadura, o si la sanación debe sanar PS
	float healAmount;
	int animId;
};
struct FXSpawn {
	Effect base;
	int entity;
	int amount;
	int animId;
};
struct FXStatus {
	Effect base;
	StatusType status;
	int modPhy;
	int modRef;
	int modLor;
	int modCha;
	int modRoll;
	float multDmg;
	float multHurt;
	float multHeal;
	int duration;
	int animId;
};
struct Technique {
	int id;
	int name; // Nombre de la técnica (es un id para cargar de la BD)
	int description; // Descripción (idem ac <<name>>)
	// Para cuando se deben hacer tiradas y cosas (?
	EconomyType economy;
	DiceType roll; // Para saber si esto será una salvación o una tirada de ataque (o algo más exótico)
	TechniqueType tech;
	AttributeType attr; // Utilizado para aplicar bonos a la tirada, salvada o dificultad
	// Costes. Dinero y PS son lo más común
	int costMoney;
	int costHP;
	int costStress;
	// Montón de valores, utilizado para realizar curaciones o aplicar daños
	int targetEnemy[5];
	int targetAlly[5];
	// ¿Cosas gráficas?
	int spriteId;
};
struct Equipment {
	EquipType type;
	int name;
	int description;
	int cost;
	int tech; // ID de la técnica que el equipamiento otorga a un personaje al tenerlo equipado
	// Modificadores de estadísticas
	int physique;
	int reflex;
	int lore;
	int charisma;
	int hurtMultiplayer;
	bool canUnequip; // Util para equipo maldito
	// Graphics
	int spriteId;
};
struct Weapon { // Lo ideal es que todas las armas tengan una técnica de ataque, pero encima pueden tener una técnica extra que sirva para tener más opciones
	Equipment base;
	// Cosas importantes, lo que realmente hace que un arma sea única
	int attack; // ID de la técnica asociada al ataque común del arma
	int attackAlt; // ID de la técnica alternativa asociada al ataque común del arma
};
struct Armor {
	Equipment base;
	// Cosas importantes, lo que realmente hace que una armadura sea única
	MaterialType material; // Material de la armadura, necesario para poder repararla
	int maxArmor;
	int armor;
};
struct Charm {
	Equipment base;
	// Cosas importantes, lo que realmente hace que un accesorio sea único
	int health;
	int armor;
	int stress;
	StatusType inmunity; // A qué efectos de estado el amuleto otorga inmunidad
};
struct Player {
	int id;
	// INFO: VITALES
	DiceType hitDice;
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	int demonTally;
	int accumulatedDamage;
	// INFO: ATRIBUTOS
	int physique[6]; // Physique + Athletics, Constitution, Medicine, Melee, Vibes
	int reflex[6]; // Reflex + Accuracy, Acrobatics, Mischief, Perception, Touch
	int lore[6]; // Lore + Arcanum, Beasts, Dream, Dungeons, Nature
	int charisma[6]; // Charisma + Anima, Authority, Drama, Kinship, Passion
	int phyBonus;
	int refBonus;
	int lorBonus;
	int chaBonus;
	int enemyBonus[8];
	// INFO: OTRAS COSAS
	int name;
	int surname;
	int giftCurse;
	int nomGuerre;
	int description;
	Equip *weapon[3];
	Equip *armor;
	Equip *charm[2];
	DamageType weakness; // Debilidad ante ciertos tipos de daño, x2 de daño
	DamageType resistance; // Resistencia ante ciertos tipos de daño, x0.5 de daño
	StatusType inmunity; // Inmunidades ante efectos de estado
	Feature features[16];
	int featuresAmount;
	Technique *tech[10];
	int techAmount;
	Technique *skill[10];
	int skillAmount;
	Sprite *sprite;
	// INFO: ESPECIFICO DEL COMBATE
	int position;
	int initiative;
	bool major;
	bool minor;
	bool reaction;
	bool movement;
};
struct Enemy {
	int id;
	int size;
	// INFO: VITALES
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	// INFO: ATRIBUTOS
	int physique;
	int reflex;
	int lore;
	int charisma;
	int phyBonus;
	int refBonus;
	int lorBonus;
	int chaBonus;
	// INFO: OTRAS COSAS
	int name;
	int description;
	EnemyType enemy;
	DamageType weakness[2]; // Debilidad ante ciertos tipos de daño, x2 de daño
	DamageType resistances[2]; // Resistencia ante ciertos tipos de daño, x0.5 de daño
	StatusType inmunities[2]; // Inmunidades ante efectos de estado
	Technique *tech[ENEMYTECH_SIZE];
	int techAmount;
	int multiattack;
	Sprite *sprite;
	// INFO: ESPECIFICO DEL COMBATE
	int position;
	int initiative;
};
struct Combat {
	void *enemy[5];
	void *player[5];
	void *timeline[10];
	EntityType timelineType[10];
	int timelineAmount;
	int timelineOrder;
	Equip inventory[20];
	int inventoryAmount;
	int turn;
};

// Function Declaration

void LoadEnemiesFile(FILE **file, const char *enemySheet);
void LoadEnemiesOnCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, Combat *combat, int id);
void *LoadEnemy(FILE *enemyData, FILE *spriteData, FILE *techData, int id, int position);
void *ParseEnemy(FILE *spriteData, FILE *techData, char *line, int position);
void *LoadPlayer(FILE *characterData, FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, int id, int position);
void *ParsePlayer(FILE *spriteData, FILE *weaponData, FILE *armorData, FILE *charmData, FILE *techData, char *line, int position);
void UnloadEntity(EntityType type, Combat *combat, int position);
// Techniques
Technique *LoadTech(FILE *techData, int id);
Technique *ParseTech(char *line);
void PlayerLoadTech(FILE *techData, Player *player); //TODO
void UseTech(Combat *combat, void *entity, EntityType side, Technique *tech, int *randomValue);
Effect LoadEffect();
void *LoadEffectSet();
void *LoadEffectDamage();
void *LoadEffectStatus();
void *LoadEffectSpawn();

// Equipment
Weapon *LoadWeapon(FILE *weaponData, int id);
Weapon *ParseWeapon(char *line);
Armor *LoadArmor(FILE *armorData, int id);
Armor *ParseArmor(char *line);
Charm *LoadCharm(FILE *charmData, int id);
Charm *ParseCharm(char *line);
void PlayerEquip(Player *player, void *equipment);
void SetProficiency(void *player, AttributeType attr);
void SetFeature(FILE *weaponData, FILE *charmData, FILE *tech, void *player, Feature feature);
// Dice related
int DiceMean(DiceType dice);
int DiceRoll(DiceType dice, int *randomValue);
void SetSeed(int seed, int *randomValue);
void SetTimeSeed(int *randomValue);
void Random(int *randomValue);

// Relacionadas al combate
void RollInitiative(Combat *combat, int *randomValue);
void SortInitiative(void **timeline, EntityType *types, int timelineAmount);
int GetInitiative(void *entity, EntityType type);
void UnloadCombat(Combat *combat);
void DrawCombat(Combat *combat, SafeTexture *textures, Color color, bool shader, bool draw);
void StartCombat(FILE *file, FILE *enemyData, FILE *spriteData, FILE *techData, int id, Combat *combat, int *randomValue);
void MoveEntity(Combat *combat, int position);
void DamageEntity(void *entity, EntityType type, int damage, DamageType damageType);
void KillEntity(Combat *combat, void *entity, EntityType type);
void StartTurn(Combat *combat);
void EndTurn(Combat *combat);

#endif		// ENTITY_H
