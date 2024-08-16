#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

typedef enum Language Language;
typedef enum Align Align;
typedef struct SafeTexture SafeTexture;
typedef struct Message Message;
typedef struct Sprite Sprite;
typedef struct Animable Animable;
typedef struct Animation Animation;
typedef struct Button Button;

enum Language {
	LANG_SPANISH = 0,
	LANG_ENGLISH = 1,
	LANG_RUSSIAN = 2
};
// Alineamiento del texto
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
	int *codepoints; // Array de números que representan en Unicode las letras del mensaje
	int codepointAmount; // Cantidad de números dentro del array
	int id;
	Vector2 position;
	Vector2 origin;
	float fontSize;
	float spacing;
	bool useColor; // Utiliza el color universal como tinte (de manera multiplicativa)
	Align align;
};
struct Sprite {
	int textureIndex; // Índice de la textura en el array del estado para extraer el sprite
	Rectangle origin; // Origen del sprite dentro de la textura
	Rectangle dest; // Rectángulo de destino, que establece posición y escala del sprite
	Vector2 position; // Posición del pivote en la textura
	float rotation; // Rotacion aplicada a la textura respecto al pivote
	bool shader; // ¿Dibujar animable dentro del modo sombreador?
};
struct Button {
	Message *message;
	Sprite *spriteOff;
	Sprite *spriteOn;
	Vector2 position;
	float rotation;
	bool selected;
};
// Los animables son las piezas que componen una animación, son sprites que cambian en el tiempo
struct Animable {
	FILE *data; // Archivo que contiene la información de la animación
	int frame; // Frame necesario para cambiar al siguiente evento
	Sprite *sprite; // Sprite que será modificado en el tiempo. Para tener cambios bruscos, se requiere un nuevo sprite (ID)
	Rectangle deltaOrigin;
	Rectangle deltaDest;
	Vector2 offset;
	Vector2 deltaPos;
	float deltaRotation;
	int parentId;
};
struct Animation {
	int id;
	int currentFrame; // Frame actual de la animación
	int animAmount;
	int freedAmount;
	Animable *anims[8];
	Vector2 position;
	float rotation;
	bool repeat; // ¿Reiniciar la animación al terminar?
};

// Funciones para animaciones
void LoadAnimationIntoRegister(FILE *animsData, FILE *spriteData, Animation **anims, int *animAmount, int ANIM_SIZE, Vector2 position, float rotation, int id);
Animation *LoadSingleAnimation(FILE *animsData, FILE *spriteData, int id);
Animable *LoadSingleAnimable(FILE *spriteData, char *animSheet);
Animable *ParseAnimable(FILE *spriteData, char *line);
void UpdateAnimation(FILE *animsData, FILE *spriteData, Animation **anims, int *animAmount);
void UpdateAnimable(FILE *spriteData, Animation *animation, Animable **anims, int index, int currentFrame);
void DrawAnimation(Animation **anims, SafeTexture *textures, int animAmount, Color color, bool shader);
void DrawAnimable(Animable *anims, Animable *parent, SafeTexture *textures, Color color, Vector2 Position, float rotation, bool shader);
void UnloadAnimationRegister(Animation **anims, int *animAmount);
void UnloadSingleAnimationFromRegister(Animation **anims, int *animAmount, int index);

// Funciones para sprites
void LoadSpriteFromFile(const char *spriteSheet, FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE);
void LoadSpriteIntoRegister(FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE, Vector2 position, float rotation, int id);
Sprite *LoadSingleSprite(FILE *spriteData, Vector2 position, float rotation, int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Sprite **sprites, SafeTexture *textures, int spriteAmount, Color color, bool shader);
void DrawSingleSprite(Sprite *sprite, SafeTexture *textures, Color color, bool shader);
void UnloadSpriteRegister(Sprite **sprites, int *spriteAmount);
void UnloadSingleSpriteFromRegister(Sprite **sprites, int *spriteAmount, int index);

// Funciones para botones
void LoadButtonFromFile(const char *buttonSheet, FILE *spriteData, FILE *translationData, Font font, Button **button, int *buttonAmount, int BUTTON_SIZE);
void LoadButtonIntoRegister(FILE *spriteData,
			    FILE *translationData,
			    Font font,
			    Button **button,
			    int *buttonAmount,
			    int BUTTON_SIZE,
			    Vector2 position,
			    float rotation,
			    int idOff,
			    int idOn,
			    int idMessage);
Button *LoadSingleButton(FILE *spriteData, FILE *translationData, Font font, Vector2 position, float rotation, int idOff, int idOn, int idMessage);
void DrawButton(Button **buttons, SafeTexture *textures, int buttonAmount, Font font, Color color, bool shader);
void UnloadButtonRegister(Button **buttons, int *buttonAmount);

// Funciones para traducciones y texto
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
void UnloadMessageRegister(Message **messages, int *menssageAmount);
void UnloadSingleMessage(Message **message);
void ChangeTranslation(FILE **translationData, Font font,  Message **messages, int messageAmount, Button **buttons, int buttonAmount, Language language);
void UpdateMessage(FILE *translationData, Font font, Message **messages, int messageAmount, Button **buttons, int buttonAmount);

#endif			// ENTITY_H
