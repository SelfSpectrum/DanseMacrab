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
typedef struct StateData StateData;

enum GameState {
	STATE_TITLE,
	STATE_MAINMENU,
	STATE_SELECTCARDS,
	STATE_FIGHT,
	STATE_ATTACKMENU,
	STATE_DEBUG
};
struct StateData {
	GameState state;
	bool exitWindowRequested;	// Flag to request window to exit
	bool exitWindow;		// Flag to set window to exit
	//bool runGame;			// To know if the game should run, useful if the loading of a file fails
	int buttonAmount;
	int buttonPosition;
	int buttonSkip;
	int startKey;
	int selectKey;
	int acceptKey;
	int cancelKey;
	int leftKey;
	int rightKey;
	int upKey;
	int downKey;
	int extraAKey;
	int extraBKey;
	int randomValue;
};

// INFO: Input functions
void Accept(StateData *state, Sprite **sprites, Animable **anims, Button **buttons);
void Cancel(StateData *state, Sprite **sprites, Animable **anims, Button **buttons);
void Start(StateData *state, Sprite **sprites, Animable **anims, Button **buttons);
void Select(StateData *state, Sprite **sprites, Animable **anims, Button **buttons);
void Menu(StateData *state, Button **buttons);
void ChangeSelection(StateData *state, Button **buttons);
void SetState(GameState newState, StateData *state, Sprite **sprites, Animable **anims, Button **buttons);
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

	//-------------------------------------------------------------
	// Game Inputs and State
	//-------------------------------------------------------------
	
	StateData state;
	state.state = STATE_TITLE;	// INFO: Contains the current state of the game
	state.exitWindowRequested = false;	// Flag to request window to exit
	state.exitWindow = false;		// Flag to set window to exit
	//state.runGame = true;	// To know if the game should run, useful if the loading of a file fails
	state.buttonAmount = 0;
	state.buttonPos = 0;
	state.buttonSkip = 0;
	state.startKey = KEY_ENTER;
	state.selectKey = KEY_BACKSPACE;
	state.acceptKey = KEY_Z;
	state.cancelKey = KEY_X;
	state.leftKey = KEY_LEFT;
	state.rightKey = KEY_RIGHT;
	state.upKey = KEY_UP;
	state.downKey = KEY_DOWN;
	state.extraAKey = KEY_A;
	state.extraBKey = KEY_B;
	SetState(STATE_TITLE, &state, sprites, anims, buttons);
	SetExitKey(KEY_NULL);       // INFO: Disable KEY_ESCAPE to close window, X-button still works
	
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
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) state.exitWindowRequested = true;// Detect if X-button or KEY_ESCAPE have been pressed to close window
		if (exitWindowRequested) {
			// A request for close window has been issued, we can save data before closing or just show a message asking for confirmation
			if (IsKeyPressed(state.acceptKey) || IsKeyPressed(state.startKey)) state.exitWindow = true;
			else if (IsKeyPressed(state.cancelKey)) state.exitWindowRequested = false;
		}
		else {
			//UpdateAnimable(test, shader);
			for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
				if (anims[animCount] != NULL) UpdateAnimable(anims[animCount]);
			}
			for (texCount = 0; texCount < TEX_SIZE; texCount++) {
				SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), textures[texCount]);
			}
			if (IsKeyPressed(state.startKey)) Start(&state, sprites, anims, buttons);
			if (IsKeyPressed(state.acceptKey)) Accept(&state, sprites, anims, buttons);
			if (IsKeyPressed(state.cancelKey)) Cancel(&state, sprites, anims, buttons);
			Menu(&state, buttons);
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

void PlaySecSound(int id, Sound *sfxAlias, Sound *sounds) {
	id = id % SOUND_SIZE;
	UnloadSoundAlias(sfxAlias[sfxPos]);
	sfxAlias[sfxPos] = LoadSoundAlias(sounds[id]);
	PlaySound(sfxAlias[sfxPos]);
	sfxPos = (sfxPos + 1) % SFXALIAS_SIZE;
}
void Menu(StateData *state, Button **buttons) {
	if (state->buttonAmount > 0) {
		if (IsKeyPressed(state->rightKey)) {
			buttons[state->buttonPosition]->selected = false;
			state->buttonPosition++;
			ChangeSelection(state, buttons);
		}
		if (IsKeyPressed(state->leftKey)) {
			buttons[state->buttonPosition]->selected = false;
			state->buttonPosition--;
			ChangeSelection(state, buttons);
		}
		if (IsKeyPressed(state->downKey)) {
			buttons[state->buttonPosition]->selected = false;
			state->buttonPosition += state->buttonSkip;
			ChangeSelection(state, buttons);
		}
		if (IsKeyPressed(state->upKey)) {
			buttons[state->buttonPosition]->selected = false;
			state->buttonPosition -= state->buttonSkip;
			ChangeSelection(state, buttons);
		}
	}
}
void ChangeSelection(StateData *state, Button **buttons) {
	if (state->buttonPosition < 0) state->buttonPosition += state->buttonAmount;
	else if (state->buttonPosition > (state->buttonAmount - 1)) state->buttonPosition -= state->buttonAmount;
	buttons[state->buttonPosition]->selected = true;
}
void Start(StateData *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state, spritesArray, animsArray, buttonsArray);
			break;
		default:
			break;
	}
}
void Select(StateData *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray) {
	switch (state->state) {
		default:
			break;
	}
}
void Accept(StateData *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray) {
	switch (state->state) {
		case STATE_TITLE:
			PlaySecSound(0);
			SetState(STATE_FIGHT, state, spritesArray, animsArray, buttonsArray);// TODO: Change this when combat is done
			break;
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			switch (state->buttonPosition) {
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
void Cancel(StateData *state, Sprite **spritesArray, Animable **animsArray, Button **buttonsArray) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state, spritesArray, animsArray, buttonsArray);
			break;
		case STATE_MAINMENU:
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(2);
			SetState(STATE_FIGHT, state, spritesArray, animsArray, buttonsArray);
			break;
		default:
			break;
	}
}
void SetState(GameState newState, StateData *state, Sprite **sprites, Animable **anims, Button **buttons) {
	UnloadSprite(sprites);
	UnloadAnimation(anims);
	UnloadButton(buttons);
	state->state = newState;
	switch (state->state) {
		case STATE_TITLE:
			LoadSprite("./resources/layout/mainTitle.tsv");
			LoadAnimation(1, (Vector2) { 0 });
			break;
		case STATE_MAINMENU:
			LoadSprite("./resources/layout/mainMenu.tsv");
			break;
		case STATE_FIGHT:
			LoadButton("./resources/layout/fightButtons.tsv");
			ChangeSelection(state, buttons);
			state->buttonSkip = 2;
			break;
		default:
			break;
	}
}
