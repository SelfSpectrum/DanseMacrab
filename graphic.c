#include "graphic.h"

void LoadAnimationIntoRegister(FILE *animsData, FILE *spriteData, Animation **anims, int *animAmount, int ANIM_SIZE, Vector2 position, float rotation, int id) {
	if (animsData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: ANIMATION: Animation file is not open.\n");
		return;
	}
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return;
	}
	if ((*animAmount) < ANIM_SIZE) {
		anims[*animAmount] = LoadSingleAnimation(animsData, spriteData, id);
		if (anims[*animAmount] != NULL) {
			anims[*animAmount]->position = position;
			anims[*animAmount]->rotation = rotation;
			(*animAmount)++;
		}
	}
}
Animation *LoadSingleAnimation(FILE *animsData, FILE *spriteData, int id) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return NULL;
	}
	Animation *anim = (Animation *) malloc(sizeof(Animation));
	if (anim == NULL) {
		TRACELOG(LOG_INFO, "ERROR: ANIMATION: Animation load failed.\n");
		return NULL;
	}
	anim->currentFrame = 0;
	anim->freedAmount = 0;

	char line[512];
	char dir[128];
	char *token;
	char *animables;
	char *parents;
	char *saveptr;
	char *saveptr2;
	int animId;
	rewind(animsData);
	if (fgets(line, sizeof(line), animsData) == NULL) return NULL;
	while (fgets(line, sizeof(line), animsData) != NULL) {
		token = strtok_r(line, "	", &saveptr);
		animId = atoi(token);
		if (animId == id) {
			anim->id = animId;
			token = strtok_r(NULL, "	", &saveptr);
			//TRACELOG(LOG_INFO, "INFO: ANIMATION: Loading %s.\n", token);
			token = strtok_r(NULL, "	", &saveptr);
			anim->repeat = (bool) atoi(token);
			token = strtok_r(NULL, "	", &saveptr);
			animables = strtok_r(token, ",", &saveptr2);

			for (anim->animAmount = 0; (anim->animAmount < 8) && (animables != NULL); anim->animAmount++) {
				sprintf(dir, "./resources/anims/%s.tsv", strtok(animables, "\n"));
				anim->anims[anim->animAmount] = LoadSingleAnimable(spriteData, dir);
				animables = strtok_r(NULL, ",", &saveptr2);
			}
			
			int i;
			token = strtok_r(NULL, "	", &saveptr);
			parents = strtok_r(token, ",", &saveptr);
			//printf("Parents left %s\n", parents);

			for (i = 0; (i < anim->animAmount) && (parents != NULL); i++) {
				anim->anims[i]->parentId = atoi(parents);
				//printf("Id parent %d\n", anim->anims[i]->parentId);
				parents = strtok_r(NULL, ",", &saveptr);
			}
			break;
		}
	}
	return anim;
}
Animable *LoadSingleAnimable(FILE *spriteData, char *animSheet) {
	// Línea del archivo que contiene toda la información del struct
	if (!FileExists(animSheet)) return NULL;

	//TRACELOG(LOG_INFO, "INFO: ANIMATION: Reading from %s.\n", animSheet);
	FILE *file = fopen(animSheet, "r");
	char line[256];
	if (fgets(line, sizeof(line), file) != NULL) {
		Animable *anim = ParseAnimable(spriteData, line, 0);
		anim->data = file;

		//TRACELOG(LOG_INFO, "INFO: ANIMABLE: Animable loaded succesfully\n");

		return anim;
	}
	else TRACELOG(LOG_INFO, "INFO: ANIMABLE: Error opening the animation file %s!\n", animSheet);
	return NULL;
}
Animable *ParseAnimable(FILE *spriteData, char *line, int deltaFrame) {
	// Alocación dinámica, ya que muchos de los animables pueden y deben ser creados y destruidos en sucesiones rápidas, no olvidar liberar la memoria luego
	Animable *anim = (Animable *) malloc(sizeof(Animable));	
	if (anim == NULL) {
		TRACELOG(LOG_INFO, "ERROR: ANIMABLE: Animable load failed.\n");
		return NULL;
	}
	int id;
	char *token; // Para ir partiendo el string
	char *saveptr; // String cortado restante tras cada partición
	float rotation = 0;

	token = strtok_r(line, "	", &saveptr);
	anim->frame = atoi(token);

	token = strtok_r(NULL, "	", &saveptr);
	anim->offset.x = atof(token);
	token = strtok_r(NULL, "	", &saveptr);
	anim->offset.y = atof(token);
	//printf("Offset - X: %f\tY: %f\n", anim->offset.x, anim->offset.y);
	token = strtok_r(NULL, "	", &saveptr);
	rotation = atof(token);

	token = strtok_r(NULL, "	", &saveptr);
	id = atoi(token);

	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.width = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.height = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.x = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaOrigin.y = atof(token) / (anim->frame - deltaFrame);

	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.width = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.height = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.x = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaPos.y = atof(token) / (anim->frame - deltaFrame);

	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.x = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaDest.y = atof(token) / (anim->frame - deltaFrame);
	token = strtok_r(NULL, "	", &saveptr);
	anim->deltaRotation = atof(token) / (anim->frame - deltaFrame);

	anim->sprite = LoadSingleSprite(spriteData, (Vector2) { 0 }, rotation, id);

	//printf("Dest - X: %f\tY: %f\n", anim->sprite->dest.x, anim->sprite->dest.y);

	return anim;
}
void UpdateAnimation(FILE *animsData, FILE *spriteData, Animation **anims, int *animAmount) {
	int i;
	int j;

	for (i = 0; i < (*animAmount); i++) {
		//printf("Frame: %d	Amount: %d	Freed: %d\n", anims[i]->currentFrame, anims[i]->animAmount, anims[i]->freedAmount);
		for (j = 0; j < anims[i]->animAmount; j++)
			UpdateAnimable(spriteData, anims[i], anims[i]->anims, j, anims[i]->currentFrame);
		anims[i]->currentFrame++;
		if (anims[i]->freedAmount == anims[i]->animAmount) {
			if (anims[i]->repeat) {
				int id;
				Vector2 pos;
				float rot;

				id = anims[i]->id;
				pos = anims[i]->position;
				rot = anims[i]->rotation;
				free(anims[i]);
				anims[i] = NULL;
				anims[i] = LoadSingleAnimation(animsData, spriteData, id);
				anims[i]->position = pos;
				anims[i]->rotation = rot;
			}
			else {
				UnloadSingleAnimationFromRegister(anims, animAmount, i);
				i--;
			}
		}
	}
}
void UpdateAnimable(FILE *spriteData, Animation *animation, Animable **anims, int index, int currentFrame) {
	if (anims[index] == NULL) return;
	char line[256];

	Animable *aux;

	anims[index]->sprite->origin.x += anims[index]->deltaOrigin.x;
	anims[index]->sprite->origin.y += anims[index]->deltaOrigin.y;
	anims[index]->sprite->origin.width += anims[index]->deltaOrigin.width;
	anims[index]->sprite->origin.height += anims[index]->deltaOrigin.height;

	//Rectangle dest = anims[index]->sprite->dest;

	anims[index]->sprite->dest.x += anims[index]->deltaDest.x;
	anims[index]->sprite->dest.y += anims[index]->deltaDest.y;
	anims[index]->sprite->dest.width += anims[index]->deltaDest.width;
	anims[index]->sprite->dest.height += anims[index]->deltaDest.height;

	anims[index]->sprite->position.x += anims[index]->deltaPos.x;
	anims[index]->sprite->position.y += anims[index]->deltaPos.y;
	anims[index]->sprite->rotation += anims[index]->deltaRotation;

	if (anims[index]->parentId != 0) {
		if (anims[anims[index]->parentId - 1] != NULL) {
			Animable *parent = anims[anims[index]->parentId - 1];

			anims[index]->sprite->origin.x += parent->deltaOrigin.x;
			anims[index]->sprite->origin.y += parent->deltaOrigin.y;
			anims[index]->sprite->origin.width += parent->deltaOrigin.width;
			anims[index]->sprite->origin.height += parent->deltaOrigin.height;

			//Rectangle dest = anims[index]->sprite->dest;

			anims[index]->sprite->dest.x += parent->deltaDest.x;
			anims[index]->sprite->dest.y += parent->deltaDest.y;
			anims[index]->sprite->dest.width += parent->deltaDest.width;
			anims[index]->sprite->dest.height += parent->deltaDest.height;

			anims[index]->sprite->position.x += parent->deltaPos.x;
			anims[index]->sprite->position.y += parent->deltaPos.y;
			anims[index]->sprite->rotation += parent->deltaRotation;

			parent = NULL;
		}
	}

	//printf("Origin - W: %f\tH: %f\tX: %f\tY: %f\n", anims[index]->sprite->origin.width, anims[index]->sprite->origin.height, anims[index]->sprite->origin.x, anims[index]->sprite->origin.y);
	//printf("Dest - W: %f\tH: %f\tX: %f\tY: %f\n", anims[index]->sprite->dest.width, anims[index]->sprite->dest.height, anims[index]->sprite->dest.x, anims[index]->sprite->dest.y);
	//printf("Position - X: %f\tY: %f\n", anims[index]->sprite->position.x, anims[index]->sprite->position.y);

	if (currentFrame >= anims[index]->frame) {
		if (anims[index]->frame != 0) {
			if (fgets(line, sizeof(line), anims[index]->data) != NULL) {
				free(anims[index]->sprite);
				anims[index]->sprite = NULL;
				aux = ParseAnimable(spriteData, line, anims[index]->frame);

				anims[index]->frame = aux->frame;
				anims[index]->sprite = aux->sprite;
				aux->sprite = NULL;

				anims[index]->deltaOrigin = aux->deltaOrigin;
				anims[index]->deltaDest = aux->deltaDest;
				anims[index]->deltaPos = aux->deltaPos;
				anims[index]->offset = aux->offset;
				anims[index]->deltaRotation = aux->deltaRotation;

				free(aux);
				//TRACELOG(LOG_INFO, "INFO: ANIMABLE: Next animable loaded.\n");
			}
		}
		else {

			free(anims[index]->sprite);
			anims[index]->sprite = NULL;
			fclose(anims[index]->data);
			anims[index]->data = NULL;
			free(anims[index]);
			anims[index] = NULL;

			animation->freedAmount++;
			//TRACELOG(LOG_INFO, "INFO: ANIMABLE: Animable %d freed.\n", index);
		}
	}
}
void DrawAnimation(Animation **anims, SafeTexture *textures, int animAmount, Color color, bool shader) {
	int i;
	int j;
	for (i = 0; i < animAmount; i++) {
		for (j = 0; j < anims[i]->animAmount; j++) {
			if (anims[i]->anims[j]->parentId == 0) DrawAnimable(anims[i]->anims[j], NULL, textures, color, anims[i]->position, anims[i]->rotation, shader);
			else DrawAnimable(anims[i]->anims[j], anims[i]->anims[anims[i]->anims[j]->parentId - 1], textures, color, anims[i]->position, anims[i]->rotation, shader);
		}
	}
}
void DrawAnimable(Animable *anim, Animable *parent, SafeTexture *textures, Color color, Vector2 position, float rotation, bool shader) {
	if (anim == NULL) return;
	if (shader != anim->sprite->shader) return;

	Rectangle dest = anim->sprite->dest;
	
	dest.x = 0;
	dest.y = 0;

	dest.x += position.x + anim->offset.x + anim->sprite->position.x;
	dest.y += position.y + anim->offset.y + anim->sprite->position.y;
	if (parent != NULL) {
		dest.x += parent->offset.x + parent->sprite->position.x;
		dest.y += parent->offset.y + parent->sprite->position.y;
	}

	DrawTexturePro( textures[anim->sprite->textureIndex].tex,
			anim->sprite->origin,
			//anim->sprite->dest,
			dest,
			// Vector2Add(anim->sprite->position, position),
			(Vector2) { anim->sprite->dest.x, anim->sprite->dest.y },
			anim->sprite->rotation + rotation,
			color);

	//printf("Origin - W: %f\tH: %f\tX: %f\tY: %f\n", anim->sprite->origin.width, anim->sprite->origin.height, anim->sprite->origin.x, anim->sprite->origin.y);
	//printf("Dest - W: %f\tH: %f\tX: %f\tY: %f\n", dest.width, dest.height, anim->sprite->position.x, anim->sprite->position.y);
	//printf("Position - X: %f\tY: %f\n", dest.x, dest.y);
	//printf("Offset - X: %f\tY: %f\n", anim->offset.x, anim->offset.y);
}
void UnloadAnimationRegister(Animation **anims, int *animAmount) {
	int i;
	//int j;
	int amount = *animAmount;
	for (i = 0; i < amount; i++) {
		//for (j = 0; j < 8; j++) printf("%p\n", anims[j]);
		UnloadSingleAnimationFromRegister(anims, animAmount, 0);
	}
	TRACELOG(LOG_INFO, "INFO: ANIMATION: Animable array data unloaded.\n");
}
void UnloadSingleAnimationFromRegister(Animation **anims, int *animAmount, int index) {
	if (anims[index] == NULL) return;

	int i;
	for (i = 0; i < anims[index]->animAmount; i++) {
		if (anims[index]->anims[i] != NULL) {
			if (anims[index]->anims[i]->sprite != NULL) {
				free(anims[index]->anims[i]->sprite);
				anims[index]->anims[i]->sprite = NULL;
			}
			if (anims[index]->anims[i]->data != NULL) {
				fclose(anims[index]->anims[i]->data);
				anims[index]->anims[i]->data = NULL;
			}
			free(anims[index]->anims[i]);
			anims[index]->anims[i] = NULL;

			anims[index]->freedAmount++;
		}
	}

	free(anims[index]);
	anims[index] = NULL;
	(*animAmount)--;

	for (i = 0; i < (*animAmount); i++)
		if (anims[i] == NULL) {
			anims[i] = anims[i + 1];
			anims[i + 1] = NULL;
		}
}
void LoadSpriteFromFile(const char *spriteSheet, FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE) {
	if (!FileExists(spriteSheet)) {
		TRACELOG(LOG_INFO, "INFO: SPRITE: Sprite file \"%s\" not available.\n", spriteSheet);
		return;
	}
	FILE *file = fopen(spriteSheet, "r");
	char line[256];
	char *saveptr;
	while (fgets(line, sizeof(line), file) != NULL) {
		if ((*spriteAmount) >= SPRITE_SIZE) {
			TRACELOG(LOG_INFO, "WARNING: SPRITE: Sprites register full.\n");
			return;
		}
		Vector2 position = { 0 };
		position.x = atoi(strtok_r(line, "	", &saveptr));
		position.y = atoi(strtok_r(NULL, "	", &saveptr));
		float rotation = atof(strtok_r(NULL, "	", &saveptr));
		int id = atoi(strtok_r(NULL, "	", &saveptr));
		sprites[(*spriteAmount)] = LoadSingleSprite(spriteData, position, rotation, id);
		(*spriteAmount)++;
	}
	TRACELOG(LOG_INFO, "INFO: SPRITE: Sprites loaded from \"%s\" correctly.\n", spriteSheet);
	fclose(file);
}
void LoadSpriteIntoRegister(FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE, Vector2 position, float rotation, int id) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return;
	}
	if ((*spriteAmount) < SPRITE_SIZE) {
		sprites[*spriteAmount] = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, id);
		if (sprites[*spriteAmount] != NULL) {
			sprites[*spriteAmount]->dest.x = position.x;
			sprites[*spriteAmount]->dest.y = position.y;
			sprites[*spriteAmount]->rotation = rotation;
			(*spriteAmount)++;
		}
	}
}
Sprite *LoadSingleSprite(FILE *spriteData, Vector2 position, float rotation, int id) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return NULL;
	}
	rewind(spriteData);
	Sprite *sprite;
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
			//TRACELOG(LOG_INFO, "INFO: SPRITE: Parsing %s\n", token);
			sprite = ParseSprite(saveptr);
			if (sprite != NULL) {
				sprite->dest.x = position.x;
				sprite->dest.y = position.y;
				sprite->rotation = rotation;
			}
			return sprite;
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
		sprite->source.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->source.y = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->source.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->source.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.x = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->origin.y = atof(token);
		sprite->dest.x = 0;
		sprite->dest.y = 0;
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.width = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->dest.height = atof(token);
		token = strtok_r(NULL, "	", &saveptr);
		sprite->shader = (bool) atoi(token);
	}
	return sprite;
}
void DrawSprite(Sprite **sprites, SafeTexture *textures, int spriteAmount, Color color, bool shader) {
	int i;
	for (i = 0; i < spriteAmount; i++) {
		DrawSingleSprite(sprites[i], textures, color, shader);
	}
}
void DrawSingleSprite(Sprite *sprite, SafeTexture *textures, Color color, bool shader) {
	if (sprite == NULL) return;
	if (shader != sprite->shader) return;
	DrawTexturePro(textures[sprite->textureIndex].tex,
			sprite->source,
			sprite->dest,
			sprite->origin,
			sprite->rotation,
			color);
}
void UnloadSpriteRegister(Sprite **sprites, int *spriteAmount) {
	int i;
	int amount = *spriteAmount;
	for (i = 0; i < amount; i++) UnloadSingleSpriteFromRegister(sprites, spriteAmount, 0);
	TRACELOG(LOG_INFO, "INFO: SPRITE: Sprites unloaded correctly\n");
}
void UnloadSingleSpriteFromRegister(Sprite **sprites, int *spriteAmount, int index) {
	if (sprites[index] == NULL) return;

	free(sprites[index]);
	sprites[index] = NULL;
	(*spriteAmount)--;
	int i;
	for (i = 0; i < (*spriteAmount); i++)
		if (sprites[i] == NULL) {
			sprites[i] = sprites[i + 1];
			sprites[i + 1] = NULL;
		}
}
void LoadButtonFromFile(const char *buttonSheet, FILE *spriteData, FILE *translationData, Font font, Button **buttons, int *buttonAmount, int BUTTON_SIZE) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return;
	}
	if (FileExists(buttonSheet)) {
		FILE *file = fopen(buttonSheet, "r");
		char line[256];
		char *saveptr;
		while (fgets(line, sizeof(line), file) != NULL) {
			for ( ; (*buttonAmount) < BUTTON_SIZE; (*buttonAmount)++) {
				if (buttons[(*buttonAmount)] == NULL) {
					int idOff = atoi(strtok_r(line, "	", &saveptr));
					int idOn = atoi(strtok_r(NULL, "	", &saveptr));
					int idMessage = atoi(strtok_r(NULL, "	", &saveptr));
					buttons[(*buttonAmount)] = LoadSingleButton(spriteData, translationData, font, (Vector2) { 0, 0 }, 0, idOff, idOn, idMessage);
					break;
				}
			}
		}
		fclose(file);
	}
}
void LoadButtonIntoRegister(FILE *spriteData, FILE *translationData, Font font, Button **buttons, int *buttonAmount, int BUTTON_SIZE, Vector2 position, float rotation, int idOff, int idOn, int idMessage) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return;
	}
	if ((*buttonAmount) < BUTTON_SIZE) {
		buttons[(*buttonAmount)] = LoadSingleButton(spriteData, translationData, font, position, rotation, idOff, idOn, idMessage);
		if (buttons[(*buttonAmount)] != NULL) {
			buttons[(*buttonAmount)]->position = position;
			buttons[(*buttonAmount)]->rotation = rotation;
			(*buttonAmount)++;
		}
	}
}
Button *LoadSingleButton(FILE *spriteData, FILE *translationData, Font font, Vector2 position, float rotation, int idOff, int idOn, int idMessage) {
	if (spriteData == NULL) {
		TRACELOG(LOG_INFO, "ERROR: SPRITE: Sprite file is not open.\n");
		return NULL;
	}
	Button *button = (Button *) malloc(sizeof(Button));
	if (button != NULL) {
		button->position = position;
		button->rotation = rotation;
		button->spriteOff = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, idOff);
		button->spriteOn = LoadSingleSprite(spriteData, (Vector2) { 0, 0 }, 0, idOn);
		button->message = LoadSingleMessage(translationData,
						    font,
						    idMessage,
						    (Vector2) { -position.x, -position.y },
						    18, 0, true, ALIGN_CENTER);
		//printf("Button's Pos (%f, %f)\n", button->position.x, button->position.y);
		button->selected = false;
	}
	return button;
}
void DrawButton(Button **buttons, SafeTexture *textures, int buttonAmount, Font font, Color color, bool shader) {
	int i;
	Rectangle dest;
	for (i = 0; i < buttonAmount; i++) {
		if (buttons[i]->selected) {
			if (shader != buttons[i]->spriteOn->shader) continue;
			dest = (Rectangle) { 0, 0, 0, 0 };
			dest.x = buttons[i]->position.x;
			dest.y = buttons[i]->position.y;
			dest.width = buttons[i]->spriteOn->width;
			dest.height = buttons[i]->spriteOn->height;
			DrawTexturePro(textures[buttons[i]->spriteOn->textureIndex].tex,
					buttons[i]->spriteOn->source,
					dest,
					buttons[i]->spriteOn->origin,
					buttons[i]->rotation,
					color);
			//DrawButtonMessage(buttons[i], (Vector2) { buttons[i]->spriteOn->dest.width / 2, buttons[i]->spriteOn->dest.height / 2 }, font, color);
			DrawButtonMessage(buttons[i], (Vector2) { buttons[i]->spriteOn->dest.width / 2, -1 }, font, color);
		}
		else {
			if (shader != buttons[i]->spriteOff->shader) continue;
			dest = (Rectangle) { 0, 0, 0, 0 };
			dest.x = buttons[i]->position.x;
			dest.y = buttons[i]->position.y;
			dest.width = buttons[i]->spriteOff->width;
			dest.height = buttons[i]->spriteOff->height;
			DrawTexturePro(textures[buttons[i]->spriteOff->textureIndex].tex,
					buttons[i]->spriteOff->origin,
					buttons[i]->spriteOff->dest,
					buttons[i]->position,
					buttons[i]->rotation,
					color);
			//DrawButtonMessage(buttons[i], (Vector2) { buttons[i]->spriteOff->dest.width / 2, buttons[i]->spriteOff->dest.height / 2 }, font, color);
			DrawButtonMessage(buttons[i], (Vector2) { buttons[i]->spriteOff->dest.width / 2, -1 }, font, color);
		}
	}
}
void UnloadButtonRegister(Button **buttons, int *buttonAmount) {
	int i;
	for (i = 0; i < (*buttonAmount); i++) {
		if (buttons[i]->message != NULL) UnloadSingleMessage(&buttons[i]->message);
		free(buttons[i]);
		buttons[i] = NULL;
	}
	(*buttonAmount) = 0;
	TRACELOG(LOG_INFO, "INFO: BUTTONS: Buttons unloaded correctly\n");
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
		TRACELOG(LOG_INFO, "INFO: MESSAGE: The translation data is not available.\n");
		return NULL;
	}
	Message *message = (Message *) malloc(sizeof(Message));
	if (message == NULL) {
		TRACELOG(LOG_INFO, "INFO: MESSAGE: Couldn't allocate memory for the message.\n");
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
			//int i;
			//for (i = 0; i < message->codepointAmount; i++) printf("%c", message->codepoints[i]);
			//for (i = 0; i < message->codepointAmount; i++) printf(" %d", message->codepoints[i]);
			//printf("\n");

			Vector2 mesVec = MeasureTextEx(font, token, message->fontSize, spacing);

			if (message->align == ALIGN_CENTER)
				message->position = Vector2Add(message->origin, (Vector2) { -mesVec.x / 2, 0 });
			else message->position = message->origin;
			message->position.x = round(message->position.x);
			message->position.y = round(message->position.y);

			//printf("Message's Pos (%f, %f)\n", message->position.x, message->position.y);
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
void DrawButtonMessage(Button *button, Vector2 position, Font font, Color color) {
	if (button->message == NULL) return;
	DrawTextCodepoints(font,
			    button->message->codepoints,
			    button->message->codepointAmount,
			    Vector2Add(button->message->position, position),
			    button->message->fontSize,
			    button->message->spacing,
			    button->message->useColor != button->selected ? color : BLACK);

}
void UnloadMessageRegister(Message **messages, int *messageAmount) {
	int i;
	for (i = 0; i < (*messageAmount); i++)
		UnloadSingleMessage(&messages[i]);
	(*messageAmount) = 0;
	//TRACELOG(LOG_INFO, "INFO: MESSAGE: Messages unloaded correctly\n");
}
void UnloadSingleMessage(Message **message) {
	if ((*message) == NULL) return;

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
