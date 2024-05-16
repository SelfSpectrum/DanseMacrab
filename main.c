#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#define ANIM_SIZE 16
#define TEX_SIZE 8
#define SOUND_SIZE 8
#define SFXALIAS_SIZE 8
#define DRAW_SIZE 16
#define BUTTON_SIZE 8
#define STR_SIZE 8

// INFO: Let's set some structs to work with along the gaem
//------------------------------------------------------------------------------------

typedef struct Player Player;
typedef struct Enemy Enemy;
typedef struct Weapon Weapon;
typedef struct Armor Armor;
typedef struct Charm Charm;
typedef struct Technique Technique;
typedef struct Sprite Sprite;
typedef struct Animable Animable;
typedef struct Combat Combat;
typedef struct Button Button;
typedef union Entity Entity;
typedef union Equip Equip;
typedef enum GameState GameState;
typedef enum DamageType DamageType;
typedef enum EntityType EntityType;
typedef enum EnemyType EnemyType;
typedef enum EquipType EquipType;
typedef enum DiceType DiceType;
typedef enum TechniqueType TechniqueType;
typedef enum AttributeType AttributeType;
typedef enum EconomyType EconomyType;
typedef enum StatusType StatusType;
typedef enum Language Language;
typedef enum Feature Feature;

struct Sprite {
	int textureIndex;
	Rectangle origin;
	Rectangle dest;
	Vector2 position;
	float rotation;
	bool shader;
};
struct Button {
	int textureIndex;
	Rectangle originOff;
	Rectangle originOn;
	Rectangle dest;
	Vector2 position;
	float rotation;
	bool shader;
	bool selected;
};
struct Animable {
	int frame;       // Frame needed to change to the next event
	int currentFrame;// Current frame of animation
	int index;
	FILE *data;               // File that contains the animation data
	//Texture2D texture;      // INFO: Deprecated: Texture from where the sample will come
	int textureIndex;         // Index in the Texture array from where the sample will come
	Quaternion origin;        // Rectangle of origin to get the texture sample
	Quaternion dest;          // Rectangle of destination to place the texture sample
	Vector2 position;         // Position of the destination???
	float rotation;           // Rotation of the texture sample
	Quaternion deltaOrigin;
	Quaternion deltaDest;
	Vector2 deltaPos;
	float deltaRotation;
	Vector2 offset;
	bool shader;              // Draw inside shader mode?
	bool repeat;              // Upon finishing, rewind animation?
};
enum GameState {
	STATE_TITLE,
	STATE_MAINMENU,
	STATE_SELECTCARDS,
	STATE_FIGHT,
	STATE_ATTACKMENU,
	STATE_DEBUG
};
enum DamageType {
	DMG_NONE = 0,
	DMG_SLASHING = 1,
	DMG_BLUDGEONING = 2,
	DMG_PIERCING = 3,
	DMG_BALLISTIC = 4,
	DMG_FIRE = 5,
	DMG_ICE = 6,
	DMG_ACID = 7,
	DMG_PSYCHIC = 8,
	DMG_HEALNATURE = 9,
	DMG_HEALBLOODY = 10,
	DMG_HEALPSY = 11,
	DMG_HEALARMOR = 12,
	DMG_TRUE = 13
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
	TECH_SUMMON = 8
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
	STATUS_NONE = 0,		// No effect
	STATUS_BLEED = 1,		// Take damage everytime the entity moves
	STATUS_BLINDED = 2,		// Must succeed on
	STATUS_BURNING = 3,		// 1d10 fire damage, might destroy items
	STATUS_CONFUSED = 4,		// If a tech don't hit, counts as critical fumble
	STATUS_DEAFENED = 5,		// Can't communicate
	STATUS_DEATH = 6,		// Die after some turns
	STATUS_DEVOURED = 7,		// 1d10 acid damage, deal stress
	STATUS_DROWNING = 8,		// Deal stress
	STATUS_GRAPPLED = 9,		// Can't move or dodge
	STATUS_HORRIFIED = 10,		// Can't be in front of an enemy
	STATUS_INVISIBLE = 11,		// Must succeed a perception check before roll or attack
	STATUS_LINKED = 12,		// Two or more creatures link, what happens to one, happens to all
	STATUS_MOURNFUL = 13,		// Can't do anything
	STATUS_PARALYZED = 14,		// Can't move or take actions
	STATUS_PASSANGER = 15,		// Carry an entity
	STATUS_POISONED = 16,		// Disadvantage on all rolls
	STATUS_POSSESSION = 17,		// Another creature controls actions
	STATUS_PRONE = 18,		// Melee are crits
	STATUS_RAGE = 19,		// Attack the closest creature
	STATUS_SLEEPING = 20,		// Attacks autohit
	STATUS_SUFFOCATING = 21		// Deal stress
};
enum EconomyType {
	ECO_ACTION = 0,
	ECO_BONUS = 1,
	ECO_REACTION = 2,
	ECO_OFF = 3
};
enum Language {
	LANG_SPANISH = 0,
	LANG_ENGLISH = 1,
	LANG_RUSSIAN = 2
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
	int class;
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
union Entity {
	Enemy enemy;
	Player player;
};
struct Combat {
	Entity *enemy[5];
	Entity *player[5];
	Equip inventory[20];
	int inventoryAmount;
	int turn;
};
// INFO: GFX functions
Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable *anim);
void DrawAnimable(Animable *anim, Shader shader);
void UnloadAnimable(Animable *anim);
// Animations
void LoadAnimation(int id, Vector2 offset);
void UnloadAnimation(void);
// Sprite work
void LoadSprite(const char *spriteSheet);
Sprite *LoadSingleSprite(int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Shader shader);
void UnloadSprite(void);
void UnloadSingleSprite(Sprite **sprite);
// Buttons stuff
void LoadButton(const char *buttonSheet);
Button *ParseButton(char *line);
void DrawButton(Shader shader);
void UnloadButton(void);
// Text and translations and stuff
char *LoadText(int id);			// To load a text line with the corresponding translation
char *LoadTextFormatChar(int id, char value);
char *LoadTextFormatInt(int id, int value);
void UnloadText();						//TODO
// INFO: Input functions
void Accept(void);
void Cancel(void);
void Start(void);
void Select(void);
void Menu(void);
void ChangeSelection(void);
void SetState(GameState newState);
// INFO: SFX functions
void PlaySecSound(int id);
void LowPassFilter(void *buffer, unsigned int frames);		// TODO
void Crossfade();						// TODO
// INFO: Entities functions
void LoadEnemiesFile(FILE **file, const char *enemySheet);
void LoadEnemiesOnCombat(FILE *file, int id);
Entity *LoadEnemy(int id);
Entity *LoadPlayer(int id);
void MoveEntity(Entity *entity, int position);
void DamageEntity(Entity attacker, Technique tech);
void KillEntity(Entity *entity);				//TODO
void UnloadCombat(void);
void UnloadEntity(EntityType type, int position);
void DrawCombat(void);
// Techniques
Technique LoadTech(int id);
void PlayerLoadTech(Entity *player);				//TODO
// Equipment
Equip LoadWeapon(int id);
Equip LoadArmor(int id);
Equip LoadCharm(int id);
void SetProficiency(Entity *player, AttributeType attr);
void SetFeature(Entity *player, Feature feature);
// Dice related
int DiceMean(DiceType dice);
int DiceRoll(DiceType dice);
void SetSeed(int seed);
void SetTimeSeed(void);
void Random(void);
//------------------------------------------------------------------------------------
// INFO: Program main entry point
//------------------------------------------------------------------------------------
GameState state = STATE_TITLE;					// INFO: Contains the current state of the game
Animable *anims[ANIM_SIZE] = { NULL };				// INFO: Animation handling and rendering
Texture2D textures[TEX_SIZE];					// INFO: Here I hold all the texture used in the game
Sound sounds[SOUND_SIZE];					// INFO: Here I hold all the sounds used in the game
Sound sfxAlias[SFXALIAS_SIZE];					// INFO: Used to reproduce several sounds at once
int sfxPos = 0;							// INFO: Universal position to locate one "free" sfxAlias
Sprite *sprites[DRAW_SIZE] = { NULL };				// INFO: What and where to render
int spritePos = 0;
Color globalColor = { 255, 255, 255, 255 };			// INFO: Global color used to render the white lines in all textures as colors
Combat combat = { { 0 }, { 0 } };				// INFO: Data from position, entities and stuff for combat
// ----------------------------------------------------------------------------------------
// INFO: Files to load global data
// ----------------------------------------------------------------------------------------
FILE *animsData;						// INFO: Big file with every single independent animation data
FILE *spriteData;						// INFO: Big file with the sprites that will load individually
FILE *charmData;
FILE *armorData;
FILE *weaponData;
FILE *techData;
FILE *characterData;
FILE *enemyData;						// INFO: Big file with every single enemy
FILE *dialogData;
FILE *translationData;
// ----------------------------------------------------------------------------------------
// INFO: Variables for button work, they're a lot
// ----------------------------------------------------------------------------------------
Button *buttons[BUTTON_SIZE] = { NULL };			// INFO: Set of buttons. Ideal to load per state
int buttonAmount = 0;						// INFO: Useful when needed to wrap around the buttons
int buttonPosition = 0;
int buttonSkip = 0;
int startKey = KEY_ENTER;
int selectKey = KEY_BACKSPACE;
int acceptKey = KEY_Z;
int cancelKey = KEY_X;
int leftKey = KEY_LEFT;
int rightKey = KEY_RIGHT;
int upKey = KEY_UP;
int downKey = KEY_DOWN;
int extraAKey = KEY_A;
int extraBKey = KEY_B;
int randomValue;

int main() {
	// Initialization
	//--------------------------------------------------------------------------------------
	const int virtualScreenWidth = 320;
	const int virtualScreenHeight = 180;
	const int screenWidth = 960;
	const int screenHeight = 540;

	const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

	InitWindow(screenWidth, screenHeight, "Danse Macrab");
	InitAudioDevice();

	SetExitKey(KEY_NULL);       // INFO: Disable KEY_ESCAPE to close window, X-button still works
	bool exitWindowRequested = false;   // Flag to request window to exit
	bool exitWindow = false;    // Flag to set window to exit
	
	//bool runGame = true;		// To know if the game should run, useful if the loading of a file fails

	Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
	Rectangle destRec = { -virtualRatio, -virtualRatio , screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
	Vector2 origin = { 0.0f, 0.0f };

	animsData = fopen("./resources/anims/animations.tsv", "r");
	spriteData = fopen("./resources/gfx/sprites.tsv", "r");
	charmData = fopen("./resources/combat/charms.tsv", "r");
	armorData = fopen("./resources/combat/armors.tsv", "r");
	weaponData = fopen("./resources/combat/weapons.tsv", "r");
	techData = fopen("./resources/combat/tech.tsv", "r");
	characterData = fopen("./resources/combat/characters.tsv", "r");
	enemyData = fopen("./resources/combat/enemies.tsv", "r");
	dialogData = fopen("./resources/text/dialog.tsv", "r");
	translationData = fopen("./resources/text/english.tsv", "r");

	textures[0] = LoadTexture("./resources/gfx/bigSprites00.png");
	textures[3] = LoadTexture("./resources/gfx/cards.png");
	textures[4] = LoadTexture("./resources/gfx/UI.png");
	textures[5] = LoadTexture("./resources/gfx/abilities.png");
	textures[6] = LoadTexture("./resources/gfx/attacks.png");
	textures[7] = LoadTexture("./resources/gfx/entities.png");

	Shader shader = LoadShader(0, "contour.fs");

	Music music = LoadMusicStream("./resources/sfx/title.mp3");
	music.looping = true;
	sounds[0] = LoadSound("./resources/sfx/pressStart.mp3");
	sounds[1] = LoadSound("./resources/sfx/buttonSelect.wav");
	sounds[2] = LoadSound("./resources/sfx/buttonCancel.wav");
	sounds[3] = LoadSound("./resources/sfx/error.wav");
	PlayMusicStream(music);
	SetMusicVolume(music, 1.0f);

	int animCount;
	int texCount;
	int sfxCount;

	SetState(STATE_TITLE);
	
	//combat.playable[2] = LoadEntity("", ENTITY_PLAYER); TODO

	SetTargetFPS(60);           // INFO: Set our game to run at 60 frames-per-second

	// INFO: Main game loop
	//--------------------------------------------------------------------------------------
	while (!exitWindow) {
// INFO: Update: This is for calculations and events which do not affect Texture or Drawing in a direct way
//----------------------------------------------------------------------------------
		UpdateMusicStream(music);	// Update music buffer with new stream data
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;	// Detect if X-button or KEY_ESCAPE have been pressed to close window
		if (exitWindowRequested) {
			// A request for close window has been issued, we can save data before closing or just show a message asking for confirmation
			if (IsKeyPressed(acceptKey) || IsKeyPressed(startKey)) exitWindow = true;
			else if (IsKeyPressed(cancelKey)) exitWindowRequested = false;
		}
		else {
			//UpdateAnimable(test, shader);
			for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
				if (anims[animCount] != NULL) UpdateAnimable(anims[animCount]);
			}
			for (texCount = 0; texCount < TEX_SIZE; texCount++) {
				SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), textures[texCount]);
			}
			if (IsKeyPressed(startKey)) Start();
			if (IsKeyPressed(acceptKey)) Accept();
			if (IsKeyPressed(cancelKey)) Cancel();
			Menu();
		}
// INFO: Texture: In this texture mode I create an smaller version of the game which is later rescaled in the draw mode
//----------------------------------------------------------------------------------
		BeginTextureMode(target);
			ClearBackground(BLACK);
			BeginMode2D(worldSpaceCamera);
				if (exitWindowRequested) {
					DrawText("Are you sure you want to exit program?", 50, 90, 8, globalColor);
				}
				else {
					for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
						if (anims[animCount] != NULL) DrawAnimable(anims[animCount], shader);
					}
					DrawSprite(shader);
					DrawButton(shader);
					BeginShaderMode(shader);
						DrawTexturePro(textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { 0, 0 }, 0, globalColor);
						DrawTexturePro(textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -64, 0 }, 0, globalColor);
						DrawTexturePro(textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -128, 0 }, 0, globalColor);
						DrawTexturePro(textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -196, 0 }, 0, globalColor);
						DrawTexturePro(textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -256, 0 }, 0, globalColor);
						DrawTexturePro(textures[5], (Rectangle) { 0, 0, 16, 16 }, (Rectangle) { 0, 0, 16, 16 }, (Vector2) { 0, -16 }, 0, globalColor);
					EndShaderMode();
				}
			EndMode2D();
		EndTextureMode();
// INFO: Draw: Take the texture in lower resolution and rescale it to a bigger res, all this while preserving pixel perfect
//----------------------------------------------------------------------------------
		BeginDrawing();
			ClearBackground(RED);
			BeginMode2D(screenSpaceCamera);
				DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
			EndMode2D();
			DrawFPS(10, 10);
		EndDrawing();
	}
// INFO: De-Initialization
//--------------------------------------------------------------------------------------
	StopMusicStream(music);
	UnloadShader(shader);
	UnloadRenderTexture(target);
	UnloadSprite();
	UnloadButton();
	UnloadCombat();
	UnloadAnimation();
	UnloadMusicStream(music);   // Unload music stream buffers from RAM

	for (texCount = 0; texCount < TEX_SIZE; texCount++) UnloadTexture(textures[texCount]);
	for (sfxCount = 0; sfxCount < SFXALIAS_SIZE; sfxCount++) UnloadSoundAlias(sfxAlias[sfxCount]);
	for (sfxCount = 0; sfxCount < SOUND_SIZE; sfxCount++) UnloadSound(sounds[sfxCount]);

	if (animsData != NULL) fclose(animsData);
	if (spriteData != NULL) fclose(spriteData);
	if (charmData != NULL) fclose(charmData);
	if (armorData != NULL) fclose(armorData);
	if (weaponData != NULL) fclose(weaponData);
	if (techData != NULL) fclose(techData);
	if (characterData != NULL) fclose(characterData);
	if (enemyData != NULL) fclose(enemyData);
	if (dialogData != NULL) fclose(dialogData);
	if (translationData != NULL) fclose(translationData);

	CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
	CloseWindow();              // Close window and OpenGL context

	return 0;
}

Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset) {
	Animable *anim = (Animable *) malloc(sizeof(Animable));		// Dynamic allocation since many animables might be created and destroyed in quick successions, don't forget to free later
	if (anim != NULL) {

		if (FileExists(animSheet)) {				// If the file has problems to open, lack of memory won't be a problem, I hope
			char line[256];					// Line from the file that contains all the struct data
			FILE *file = fopen(animSheet, "r");
			if (fgets(line, sizeof(line), file) != NULL) {
				ParseAnimable(line, anim);
				printf("INFO: ANIMABLE: Animable loaded succesfully\n");
				anim->currentFrame = 0;
				anim->data = file;
				anim->repeat = repeat;
				anim->index = index;
				anim->offset = offset;
			}
		}
		else printf("INFO: ANIMABLE: Error opening the animation file %s!\n", animSheet);
	}
	return anim;
}
void ParseAnimable(char *line, Animable *anim) {
	char *token;
	char *saveptr;
	//printf("INFO: ANIMABLE: Parsing line \"%s\".\n", line);
	token = strtok_r(line, "	", &saveptr);
	anim->frame = atoi(token);
	//printf("INFO: ANIMABLE: Frame %d parsed successfuly.\n", anim->frame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->textureIndex = atoi(token);
	//printf("INFO: ANIMABLE: Texture Index %d parsed successfuly.\n", anim->textureIndex);
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.w = atof(token);
	//printf("INFO: ANIMABLE: Origin W parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.x = atof(token);
	//printf("INFO: ANIMABLE: Origin X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.y = atof(token);
	//printf("INFO: ANIMABLE: Origin Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.z = atof(token);
	//printf("INFO: ANIMABLE: Origin Z parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.w = atof(token);
	//printf("INFO: ANIMABLE: Dest W parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.x = atof(token);
	//printf("INFO: ANIMABLE: Dest X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.y = atof(token);
	//printf("INFO: ANIMABLE: Dest Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.z = atof(token);
	//printf("INFO: ANIMABLE: Dest Z parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->position.x = atof(token);
	//printf("INFO: ANIMABLE: Pos X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->position.y = atof(token);
	//printf("INFO: ANIMABLE: Pos Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->rotation = atof(token);
	//printf("INFO: ANIMABLE: Rotation parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.w = atof(token);
	//printf("INFO: ANIMABLE: Delta Origin W parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.x = atof(token);
	//printf("INFO: ANIMABLE: Delta Origin X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.y = atof(token);
	//printf("INFO: ANIMABLE: Delta Origin Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.z = atof(token);
	//printf("INFO: ANIMABLE: Delta Origin Z parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.w = atof(token);
	//printf("INFO: ANIMABLE: Delta Dest W parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.x = atof(token);
	//printf("INFO: ANIMABLE: Delta Dest X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.y = atof(token);
	//printf("INFO: ANIMABLE: Delta Dest Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.z = atof(token);
	//printf("INFO: ANIMABLE: Delta Dest Z parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.x = atof(token);
	//printf("INFO: ANIMABLE: Delta Pos X parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.y = atof(token);
	//printf("INFO: ANIMABLE: Delta Pos Y parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaRotation = atof(token);
	//printf("INFO: ANIMABLE: Delta Rotation parsed successfuly.\n");
	token = strtok_r(NULL, "	", &saveptr);
	anim->shader = (bool) atoi(token);
	//printf("INFO: ANIMABLE: Shader use parsed successfuly.\n");
}
void UpdateAnimable(Animable *anim) {
	if (anim != NULL) {
		char line[256];
		anim->origin = QuaternionAdd(anim->origin, anim->deltaOrigin);
		anim->dest = QuaternionAdd(anim->dest, anim->deltaDest);
		anim->position = Vector2Add(anim->position, anim->deltaPos);
		anim->rotation += anim->deltaRotation;
		anim->currentFrame++;
		if (anim->currentFrame >= anim->frame) {
			if (anim->frame != 0) {
				if (fgets(line, sizeof(line), anim->data) != NULL) {
					//printf("INFO: ANIMABLE: Next animable line loaded.\n");
					ParseAnimable(line, anim);
					//printf("INFO: ANIMABLE: Next animable line parsed.\n");
				}
			}
			else if (anim->repeat) {
				anim->currentFrame = 0;
				rewind(anim->data);
				if (fgets(line, sizeof(line), anim->data) != NULL) ParseAnimable(line, anim);
			}
			else UnloadAnimable(anim);
		}
	}
}
void DrawAnimable(Animable *anim, Shader shader) {
	if (anim != NULL) {
		//printf("%u\n", anim->currentFrame);   // TODO: A good way of view the frame count as debug inside game
		if (anim->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[anim->textureIndex],
				(Rectangle) { anim->origin.w, anim->origin.x, anim->origin.y, anim->origin.z },
				(Rectangle) { anim->dest.w, anim->dest.x, anim->dest.y, anim->dest.z },
				Vector2Add(anim->position, anim->offset),
				anim->rotation,
				globalColor);
		if (anim->shader) EndShaderMode();
	}
}
void UnloadAnimable(Animable *anim) {
	if (anim != NULL) {
		anims[anim->index] = NULL;
		fclose(anim->data);
		// UnloadTexture(anim->texture);
		free(anim);
		printf("INFO: ANIMABLE: Animable unloaded succesfully\n");
	}
}
void LoadAnimation(int id, Vector2 offset) {
	if (animsData == NULL) {
		printf("ERROR: ANIMATION: Error opening animation file\n");
		return;
	}
	int i = 1;
	int j;
	char line[256];
	char *token;
	char *saveptr;
	bool repeat;
	rewind(animsData);
	if (fgets(line, sizeof(line), animsData) == NULL) return;
	while (fgets(line, sizeof(line), animsData) != NULL) {
		if (i == id) {
			token = strtok_r(line, "	", &saveptr);
			printf("INFO: ANIMATION: Loading %s in the %d register\n", token, i);
			while (token != NULL) {
				for (j = 0; j < ANIM_SIZE; j++) {
					if (anims[j] == NULL) {
						token = strtok_r(NULL, "	", &saveptr);
						printf("INFO: ANIMATION: Direction %s\n", token);
						repeat = (bool) atoi(strtok_r(NULL, "	", &saveptr));
						printf("INFO: ANIMATION: Repeat %d\n", repeat);
						anims[j] = LoadAnimable(token, repeat, j, offset);
						token = strtok_r(NULL, "	", &saveptr);
						break;
					}
				}
			}
			break;
		}
		i++;
	}
}
void UnloadAnimation(void) {
	int i;
	for (i = 0; i < ANIM_SIZE; i++) {
		if (anims[i] != NULL) {
			UnloadAnimable(anims[i]);
		}
	}
	printf("INFO: ANIMATION: Animable array data unloaded.\n");
}
void LoadSprite(const char *spriteSheet) {
	if (FileExists(spriteSheet)) {
		FILE *file = fopen(spriteSheet, "r");
		char line[256];
		while (fgets(line, sizeof(line), file) != NULL) {
			if (spritePos >= DRAW_SIZE) {
				printf("WARNING: SPRITE: Sprites register full.\n");
				break;
			}
			sprites[spritePos] = ParseSprite(line);
			spritePos++;
		}
		printf("INFO: SPRITE: Sprites loaded from \"%s\" correctly.\n", spriteSheet);
		fclose(file);
	}
	else printf("INFO: SPRITE: Sprite file \"%s\" not available.\n", spriteSheet);
}
Sprite *LoadSingleSprite(int id) {
	rewind(spriteData);
	if (spriteData != NULL) {
		char line[256];
		char *token;
		char *saveptr;
		int spriteId = 0;
		if (fgets(line, sizeof(line), spriteData) == NULL) return NULL;
		while (fgets(line, sizeof(line), spriteData) != NULL) {
			token = strtok_r(line, "	", &saveptr);
			spriteId = atoi(token);
			if (spriteId == id) {
				token = strtok_r(line, "	", &saveptr); //To delete the sprite name
				printf("INFO: SPRITE: Parsing %s\n", token);
				return ParseSprite(line);
			}
		}
	}
	return NULL;
}
Sprite *ParseSprite(char *line) {
	Sprite *sprite = (Sprite *) malloc(sizeof(Sprite));
	char *token;
	char *saveptr;
	if (sprite != NULL) {
		token = strtok_r(line, "	", &saveptr);
		sprite->textureIndex = atoi(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->position.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->position.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->rotation = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->shader = (bool) atoi(token);
	}
	return sprite;
}
void DrawSprite(Shader shader) {
	int i;
	for (i = 0; i <= spritePos; i++) {
		if (sprites[i] != NULL  ) {
			if (sprites[i]->shader) BeginShaderMode(shader);
			DrawTexturePro(textures[sprites[i]->textureIndex],
					sprites[i]->origin,
					sprites[i]->dest,
					sprites[i]->position,
					sprites[i]->rotation,
					globalColor);
			if (sprites[i]->shader) EndShaderMode();
		}
	}
}
void UnloadSprite(void) {
	int i;
	for (i = 0; i <= spritePos; i++) {
		if (sprites[i] != NULL) {
			free(sprites[i]);
			sprites[i] = NULL;
		}
	}
	spritePos = 0;
}
void UnloadSingleSprite(Sprite **sprite) {
	free(*sprite);
	*sprite = NULL;
}
void LoadButton(const char *buttonSheet) {
	if (FileExists(buttonSheet)) {
		FILE *file = fopen(buttonSheet, "r");
		char line[256];
		int i;
		while (fgets(line, sizeof(line), file) != NULL) {
			for (i = 0; i < BUTTON_SIZE; i++) {
				if (buttons[i] == NULL) {
					buttons[i] = ParseButton(line);
					buttonAmount++;
					break;
				}
			}
		}
		fclose(file);
	}
}
Button *ParseButton(char *line) {
	Button *button = (Button *) malloc(sizeof(Button));
	char *token;
	char *saveptr;
	if (button != NULL) {
		token = strtok_r(line, "	", &saveptr);
		button->textureIndex = atoi(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOff.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOff.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOff.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOff.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOn.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOn.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOn.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->originOn.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->dest.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->dest.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->dest.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->dest.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->position.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->position.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->rotation = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		button->shader = (bool) atoi(token);
		button->selected = false;
	}
	return button;
}
void DrawButton(Shader shader) {
	int i;
	for (i = 0; i < BUTTON_SIZE; i++) {
		if (buttons[i] != NULL  ) {
			if (buttons[i]->shader) BeginShaderMode(shader);
			DrawTexturePro(textures[buttons[i]->textureIndex],
					(buttons[i]->selected) ? buttons[i]->originOn : buttons[i]->originOff,
					buttons[i]->dest,
					buttons[i]->position,
					buttons[i]->rotation,
					globalColor);
			if (buttons[i]->shader) EndShaderMode();
		}
	}
}
void UnloadButton(void) {
	int i;
	buttonAmount = 0;
	for (i = 0; i < BUTTON_SIZE; i++) {
		if (buttons[i] != NULL) {
			free(buttons[i]);
			buttons[i] = NULL;
		}
	}
	printf("INFO: BUTTONS: Buttons unloaded correctly\n");
}
char *LoadText(int id) {
	char line[256];
	char *token;
	char *saveptr;
	int textId = 0;
	rewind(translationData);
	if (fgets(line, sizeof(line), translationData) != NULL) return NULL;
	while (fgets(line, sizeof(line), translationData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		textId = atoi(token);
		if (textId == id) {
			char *result = strdup(line);
			if (result == NULL) return "ERROR";
			return result;
		}
	}
	return "ERROR";
}
char *LoadTextFormatChar(int id, char value) {
	char line[256];
	char *token;
	char *saveptr;
	int textId = 0;
	rewind(translationData);
	while (fgets(line, sizeof(line), translationData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		textId = atoi(token);
		if (textId == id) {
			sprintf(line, line, value);
			char *result = strdup(line);
			if (result == NULL) return "ERROR";
			return result;
		}
	}
	return "ERROR";
}
char *LoadTextFormatInt(int id, int value) {
	char line[256];
	char *token;
	char *saveptr;
	int textId = 0;
	rewind(translationData);
	while (fgets(line, sizeof(line), translationData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		textId = atoi(token);
		if (textId == id) {
			sprintf(line, line, value);
			char *result = strdup(line);
			if (result == NULL) return "ERROR";
			return result;
		}
	}
	return "ERROR";
}
int DiceRoll(DiceType dice) {
	int roll;
	switch (dice) {
		case DICE_D100:
		case DICE_SAVED100:
			roll = (randomValue % 100) +  1;
			break;
		case DICE_D20:
		case DICE_SAVED20:
			roll = (randomValue % 20) + 1;
			break;
		case DICE_D12:
		case DICE_SAVED12:
			roll = (randomValue % 12) + 1;
			break;
		case DICE_D10:
		case DICE_SAVED10:
			roll = (randomValue % 10) + 1;
			break;
		case DICE_D8:
		case DICE_SAVED8:
			roll = (randomValue % 8) + 1;
			break;
		case DICE_D6:
		case DICE_SAVED6:
			roll = (randomValue % 6) + 1;
			break;
		case DICE_D4:
		case DICE_SAVED4:
			roll = (randomValue % 4) + 1;
			break;
		default:
			roll = 0;
			break;
	}
	Random();
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
void SetSeed(int seed) {
	srand(seed);
	randomValue = rand();
}
void SetTimeSeed(void) {
	srand(time(NULL));
	randomValue = rand();
}
void Random(void) {
	randomValue = rand();
}
void LoadEnemiesFile(FILE **file, const char *enemySheet) {
	if (*file != NULL) fclose(*file);
	if (FileExists(enemySheet)) *file = fopen(enemySheet, "r");
}
void LoadEnemiesOnCombat(FILE *file, int id) {
	char line[512];
	char *token;
	char *saveptr;
	int i = 1;
	fgets(line, sizeof(line), file);
	while (fgets(line, sizeof(line), file) != NULL) {
		if (i == id) {
			token = strtok_r(line, "	", &saveptr);
			combat.enemy[0] = LoadEnemy(atoi(token));
			if (token[0] != '0') combat.enemy[0]->enemy.position = 0;
			token = strtok_r(NULL, "	", &saveptr);
			combat.enemy[1] = LoadEnemy(atoi(token));
			if (token[0] != '0') combat.enemy[1]->enemy.position = 1;
			token = strtok_r(NULL, "	", &saveptr);
			combat.enemy[2] = LoadEnemy(atoi(token));
			if (token[0] != '0') combat.enemy[2]->enemy.position = 2;
			token = strtok_r(NULL, "	", &saveptr);
			combat.enemy[3] = LoadEnemy(atoi(token));
			if (token[0] != '0') combat.enemy[3]->enemy.position = 0;
			token = strtok_r(NULL, "	", &saveptr);
			combat.enemy[4] = LoadEnemy(atoi(token));
			if (token[0] != '0') combat.enemy[4]->enemy.position = 0;
		}
		i++;
	}
}
Entity *LoadEnemy(int id) {
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
			enemy->enemy.sprite = LoadSingleSprite(atoi(token));
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
				enemy->enemy.tech[enemy->enemy.techAmount] = LoadTech(atoi(tech));
				tech = strtok_r(NULL, ",", &saveptr);
				enemy->enemy.techAmount++;
			}
			return enemy;
		}
	}
	return NULL;
}
Entity *LoadPlayer(int id) {
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
			token = strtok_r(line, "	", &saveptr);
			player->player.name = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.surname = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.class = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.nomGuerre = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.description = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.sprite = LoadSingleSprite(atoi(token));
			token = strtok_r(line, "	", &saveptr);
			player->player.physique[0] = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.reflex[0] = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.lore[0] = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.charisma[0] = atoi(token);
			token = strtok_r(line, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(line, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(line, "	", &saveptr);
			SetProficiency(player, (AttributeType) atoi(token));
			token = strtok_r(line, "	", &saveptr);
			player->player.hitDice = (DiceType) atoi(token);
			player->player.maxHealth = DiceMean(player->player.hitDice) + player->player.physique[0];
			player->player.health = player->player.maxHealth;
			token = strtok_r(line, "	", &saveptr);
			player->player.weakness = (DamageType) atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.resistance = (DamageType) atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.inmunity = (StatusType) atoi(token);
			token = strtok_r(line, "	", &saveptr);
			player->player.weapon = LoadWeapon(atoi(token));
			token = strtok_r(line, "	", &saveptr);
			player->player.armor = LoadArmor(atoi(token));
			token = strtok_r(line, "	", &saveptr);
			player->player.charm = LoadCharm(atoi(token));
			token = strtok_r(line, "	", &saveptr);
			feature = strtok_r(token, ",", &saveptr);
			while (token != NULL) {
				SetFeature(player, (Feature) atoi(feature));
				feature = strtok_r(NULL, ",", &saveptr);
				player->player.featuresAmount++;
			}
			return player;
		}
	}
	return player;
}
void DamageEntity(Entity attacker, Technique tech) {
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
void KillEntity(Entity *entity) {
	//idk
}
void UnloadCombat(void) {
	int i;
	for (i = 0; i < 5; i++) {
		UnloadEntity(ENTITY_ENEMY, i);
		UnloadEntity(ENTITY_PLAYER, i);
	}
	printf("INFO: COMBAT: Combat unloaded successfully.\n");
}
void UnloadEntity(EntityType type, int position) {
	switch (type) {
		case ENTITY_ENEMY:
			if (combat.enemy[position] == NULL) return;
			free(combat.enemy[position]->enemy.sprite);
			free(combat.enemy[position]);
			combat.enemy[position] = NULL;
			return;
		case ENTITY_PLAYER:
			if (combat.player[position] == NULL) return;
			free(combat.player[position]->player.sprite);
			free(combat.player[position]);
			combat.player[position] = NULL;
			return;
	}
}
void DrawCombat(void) {
	int i;
	Sprite *sprite;
	for (i = 0; i < 5; i++) {
		sprite = combat.enemy[i]->enemy.sprite;
		DrawTexturePro(textures[sprite->textureIndex], sprite->origin, sprite->dest, Vector2Add(sprite->position, (Vector2) { -64 * combat.enemy[i]->enemy.position, 0 }), sprite->rotation, globalColor);
		sprite = combat.player[i]->player.sprite;
		DrawTexturePro(textures[sprite->textureIndex], sprite->origin, sprite->dest, Vector2Add(sprite->position, (Vector2) { -64 * combat.player[i]->player.position, 0 }), sprite->rotation, globalColor);
	}
}
Technique LoadTech(int id) {
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
Equip LoadWeapon(int id) {
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
Equip LoadArmor(int id) {
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
Equip LoadCharm(int id) {
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
void SetFeature(Entity *player, Feature feature) {
	switch (feature) {
		case FEAT_DEMONHANDS:
			//TODO: Add 5? random demon hands techniques
			break;
		case FEAT_PASSIONFUL:
			player->player.charisma[5] += 1;
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
			player->player.charm = LoadCharm(1);
		default:
			break;
	}
	player->player.features[player->player.featuresAmount] = feature;
	player->player.featuresAmount++;
}
void PlaySecSound(int id) {
	id = id % SOUND_SIZE;
	UnloadSoundAlias(sfxAlias[sfxPos]);
	sfxAlias[sfxPos] = LoadSoundAlias(sounds[id]);
	PlaySound(sfxAlias[sfxPos]);
	sfxPos = (sfxPos + 1) % SFXALIAS_SIZE;
}
void Menu(void) {
	if (buttonAmount > 0) {
		if (IsKeyPressed(rightKey)) {
			buttons[buttonPosition]->selected = false;
			buttonPosition++;
			ChangeSelection();
		}
		if (IsKeyPressed(leftKey)) {
			buttons[buttonPosition]->selected = false;
			buttonPosition--;
			ChangeSelection();
		}
		if (IsKeyPressed(downKey)) {
			buttons[buttonPosition]->selected = false;
			buttonPosition += buttonSkip;
			ChangeSelection();
		}
		if (IsKeyPressed(upKey)) {
			buttons[buttonPosition]->selected = false;
			buttonPosition -= buttonSkip;
			ChangeSelection();
		}
	}
}
void ChangeSelection(void) {
	if (buttonPosition < 0) buttonPosition += buttonAmount;
	else if (buttonPosition > (buttonAmount - 1)) buttonPosition -= buttonAmount;
	buttons[buttonPosition]->selected = true;
}
void Start(void) {
	switch (state) {
		case STATE_TITLE:
			Accept();
			break;
		default:
			break;
	}
}
void Select(void) {
	switch (state) {
		default:
			break;
	}
}
void Accept(void) {
	switch (state) {
		case STATE_TITLE:
			PlaySecSound(0);
			SetState(STATE_FIGHT);		// TODO: Change this when combat is done
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			switch (buttonPosition) {
				case 0:
					PlaySecSound(1);
					SetState(STATE_ATTACKMENU);
					break;
				default:
					PlaySecSound(3);
					break;
			}
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(1);
			break;
		default:
			break;
	}
}
void Cancel(void) {
	switch (state) {
		case STATE_TITLE:
			Accept();
			break;
		case STATE_MAINMENU:
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(2);
			SetState(STATE_FIGHT);
			break;
		default:
			break;
	}
}
void SetState(GameState newState) {
	UnloadSprite();
	UnloadAnimation();
	UnloadButton();
	state = newState;
	switch (state) {
		case STATE_TITLE:
			LoadSprite("./resources/layout/mainTitle.tsv");
			LoadAnimation(1, (Vector2) { 0 });
			break;
		case STATE_MAINMENU:
			LoadSprite("./resources/layout/mainMenu.tsv");
			break;
		case STATE_FIGHT:
			LoadButton("./resources/layout/fightButtons.tsv");
			ChangeSelection();
			buttonSkip = 2;
			break;
		default:
			break;
	}
}
