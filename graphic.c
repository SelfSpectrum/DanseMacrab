#include "graphic.h"

void LoadAnimable(FILE *animsData, Animable **anims, Vector2 offset, int *animAmount, int ANIM_SIZE, int id) {
	if (animsData == NULL) {
		printf("ERROR: ANIMATION: Error opening animation file\n");
		return;
	}
	char line[256];
	char *token;
	char *saveptr;
	bool repeat;
	int animId;
	rewind(animsData);
	if (fgets(line, sizeof(line), animsData) == NULL) return;
	while (fgets(line, sizeof(line), animsData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		animId = atoi(token);
		if (animId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			printf("INFO: ANIMATION: Loading %s with ID %d in the %d register\n", token, animId, (*animAmount));
			token = strtok_r(NULL, "	", &saveptr);
			for ( ; (*animAmount) < ANIM_SIZE && token != NULL; (*animAmount)++) {
				printf("INFO: ANIMATION: Direction %s\n", token);
				repeat = (bool) atoi(strtok_r(NULL, "	", &saveptr));
				printf("INFO: ANIMATION: Repeat %d\n", repeat);
				anims[(*animAmount)] = LoadSingleAnimable(token, repeat, *animAmount, offset);
				token = strtok_r(NULL, "	", &saveptr);
			}
			break;
		}
	}
}
Animable *LoadSingleAnimable(const char *animSheet, bool repeat, int index, Vector2 offset) {
	// Dynamic allocation since many animables might be created and destroyed in quick successions, don't forget to free later
	Animable *anim = (Animable *) malloc(sizeof(Animable));	
	// If the file has problems to open, lack of memory won't be a problem, I hope
	if (anim == NULL) return NULL;
	if (FileExists(animSheet)) {
		// Line from the file that contains all the struct data
		char line[256];
		FILE *file = fopen(animSheet, "r");
		if (fgets(line, sizeof(line), file) != NULL) {
			ParseAnimable(line, anim);
			printf("INFO: ANIMABLE: Animable loaded succesfully\n");
			anim->currentFrame = 0;
			anim->data = file;
			anim->repeat = repeat;
			anim->index = index;
			anim->offset = offset;
			anim->onUse = true;
		}
	}
	else printf("INFO: ANIMABLE: Error opening the animation file %s!\n", animSheet);
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
void UpdateAnimable(Animable **anims, int *animAmount, int ANIM_SIZE) {
	int i;
	for (i = 0; i < (*animAmount); i++) {
		if (!anims[i]->onUse) continue;
		char line[256];
		anims[i]->origin = QuaternionAdd(anims[i]->origin, anims[i]->deltaOrigin);
		anims[i]->dest = QuaternionAdd(anims[i]->dest, anims[i]->deltaDest);
		anims[i]->position = Vector2Add(anims[i]->position, anims[i]->deltaPos);
		anims[i]->rotation += anims[i]->deltaRotation;
		anims[i]->currentFrame++;
		if (anims[i]->currentFrame >= anims[i]->frame) {
			if (anims[i]->frame != 0) {
				if (fgets(line, sizeof(line), anims[i]->data) != NULL) {
					//printf("INFO: ANIMABLE: Next animable line loaded.\n");
					ParseAnimable(line, anims[i]);
					//printf("INFO: ANIMABLE: Next animable line parsed.\n");
				}
			}
			else if (anims[i]->repeat) {
				anims[i]->currentFrame = 0;
				rewind(anims[i]->data);
				if (fgets(line, sizeof(line), anims[i]->data) != NULL) ParseAnimable(line, anims[i]);
			}
			else UnloadSingleAnimable(anims, animAmount, i, ANIM_SIZE);
		}
	}
}
void DrawAnimable(Animable **anims, SafeTexture *textures, int animAmount, Shader shader, Color color) {
	int i;
	for (i = 0; i < animAmount; i++) {
		if (!anims[i]->onUse) continue;
		//printf("%u\n", anim->currentFrame);   // TODO: A good way of view the frame count as debug inside game
		if (anims[i]->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[anims[i]->textureIndex].tex,
				(Rectangle) { anims[i]->origin.w, anims[i]->origin.x, anims[i]->origin.y, anims[i]->origin.z },
				(Rectangle) { anims[i]->dest.w, anims[i]->dest.x, anims[i]->dest.y, anims[i]->dest.z },
				Vector2Add(anims[i]->position, anims[i]->offset),
				anims[i]->rotation,
				color);
		if (anims[i]->shader) EndShaderMode();
	}
}
void UnloadAnimable(Animable **anims, int *animAmount) {
	int i;
	for (i = 0; i < (*animAmount); i++) {
		anims[i]->onUse = false;
		fclose(anims[i]->data); // TODO: must ensure safety on the file
		anims[i]->data = NULL;
		free(anims[i]);
		anims[i] = NULL;
	}
	(*animAmount) = 0;
	printf("INFO: ANIMATION: Animable array data unloaded.\n");
}
void UnloadSingleAnimable(Animable **anims, int *animAmount, int position, int ANIM_SIZE) {
	anims[position]->onUse = false;
	fclose(anims[position]->data);
	anims[position]->data = NULL;
	free(anims[position]);
	anims[position] = NULL;
	(*animAmount)--;
	int i;
	for (i = 0; i < (ANIM_SIZE - 1); i++)
		if (anims[i] == NULL) anims[i] = anims[i + 1];
}
void LoadSprite(const char *spriteSheet, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE) {
	if (!FileExists(spriteSheet)) {
		printf("INFO: SPRITE: Sprite file \"%s\" not available.\n", spriteSheet);
		return;
	}
	FILE *file = fopen(spriteSheet, "r");
	char line[256];
	while (fgets(line, sizeof(line), file) != NULL) {
		if ((*spriteAmount) >= SPRITE_SIZE) {
			printf("WARNING: SPRITE: Sprites register full.\n");
			break;
		}
		sprites[(*spriteAmount)] = ParseSprite(line);
		(*spriteAmount)++;
	}
	printf("INFO: SPRITE: Sprites loaded from \"%s\" correctly.\n", spriteSheet);
	fclose(file);
}
Sprite *LoadSingleSprite(FILE *spriteData, int id) {
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
void DrawSprite(Sprite **sprites, SafeTexture *textures, int spriteAmount, Shader shader, Color color) {
	int i;
	for (i = 0; i < spriteAmount; i++) {
		if (sprites[i]->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[sprites[i]->textureIndex].tex,
				sprites[i]->origin,
				sprites[i]->dest,
				sprites[i]->position,
				sprites[i]->rotation,
				color);
		if (sprites[i]->shader) EndShaderMode();
	}
}
void UnloadSprite(Sprite **sprites, int *spriteAmount) {
	int i;
	for (i = 0; i < (*spriteAmount); i++) {
		free(sprites[i]);
		sprites[i] = NULL;
	}
	(*spriteAmount) = 0;
	printf("INFO: SPRITE: Sprites unloaded correctly\n");
}
void UnloadSingleSprite(Sprite **sprites, int *spriteAmount, int position, int SPRITE_SIZE) {
	free(sprites[position]);
	sprites[position] = NULL;
	(*spriteAmount)--;
	int i;
	for (i = 0; i < (SPRITE_SIZE - 1); i++)
		if (sprites[i] == NULL) sprites[i] = sprites[i + 1];
}
void LoadButton(const char *buttonSheet, FILE *translationData, Font *font, Button **buttons, int *buttonAmount, int BUTTON_SIZE) {
	if (FileExists(buttonSheet)) {
		FILE *file = fopen(buttonSheet, "r");
		char line[256];
		while (fgets(line, sizeof(line), file) != NULL) {
			for ( ; (*buttonAmount) < BUTTON_SIZE; (*buttonAmount)++) {
				if (buttons[(*buttonAmount)] == NULL) {
					buttons[(*buttonAmount)] = ParseButton(line, translationData, font);
					break;
				}
			}
		}
		fclose(file);
	}
}
Button *ParseButton(char *line, FILE *translationData, Font *font) {
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
		token = strtok_r(NULL, "	", &saveptr);
		button->message = LoadSingleMessage(translationData,
						    font,
						    atoi(token),
						    (Vector2) { -button->position.x + 16, -button->position.y },
						    16, 0, true, ALIGN_LEFT);
		printf("Button's Pos (%f, %f)\n", button->position.x, button->position.y);
		button->selected = false;
	}
	return button;
}
void DrawButton(Button **buttons, SafeTexture *textures, int buttonAmount, Shader shader, Font font, Color color) {
	int i;
	for (i = 0; i < buttonAmount; i++) {
		if (buttons[i]->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[buttons[i]->textureIndex].tex,
				(buttons[i]->selected) ? buttons[i]->originOn : buttons[i]->originOff,
				buttons[i]->dest,
				buttons[i]->position,
				buttons[i]->rotation,
				color);
		if (buttons[i]->shader) EndShaderMode();
		DrawButtonMessage(buttons[i], font, color);
	}
}
void UnloadButton(Button **buttons, int *buttonAmount) {
	int i;
	for (i = 0; i < (*buttonAmount); i++) {
		if (buttons[i]->message != NULL) UnloadSingleMessage(&buttons[i]->message);
		free(buttons[i]);
		buttons[i] = NULL;
	}
	(*buttonAmount) = 0;
	printf("INFO: BUTTONS: Buttons unloaded correctly\n");
}
void LoadMessageIntoRegister(FILE *translationData, Font *font, Message **messages, int *messageAmount, int MSG_SIZE, Vector2 position, float fontSize, float spacing, bool useColor, Align align, int id) {
	if ((*messageAmount) < MSG_SIZE) {
		messages[*messageAmount] = LoadSingleMessage(translationData,
							     font,
							     id,
							     position,
							     fontSize,
							     spacing,
							     useColor,
							     align);
		if (messages[*messageAmount] != NULL) (*messageAmount)++;
	}
}
Message *LoadSingleMessage(FILE *translationData, Font *font, int id, Vector2 position, float fontSize, float spacing, bool useColor, Align align) {
	if (id <= 0) return NULL;
	if (translationData == NULL) {
		printf("INFO: MESSAGE: The translation data is not available.\n");
		return NULL;
	}
	Message *message = (Message *) malloc(sizeof(Message));
	if (message == NULL) {
		printf("INFO: MESSAGE: Couldn't allocate memory for the message.\n");
		return NULL;
	}

	message->id = id;
	message->origin = position;
	message->fontSize = fontSize;
	message->spacing = spacing;
	message->useColor = useColor;
	message->align = align;
	
	char line[512];
	char *token;
	char *saveptr;
	int textId = 1;
	rewind(translationData);
	if (fgets(line, sizeof(line), translationData) == NULL) return message;
	while (fgets(line, sizeof(line), translationData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		textId = atoi(token);
		if (textId == id) {
			token = strtok_r(NULL, "	", &saveptr);
			//printf("%s\n", token);

			message->codepoints = LoadCodepoints(token, &message->codepointAmount);
			int i;
			for (i = 0; i < message->codepointAmount; i++) printf("%c", message->codepoints[i]);
			for (i = 0; i < message->codepointAmount; i++) printf(" %d", message->codepoints[i]);
			printf("\n");

			Vector2 mesVec = MeasureTextEx(*font, token, message->fontSize, spacing);

			if (message->align == ALIGN_CENTER)
				message->position = Vector2Add(message->origin, (Vector2) { -mesVec.x / 2, 0 });
			else message->position = message->origin;
			message->position.x = round(message->position.x);
			message->position.y = round(message->position.y);

			printf("Message's Pos (%f, %f)\n", message->position.x, message->position.y);
			return message;
		}
	}
	return message;
}
void DrawMessage(Message **messages, int messageAmount, Font font, Color color) {
	int i;
	for (i = 0; i < messageAmount; i++)
		DrawSingleMessage(messages[i], font, color);
}
void DrawSingleMessage(Message *message, Font font, Color color) {
	DrawTextCodepoints(font,
		    message->codepoints,
		    message->codepointAmount,
		    message->position,
		    message->fontSize,
		    message->spacing,
		    message->useColor ? color : BLACK);
}
void DrawButtonMessage(Button *button, Font font, Color color) {
	if (button->message == NULL) return;
	DrawSingleMessage(button->message, font, button->selected ? BLACK : color);

}
void UnloadMessage(Message **messages, int *messageAmount) {
	int i;
	for (i = 0; i < (*messageAmount); i++) {
		UnloadSingleMessage(&messages[i]);
	}
	(*messageAmount) = 0;
	printf("INFO: MESSAGE: Messages unloaded correctly\n");
}
void UnloadSingleMessage(Message **message) {
	UnloadCodepoints((*message)->codepoints);
	free(*message);
	(*message) = NULL;
}
void ChangeTranslation(FILE **translationData, Font *font, Message **messages, int messageAmount, Language language) {
	if (*translationData != NULL) {
		free(*translationData);
		*translationData = NULL;
	}
	switch (language) {
		case LANG_SPANISH:
			if (FileExists("./resources/text/spanish.tsv"))
				(*translationData) = fopen("./resources/text/spanish.tsv", "r");

			int spaCode[144] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 160, 176, 191, 193, 201, 205, 209, 211, 215, 218, 225, 233, 237, 241, 243, 247, 250 };

			UnloadFont(*font);
			(*font) = LoadFontEx("./resources/fonts/Pixel-UniCode.ttf", 32, spaCode, 144);
			break;
		case LANG_ENGLISH:
			if (FileExists("./resources/text/english.tsv"))
				(*translationData) = fopen("./resources/text/english.tsv", "r");

			int engCode[129] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 160, 176 };

			UnloadFont(*font);
			(*font) = LoadFontEx("./resources/fonts/Pixel-UniCode.ttf", 32, engCode, 129);
			break;
		case LANG_RUSSIAN:
			if (FileExists("./resources/text/russian.tsv"))
				(*translationData) = fopen("./resources/text/russian.tsv", "r");

			int rusCode[141] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 91, 92, 93, 94, 95, 123, 124, 125, 126, 191, 1025, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1105 };

			UnloadFont(*font);
			(*font) = LoadFontEx("./resources/fonts/Pixel-UniCode.ttf", 32, rusCode, 141);
			break;
		default:
			// TODO: Failsafe if something goes wrong
			break;
	}
	UpdateMessage(*translationData, font, messages, messageAmount);
}
void UpdateMessage(FILE *translationData, Font *font, Message **messages, int messageAmount) {
	int i;
	Message *aux;
	for (i = 0; i < messageAmount; i++) {
		aux = LoadSingleMessage(translationData,
					font,
					messages[i]->id,
					messages[i]->origin,
					messages[i]->fontSize,
					messages[i]->spacing,
					messages[i]->useColor,
					messages[i]->align);
		free(messages[i]);
		messages[i] = NULL;
		messages[i] = aux;
		aux = NULL;
	}
}
