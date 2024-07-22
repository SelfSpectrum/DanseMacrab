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
typedef struct PlayerPref PlayerPref;
typedef struct SafeSound SafeSound;

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
struct SafeSound {
	Sound sound;
	bool init;
};
struct PlayerPref {
	bool firstTime;
	char namePref[64];
	Language language;
	float musicVolume;
	float sfxVolume;
};
struct StateData {
	// State
	GameState state;
	bool exitWindowRequested; // Flag to request window to exit
	bool exitWindow; // Flag to set window to exit
	//bool runGame; // To know if the game should run
	int randomValue;
	PlayerPref pref;
	// Visuals
	Color globalColor; // Used to render the white lines in all textures as colors
	Font font;
	SafeTexture textures[TEX_SIZE]; // Here I hold all the texture used in the game
	Animable *anims[ANIM_SIZE]; // Animation handling and rendering
	Sprite *sprites[SPRITE_SIZE]; // INFO: What and where to render
	Message *messages[MSG_SIZE];
	int animAmount;
	int spriteAmount;
	int messageAmount;
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
	SafeSound sounds[SOUND_SIZE]; // Here I hold all the sounds used in the game
	SafeSound sfxAlias[SFXALIAS_SIZE]; // Used to reproduce several sounds at once
	int sfxPosition; // Position to locate one "free" sfxAlias
	// Combat
	Combat combat;
	// Files
	FILE *animsData;
	FILE *spriteData;
	FILE *charmData;
	FILE *armorData;
	FILE *weaponData;
	FILE *techData;
	FILE *characterData;
	FILE *enemyData;
	FILE *dialogData;
	FILE *translationData;
};

// INFO: Input functions
void Accept(StateData *state);
void Cancel(StateData *state);
void ExtraA(StateData *state);
void ExtraB(StateData *state);
void Start(StateData *state);
void Select(StateData *state);
void Menu(StateData *state);
void ChangeSelection(StateData *state);
void SetState(StateData *state, GameState newState);
// INFO: Player Preferences functions
void SavePrefs(PlayerPref prefs);
PlayerPref LoadPrefs();
// INFO: SFX functions
void PlaySecSound(int id, StateData *state);
//void LowPassFilter(void *buffer, unsigned int frames); // TODO
//void Crossfade(); // TODO

int main() {
	//-------------------------------------------------------------
	// Camera & Pixel Perfect
	//-------------------------------------------------------------
	
	const int virtualScreenWidth = 320;
	const int virtualScreenHeight = 180;
	const int screenWidth = 960;
	const int screenHeight = 540;
	InitWindow(screenWidth, screenHeight, "Danse Macrab");
	const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;
	Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	Rectangle sourceRec = { 0.0f, 0.0f, (float) target.texture.width, - (float) target.texture.height };
	Rectangle destRec = { -virtualRatio, -virtualRatio , screenWidth + (virtualRatio * 2), screenHeight + (virtualRatio * 2) };
	Vector2 origin = { 0.0f, 0.0f };
	SetTargetFPS(60);// INFO: Set our game to run at 60 frames-per-second

	//-------------------------------------------------------------
	// Game Inputs and State
	//-------------------------------------------------------------
	
	StateData state; // Contains the current state of the game
	state.exitWindowRequested = false;
	state.exitWindow = false;
	//state.runGame = true;
	SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works
	state.combat = (Combat) { { NULL }, { NULL }, { 0 }, 0, 0 }; // Data from position, entities and stuff
	//combat.playable[2] = LoadEntity("", ENTITY_PLAYER); TODO
	Shader shader = LoadShader(0, "contour.fs");

	int i = 0;

	//-------------------------------------------------------------
	// Audio and Sound
	//-------------------------------------------------------------
	
	InitAudioDevice();

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
			UpdateAnimable(state.anims, &state.animAmount, ANIM_SIZE);
			for (i = 0; i < TEX_SIZE; i++) {
				SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), state.textures[i].tex);
			}
			if (IsKeyPressed(state.startKey)) Start(&state);
			if (IsKeyPressed(state.acceptKey)) Accept(&state);
			if (IsKeyPressed(state.cancelKey)) Cancel(&state);
			if (IsKeyPressed(state.extraAKey)) ExtraA(&state);
			if (IsKeyPressed(state.extraBKey)) ExtraB(&state);
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
					//DrawAnimable(state.anims, state.textures, state.animAmount, shader, state.globalColor);
					//DrawSprite(state.sprites, state.textures, state.spriteAmount, shader, state.globalColor);
					DrawButton(state.buttons, state.textures, state.buttonAmount, shader, state.font, state.globalColor);
					DrawMessage(state.messages, state.messageAmount, state.font, state.globalColor);
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
	printf("Breakpoint base 0\n");
	UnloadShader(shader);
	printf("Breakpoint base 1\n");
	UnloadRenderTexture(target);
	printf("Breakpoint base 2\n");
	UnloadSprite(state.sprites, &state.spriteAmount);
	printf("Breakpoint base 3\n");
	UnloadButton(state.buttons, &state.buttonAmount);
	printf("Breakpoint base 4\n");
	UnloadCombat(&state.combat);
	printf("Breakpoint base 5\n");
	UnloadAnimable(state.anims, &state.animAmount);
	printf("Breakpoint base 6\n");
	UnloadMessage(state.messages, &state.messageAmount);
	printf("Breakpoint base 7\n");
	UnloadMusicStream(state.music); // Unload music stream buffers from RAM
	printf("Breakpoint base 8\n");
	UnloadFont(state.font);
	printf("Breakpoint base 9\n");

	if (state.animsData != NULL) {
		fclose(state.animsData);
		state.animsData = NULL;
		printf("Breakpoint 0\n");
	}
	if (state.spriteData != NULL) {
		fclose(state.spriteData);
		state.spriteData = NULL;
		printf("Breakpoint 1\n");
	}
	if (state.charmData != NULL) {
		fclose(state.charmData);
		state.charmData = NULL;
		printf("Breakpoint 2\n");
	}
	if (state.armorData != NULL) {
		fclose(state.armorData);
		state.armorData = NULL;
		printf("Breakpoint 3\n");
	}
	if (state.weaponData != NULL) {
		fclose(state.weaponData);
		state.weaponData = NULL;
		printf("Breakpoint 4\n");
	}
	if (state.techData != NULL) {
		fclose(state.techData);
		state.techData = NULL;
		printf("Breakpoint 5\n");
	}
	if (state.characterData != NULL) {
		fclose(state.characterData);
		state.characterData = NULL;
		printf("Breakpoint 6\n");
	}
	if (state.enemyData != NULL) {
		fclose(state.enemyData);
		state.enemyData = NULL;
		printf("Breakpoint 7\n");
	}
	if (state.dialogData != NULL) {
		fclose(state.dialogData);
		state.dialogData = NULL;
		printf("Breakpoint 8\n");
	}
	if (state.translationData != NULL) {
		fclose(state.translationData);
		state.translationData = NULL;
		printf("Breakpoint 9\n");
	}

	for (i = 0; i < TEX_SIZE; i++) {
		printf("Tex %d\n", i);
		if (state.textures[i].init) {
			UnloadTexture(state.textures[i].tex);
			state.textures[i].init = false;
		}
	}
	for (i = 0; i < SFXALIAS_SIZE; i++) {
		printf("Sfx %d\n", i);
		if (state.sfxAlias[i].init) {
			UnloadSoundAlias(state.sfxAlias[i].sound);
			state.sfxAlias[i].init = false;
		}
	}
	for (i = 0; i < SOUND_SIZE; i++) {
		printf("Snd %d\n", i);
		if (state.sounds[i].init) {
			UnloadSound(state.sounds[i].sound);
			state.sounds[i].init = false;
		}
	}

	CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
	CloseWindow();              // Close window and OpenGL context

	return 0;
}

void PlaySecSound(int id, StateData *state) {
	id = id % SOUND_SIZE;
	if (state->sfxAlias[state->sfxPosition].init)
		UnloadSoundAlias(state->sfxAlias[state->sfxPosition].sound);
	state->sfxAlias[state->sfxPosition].sound = LoadSoundAlias(state->sounds[id].sound);
	state->sfxAlias[state->sfxPosition].init = true;
	PlaySound(state->sfxAlias[state->sfxPosition].sound);
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
void ExtraA(StateData *state) {
	state->pref.language = (Language) (((int) state->pref.language - 1) % 3);
	if ((int)state->pref.language < 0)
		state->pref.language = (Language) ((int) state->pref.language + 3);
	ChangeTranslation(&state->translationData, &state->font, state->messages, state->messageAmount, state->pref.language);
	SavePrefs(state->pref);
}
void ExtraB(StateData *state) {
	state->pref.language = (Language) (((int) state->pref.language + 1) % 3);
	ChangeTranslation(&state->translationData, &state->font, state->messages, state->messageAmount, state->pref.language);
	SavePrefs(state->pref);
}
void SetState(StateData *state, GameState newState) {
	int i;
	if (newState != STATE_INIT) {
		UnloadSprite(state->sprites, &state->spriteAmount);
		UnloadAnimable(state->anims, &state->animAmount);
		UnloadButton(state->buttons, &state->buttonAmount);
		UnloadMessage(state->messages, &state->messageAmount);
	}
	
	printf("State Breakpoint 4\n");

	state->state = newState;
	printf("INFO: STATE: Loading state %d.\n", (int) newState);
	switch (state->state) {
		case STATE_INIT:
			state->pref = LoadPrefs();

			state->startKey = KEY_ENTER;
			state->selectKey = KEY_BACKSPACE;
			state->acceptKey = KEY_Z;
			state->cancelKey = KEY_X;
			state->leftKey = KEY_LEFT;
			state->rightKey = KEY_RIGHT;
			state->upKey = KEY_UP;
			state->downKey = KEY_DOWN;
			state->extraAKey = KEY_A;
			state->extraBKey = KEY_S;
			
			state->sfxPosition = 0;
			state->animAmount = 0;
			state->spriteAmount = 0;
			state->messageAmount = 0;
			state->buttonAmount = 0;
			state->buttonPosition = 0;
			state->buttonSkip = 0;

			state->globalColor = (Color) {255, 255, 255, 255};

			for (i = 0; i < TEX_SIZE; i++) state->textures[i].init = false;
			for (i = 0; i < SOUND_SIZE; i++) state->sounds[i].init = false;
			for (i = 0; i < SFXALIAS_SIZE; i++) state->sfxAlias[i].init = false;
			for (i = 0; i < BUTTON_SIZE; i++) state->buttons[i] = NULL;
			for (i = 0; i < ANIM_SIZE; i++) state->anims[i] = NULL;
			for (i = 0; i < SPRITE_SIZE; i++) state->sprites[i]  = NULL;
			for (i = 0; i < MSG_SIZE; i++) state->messages[i] = NULL;

			if (FileExists("./resources/anims/animations.tsv"))
				state->animsData = fopen("./resources/anims/animations.tsv", "r");
			else state->animsData = NULL;
			if (FileExists("./resources/gfx/sprites.tsv"))
				state->spriteData = fopen("./resources/gfx/sprites.tsv", "r");
			else state->spriteData = NULL;
			if (FileExists("./resources/combat/charms.tsv"))
				state->charmData = fopen("./resources/combat/charms.tsv", "r");
			else state->charmData = NULL;
			if (FileExists("./resources/combat/armors.tsv"))
				state->armorData = fopen("./resources/combat/armors.tsv", "r");
			else state->armorData = NULL;
			if (FileExists("./resources/combat/weapons.tsv"))
				state->weaponData = fopen("./resources/combat/weapons.tsv", "r");
			else state->weaponData = NULL;
			if (FileExists("./resources/combat/tech.tsv"))
				state->techData = fopen("./resources/combat/tech.tsv", "r");
			else state->techData = NULL;
			if (FileExists("./resources/combat/characters.tsv"))
				state->characterData = fopen("./resources/combat/characters.tsv", "r");
			else state->characterData = NULL;
			if (FileExists("./resources/combat/enemies.tsv"))
				state->enemyData = fopen("./resources/combat/enemies.tsv", "r");
			else state->enemyData = NULL;
			if (FileExists("./resources/text/dialog.tsv"))
				state->dialogData = fopen("./resources/text/dialog.tsv", "r");
			else state->dialogData = NULL;

			state->translationData = NULL;
			state->font = LoadFont("./resources/fonts/Pixelatus.ttf");
			ChangeTranslation(&state->translationData,
					  &state->font,
					  state->messages,
					  state->messageAmount,
					  state->pref.language);

			state->textures[0].tex = LoadTexture("./resources/gfx/bigSprites00.png");
			state->textures[0].init = true;
			state->textures[3].tex = LoadTexture("./resources/gfx/cards.png");
			state->textures[3].init = true;
			state->textures[4].tex = LoadTexture("./resources/gfx/UI.png");
			state->textures[4].init = true;
			state->textures[5].tex = LoadTexture("./resources/gfx/abilities.png");
			state->textures[5].init = true;
			state->textures[6].tex = LoadTexture("./resources/gfx/attacks.png");
			state->textures[6].init = true;
			state->textures[7].tex = LoadTexture("./resources/gfx/entities.png");
			state->textures[7].init = true;

			state->sounds[0].sound = LoadSound("./resources/sfx/pressStart.mp3");
			state->sounds[0].init = true;
			state->sounds[1].sound = LoadSound("./resources/sfx/buttonSelect.wav");
			state->sounds[1].init = true;
			state->sounds[2].sound = LoadSound("./resources/sfx/buttonCancel.wav");
			state->sounds[2].init = true;
			state->sounds[3].sound = LoadSound("./resources/sfx/error.wav");
			state->sounds[3].init = true;

			SetState(state, STATE_TITLE);
			break;
		case STATE_TITLE:
			LoadSprite("./resources/layout/mainTitle.tsv", state->sprites, &state->spriteAmount, SPRITE_SIZE);
			LoadAnimable(state->animsData, state->anims, (Vector2) { 0 }, &state->animAmount, ANIM_SIZE, 1);
			LoadMessageIntoRegister(state->translationData,
						&state->font,
						state->messages,
						&state->messageAmount,
						MSG_SIZE,
						(Vector2) {160, 154}, // Position
						16, // Font size
						0, // Spacing
						true, ALIGN_CENTER, 1);

			state->music = LoadMusicStream("./resources/sfx/title.mp3");
			state->music.looping = true;

			PlayMusicStream(state->music);
			SetMusicVolume(state->music, 1.0f);

			break;
		case STATE_MAINMENU:
			LoadSprite("./resources/layout/mainMenu.tsv", state->sprites, &state->spriteAmount, SPRITE_SIZE);
			break;
		case STATE_FIGHT:
			LoadButton("./resources/layout/fightButtons.tsv", state->translationData, &state->font, state->buttons, &state->buttonAmount, BUTTON_SIZE);
			ChangeSelection(state);
			state->buttonSkip = 2;
			break;
		default:
			break;
	}
}
void SavePrefs(PlayerPref prefs) {
	char buffer[512]; // Big buffer to save all data from the user and ensure it gets saved properly
	sprintf(buffer, "fsp=%d\nname=%s\nlang=%d\nmus=%.2f\nsfx=%.2f",
			(int) prefs.firstTime,
			prefs.namePref,
			(int) prefs.language,
			prefs.musicVolume,
			prefs.sfxVolume);
	SaveFileText("PlayerPrefs.data", buffer);
}
PlayerPref LoadPrefs() {
	PlayerPref prefs = { true, "AAA", 0, 0.0f, 0.0f }; // Default preferences
	if (!FileExists("PlayerPrefs.data")) {
		printf("INFO: PREFS: Prefs file does not exist, creating one.\n");
		SavePrefs(prefs);
		return prefs;
	}
	char *buffer = LoadFileText("PlayerPrefs.data");
	if (buffer == NULL) {
		//TODO: Remember to drop an error when this happen and to abort, informing it and to choose to reload or override
		printf("INFO: PREFS: Prefs file loading error, returning default.\n");
		return prefs;
	}
	int ftp;
	int lang;
	sscanf(buffer, "fsp=%d\nname=%s\nlang=%d\nmus=%f\nsfx=%f",
			&ftp,
			prefs.namePref,
			&lang,
			&prefs.musicVolume,
			&prefs.sfxVolume);
	prefs.firstTime = (bool) ftp;
	prefs.language = (Language) lang;
	UnloadFileText(buffer); // Unload this file data
	return prefs;
}
