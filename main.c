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

typedef enum GameState GameState;

enum GameState {
	STATE_TITLE,
	STATE_MAINMENU,
	STATE_SELECTCARDS,
	STATE_FIGHT,
	STATE_ATTACKMENU,
	STATE_DEBUG
};

// INFO: Input functions
void Accept(GameState *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray);
void Cancel(GameState *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray);
void Start(GameState *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray);
void Select(GameState *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray);
void Menu(int rightKey, int leftKey, int downKey, int upKey);
void ChangeSelection(void);
void SetState(GameState newState, GameState *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray);
// INFO: SFX functions
void PlaySecSound(int id);
void LowPassFilter(void *buffer, unsigned int frames);		// TODO
void Crossfade();						// TODO

int main() {
	//-------------------------------------------------------------
	// Camera & Pixel Perfect
	//-------------------------------------------------------------
	
	const int virtualScreenWidth = 320;
	const int virtualScreenHeight = 180;
	const int screenWidth = 960;
	const int screenHeight = 540;
	const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;
	Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
	Rectangle destRec = { -virtualRatio, -virtualRatio , screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
	Vector2 origin = { 0.0f, 0.0f };
	InitWindow(screenWidth, screenHeight, "Danse Macrab");

	//-------------------------------------------------------------
	// Game Inputs and State
	//-------------------------------------------------------------
	
	SetExitKey(KEY_NULL);       // INFO: Disable KEY_ESCAPE to close window, X-button still works
	bool exitWindowRequested = false;	// Flag to request window to exit
	bool exitWindow = false;		// Flag to set window to exit
	//bool runGame = true;	// To know if the game should run, useful if the loading of a file fails
	GameState state = STATE_TITLE;	// INFO: Contains the current state of the game

	//-------------------------------------------------------------
	// Files to load data
	//-------------------------------------------------------------
	
	FILE *animsData = fopen("./resources/anims/animations.tsv", "r");
	FILE *spriteData = fopen("./resources/gfx/sprites.tsv", "r");
	FILE *charmData = fopen("./resources/combat/charms.tsv", "r");
	FILE *armorData = fopen("./resources/combat/armors.tsv", "r");
	FILE *weaponData = fopen("./resources/combat/weapons.tsv", "r");
	FILE *techData = fopen("./resources/combat/tech.tsv", "r");
	FILE *characterData = fopen("./resources/combat/characters.tsv", "r");
	FILE *enemyData = fopen("./resources/combat/enemies.tsv", "r");
	FILE *dialogData = fopen("./resources/text/dialog.tsv", "r");
	FILE *translationData = fopen("./resources/text/english.tsv", "r");

	//-------------------------------------------------------------
	// Textures used to load sprites and funny stuff
	//-------------------------------------------------------------
	
	Texture2D textures[TEX_SIZE];	// INFO: Here I hold all the texture used in the game
	textures[0] = LoadTexture("./resources/gfx/bigSprites00.png");
	textures[3] = LoadTexture("./resources/gfx/cards.png");
	textures[4] = LoadTexture("./resources/gfx/UI.png");
	textures[5] = LoadTexture("./resources/gfx/abilities.png");
	textures[6] = LoadTexture("./resources/gfx/attacks.png");
	textures[7] = LoadTexture("./resources/gfx/entities.png");
	Animable *anims[ANIM_SIZE] = { NULL };	// INFO: Animation handling and rendering
	Sprite *sprites[DRAW_SIZE] = { NULL };	// INFO: What and where to render
	Message *messages[MSG_SIZE] = { NULL };
	Color globalColor = { 255, 255, 255, 255 };	// INFO: Global color used to render the white lines in all textures as colors
	int spritePos = 0;
	int messagePos = 0;
	int animCount = 0;
	int texCount = 0;
	int sfxCount = 0;

	//-------------------------------------------------------------
	// Variables for button work, they're a lot
	//-------------------------------------------------------------
	
	Button *buttons[BUTTON_SIZE] = { NULL };
	int buttonAmount = 0;
	int buttonPos = 0;
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

	Shader shader = LoadShader(0, "contour.fs");

	Font fontDefault = LoadFont("./resources/fonts/pixelperfect.ttf");

	//-------------------------------------------------------------
	// Files to load data
	//-------------------------------------------------------------
	
	InitAudioDevice();
	Music music = LoadMusicStream("./resources/sfx/title.mp3");
	music.looping = true;

	Sound sounds[SOUND_SIZE];	// INFO: Here I hold all the sounds used in the game
	Sound sfxAlias[SFXALIAS_SIZE];	// INFO: Used to reproduce several sounds at once
	int sfxPos = 0;			// INFO: Universal position to locate one "free" sfxAlias
	sounds[0] = LoadSound("./resources/sfx/pressStart.mp3");
	sounds[1] = LoadSound("./resources/sfx/buttonSelect.wav");
	sounds[2] = LoadSound("./resources/sfx/buttonCancel.wav");
	sounds[3] = LoadSound("./resources/sfx/error.wav");
	PlayMusicStream(music);
	SetMusicVolume(music, 1.0f);

	int randomValue;

	SetState(STATE_TITLE, sprites, anims, buttons);
	
	//-------------------------------------------------------------
	// Files to load data
	//-------------------------------------------------------------
	
	Combat combat = { { NULL }, { NULL } };// INFO: Data from position, entities and stuff for combat
	//combat.playable[2] = LoadEntity("", ENTITY_PLAYER); TODO

	SetTargetFPS(60);// INFO: Set our game to run at 60 frames-per-second

	while (!exitWindow) {

		//-------------------------------------------------------------
		// INFO: Update: This is for calculations and events which do not affect Texture or Drawing in a direct way
		//-------------------------------------------------------------

		UpdateMusicStream(music);// Update music buffer with new stream data
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;// Detect if X-button or KEY_ESCAPE have been pressed to close window
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
			if (IsKeyPressed(startKey)) Start(state, &state, sprites, anims, buttons);
			if (IsKeyPressed(acceptKey)) Accept(state, &state, sprites, anims, buttons);
			if (IsKeyPressed(cancelKey)) Cancel(state, &state, sprites, anims, buttons);
			Menu();
		}

		//-------------------------------------------------------------
		// INFO: Texture: In this texture mode I create an smaller version of the game which is later rescaled in the draw mode
		//-------------------------------------------------------------

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

		//-------------------------------------------------------------
		// INFO: Draw: Take the texture in lower resolution and rescale it to a bigger res, all this while preserving pixel perfect
		//-------------------------------------------------------------

		BeginDrawing();
			ClearBackground(RED);
			BeginMode2D(screenSpaceCamera);
				DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
			EndMode2D();
			DrawFPS(10, 10);
		EndDrawing();
	}

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

void PlaySecSound(int id) {
	id = id % SOUND_SIZE;
	UnloadSoundAlias(sfxAlias[sfxPos]);
	sfxAlias[sfxPos] = LoadSoundAlias(sounds[id]);
	PlaySound(sfxAlias[sfxPos]);
	sfxPos = (sfxPos + 1) % SFXALIAS_SIZE;
}
void Menu(int rightKey, int leftKey, int downKey, int upKey) {
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
void Start(GameState *state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (&state) {
		case STATE_TITLE:
			Accept(state, spritesArray, animsArray, buttonsArray);
			break;
		default:
			break;
	}
}
void Select(GameState *state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (&state) {
		default:
			break;
	}
}
void Accept(GameState *state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (&state) {
		case STATE_TITLE:
			PlaySecSound(0);
			SetState(STATE_FIGHT, state, spritesArray, animsArray, buttonsArray);// TODO: Change this when combat is done
			break;
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			switch (buttonPosition) {
				case 0:
					PlaySecSound(1);
					SetState(STATE_ATTACKMENU, state, spritesArray, animsArray, buttonsArray);
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
void Cancel(GameState *state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	switch (state) {
		case STATE_TITLE:
			Accept(state, spritesArray, animsArray, buttonsArray);
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
void SetState(GameState newState, GameState *state, Sprite **spritesArray, Animable **animsArray, Button *buttonsArray) {
	UnloadSprite(spritesArray);
	UnloadAnimation(animsArray);
	UnloadButton(buttonsArray);
	&state = newState;
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
