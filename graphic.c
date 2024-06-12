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
	if (anim != NULL) {
		// If the file has problems to open, lack of memory won't be a problem, I hope
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
void UpdateAnimable(Animable **anims, int *animAmount, int ANIM_SIZE) {
	int i;
	for (i = 0; i < (*animAmount); i++) {
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
void DrawAnimable(Animable **anims, SafeTexture *textures, int *animAmount, Shader shader, Color color) {
	int i;
	for (i = 0; i < (*animAmount); i++) {
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
		fclose(anims[i]->data);
		free(anims[i]);
		anims[i] = NULL;
	}
	(*animAmount) = 0;
	printf("INFO: ANIMATION: Animable array data unloaded.\n");
}
void UnloadSingleAnimable(Animable **anims, int *animAmount, int position, int ANIM_SIZE) {
	fclose(anims[position]->data);
	free(anims[position]);
	anims[position] = NULL;
	(*animAmount)--;
	int i;
	for (i = 0; i < (ANIM_SIZE - 1); i++)
		if (anims[i] == NULL) anims[i] = anims[i + 1];
}
void LoadSprite(const char *spriteSheet, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE) {
	if (FileExists(spriteSheet)) {
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
	else printf("INFO: SPRITE: Sprite file \"%s\" not available.\n", spriteSheet);
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
void DrawSprite(Sprite **sprites, SafeTexture *textures, int *spriteAmount, Shader shader, Color color) {
	int i;
	for (i = 0; i < (*spriteAmount); i++) {
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
void LoadButton(const char *buttonSheet, Button **buttons, int *buttonAmount, int BUTTON_SIZE) {
	if (FileExists(buttonSheet)) {
		FILE *file = fopen(buttonSheet, "r");
		char line[256];
		while (fgets(line, sizeof(line), file) != NULL) {
			for ( ; (*buttonAmount) < BUTTON_SIZE; (*buttonAmount)++) {
				if (buttons[(*buttonAmount)] == NULL) {
					buttons[(*buttonAmount)] = ParseButton(line);
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
void DrawButton(Button **buttons, SafeTexture *textures, int *buttonAmount, Shader shader, Color color) {
	int i;
	for (i = 0; i < (*buttonAmount); i++) {
		if (buttons[i]->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[buttons[i]->textureIndex].tex,
				(buttons[i]->selected) ? buttons[i]->originOn : buttons[i]->originOff,
				buttons[i]->dest,
				buttons[i]->position,
				buttons[i]->rotation,
				color);
		if (buttons[i]->shader) EndShaderMode();
	}
}
void UnloadButton(Button **buttons, int *buttonAmount) {
	int i;
	for (i = 0; i < (*buttonAmount); i++) {
		free(buttons[i]);
		buttons[i] = NULL;
	}
	(*buttonAmount) = 0;
	printf("INFO: BUTTONS: Buttons unloaded correctly\n");
}
Message *LoadMessage(FILE *translationData, int id) {
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
void DrawMessage(Message message, Font font, Color color) {
	
	DrawTextPro(font, message.string, message.position, message.origin, message.rotation, 8, 10, color);
}
void UnloadMessage(Message **messages, int *messageAmount) {
	int i;
	for (i = 0; i < (*messageAmount); i++) {
		free(messages[i]);
		messages[i] = NULL;
	}
	(*messageAmount) = 0;
	printf("INFO: MESSAGE: Messages unloaded correctly\n");
}
