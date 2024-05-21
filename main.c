#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>
#include "entity.h"

#define ANIM_SIZE 16
#define TEX_SIZE 8
#define SOUND_SIZE 8
#define SFXALIAS_SIZE 8
#define DRAW_SIZE 16
#define BUTTON_SIZE 8
#define MSG_SIZE 8

typedef struct Message Message;
typedef struct Sprite Sprite;
typedef struct Animable Animable;
typedef struct Combat Combat;
typedef struct Button Button;
typedef enum GameState GameState;
typedef enum Language Language;
typedef enum Feature Feature;

struct Message {
	char *string;
	int id;
	Vector2 position;
	Vector2 origin;
	float rotation;
	float fontSize;
};
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
enum Language {
	LANG_SPANISH = 0,
	LANG_ENGLISH = 1,
	LANG_RUSSIAN = 2
};

// INFO: GFX functions
Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable *anim);
void DrawAnimable(Animable *anim, Shader shader);
void UnloadAnimable(Animable *anim);
// Animations
void LoadAnimation(int id, Vector2 offset);
void UnloadAnimation(Animable **animationArray);
// Sprite work
void LoadSprite(const char *spriteSheet);
Sprite *LoadSingleSprite(int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Shader shader);
void UnloadSprite(Sprite **spriteArray);
void UnloadSingleSprite(Sprite **sprite);
// Buttons stuff
void LoadButton(const char *buttonSheet);
Button *ParseButton(char *line);
void DrawButton(Shader shader);
void UnloadButton(Button **buttonArray);
// Text and translations and stuff
Message *LoadMessage(int id);			// To load a text line with the corresponding translation
void RenderMessage(Font font);
void UnloadMessage(Message **message);
// INFO: Input functions
void Accept(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
void Cancel(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
void Start(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
void Select(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
void Menu(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
void ChangeSelection(void);
void SetState(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray);
// INFO: SFX functions
void PlaySecSound(int id);
void LowPassFilter(void *buffer, unsigned int frames);		// TODO
void Crossfade();						// TODO

//------------------------------------------------------------------------------------
// INFO: Program main entry point
//------------------------------------------------------------------------------------
Animable *anims[ANIM_SIZE] = { NULL };				// INFO: Animation handling and rendering
Texture2D textures[TEX_SIZE];					// INFO: Here I hold all the texture used in the game
Sound sounds[SOUND_SIZE];					// INFO: Here I hold all the sounds used in the game
Sound sfxAlias[SFXALIAS_SIZE];					// INFO: Used to reproduce several sounds at once
int sfxPos = 0;							// INFO: Universal position to locate one "free" sfxAlias
Sprite *sprites[DRAW_SIZE] = { NULL };				// INFO: What and where to render
int spritePos = 0;
Color globalColor = { 255, 255, 255, 255 };			// INFO: Global color used to render the white lines in all textures as colors
Combat combat = { { NULL }, { NULL } };				// INFO: Data from position, entities and stuff for combat
Message *messages[MSG_SIZE] = { NULL };
int messagePos;
Font fontDefault;
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

	GameState state = STATE_TITLE;					// INFO: Contains the current state of the game

	Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
	Rectangle destRec = { -virtualRatio, -virtualRatio , screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
	Vector2 origin = { 0.0f, 0.0f };

	fontDefault = LoadFont("./resources/fonts/pixelperfect.ttf");

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
	UnloadFont(fontDefault);
	UnloadSprite(sprites);
	UnloadButton(buttons);
	UnloadCombat(&combat);
	UnloadAnimation(anims);
	UnloadMessage(messages);
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
void UnloadAnimation(Animable **animationArray) {
	int i;
	for (i = 0; i < ANIM_SIZE; i++) {
		if (animationArray[i] != NULL) {
			UnloadAnimable(animationArray[i]);
			animationArray[i] = NULL;
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
void UnloadSprite(Sprite **spriteArray) {
	int i;
	for (i = 0; i <= spritePos; i++) {
		if (spriteArray[i] != NULL) {
			free(spriteArray[i]);
			spriteArray[i] = NULL;
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
void UnloadButton(Button **buttonArray) {
	int i;
	buttonAmount = 0;
	for (i = 0; i < BUTTON_SIZE; i++) {
		if (buttonArray[i] != NULL) {
			free(buttonArray[i]);
			buttonArray[i] = NULL;
		}
	}
	printf("INFO: BUTTONS: Buttons unloaded correctly\n");
}
Message *LoadMessage(int id) {
	Message *message = (Message *) malloc(sizeof(Message));
	if (message == NULL) return NULL;
	message->id = id;
	char line[512];
	char *token;
	char *saveptr;
	int textId = 0;
	rewind(translationData);
	if (fgets(line, sizeof(line), translationData) != NULL) return message;
	while (fgets(line, sizeof(line), translationData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		textId = atoi(token);
		if (textId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			strcpy(message->string, token);
			return message;
		}
	}
	return message;
}
void RenderMessage(Font font) {
	//
}
void UnloadMessage(Message **message) {
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
void Start(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (state) {
		case STATE_TITLE:
			Accept(state, spritesArray, animsArray, buttonsArray);
			break;
		default:
			break;
	}
}
void Select(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (state) {
		default:
			break;
	}
}
void Accept(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
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
void Cancel(GameState state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
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
void SetState(GameState newState, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	UnloadSprite(spritesArray);
	UnloadAnimation(animsArray);
	UnloadButton(buttonsArray);
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
