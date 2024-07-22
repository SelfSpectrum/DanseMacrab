#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <raylib.h>
#include <raymath.h>

typedef enum Language Language;
typedef enum Align Align;
typedef struct SafeTexture SafeTexture;
typedef struct Message Message;
typedef struct Sprite Sprite;
typedef struct Animable Animable;
typedef struct Button Button;

enum Language {
	LANG_SPANISH = 0,
	LANG_ENGLISH = 1,
	LANG_RUSSIAN = 2
};
enum Align {
	ALIGN_LEFT = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2
};
struct SafeTexture {
	Texture2D tex;
	bool init;
};
struct Message {
	int *codepoints;
	int codepointAmount;
	//char data[256];
	int id;
	Vector2 position;
	Vector2 origin;
	float fontSize;
	float spacing;
	bool useColor;
	Align align;
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
	Message *message;
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
	int frame; // Frame needed to change to the next event
	int currentFrame; // Current frame of animation
	int index;
	FILE *data; // File that contains the animation data
	int textureIndex; // Index in the Texture array from where the sample will come
	Quaternion origin; // Rectangle of origin to get the texture sample
	Quaternion dest; // Rectangle of destination to place the texture sample
	Vector2 position; // Position of the destination???
	float rotation; // Rotation of the texture sample
	Quaternion deltaOrigin;
	Quaternion deltaDest;
	Vector2 deltaPos;
	float deltaRotation;
	Vector2 offset;
	bool shader; // Draw inside shader mode?
	bool repeat; // Upon finishing, rewind animation?
	bool onUse;
};

// Animation work?
void LoadAnimable(FILE *animsData, Animable **anims, Vector2 offset, int *animAmount, int ANIM_SIZE, int id);
Animable *LoadSingleAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable **anims, int *animAmount, int ANIM_SIZE);
void DrawAnimable(Animable **anims, SafeTexture *textures, int animAmount, Shader shader, Color color);
void UnloadAnimable(Animable **animationArray, int *animAmount);
void UnloadSingleAnimable(Animable **anims, int *animAmount, int position, int ANIM_SIZE);

// Sprite work
void LoadSprite(const char *spriteSheet, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE);
void LoadSpriteIntoRegister(FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE, Vector2 position, int id);
Sprite *LoadSingleSprite(FILE *spriteData, int id);
Sprite *ParseSprite(char *line, bool useFile);
void DrawSprite(Sprite **sprites, SafeTexture *textures, int spriteAmount, Shader shader, Color color);
void UnloadSprite(Sprite **sprites, int *spriteAmount);
void UnloadSingleSprite(Sprite **sprites, int *spriteAmount, int position, int SPRITE_SIZE);

// Buttons stuff
void LoadButton(const char *buttonSheet, FILE *translationData, Font font, Button **button, int *buttonAmount, int BUTTON_SIZE);
Button *ParseButton(char *line, FILE *translationData, Font font);
void DrawButton(Button **buttons, SafeTexture *textures, int buttonAmount, Shader shader, Font font, Color color);
void UnloadButton(Button **buttons, int *buttonAmount);

// Text and translations and stuff
void LoadMessageIntoRegister(FILE *translationData,
			     Font font,
			     Message **messages,
			     int *messageAmount,
			     int MSG_SIZE,
			     Vector2 position,
			     float fontSize,
			     float spacing,
			     bool useColor,
			     Align align,
			     int id); // To load a text line with the corresponding translation
Message *LoadSingleMessage(FILE *translationData, Font font, int id, Vector2 position, float fontSize, float spacing, bool useColor, Align align);
void DrawMessage(Message **messages, int messageAmount, Font font, Color color);
void DrawSingleMessage(Message *message, Font font, Color color);
void DrawButtonMessage(Button *button, Font font, Color color);
void UnloadMessage(Message **messages, int *menssageAmount);
void UnloadSingleMessage(Message **message);
void ChangeTranslation(FILE **translationData, Font font,  Message **messages, int messageAmount, Button **buttons, int buttonAmount, Language language);
void UpdateMessage(FILE *translationData, Font font, Message **messages, int messageAmount, Button **buttons, int buttonAmount);

#endif			// ENTITY_H
