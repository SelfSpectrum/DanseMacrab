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
//void LowPassFilter(void *buffer, unsigned int frames);		// TODO
//void Crossfade();						// TODO

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
	// Files to load data
	//-------------------------------------------------------------

	//int codepoints[512] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131, 1132, 1133, 1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151, 1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199, 1200, 1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215, 1216, 1217, 1218, 1219, 1220, 1221, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231, 1232, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268, 1269, 1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278};
	//int codepointsAmount = 512;
	Shader shader = LoadShader(0, "contour.fs");
	//Font font = LoadFontEx("./resources/fonts/unifont-14.0.04.ttf", 16, codepoints, codepointsAmount);
	Font font = LoadFont("./resources/fonts/Pixelatus.ttf");

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
					DrawAnimable(state.anims, state.textures, state.animAmount, shader, state.globalColor);
					DrawSprite(state.sprites, state.textures, state.spriteAmount, shader, state.globalColor);
					DrawButton(state.buttons, state.textures, state.buttonAmount, shader, state.globalColor);
					DrawMessage(state.messages, state.messageAmount, font, state.globalColor);
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
	UnloadFont(font);

	printf("Breakpoint Base\n");
	if (state.animsData != NULL) fclose(state.animsData);
	printf("Breakpoint 0\n");
	if (state.spriteData != NULL) fclose(state.spriteData);
	printf("Breakpoint 1\n");
	if (state.charmData != NULL) fclose(state.charmData);
	printf("Breakpoint 2\n");
	if (state.armorData != NULL) fclose(state.armorData);
	printf("Breakpoint 3\n");
	if (state.weaponData != NULL) fclose(state.weaponData);
	printf("Breakpoint 4\n");
	if (state.techData != NULL) fclose(state.techData);
	printf("Breakpoint 5\n");
	if (state.characterData != NULL) fclose(state.characterData);
	printf("Breakpoint 6\n");
	if (state.enemyData != NULL) fclose(state.enemyData);
	printf("Breakpoint 7\n");
	if (state.dialogData != NULL) fclose(state.dialogData);
	printf("Breakpoint 8\n");
	if (state.translationData != NULL) fclose(state.translationData);
	printf("Breakpoint 9\n");

	for (i = 0; i < TEX_SIZE; i++)
		if (state.textures[i].init)
			UnloadTexture(state.textures[i].tex);
	for (i = 0; i < SFXALIAS_SIZE; i++)
		if (state.sfxAlias[i].init)
			UnloadSoundAlias(state.sfxAlias[i].sound);
	for (i = 0; i < SOUND_SIZE; i++)
		if (state.sounds[i].init)
			UnloadSound(state.sounds[i].sound);

	printf("Breakpoint 10\n");

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
	ChangeTranslation(&state->translationData, state->messages, state->messageAmount, state->pref.language);
	SavePrefs(state->pref);
}
void ExtraB(StateData *state) {
	state->pref.language = (Language) (((int) state->pref.language + 1) % 3);
	ChangeTranslation(&state->translationData, state->messages, state->messageAmount, state->pref.language);
	SavePrefs(state->pref);
}
void SetState(StateData *state, GameState newState) {
	int i;
	if (newState != STATE_INIT) {
		UnloadSprite(state->sprites, &state->spriteAmount);
		printf("State Breakpoint 0\n");
		UnloadAnimable(state->anims, &state->animAmount);
		printf("State Breakpoint 1\n");
		UnloadButton(state->buttons, &state->buttonAmount);
		printf("State Breakpoint 2\n");
		UnloadMessage(state->messages, &state->messageAmount);
		printf("State Breakpoint 3\n");
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
			ChangeTranslation(&state->translationData,
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
						state->messages,
						&state->messageAmount,
						MSG_SIZE,
						(Vector2) {160, 158}, // Position
						24, // Font size
						1, // Spacing
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
			LoadButton("./resources/layout/fightButtons.tsv", state->buttons, &state->buttonAmount, BUTTON_SIZE);
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
