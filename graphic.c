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
		sprites[(*spriteAmount)] = ParseSprite(line, false);
		(*spriteAmount)++;
	}
	printf("INFO: SPRITE: Sprites loaded from \"%s\" correctly.\n", spriteSheet);
	fclose(file);
}
void LoadSpriteIntoRegister(FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE, Vector2 position, int id) {
	if ((*spriteAmount) < SPRITE_SIZE) {
		sprites[*spriteAmount] = LoadSingleSprite(spriteData, id);
		sprites[*spriteAmount]->position = position;
		if (sprites[*spriteAmount] != NULL) (*spriteAmount)++;
	}
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
				token = strtok_r(NULL, "	", &saveptr); //To delete the sprite name
				printf("INFO: SPRITE: Parsing %s\n", token);
				return ParseSprite(saveptr, true);
			}
		}
	}
	return NULL;
}
Sprite *ParseSprite(char *line, bool useFile) {
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
		DrawSingleSprite(sprites[i], textures, shader, color);
	}
}
void DrawSingleSprite(Sprite *sprite, SafeTexture *textures, Shader shader, Color color) {
	if (sprite == NULL) return;
	if (sprites->shader) BeginShaderMode(shader);
	DrawTexturePro(textures[sprites[i]->textureIndex].tex,
			sprites->origin,
			sprites->dest,
			sprites->position,
			sprites->rotation,
			color);
	if (sprites->shader) EndShaderMode();
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
void LoadButton(const char *buttonSheet, FILE *translationData, Font font, Button **buttons, int *buttonAmount, int BUTTON_SIZE) {
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
Button *ParseButton(char *line, FILE *translationData, Font font) {
	Button *button = (Button *) malloc(sizeof(Button));
	char *token;
	char *saveptr;
	if (button != NULL) {
		token = strtok_r(line, "	", &saveptr);
		// Sprite off
		token = strtok_r(line, "	", &saveptr);
		// Sprite on
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
void LoadMessageIntoRegister(FILE *translationData, Font font, Message **messages, int *messageAmount, int MSG_SIZE, Vector2 position, float fontSize, float spacing, bool useColor, Align align, int id) {
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
Message *LoadSingleMessage(FILE *translationData, Font font, int id, Vector2 position, float fontSize, float spacing, bool useColor, Align align) {
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

			Vector2 mesVec = MeasureTextEx(font, token, message->fontSize, spacing);

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
void ChangeTranslation(FILE **translationData, Font font, Message **messages, int messageAmount, Button **buttons, int buttonAmount, Language language) {
	if (*translationData != NULL) {
		fclose(*translationData);
		(*translationData) = NULL;
	}
	switch (language) {
		case LANG_SPANISH:
			if (FileExists("./resources/text/spanish.tsv"))
				(*translationData) = fopen("./resources/text/spanish.tsv", "r");
			break;
		case LANG_ENGLISH:
			if (FileExists("./resources/text/english.tsv"))
				(*translationData) = fopen("./resources/text/english.tsv", "r");
			break;
		case LANG_RUSSIAN:
			if (FileExists("./resources/text/russian.tsv"))
				(*translationData) = fopen("./resources/text/russian.tsv", "r");
			break;
		default:
			// TODO: Failsafe if something goes wrong
			break;
	}
	UpdateMessage(*translationData, font, messages, messageAmount, buttons, buttonAmount);
}
void UpdateMessage(FILE *translationData, Font font, Message **messages, int messageAmount, Button **buttons, int buttonAmount) {
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
	for (i = 0; i < buttonAmount; i++) {
		if (buttons[i]->message != NULL) {
			aux = LoadSingleMessage(translationData,
						font,
						buttons[i]->message->id,
						buttons[i]->message->origin,
						buttons[i]->message->fontSize,
						buttons[i]->message->spacing,
						buttons[i]->message->useColor,
						buttons[i]->message->align);
			free(buttons[i]->message);
			buttons[i]->message = NULL;
			buttons[i]->message = aux;
			aux = NULL;
		}
	}
}
