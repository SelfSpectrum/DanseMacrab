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
	int textSpeed;
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
void PlaySecSound(StateData *state, int id);
//void LowPassFilter(void *buffer, unsigned int frames); // TODO
//void Crossfade(); // TODO
// INFO: Some shortcuts for graphics
void SetHorizontalKeys(StateData *state);
void SetVerticalKeys(StateData *state);
void SetOnlyVerticalKeys(StateData *state);
void LoadSprite(StateData *state, Vector2 position, float rotation, int id);
void LoadButton(StateData *state, Vector2 position, float rotation, int idOff, int idOn, int idMessage);
void LoadMessage(StateData *state, Vector2 position, float fontSize, float spacing, bool useColor, Align align, int id);

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

	SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

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
					DrawCombat(state.combat, state.textures, shader, state.globalColor);
					DrawAnimable(state.anims, state.textures, state.animAmount, shader, state.globalColor);
					DrawSprite(state.sprites, state.textures, state.spriteAmount, shader, state.globalColor);
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
	UnloadShader(shader);
	UnloadRenderTexture(target);
	UnloadSprite(state.sprites, &state.spriteAmount);
	UnloadButton(state.buttons, &state.buttonAmount);
	UnloadCombat(&state.combat);
	UnloadAnimable(state.anims, &state.animAmount);
	UnloadMessage(state.messages, &state.messageAmount);
	UnloadMusicStream(state.music); // Unload music stream buffers from RAM
	UnloadFont(state.font);

	if (state.animsData != NULL) {
		fclose(state.animsData);
		state.animsData = NULL;
	}
	if (state.spriteData != NULL) {
		fclose(state.spriteData);
		state.spriteData = NULL;
	}
	if (state.charmData != NULL) {
		fclose(state.charmData);
		state.charmData = NULL;
	}
	if (state.armorData != NULL) {
		fclose(state.armorData);
		state.armorData = NULL;
	}
	if (state.weaponData != NULL) {
		fclose(state.weaponData);
		state.weaponData = NULL;
	}
	if (state.techData != NULL) {
		fclose(state.techData);
		state.techData = NULL;
	}
	if (state.characterData != NULL) {
		fclose(state.characterData);
		state.characterData = NULL;
	}
	if (state.enemyData != NULL) {
		fclose(state.enemyData);
		state.enemyData = NULL;
	}
	if (state.dialogData != NULL) {
		fclose(state.dialogData);
		state.dialogData = NULL;
	}
	if (state.translationData != NULL) {
		fclose(state.translationData);
		state.translationData = NULL;
	}

	for (i = 0; i < TEX_SIZE; i++) {
		//printf("Tex %d\n", i);
		if (state.textures[i].init) {
			UnloadTexture(state.textures[i].tex);
			state.textures[i].init = false;
		}
	}
	for (i = 0; i < SFXALIAS_SIZE; i++) {
		//printf("Sfx %d\n", i);
		if (state.sfxAlias[i].init) {
			UnloadSoundAlias(state.sfxAlias[i].sound);
			state.sfxAlias[i].init = false;
		}
	}
	for (i = 0; i < SOUND_SIZE; i++) {
		//printf("Snd %d\n", i);
		if (state.sounds[i].init) {
			UnloadSound(state.sounds[i].sound);
			state.sounds[i].init = false;
		}
	}

	CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
	CloseWindow();              // Close window and OpenGL context

	return 0;
}

void PlaySecSound(StateData *state, int id) {
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
	switch (state->state) {
		case STATE_SELECTLANGUAGE:
			switch (state->buttonPosition) {
				case 0:
					state->pref.language = (Language) 0;
					ChangeTranslation(&state->translationData,
							  state->font,
							  state->messages,
							  state->messageAmount,
							  state->buttons,
							  state->buttonAmount,
							  state->pref.language);
					break;
				case 1:
					state->pref.language = (Language) 1;
					ChangeTranslation(&state->translationData,
							  state->font,
							  state->messages,
							  state->messageAmount,
							  state->buttons,
							  state->buttonAmount,
							  state->pref.language);
					break;
				case 2:
					state->pref.language = (Language) 2;
					ChangeTranslation(&state->translationData,
							  state->font,
							  state->messages,
							  state->messageAmount,
							  state->buttons,
							  state->buttonAmount,
							  state->pref.language);
					break;
				default:
					break;
			}
			break;
		case STATE_FIGHT:
			switch (state->buttonPosition) {
				case 0:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 5);
					break;
				case 1:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 6);
					break;
				case 2:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 81);
					break;
				case 3:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 7);
					break;
				case 4:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 8);
					break;
				case 5:
					UnloadMessage(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {146, 128}, 18, 0, true, ALIGN_LEFT, 80);
					break;
				default:
					UnloadMessage(state->messages, &state->messageAmount);
					break;
			}
			break;
		default:
			break;
	}
}
void Start(StateData *state) {
	switch (state->state) {
		case STATE_SELECTLANGUAGE:
			Accept(state);
			break;
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
			PlaySecSound(state, 0);
			SetState(state, STATE_FIGHT);
			break;
		case STATE_SELECTLANGUAGE:
			state->pref.firstTime = false;
			SavePrefs(state->pref);
			SetState(state, STATE_FIGHT);
			break;
		case STATE_MAINMENU:
			break;
		case STATE_FIGHT:
			switch (state->buttonPosition) {
				case 0:
					PlaySecSound(state, 1);
					SetState(state, STATE_ATTACKMENU);
					break;
				default:
					PlaySecSound(state, 3);
					break;
			}
			break;
		case STATE_ATTACKMENU:
			PlaySecSound(state, 1);
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
			PlaySecSound(state, 2);
			SetState(state, STATE_FIGHT);
			break;
		default:
			break;
	}
}
void ExtraA(StateData *state) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state);
			break;
		default:
			break;
	}
	/*
	state->pref.language = (Language) (((int) state->pref.language - 1) % 3);
	if ((int)state->pref.language < 0)
		state->pref.language = (Language) ((int) state->pref.language + 3);
	ChangeTranslation(&state->translationData,
			  state->font,
			  state->messages,
			  state->messageAmount,
			  state->buttons,
			  state->buttonAmount,
			  state->pref.language);
	SavePrefs(state->pref);
	*/
}
void ExtraB(StateData *state) {
	switch (state->state) {
		case STATE_TITLE:
			Accept(state);
			break;
		default:
			break;
	}
	/*
	state->pref.language = (Language) (((int) state->pref.language + 1) % 3);
	ChangeTranslation(&state->translationData,
			  state->font,
			  state->messages,
			  state->messageAmount,
			  state->buttons,
			  state->buttonAmount,
			  state->pref.language);
	SavePrefs(state->pref);
	*/
}
void SetState(StateData *state, GameState newState) {
	int i;
	if (newState != STATE_INIT) {
		UnloadSprite(state->sprites, &state->spriteAmount);
		UnloadAnimable(state->anims, &state->animAmount);
		UnloadButton(state->buttons, &state->buttonAmount);
		UnloadMessage(state->messages, &state->messageAmount);
	}
	state->state = newState;
	printf("INFO: STATE: Loading state %d.\n", (int) newState);
	switch (state->state) {
		case STATE_INIT:
			state->pref = LoadPrefs();

			state->exitWindowRequested = false;
			state->exitWindow = false;
			//state.runGame = true;

			state->startKey = KEY_ENTER;
			state->selectKey = KEY_BACKSPACE;
			state->acceptKey = KEY_Z;
			state->cancelKey = KEY_X;
			SetHorizontalKeys(state);
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
			if (FileExists("./resources/combat/techniques.tsv"))
				state->techData = fopen("./resources/combat/techniques.tsv", "r");
			else state->techData = NULL;
			if (FileExists("./resources/combat/characters.tsv"))
				state->characterData = fopen("./resources/combat/characters.tsv", "r");
			else state->characterData = NULL;
			if (FileExists("./resources/combat/enemies.tsv"))
				state->enemyData = fopen("./resources/combat/enemies.tsv", "r");
			else state->enemyData = NULL;
			if (FileExists("./resources/text/dialogs.tsv"))
				state->dialogData = fopen("./resources/text/dialogs.tsv", "r");
			else state->dialogData = NULL;

			state->translationData = NULL;
			int codepoints[210] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 160, 1050, 1051, 1052, 176, 1053, 1054, 1055, 191, 1025, 193, 1056, 1057, 201, 1058, 205, 209, 1059, 211, 1060, 215, 218, 1061, 1062, 225, 1063, 233, 1064, 237, 1065, 241, 243, 1066, 247, 1067, 250, 1068, 1069, 1070, 1071, 1072, 1040, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1105};
			state->font = LoadFontEx("./resources/fonts/Pixel-UniCode.ttf", 1024, codepoints, 210);

			ChangeTranslation(&state->translationData,
					  state->font,
					  state->messages,
					  state->messageAmount,
					  state->buttons,
					  state->buttonAmount,
					  state->pref.language);

			state->textures[0].tex = LoadTexture("./resources/gfx/bigSprites00.png");
			state->textures[0].init = true;
			state->textures[2].tex = LoadTexture("./resources/gfx/characters.png");
			state->textures[2].init = true;
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

			state->combat = (Combat) { { NULL }, { NULL }, NULL, { 0 }, 0, 0 }; // Data from position, entities and stuff
			state->combat.player[2] = LoadPlayer(state->characterData, state->spriteData, state->weaponData, state->armorData, state->charmData, state->techData, 1);
			state->combat.player[2]->player.position = 2;

			if (state->pref.firstTime)
				SetState(state, STATE_SELECTLANGUAGE);
			else
				SetState(state, STATE_TITLE);

			break;
		case STATE_TITLE:
			//LoadSpriteFromFile("./resources/layout/mainTitle.tsv", state->spriteData, state->sprites, &state->spriteAmount, SPRITE_SIZE);
			LoadSprite(state, (Vector2) { -80, -38 }, 0, 2);
			LoadSprite(state, (Vector2) { -60, -140 }, 0, 7);
			//LoadAnimable(state->animsData, state->anims, (Vector2) { 0 }, &state->animAmount, ANIM_SIZE, 1);
			LoadMessage(state, (Vector2) { 161, 154 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 155 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 159, 154 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 153 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 154 }, 18, 0, true, ALIGN_CENTER, 1);

			state->music = LoadMusicStream("./resources/sfx/title.mp3");
			state->music.looping = true;

			PlayMusicStream(state->music);
			SetMusicVolume(state->music, 1.0f);

			break;
		case STATE_SELECTLANGUAGE:
			SetOnlyVerticalKeys(state);

			LoadMessage(state, (Vector2) { 160, 20 }, 18, 0, true, ALIGN_CENTER, 100);
			LoadButton(state, (Vector2) { -138, -100 }, 0, 10, 11, 102);
			LoadButton(state, (Vector2) { -138, -116 }, 0, 10, 11, 101);
			LoadButton(state, (Vector2) { -138, -132 }, 0, 10, 11, 103);
			ChangeSelection(state);

			break;
		case STATE_MAINMENU:
			//LoadSprite("./resources/layout/mainMenu.tsv", state->spriteData, state->sprites, &state->spriteAmount, SPRITE_SIZE);
			break;
		case STATE_FIGHT:
			SetHorizontalKeys(state);
			LoadSprite(state, (Vector2) { 0, -132 }, 0, 102);
			//LoadSprite(state, (Vector2) { 0, -86 }, 100);
			//LoadButton("./resources/layout/fightButtons.tsv", state->spriteData, state->translationData, state->font, state->buttons, &state->buttonAmount, BUTTON_SIZE);
			LoadButton(state, (Vector2) { -112, -132 }, 0, 756, 884, 0);
			LoadButton(state, (Vector2) { -128, -132 }, 0, 772, 900, 0);
			LoadButton(state, (Vector2) { -112, -148 }, 0, 777, 905, 0);
			LoadButton(state, (Vector2) { -128, -148 }, 0, 762, 890, 0);
			LoadButton(state, (Vector2) { -112, -164 }, 0, 757, 885, 0);
			LoadButton(state, (Vector2) { -128, -164 }, 0, 515, 643, 0);
			ChangeSelection(state);
			state->buttonSkip = 2;
			break;
		default:
			break;
	}
}
void SavePrefs(PlayerPref prefs) {
	char buffer[512]; // Big buffer to save all data from the user and ensure it gets saved properly
	sprintf(buffer, "fsp=%d\nname=%s\nlang=%d\nmus=%.2f\nsfx=%.2f\ntext=%d",
			(int) prefs.firstTime,
			prefs.namePref,
			(int) prefs.language,
			prefs.musicVolume,
			prefs.sfxVolume,
			prefs.textSpeed);
	SaveFileText("PlayerPrefs.data", buffer);
}
PlayerPref LoadPrefs() {
	PlayerPref prefs = { true, "AAA", 0, 0.0f, 0.0f, 5 }; // Default preferences
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
	sscanf(buffer, "fsp=%d\nname=%s\nlang=%d\nmus=%f\nsfx=%f\ntext=%d",
			&ftp,
			prefs.namePref,
			&lang,
			&prefs.musicVolume,
			&prefs.sfxVolume,
			&prefs.textSpeed);
	prefs.firstTime = (bool) ftp;
	prefs.language = (Language) lang;
	UnloadFileText(buffer); // Unload this file data
	return prefs;
}
void SetHorizontalKeys(StateData *state) {
	state->leftKey = KEY_LEFT;
	state->rightKey = KEY_RIGHT;
	state->upKey = KEY_UP;
	state->downKey = KEY_DOWN;
}
void SetVerticalKeys(StateData *state) {
	state->leftKey = KEY_UP;
	state->rightKey = KEY_DOWN;
	state->upKey = KEY_LEFT;
	state->downKey = KEY_RIGHT;
}
void SetOnlyVerticalKeys(StateData *state) {
	state->leftKey = KEY_UP;
	state->rightKey = KEY_DOWN;
	state->upKey = KEY_NULL;
	state->downKey = KEY_NULL;
}
void LoadSprite(StateData *state, Vector2 position, float rotation, int id) {
	LoadSpriteIntoRegister(state->spriteData, state->sprites, &state->spriteAmount, SPRITE_SIZE, position, rotation, id);
}
void LoadButton(StateData *state, Vector2 position, float rotation, int idOff, int idOn, int idMessage) {
	LoadButtonIntoRegister(state->spriteData, state->translationData, state->font, state->buttons, &state->buttonAmount, BUTTON_SIZE, position, rotation, idOff, idOn, idMessage);
}
void LoadMessage(StateData *state, Vector2 position, float fontSize, float spacing, bool useColor, Align align, int id) {
	LoadMessageIntoRegister(state->translationData,
				state->font,
				state->messages,
				&state->messageAmount,
				MSG_SIZE,
				position,
				fontSize,
				spacing,
				useColor,
				align,
				id);
}
