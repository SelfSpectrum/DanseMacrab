Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset) {
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
void UpdateAnimable(Animable *anim) {
	if (anim != NULL) {
		char line[256];
		anim->origin = QuaternionAdd(anim->origin, anim->deltaOrigin);
		anim->dest = QuaternionAdd(anim->dest, anim->deltaDest);
		anim->position = Vector2Add(anim->position, anim->deltaPos);
		anim->rotation += anim->deltaRotation;
		anim->currentFrame++;
		if (anim->currentFrame >= anim->frame) {
			if (anim->frame != 0) {
				if (fgets(line, sizeof(line), anim->data) != NULL) {
					//printf("INFO: ANIMABLE: Next animable line loaded.\n");
					ParseAnimable(line, anim);
					//printf("INFO: ANIMABLE: Next animable line parsed.\n");
				}
			}
			else if (anim->repeat) {
				anim->currentFrame = 0;
				rewind(anim->data);
				if (fgets(line, sizeof(line), anim->data) != NULL) ParseAnimable(line, anim);
			}
			else UnloadAnimable(anim);
		}
	}
}
void DrawAnimable(Animable *anim, Shader shader) {
	if (anim != NULL) {
		//printf("%u\n", anim->currentFrame);   // TODO: A good way of view the frame count as debug inside game
		if (anim->shader) BeginShaderMode(shader);
		DrawTexturePro(textures[anim->textureIndex],
				(Rectangle) { anim->origin.w, anim->origin.x, anim->origin.y, anim->origin.z },
				(Rectangle) { anim->dest.w, anim->dest.x, anim->dest.y, anim->dest.z },
				Vector2Add(anim->position, anim->offset),
				anim->rotation,
				globalColor);
		if (anim->shader) EndShaderMode();
	}
}
void UnloadAnimable(Animable *anim) {
	if (anim != NULL) {
		anims[anim->index] = NULL;
		fclose(anim->data);
		// UnloadTexture(anim->texture);
		free(anim);
		printf("INFO: ANIMABLE: Animable unloaded succesfully\n");
	}
}
void LoadAnimation(int id, Vector2 offset) {
	if (animsData == NULL) {
		printf("ERROR: ANIMATION: Error opening animation file\n");
		return;
	}
	int i = 1;
	int j;
	char line[256];
	char *token;
	char *saveptr;
	bool repeat;
	rewind(animsData);
	if (fgets(line, sizeof(line), animsData) == NULL) return;
	while (fgets(line, sizeof(line), animsData) != NULL) {
		if (i == id) {
			token = strtok_r(line, "	", &saveptr);
			printf("INFO: ANIMATION: Loading %s in the %d register\n", token, i);
			while (token != NULL) {
				for (j = 0; j < ANIM_SIZE; j++) {
					if (anims[j] == NULL) {
						token = strtok_r(NULL, "	", &saveptr);
						printf("INFO: ANIMATION: Direction %s\n", token);
						repeat = (bool) atoi(strtok_r(NULL, "	", &saveptr));
						printf("INFO: ANIMATION: Repeat %d\n", repeat);
						anims[j] = LoadAnimable(token, repeat, j, offset);
						token = strtok_r(NULL, "	", &saveptr);
						break;
					}
				}
			}
			break;
		}
		i++;
	}
}
void UnloadAnimation(Animable **animationArray) {
	int i;
	for (i = 0; i < ANIM_SIZE; i++) {
		if (animationArray[i] != NULL) {
			UnloadAnimable(animationArray[i]);
			animationArray[i] = NULL;
		}
	}
	printf("INFO: ANIMATION: Animable array data unloaded.\n");
}
void LoadSprite(const char *spriteSheet, int *spritePosition, int SPRITE_SIZE) {
	if (FileExists(spriteSheet)) {
		FILE *file = fopen(spriteSheet, "r");
		char line[256];
		while (fgets(line, sizeof(line), file) != NULL) {
			if (*spritePosition >= SPRITE_SIZE) {
				printf("WARNING: SPRITE: Sprites register full.\n");
				break;
			}
			sprites[*spritePosition] = ParseSprite(line);
			*spritePosition++;
		}
		printf("INFO: SPRITE: Sprites loaded from \"%s\" correctly.\n", spriteSheet);
		fclose(file);
	}
	else printf("INFO: SPRITE: Sprite file \"%s\" not available.\n", spriteSheet);
}
Sprite *LoadSingleSprite(int id) {
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
void DrawSprite(Shader shader, Sprite **sprites, int *spritePosition, Color color) {
	int i;
	for (i = 0; i <= *spritePosition; i++) {
		if (sprites[i] != NULL  ) {
			if (sprites[i]->shader) BeginShaderMode(shader);
			DrawTexturePro(textures[sprites[i]->textureIndex],
					sprites[i]->origin,
					sprites[i]->dest,
					sprites[i]->position,
					sprites[i]->rotation,
					color);
			if (sprites[i]->shader) EndShaderMode();
		}
	}
}
void UnloadSprite(Sprite **sprites, int *spritePosition) {
	int i;
	for (i = 0; i <= *spritePosition; i++) {
		if (sprites[i] != NULL) {
			free(sprites[i]);
			sprites[i] = NULL;
		}
	}
	*spritePosition = 0;
}
void UnloadSingleSprite(Sprite **sprites, int position) {
	free(sprites[position]);
	sprites[position] = NULL;
}
void LoadButton(const char *buttonSheet) {
	if (FileExists(buttonSheet)) {
		FILE *file = fopen(buttonSheet, "r");
		char line[256];
		int i;
		while (fgets(line, sizeof(line), file) != NULL) {
			for (i = 0; i < BUTTON_SIZE; i++) {
				if (buttons[i] == NULL) {
					buttons[i] = ParseButton(line);
					buttonAmount++;
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
void DrawButton(Shader shader) {
	int i;
	for (i = 0; i < BUTTON_SIZE; i++) {
		if (buttons[i] != NULL  ) {
			if (buttons[i]->shader) BeginShaderMode(shader);
			DrawTexturePro(textures[buttons[i]->textureIndex],
					(buttons[i]->selected) ? buttons[i]->originOn : buttons[i]->originOff,
					buttons[i]->dest,
					buttons[i]->position,
					buttons[i]->rotation,
					globalColor);
			if (buttons[i]->shader) EndShaderMode();
		}
	}
}
void UnloadButton(Button **buttonArray) {
	int i;
	buttonAmount = 0;
	for (i = 0; i < BUTTON_SIZE; i++) {
		if (buttonArray[i] != NULL) {
			free(buttonArray[i]);
			buttonArray[i] = NULL;
		}
	}
	printf("INFO: BUTTONS: Buttons unloaded correctly\n");
}
Message *LoadMessage(int id) {
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
void RenderMessage(Font font) {
	//
}
void UnloadMessage(Message **message) {
}
