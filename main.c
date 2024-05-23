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
#define SPRITE_SIZE 16
#define BUTTON_SIZE 8
#define MSG_SIZE 8

typedef enum GameState GameState;
typedef struct StateData StateData;

enum GameState {
	STATE_INIT,
	STATE_SELECTLANGUAGE,
	STATE_INITIALSCENE,
	STATE_TITLE,
	STATE_MAINMENU,
	STATE_SELECTCARDS,
	STATE_FIGHT,
	STATE_ATTACKMENU,
	STATE_DEBUG
};
struct StateData {
	// State
	GameState state;
	bool exitWindowRequested; // Flag to request window to exit
	bool exitWindow; // Flag to set window to exit
	//bool runGame; // To know if the game should run
	int randomValue;
	// Visuals
	Color globalColor; // Used to render the white lines in all textures as colors
	Texture2D textures[TEX_SIZE]; // Here I hold all the texture used in the game
	Animable *anims[ANIM_SIZE]; // Animation handling and rendering
	Sprite *sprites[SPRITE_SIZE]; // INFO: What and where to render
	Message *messages[MSG_SIZE];
	int spritePosition;
	int messagePosition;
	// Inputs
	Button *buttons[BUTTON_SIZE];
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
	// Sounds
	Music music;
	Sound sounds[SOUND_SIZE]; // Here I hold all the sounds used in the game
	Sound sfxAlias[SFXALIAS_SIZE]; // Used to reproduce several sounds at once
	int sfxPosition; // Position to locate one "free" sfxAlias
	// Combat
	Combat combat;
};

// INFO: Input functions
void Accept(StateData *state);
void Cancel(StateData *state);
void Start(StateData *state);
void Select(StateData *state);
void Menu(StateData *state);
void ChangeSelection(StateData *state);
void SetState(StateData *state, GameState newState);
// INFO: SFX functions
void PlaySecSound(int id, StateData *state);
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
	SetTargetFPS(60);// INFO: Set our game to run at 60 frames-per-second

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

	Shader shader = LoadShader(0, "contour.fs");
	Font fontDefault = LoadFont("./resources/fonts/pixelperfect.ttf");

	//-------------------------------------------------------------
	// Game Inputs and State
	//-------------------------------------------------------------
	
	StateData state; // Contains the current state of the game
	state.exitWindowRequested = false;
	state.exitWindow = false;
	//state.runGame = true;
	state.buttonAmount = 0;
	state.buttonPosition = 0;
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
	SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works
	state.combat = (Combat) { { NULL }, { NULL } };// Data from position, entities and stuff
	//combat.playable[2] = LoadEntity("", ENTITY_PLAYER); TODO

	//-------------------------------------------------------------
	// Textures used to load sprites and funny stuff
	//-------------------------------------------------------------
	
	state.globalColor = (Color) { 255, 255, 255, 255 };

	state.spritePosition = 0;
	state.messagePosition = 0;

	int animCount = 0;
	int texCount = 0;
	int sfxCount = 0;

	//-------------------------------------------------------------
	// Audio and Sound
	//-------------------------------------------------------------
	
	InitAudioDevice();
	state.music = LoadMusicStream("./resources/sfx/title.mp3");
	state.music.looping = true;

	state.sfxPosition = 0;

	PlayMusicStream(state.music);
	SetMusicVolume(state.music, 1.0f);

	SetState(&state, STATE_INIT);

	while (!state.exitWindow) {

		//-------------------------------------------------------------
		// INFO: Update: This is for calculations and events which do not affect Texture or Drawing in a direct way
		//-------------------------------------------------------------

		UpdateMusicStream(state.music);// Update music buffer with new stream data
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) state.exitWindowRequested = true;// Detect if X-button or KEY_ESCAPE have been pressed to close window
		if (state.exitWindowRequested) {
			// A request for close window has been issued, we can save data before closing or just show a message asking for confirmation
			if (IsKeyPressed(state.acceptKey) || IsKeyPressed(state.startKey)) state.exitWindow = true;
			else if (IsKeyPressed(state.cancelKey)) state.exitWindowRequested = false;
		}
		else {
			//UpdateAnimable(test, shader);
			for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
				if (state.anims[animCount] != NULL) UpdateAnimable(state.anims[animCount]);
			}
			for (texCount = 0; texCount < TEX_SIZE; texCount++) {
				SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), state.textures[texCount]);
			}
			if (IsKeyPressed(state.startKey)) Start(&state);
			if (IsKeyPressed(state.acceptKey)) Accept(&state);
			if (IsKeyPressed(state.cancelKey)) Cancel(&state);
			Menu(&state);
		}

		//-------------------------------------------------------------
		// INFO: Texture: In this texture mode I create an smaller version of the game which is later rescaled in the draw mode
		//-------------------------------------------------------------

		BeginTextureMode(target);
			ClearBackground(BLACK);
			BeginMode2D(worldSpaceCamera);
				if (state.exitWindowRequested) {
					DrawText("Are you sure you want to exit program?", 50, 90, 8, state.globalColor);
				}
				else {
					for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
						if (state.anims[animCount] != NULL) DrawAnimable(state.anims[animCount], shader);
					}
					DrawSprite(shader, state.sprites, &state.spritePosition, state.globalColor);
					DrawButton(shader);
					BeginShaderMode(shader);
						DrawTexturePro(state.textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { 0, 0 }, 0, state.globalColor);
						DrawTexturePro(state.textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -64, 0 }, 0, state.globalColor);
						DrawTexturePro(state.textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -128, 0 }, 0, state.globalColor);
						DrawTexturePro(state.textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -196, 0 }, 0, state.globalColor);
						DrawTexturePro(state.textures[7], (Rectangle) { 0, 0, 64, 64 }, (Rectangle) { 0, 0, 64, 64 }, (Vector2) { -256, 0 }, 0, state.globalColor);
						DrawTexturePro(state.textures[5], (Rectangle) { 0, 0, 16, 16 }, (Rectangle) { 0, 0, 16, 16 }, (Vector2) { 0, -16 }, 0, state.globalColor);
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

	StopMusicStream(state.music);
	UnloadShader(shader);
	UnloadRenderTexture(target);
	UnloadFont(fontDefault);
	UnloadSprite(state.sprites, &state.spritePosition);
	UnloadButton(state.buttons);
	UnloadCombat(&state.combat);
	UnloadAnimation(state.anims);
	UnloadMessage(state.messages);
	UnloadMusicStream(state.music); // Unload music stream buffers from RAM

	for (texCount = 0; texCount < TEX_SIZE; texCount++) UnloadTexture(state.textures[texCount]);
	for (sfxCount = 0; sfxCount < SFXALIAS_SIZE; sfxCount++) UnloadSoundAlias(state.sfxAlias[sfxCount]);
	for (sfxCount = 0; sfxCount < SOUND_SIZE; sfxCount++) UnloadSound(state.sounds[sfxCount]);

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

void PlaySecSound(int id, StateData *state) {
	id = id % SOUND_SIZE;
	UnloadSoundAlias(state->sfxAlias[state->sfxPosition]);
	state->sfxAlias[state->sfxPosition] = LoadSoundAlias(state->sounds[id]);
	PlaySound(state->sfxAlias[state->sfxPosition]);
	state->sfxPosition = (state->sfxPosition + 1) % SFXALIAS_SIZE;
}
void Menu(StateData *state) {
	if (state->buttonAmount > 0) {
		if (IsKeyPressed(state->rightKey)) {
			state->buttons[state->buttonPosition]->selected = false;
			state->buttonPosition++;
			ChangeSelection(state);
		}
		if (IsKeyPressed(state->leftKey)) {
			state->buttons[state->buttonPosition]->selected = false;
			state->buttonPosition--;
			ChangeSelection(state);
		}
		if (IsKeyPressed(state->downKey)) {
			state->buttons[state->buttonPosition]->selected = false;
			state->buttonPosition += state->buttonSkip;
			ChangeSelection(state);
		}
		if (IsKeyPressed(state->upKey)) {
			state->buttons[state->buttonPosition]->selected = false;
			state->buttonPosition -= state->buttonSkip;
			ChangeSelection(state);
		}
	}
}
void ChangeSelection(StateData *state) {
	if (state->buttonPosition < 0) state->buttonPosition += state->buttonAmount;
	else if (state->buttonPosition > (state->buttonAmount - 1)) state->buttonPosition -= state->buttonAmount;
	state->buttons[state->buttonPosition]->selected = true;
}
void Start(StateData *state) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state);
			break;
		default:
			break;
	}
}
void Select(StateData *state) {
	switch (state->state) {
		default:
			break;
	}
}
void Accept(StateData *state) {
	switch (state->state) {
		case STATE_TITLE:
			PlaySecSound(0, state);
			SetState(state, STATE_FIGHT);// TODO: Change this when combat is done
			break;
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			switch (state->buttonPosition) {
				case 0:
					PlaySecSound(1, state);
					SetState(state, STATE_ATTACKMENU);
					break;
				default:
					PlaySecSound(3, state);
					break;
			}
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(1, state);
			break;
		default:
			break;
	}
}
void Cancel(StateData *state) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state);
			break;
		case STATE_MAINMENU:
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(2, state);
			SetState(state, STATE_FIGHT);
			break;
		default:
			break;
	}
}
void SetState(StateData *state, GameState newState) {
	int i;
	UnloadSprite(state->sprites, &state->spritePosition);
	UnloadAnimation(state->anims);
	UnloadButton(state->buttons);
	state->state = newState;
	switch (state->state) {
		case STATE_INIT:
			state->textures[0] = LoadTexture("./resources/gfx/bigSprites00.png");
			state->textures[3] = LoadTexture("./resources/gfx/cards.png");
			state->textures[4] = LoadTexture("./resources/gfx/UI.png");
			state->textures[5] = LoadTexture("./resources/gfx/abilities.png");
			state->textures[6] = LoadTexture("./resources/gfx/attacks.png");
			state->textures[7] = LoadTexture("./resources/gfx/entities.png");
			state->sounds[0] = LoadSound("./resources/sfx/pressStart.mp3");
			state->sounds[1] = LoadSound("./resources/sfx/buttonSelect.wav");
			state->sounds[2] = LoadSound("./resources/sfx/buttonCancel.wav");
			state->sounds[3] = LoadSound("./resources/sfx/error.wav");

			for (i = 0; i < BUTTON_SIZE; i++) state->buttons[i] = NULL;
			for (i = 0; i < ANIM_SIZE; i++) state->anims[i] = NULL;
			for (i = 0; i < SPRITE_SIZE; i++) state->sprites[i]  = NULL;
			for (i = 0; i < MSG_SIZE; i++) state->messages[i] = NULL;
			break;
		case STATE_TITLE:
			LoadSprite("./resources/layout/mainTitle.tsv", &state->spritePosition, SPRITE_SIZE);
			LoadAnimation(1, (Vector2) { 0 });
			break;
		case STATE_MAINMENU:
			LoadSprite("./resources/layout/mainMenu.tsv", &state->spritePosition, SPRITE_SIZE);
			break;
		case STATE_FIGHT:
			LoadButton("./resources/layout/fightButtons.tsv");
			ChangeSelection(state);
			state->buttonSkip = 2;
			break;
		default:
			break;
	}
}
