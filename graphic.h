#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

typedef struct Message Message;
typedef struct Sprite Sprite;
typedef struct Animable Animable;
typedef struct Button Button;
typedef enum Language Language;

struct Message {
	char *string;
	int id;
	Vector2 position;
	Vector2 origin;
	float rotation;
	float fontSize;
};
struct Sprite {
	int textureIndex;
	Rectangle origin;
	Rectangle dest;
	Vector2 position;
	float rotation;
	bool shader;
};
struct Button {
	int textureIndex;
	Rectangle originOff;
	Rectangle originOn;
	Rectangle dest;
	Vector2 position;
	float rotation;
	bool shader;
	bool selected;
};
struct Animable {
	int frame;       // Frame needed to change to the next event
	int currentFrame;// Current frame of animation
	int index;
	FILE *data;               // File that contains the animation data
	int textureIndex;         // Index in the Texture array from where the sample will come
	Quaternion origin;        // Rectangle of origin to get the texture sample
	Quaternion dest;          // Rectangle of destination to place the texture sample
	Vector2 position;         // Position of the destination???
	float rotation;           // Rotation of the texture sample
	Quaternion deltaOrigin;
	Quaternion deltaDest;
	Vector2 deltaPos;
	float deltaRotation;
	Vector2 offset;
	bool shader;              // Draw inside shader mode?
	bool repeat;              // Upon finishing, rewind animation?
};
enum Language {
	LANG_SPANISH = 0,
	LANG_ENGLISH = 1,
	LANG_RUSSIAN = 2
};

// INFO: GFX functions
void LoadAnimable(FILE *animsData, Animable **anims, Vector2 offset, int *animAmount, int ANIM_SIZE, int id);
Animable *LoadSingleAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable **anims, int *animAmount, int ANIM_SIZE);
void DrawAnimable(Animable **anims, Texture2D *textures, int *animAmount, Shader shader, Color color);
void UnloadSingleAnimable(Animable **anims, int *animAmount, int position, int ANIM_SIZE);
void UnloadAnimable(Animable **animationArray, int *animAmount);
// Sprite work
void LoadSprite(const char *spriteSheet, int *spriteAmount, int SPRITE_SIZE);
Sprite *LoadSingleSprite(FILE *spriteData, int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Sprite **sprites, Texture2D *textures, int *spriteAmount, Shader shader, Color color);
void UnloadSprite(Sprite **sprites, int *spriteAmount);
void UnloadSingleSprite(Sprite **sprites, int *spriteAmount, int position, int SPRITE_SIZE);
// Buttons stuff
void LoadButton(const char *buttonSheet, int *buttonAmount);
Button *ParseButton(char *line);
void DrawButton(Button **buttons, Texture2D *textures, int *buttonAmount, Shader shader, Color color);
void UnloadButton(Button **buttons, int *buttonAmount);
// Text and translations and stuff
Message *LoadMessage(FILE *translationData, int id);			// To load a text line with the corresponding translation
void RenderMessage(Font font);
void UnloadMessage(Message **messages, int *menssageAmount);

#endif			// ENTITY_H
