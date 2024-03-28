#include <stdio.h>
#include <math.h>
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
typedef enum EquipType EquipType;
typedef enum DiceType DiceType;
typedef enum TechniqueType TechniqueType;
typedef enum AttributeType AttributeType;
typedef enum StatusType StatusType;

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
	unsigned int frame;       // Frame needed to change to the next event
	unsigned int currentFrame;// Current frame of animation
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
	STATE_FIGHT
};
enum DamageType {
	DMG_NONE = 0,
	DMG_SLASHING = 1,
	DMG_BLUDGEONING = 2,
	DMG_PIERCING = 3,
	DMG_FIRE = 4,
	DMG_ICE = 5,
	DMG_ACID = 6,
	DMG_PSYCHIC = 7
};
enum EntityType {
	ENTITY_ENEMY = 0,
	ENTITY_PLAYER = 1
};
enum EquipType {
	EQUIP_WEAPON = 0,
	EQUIP_ARMOR = 1,
	EQUIP_CHARM = 2
};
enum DiceType {
	DICE_D100,
	DICE_D20,
	DICE_D12,
	DICE_2D6,
	DICE_D10,
	DICE_D8,
	DICE_D6,
	DICE_D4,
	DICE_SAVED100,
	DICE_SAVED20,
	DICE_SAVED12,
	DICE_SAVE2D6,
	DICE_SAVED10,
	DICE_SAVED8,
	DICE_SAVED6,
	DICE_SAVED4
};
enum TechniqueType {
	TECH_ATTACK,
	TECH_CHECK,
	TECH_TOME,
	TECH_SONG,
	TECH_DRACONIC,
	TECH_GOREART,
	TECH_DEMONHAND,
	TECH_SUMMON
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
	ATTR_DREAM = 15,
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
	STATUS_NONE = 0,
	STATUS_BLEED = 1,
	STATUS_BLINDED = 2,
	STATUS_BURNING = 3,
	STATUS_CONFUSED = 4,
	STATUS_DEAFENED = 5,
	STATUS_DEATH = 6,
	STATUS_DEVOURED = 7,
	STATUS_DROWNING = 8,
	STATUS_GRAPPLED = 9,
	STATUS_HORRIFIED = 10,
	STATUS_INVISIBLE = 11,
	STATUS_MOURNFUL = 12,
	STATUS_PARALYZED = 13,
	STATUS_PASSANGER = 14,
	STATUS_POISONED = 15,
	STATUS_POSSESSION = 16,
	STATUS_PRONE = 17,
	STATUS_RAGE = 18,
	STATUS_SLEEPING = 19,
	STATUS_SUFFOCATING = 20
};

struct Technique {
	// For when rolling and stuff (?
	DiceType roll;
	AttributeType attr;		// Used for bonusses on the roll, save or difficulty
	// Costs, money and HP are the commonest
	int costMoney;
	int costHP;
	// Value pool, could be used for damage or healing
	DiceType diceSide;
	int diceAmount;
	int flatBonus;
	StatusType status;		// Apply status effect to apply
};
struct Weapon {
	EquipType type;
	char name[64];
	char description[256];
	int cost;
	Technique attack;
	Technique tech;
};
struct Armor {
	EquipType type;
	char name[64];
	char description[256];
	int cost;
	Technique tech;
};
struct Charm {
	EquipType type;
	char name[64];
	char description[256];
	int cost;
	int health;
	int stamina;
	int stress;
};
union Equip {
	Weapon weapon;
	Armor armor;
	Charm charm;
};
struct Player {
	EntityType type;
	// INFO: VITALS
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	// INFO: ATTRIBUTES
	int physique[6];		// Physique + Athletics, Constitution, Medicine, Melee, Vibes
	int reflex[6];			// Reflex + Accuracy, Acrobatics, Mischief, Perception, Touch
	int lore[6];			// Lore + Arcanum, Beasts, Dream, Dungeons, Nature
	int charisma[6];		// Charisma + Anima, Authority, Drama, Kinship, Passion
	// INFO: OTHER STUFF
	char name[64];
	char class[32];
	char description[128];
	Weapon weapon;
	Armor armor;
	Charm charm;
	Sprite *sprite;			// Might be wrong xd
	//Animable *anim;		// Might be useful? Is this even the correct way? No idea
};
struct Enemy {
	EntityType type;
	// INFO: VITALS
	int maxHealth;
	int health;
	int maxStress;
	int stress;
	// INFO: ATTRIBUTES
	int physique[6];		// Physique + Athletics, Constitution, Medicine, Melee, Vibes
	int reflex[6];			// Reflex + Accuracy, Acrobatics, Mischief, Perception, Touch
	int lore[6];			// Lore + Arcanum, Beasts, Dream, Dungeons, Nature
	int charisma[6];		// Charisma + Anima, Authority, Drama, Kinship, Passion
	// INFO: OTHER STUFF
	char name[64];
	char description[128];
	DamageType weakness[2];
	DamageType resistances[2];
	Sprite *sprite;			// Might be wrong xd
	//Animable *anim;		// Might be useful? Is this even the correct way? No idea
};
union Entity {
	Enemy enemy;
	Player player;
};
struct Combat {
	Entity *enemy[5];
	Entity *playable[5];
	int turn;
};

// TODO: Perhaps a good way to clean code is by turning the whole dual camera system into an struct and some functions

// INFO: Function declarations
//------------------------------------------------------------------------------------
// INFO: GFX functions
Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable *anim);
void DrawAnimable(Animable *anim, Shader shader);
void UnloadAnimable(Animable *anim);
void LoadAnimation(int id, Vector2 offset);
void UnloadAnimation(void);
void LoadSprite(const char *spriteSheet);
Sprite *LoadSingleSprite(int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Shader shader);
void UnloadSprite(void);
void LoadCombat(const char *combatSheet);
void LoadButton(const char *buttonSheet);
Button *ParseButton(char *line);
void DrawButton(Shader shader);
void UnloadButton(void);
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
void LowPassFilter(void *buffer, unsigned int frames);
void Crossfade();
// INFO: Entities functions
//------------------------------------------------------------------------------------
void LoadEnemies(const char *enemySheet);
void LoadPlayer(const char *playerSheet);
void MoveEntity(Entity *entity, int position);
void DamageEntity(Entity *entity, Technique tech);
void KillEntity(Entity *entity);
void UnloadEntity(Entity *entity);
//------------------------------------------------------------------------------------
// INFO: Program main entry point
//------------------------------------------------------------------------------------
GameState state = STATE_TITLE;					// INFO: Contains the current state of the game
Animable *anims[ANIM_SIZE] = { NULL };				// INFO: Animation handling and rendering
FILE *animsData;						// INFO: Big file with every single independent animation data
Texture2D textures[TEX_SIZE];					// INFO: Here I hold all the texture used in the game
Sound sounds[SOUND_SIZE];					// INFO: Here I hold all the sounds used in the game
Sound sfxAlias[SFXALIAS_SIZE];					// INFO: Used to reproduce several sounds at once
int sfxPos = 0;							// INFO: Universal position to locate one "free" sfxAlias
Sprite *sprites[DRAW_SIZE] = { NULL };				// INFO: What and where to render
Color globalColor = { 255, 255, 255, 255 };			// INFO: Global color used to render the white lines in all textures as colors
Combat combat = { { NULL }, { NULL } };				// INFO: Data from position, entities and stuff for combat
FILE *spriteData;						// INFO: Big file with the sprites that will load individually
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

	Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
	//Rectangle destRec = { -virtualRatio, -virtualRatio , screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
	Rectangle destRec = { 20.0f, -20.0f, screenWidth, screenHeight };
	Vector2 origin = { 0.0f, 0.0f };

	animsData = fopen("./resources/anims/animations.tsv", "r");
	spriteData = fopen("./resources/gfx/sprites.tsv", "r");
	textures[0] = LoadTexture("./resources/gfx/bigSprites00.png");
	textures[3] = LoadTexture("./resources/gfx/cards.png");
	textures[4] = LoadTexture("./resources/gfx/UI.png");
	textures[5] = LoadTexture("./resources/gfx/abilities.png");
	textures[6] = LoadTexture("./resources/gfx/attacks.png");
	textures[7] = LoadTexture("./resources/gfx/entities.png");

	//Animable *test = LoadAnimable("./resources/anims/mainMenu/crab.tsv", true);                   // TODO: Delete test and load a whole anim with anims

	Shader shader = LoadShader(0, "contour.fs");
	//SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), texture);      // INFO: General structure of how to load a texture

	Music music = LoadMusicStream("./resources/sfx/title.mp3");
	music.looping = true;
	sounds[0] = LoadSound("./resources/sfx/pressStart.mp3");
	sounds[1] = LoadSound("./resources/sfx/selectButton.mp3");
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
	UnloadAnimation();
	UnloadMusicStream(music);   // Unload music stream buffers from RAM

	for (texCount = 0; texCount < TEX_SIZE; texCount++) UnloadTexture(textures[texCount]);
	for (sfxCount = 0; sfxCount < SFXALIAS_SIZE; sfxCount++) UnloadSoundAlias(sfxAlias[sfxCount]);
	for (sfxCount = 0; sfxCount < SOUND_SIZE; sfxCount++) UnloadSound(sounds[sfxCount]);

	fclose(animsData);
	fclose(spriteData);

	CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
	CloseWindow();              // Close window and OpenGL context

	return 0;
}

Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset) {
	Animable *anim = (Animable *) malloc(sizeof(Animable));          // Dynamic allocation since many animables might be created and destroyed in quick successions, don't forget to free later
	if (anim != NULL) {
		char line[256];       // Line from the file that contains all the struct data
		FILE *file = fopen(animSheet, "r");
		if (file == NULL) {     // If the file has problems to open, lack of memory won't be a problem, I hope
			printf("INFO: ANIMABLE: Error opening the animation file %s!\n", animSheet);
			fclose(file);
			return NULL;
		}
		if (fgets(line, sizeof(line), file)) {
			ParseAnimable(line, anim);
			printf("INFO: ANIMABLE: Animable loaded succesfully\n");
			anim->currentFrame = 0;
			anim->data = file;
			anim->repeat = repeat;
			anim->index = index;
			anim->offset = offset;
		}
		else {
			printf("INFO: ANIMABLE: File does not contains usable data!\n");
			fclose(file);
			return NULL;
		}
	}
	return anim;
}
void ParseAnimable(char *line, Animable *anim) {
	char *token;
	char *saveptr;
	token = strtok_r(line, "	", &saveptr);
	anim->frame = (unsigned int) atoi(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->textureIndex = atoi(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.w = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->origin.z = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.w = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->dest.z = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->position.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->position.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->rotation = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.w = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.z = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.w = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.z = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.y = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaRotation = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->shader = (bool) atoi(token);
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
				fgets(line, sizeof(line), anim->data);
				ParseAnimable(line, anim);
			}
			else if (anim->repeat) {
				anim->currentFrame = 0;
				rewind(anim->data);
				fgets(line, sizeof(line), anim->data);
				ParseAnimable(line, anim);
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
	int i = 0;
	int j;
	char line[256];
	char *token;
	char *saveptr;
	bool repeat;
	rewind(animsData);
	while (fgets(line, sizeof(line), animsData)) {
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
}
void LoadSprite(const char *spriteSheet) {
	FILE *file = fopen(spriteSheet, "r");
	if (file != NULL) {
		char line[256];
		int i;
		while (fgets(line, sizeof(line), file)) {
			for (i = 0; i < DRAW_SIZE; i++) {
				if (sprites[i] == NULL) {
					sprites[i] = ParseSprite(line);
					break;
				}
			}
		}
	}
	fclose(file);
}
Sprite *LoadSingleSprite(int id) {
	rewind(spriteData);
	if (spriteData != NULL) {
		char line[256];
		int i = 1;
		fgets(line, sizeof(line), spriteData);
		while (fgets(line, sizeof(line), spriteData)) {
			if (i == id) {
				return ParseSprite(line);
			}
			i++;
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
	for (i = 0; i < DRAW_SIZE; i++) {
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
	for (i = 0; i < DRAW_SIZE; i++) {
		if (sprites[i] != NULL) {
			free(sprites[i]);
			sprites[i] = NULL;
		}
	}
}
void LoadButton(const char *spriteSheet) {
	FILE *file = fopen(spriteSheet, "r");
	if (file != NULL) {
		char line[256];
		int i;
		while (fgets(line, sizeof(line), file)) {
			for (i = 0; i < BUTTON_SIZE; i++) {
				if (buttons[i] == NULL) {
					buttons[i] = ParseButton(line);
					buttonAmount++;
					break;
				}
			}
		}
	}
	fclose(file);
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
			SetState(STATE_FIGHT);
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			PlaySecSound(1);
			break;
		default:
			break;
	}
}
void Cancel(void) {
	switch (state) {
		case STATE_TITLE:
			break;
		case STATE_MAINMENU:
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
			LoadAnimation(0, (Vector2) { 0 });
			break;
		case STATE_MAINMENU:
			LoadSprite("./resources/layout/mainMenu.tsv");
			break;
		case STATE_FIGHT:
			LoadButton("./resources/layout/fightButtons.tsv");
			buttonSkip = 2;
			break;
		default:
			break;
	}
}
