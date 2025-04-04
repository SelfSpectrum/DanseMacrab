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
	STATE_INIT, // Inicialziacion y carga del estado del juego y todos los archivos necesarios, si hay errores de archivos, se detectan aquí
	STATE_SELECTLANGUAGE,
	STATE_INITIALSCENE,
	STATE_TITLE,
	STATE_MAINMENU,
	STATE_SELECTCARDS,
	STATE_FIGHT,
	STATE_ATTACKMENU,
	STATE_FIGHTTIMELINE,
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
	float generalVolume;
	float musicVolume;
	float sfxVolume;
	int textSpeed;
};
struct StateData {
	// Estado
	GameState state;
	bool onCombat;
	bool exitWindowRequested; // Bandera para solicitar que se cierre la ventana
	bool exitWindow; // Bandera para establecer que se ha de cerrar la ventana
	//bool runGame; // Útil para saber si el juego debería ejecutarse, ideal para errores enmedio de la ejecución
	bool debug;
	int randomValue;
	PlayerPref pref;
	// Visuals
	Color globalColor; // Utilizado para renderizar colores sobre todas las texturas
	Font font;
	SafeTexture textures[TEX_SIZE]; // Todas las texturas que se utilizan durante el tiempo de ejecución se mantienen aquí
	Animation *anims[ANIM_SIZE];
	Sprite *sprites[SPRITE_SIZE];
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
	SafeSound sounds[SOUND_SIZE]; // Todas los sonidos que se utilizan durante el tiempo de ejecución se mantienen aquí
	SafeSound sfxAlias[SFXALIAS_SIZE]; // Un array de sonidos para copiar referencias de "sounds", así se pueden tener efectos de sonido en paralelo
	int sfxPosition; // Posición para localizar un sfxAlias "libre"
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
	FILE *combatData;
};

// INFO: Funciones para las entradas de datos (botones físicos y tal)
void Accept(StateData *state);
void Cancel(StateData *state);
void ExtraA(StateData *state);
void ExtraB(StateData *state);
void Start(StateData *state);
void Select(StateData *state);
void Menu(StateData *state);
void ChangeSelection(StateData *state);
void SetState(StateData *state, GameState newState);
// INFO: Funciones para trabajar con las preferencias del jugador
void SavePrefs(PlayerPref prefs);
PlayerPref LoadPrefs();
// INFO: Funciones para trabajar con audio y efectos de sonido
void PlaySecSound(StateData *state, int id);
//void LowPassFilter(void *buffer, unsigned int frames); // TODO
//void Crossfade(); // TODO
// INFO: Some shortcuts for graphics
void SetHorizontalKeys(StateData *state);
void SetOnlyHorizontalKeys(StateData *state);
void SetVerticalKeys(StateData *state);
void SetOnlyVerticalKeys(StateData *state);
void LoadFiles(StateData *state);
void LoadSprite(StateData *state, Vector2 position, float rotation, int id);
void LoadAnimation(StateData *state, Vector2 position, float rotation, int id);
void LoadButton(StateData *state, Vector2 position, float rotation, int idOff, int idOn, int idMessage);
void LoadMessage(StateData *state, Vector2 position, float fontSize, float spacing, bool useColor, Align align, int id);

int main() {
	//-------------------------------------------------------------
	// Cámara y efecto de Píxeles Perfectos
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
	SetTargetFPS(60);// INFO: Establece al juego para que corra a 60 FPS

	//-------------------------------------------------------------
	// Inputs del juego y estado del mismo
	//-------------------------------------------------------------
	
	StateData state; // Contiene el estado actual del juego

	SetExitKey(KEY_NULL); // Desabilita la el cerrado del juego con la tecla ESC, el boton X aún funciona

	Shader shader = LoadShader(0, "contour.fs");

	int i = 0; // Embeces hay que iterar sobre arreglos, como en el caso de las texturas o sonidos cargados

	//-------------------------------------------------------------
	// Audio y sonido
	//-------------------------------------------------------------
	
	InitAudioDevice();

	SetState(&state, STATE_INIT);

	while (!state.exitWindow) {

		//-------------------------------------------------------------
		// INFO: Bloque de Actualizado: Aquí se realizan cálculos y eventos los cuales no afecten el Texturizado ni el Dibujado de manera directa
		//-------------------------------------------------------------

		UpdateMusicStream(state.music);// Actualiza el buffer de la música con nueva información
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) state.exitWindowRequested = true;// Detecta si se presiona la X o KEY_ESCAPE para cerrar el juego
		if (state.exitWindowRequested) {
			// Una solicitud para cerrar la ventana se ha envíado, aquí ahora se puede pedir confirmación y guardar cambios.
			if (IsKeyPressed(state.acceptKey) || IsKeyPressed(state.startKey)) state.exitWindow = true;
			else if (IsKeyPressed(state.cancelKey)) state.exitWindowRequested = false;
		}
		else {
			UpdateAnimation(state.animsData, state.spriteData, state.anims, &state.animAmount);
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
		// INFO: Bloque de Texturizado: Aquí se renderiza todos los contenidos que capta la cámara sobre una textura pequeña, bastante útil para preservar el aspecto de los píxeles.
		//-------------------------------------------------------------

		BeginTextureMode(target);
			ClearBackground(BLACK);
			BeginMode2D(worldSpaceCamera);
				if (state.exitWindowRequested) {
					DrawText("Are you sure you want to exit program?", 50, 90, 8, state.globalColor);
				}
				else {
					DrawCombat(&state.combat, state.textures, state.globalColor, false, state.onCombat);
					DrawAnimation(state.anims, state.textures, state.animAmount, state.globalColor, false);
					DrawSprite(state.sprites, state.textures, state.spriteAmount, state.globalColor, false);
					DrawButton(state.buttons, state.textures, state.buttonAmount, state.font, state.globalColor, false);

					BeginShaderMode(shader);
						DrawCombat(&state.combat, state.textures, state.globalColor, true, state.onCombat);
						DrawAnimation(state.anims, state.textures, state.animAmount, state.globalColor, true);
						DrawSprite(state.sprites, state.textures, state.spriteAmount, state.globalColor, true);
						DrawButton(state.buttons, state.textures, state.buttonAmount, state.font, state.globalColor, true);
					EndShaderMode();
					
					DrawMessage(state.messages, state.messageAmount, state.font, state.globalColor);
				}
			EndMode2D();
		EndTextureMode();

		//-------------------------------------------------------------
		// INFO: Bloque de Dibujado: Los contenidos de la cámara son renderizados en un nuevo tamaño para tener una resolución deseada sin afectar al resto del juego.
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
	UnloadSpriteRegister(state.sprites, &state.spriteAmount);
	UnloadButtonRegister(state.buttons, &state.buttonAmount);
	UnloadCombat(&state.combat);
	UnloadAnimationRegister(state.anims, &state.animAmount);
	UnloadMessageRegister(state.messages, &state.messageAmount);
	UnloadMusicStream(state.music); // Se descargan los buffers para los canales de música de la RAM
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
	if (state.combatData != NULL) {
		fclose(state.combatData);
		state.combatData = NULL;
	}

	for (i = 0; i < TEX_SIZE; i++) {
		if (state.textures[i].init) {
			UnloadTexture(state.textures[i].tex);
			state.textures[i].init = false;
		}
	}
	for (i = 0; i < SFXALIAS_SIZE; i++) {
		if (state.sfxAlias[i].init) {
			UnloadSoundAlias(state.sfxAlias[i].sound);
			state.sfxAlias[i].init = false;
		}
	}
	for (i = 0; i < SOUND_SIZE; i++) {
		if (state.sounds[i].init) {
			UnloadSound(state.sounds[i].sound);
			state.sounds[i].init = false;
		}
	}

	CloseAudioDevice(); // Cerrar el dispositivo de audio (también para el canal de música)
	CloseWindow(); // Cierra la ventana y el contexto de OpenGL

	return 0;
}

void PlaySecSound(StateData *state, int id) { // INFO: Para permitir sonidos de manera secuencial, sin que se interrumpan entre ellos
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
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 5);
					break;
				case 1:
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 6);
					break;
				case 2:
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 13);
					break;
				case 3:
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 7);
					break;
				case 4:
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 98);
					break;
				case 5:
					UnloadMessageRegister(state->messages, &state->messageAmount);
					LoadMessage(state, (Vector2) {147, 128}, 18, 0, true, ALIGN_LEFT, 80);
					break;
				default:
					UnloadMessageRegister(state->messages, &state->messageAmount);
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
		case STATE_DEBUG:
			LoadFiles(state);
			break;
		case STATE_TITLE:
			Accept(state);
			break;
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
		UnloadSpriteRegister(state->sprites, &state->spriteAmount);
		UnloadAnimationRegister(state->anims, &state->animAmount);
		UnloadButtonRegister(state->buttons, &state->buttonAmount);
		UnloadMessageRegister(state->messages, &state->messageAmount);
	}
	state->state = newState;
	TraceLog(LOG_INFO, "INFO: STATE: Loading state %d.\n", (int) newState);
	switch (state->state) {
		case STATE_INIT:
			state->pref = LoadPrefs();

			state->onCombat = false;
			state->exitWindowRequested = false;
			state->exitWindow = false;
			//state.runGame = true;
			//state->debug = true;
			state->debug = false;

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
			
			// Carga de todos los archivos/base de datos
			state->animsData = NULL;
			state->spriteData = NULL;
			state->charmData = NULL;
			state->armorData = NULL;
			state->weaponData = NULL;
			state->techData = NULL;
			state->characterData = NULL;
			state->enemyData = NULL;
			state->dialogData = NULL;
			state->translationData = NULL;
			state->combatData = NULL;

			LoadFiles(state);

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
			
			state->music = LoadMusicStream("./resources/sfx/title.mp3");
			state->music.looping = true;

			state->sounds[0].sound = LoadSound("./resources/sfx/pressStart.mp3");
			state->sounds[0].init = true;
			state->sounds[1].sound = LoadSound("./resources/sfx/buttonSelect.wav");
			state->sounds[1].init = true;
			state->sounds[2].sound = LoadSound("./resources/sfx/buttonCancel.wav");
			state->sounds[2].init = true;
			state->sounds[3].sound = LoadSound("./resources/sfx/error.wav");
			state->sounds[3].init = true;

			state->combat = (Combat) { { NULL }, { NULL }, // Enemigos y personajes jugables
						   { NULL }, { ENTITY_NONE }, 0, 0, // Línea de tiempo
						   { 0 }, 0, 0 }; // Inventario y turnos
			state->combat.player[1] = LoadPlayer(state->characterData, state->spriteData, state->weaponData, state->armorData, state->charmData, state->techData, 2, 1);
			state->combat.player[2] = LoadPlayer(state->characterData, state->spriteData, state->weaponData, state->armorData, state->charmData, state->techData, 1, 2);
			state->combat.player[3] = LoadPlayer(state->characterData, state->spriteData, state->weaponData, state->armorData, state->charmData, state->techData, 3, 3);
			StartCombat(state->combatData, state->enemyData, state->spriteData, state->techData, 1, &state->combat, &state->randomValue);

			if (state->debug) SetState(state, STATE_DEBUG);
			else {
				if (state->pref.firstTime)
					SetState(state, STATE_SELECTLANGUAGE);
				else
					SetState(state, STATE_TITLE);
			}

			break;
		case STATE_DEBUG:
			LoadAnimation(state, (Vector2) { 120, 120 }, 0, 11);
			LoadAnimation(state, (Vector2) { 120, 60 }, 0, 13);
			LoadAnimation(state, (Vector2) { 60, 60 }, 0, 14);
			break;
		case STATE_TITLE:
			LoadSprite(state, (Vector2) { -80, -38 }, 0, 2);
			LoadSprite(state, (Vector2) { -60, -140 }, 0, 7);
			LoadAnimation(state, (Vector2) { 0 }, 0, 1);
			LoadMessage(state, (Vector2) { 161, 154 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 155 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 159, 154 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 153 }, 18, 0, false, ALIGN_CENTER, 1);
			LoadMessage(state, (Vector2) { 160, 154 }, 18, 0, true, ALIGN_CENTER, 1);

			PlayMusicStream(state->music);
			SetMusicVolume(state->music, 1.0f);

			break;
		case STATE_SELECTLANGUAGE:
			SetOnlyVerticalKeys(state);

			LoadMessage(state, (Vector2) { 160, 20 }, 18, 0, true, ALIGN_CENTER, 100);
			LoadButton(state, (Vector2) { -128, -100 }, 0, 10, 11, 102);
			LoadButton(state, (Vector2) { -128, -116 }, 0, 10, 11, 101);
			LoadButton(state, (Vector2) { -128, -132 }, 0, 10, 11, 103);
			ChangeSelection(state);

			break;
		case STATE_MAINMENU:
			//LoadSprite("./resources/layout/mainMenu.tsv", state->spriteData, state->sprites, &state->spriteAmount, SPRITE_SIZE);
			break;
		case STATE_FIGHT:
			SetHorizontalKeys(state);
			state->onCombat = true;
			//LoadSprite(state, (Vector2) { 0, -131 }, 0, 102);
			//LoadButton("./resources/layout/fightButtons.tsv", state->spriteData, state->translationData, state->font, state->buttons, &state->buttonAmount, BUTTON_SIZE);
			LoadButton(state, (Vector2) { -112, -131 }, 0, 756, 884, 0);
			LoadButton(state, (Vector2) { -128, -131 }, 0, 772, 900, 0);
			LoadButton(state, (Vector2) { -112, -147 }, 0, 777, 905, 0);
			LoadButton(state, (Vector2) { -128, -147 }, 0, 762, 890, 0);
			LoadButton(state, (Vector2) { -112, -163 }, 0, 766, 894, 0);
			LoadButton(state, (Vector2) { -128, -163 }, 0, 515, 643, 0);
			ChangeSelection(state);
			state->buttonSkip = 2;
			break;
		case STATE_ATTACKMENU:
			SetOnlyHorizontalKeys(state);
			
			break;
		case STATE_FIGHTTIMELINE:
			break;
		default:
			break;
	}
}
void SavePrefs(PlayerPref prefs) {
	char buffer[512]; // Un buffer gigante para guardar todos los datos que el usuario posee y asegurar que se guardan apropiadamente
	sprintf(buffer, "fsp=%d\nname=%s\nlang=%d\nvol=%.2f\nmus=%.2f\nsfx=%.2f\ntext=%d",
			(int) prefs.firstTime,
			prefs.namePref,
			(int) prefs.language,
			prefs.generalVolume,
			prefs.musicVolume,
			prefs.sfxVolume,
			prefs.textSpeed);
	SaveFileText("PlayerPrefs.data", buffer);
}
PlayerPref LoadPrefs() {
	PlayerPref prefs = { true, "Survivor", 0, 0.0f, 0.0f, 0.0f, 5 }; // Preferencias por defecto
	if (!FileExists("PlayerPrefs.data")) {
		TraceLog(LOG_INFO, "INFO: PREFS: Prefs file does not exist, creating one.\n");
		SavePrefs(prefs);
		return prefs;
	}
	char *buffer = LoadFileText("PlayerPrefs.data");
	if (buffer == NULL) {
		//TODO: Recuerda crear un error cuando esto ocurra, para abortar la carga de datos, informando si se quiere recargar o sobreescribir
		TraceLog(LOG_INFO, "INFO: PREFS: Prefs file loading error, returning default.\n");
		return prefs;
	}
	int ftp;
	int lang;
	sscanf(buffer, "fsp=%d\nname=%s\nlang=%d\nvol=%f\nmus=%f\nsfx=%f\ntext=%d",
			&ftp,
			prefs.namePref,
			&lang,
			&prefs.generalVolume,
			&prefs.musicVolume,
			&prefs.sfxVolume,
			&prefs.textSpeed);
	prefs.firstTime = (bool) ftp;
	prefs.language = (Language) lang;
	UnloadFileText(buffer); // Descargo el archivo de datos
	return prefs;
}
void SetHorizontalKeys(StateData *state) {
	state->leftKey = KEY_LEFT;
	state->rightKey = KEY_RIGHT;
	state->upKey = KEY_UP;
	state->downKey = KEY_DOWN;
}
void SetOnlyHorizontalKeys(StateData *state) {
	state->leftKey = KEY_LEFT;
	state->rightKey = KEY_RIGHT;
	state->upKey = KEY_NULL;
	state->downKey = KEY_NULL;
}
void SetVerticalKeys(StateData *state) {
	state->leftKey = KEY_UP;
	state->rightKey = KEY_DOWN;
	state->upKey = KEY_LEFT;
	state->downKey = KEY_RIGHT;
}
// INFO: Útil cuando se necesita recorrer columnas de botones
void SetOnlyVerticalKeys(StateData *state) {
	state->leftKey = KEY_UP;
	state->rightKey = KEY_DOWN;
	state->upKey = KEY_NULL;
	state->downKey = KEY_NULL;
}
void LoadFiles(StateData *state) {
	// Estableciendo el archivo para cargar animaciones
	if (state->animsData != NULL) {
		fclose(state->animsData);
		state->animsData = NULL;
	}
	if (FileExists("./resources/anims/animations.tsv"))
		state->animsData = fopen("./resources/anims/animations.tsv", "r");
	else state->animsData = NULL;

	// Estableciendo el archivo para cargar sprites
	if (state->spriteData != NULL) {
		fclose(state->spriteData);
		state->spriteData = NULL;
	}
	if (FileExists("./resources/gfx/sprites.tsv"))
		state->spriteData = fopen("./resources/gfx/sprites.tsv", "r");
	else state->spriteData = NULL;

	// Estableciendo el archivo para cargar amuletos
	if (state->charmData != NULL) {
		fclose(state->charmData);
		state->charmData = NULL;
	}
	if (FileExists("./resources/combat/charms.tsv"))
		state->charmData = fopen("./resources/combat/charms.tsv", "r");
	else state->charmData = NULL;

	// Estableciendo el archivo para cargar armaduras
	if (state->armorData != NULL) {
		fclose(state->armorData);
		state->armorData = NULL;
	}
	if (FileExists("./resources/combat/armors.tsv"))
		state->armorData = fopen("./resources/combat/armors.tsv", "r");
	else state->armorData = NULL;

	// Establenciendo el archivo para cargar armas
	if (state->weaponData != NULL) {
		fclose(state->weaponData);
		state->weaponData = NULL;
	}
	if (FileExists("./resources/combat/weapons.tsv"))
		state->weaponData = fopen("./resources/combat/weapons.tsv", "r");
	else state->weaponData = NULL;

	// Estableciendo el archivo para cargar técnicas
	if (state->techData != NULL) {
		fclose(state->techData);
		state->techData = NULL;
	}
	if (FileExists("./resources/combat/techniques.tsv"))
		state->techData = fopen("./resources/combat/techniques.tsv", "r");
	else state->techData = NULL;

	// Estableciendo el archivo para cargar la información de los personajes jugables 
	if (state->characterData) {
		fclose(state->characterData);
		state->characterData = NULL;
	}
	if (FileExists("./resources/combat/characters.tsv"))
		state->characterData = fopen("./resources/combat/characters.tsv", "r");
	else state->characterData = NULL;

	// Estableciendo el archivo para cargar la información de los enemigos
	if (state->enemyData != NULL) {
		fclose(state->enemyData);
		state->enemyData = NULL;
	}
	if (FileExists("./resources/combat/enemies.tsv"))
		state->enemyData = fopen("./resources/combat/enemies.tsv", "r");
	else state->enemyData = NULL;

	// Estableciendo el archivo para cargar dialogos
	if (state->dialogData != NULL) {
		fclose(state->dialogData);
		state->dialogData = NULL;
	}
	if (FileExists("./resources/text/dialogs.tsv"))
		state->dialogData = fopen("./resources/text/dialogs.tsv", "r");
	else state->dialogData = NULL;

	// Estableciendo un archivo base para cargar combates (placeholder)
	if (state->combatData != NULL) {
		fclose(state->combatData);
		state->combatData = NULL;
	}
	if (FileExists("./resources/combat/dispairHill.tsv"))
		state->combatData = fopen("./resources/combat/dispairHill.tsv", "r");
	else state->combatData = NULL;
}
void LoadSprite(StateData *state, Vector2 position, float rotation, int id) {
	LoadSpriteIntoRegister(state->spriteData, state->sprites, &state->spriteAmount, SPRITE_SIZE, position, rotation, id);
}
void LoadButton(StateData *state, Vector2 position, float rotation, int idOff, int idOn, int idMessage) {
	LoadButtonIntoRegister(state->spriteData, state->translationData, state->font, state->buttons, &state->buttonAmount, BUTTON_SIZE, position, rotation, idOff, idOn, idMessage);
}
void LoadAnimation(StateData *state, Vector2 position, float rotation, int id) {
	LoadAnimationIntoRegister(state->animsData, state->spriteData, state->anims, &state->animAmount, ANIM_SIZE, position, rotation, id);
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
