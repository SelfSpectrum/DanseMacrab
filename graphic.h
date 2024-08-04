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
	Rectangle dest; // Rectángulo de destino, que establece posición del pivote y escala del sprite
	Vector2 position; // Posición del pivote en el mundo
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
	int frame; // Frame necesario para cambiar al siguiente evento
	FILE *data; // Archivo que contiene la información de la animación
	Sprite *sprite; // Sprite que será modificado en el tiempo. Para tener cambios bruscos, se requiere un nuevo sprite (ID)
	Rectangle deltaOrigin;
	Rectangle deltaDest;
	Vector2 deltaPos;
	float deltaRotation;
};
struct Animation {
	int id;
	int currentFrame; // Frame actual de la animación
	int animsAmount;
	Animable *anims[8];
	bool repeat; // ¿Reiniciar la animación al terminar?
	bool onUse;
}

// Funciones para animaciones
void LoadAnimationIntoRegister(FILE *animsData, FILE *spriteData, Animable **anims, int *animAmount, int ANIM_SIZE, Vector2 position, float rotation, int id);
Animable *LoadSingleAnimable(FILE *spriteData, char *animSheet, Vector2 position, float rotation);
void UpdateAnimation(Animation *anims, int *animAmount, int ANIM_SIZE);
void UpdateAnimable(Animable *anims, int *animAmount, int ANIM_SIZE);
void DrawAnimable(Animable *anims, SafeTexture *textures, int animAmount, Shader shader, Color color);
void UnloadAnimable(Animation *anims, int *animAmount);
void UnloadSingleAnimable(Animable *anims, int *animAmount, int position, int ANIM_SIZE);

// Funciones para sprites
void LoadSpriteFromFile(const char *spriteSheet, FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE);
void LoadSpriteIntoRegister(FILE *spriteData, Sprite **sprites, int *spriteAmount, int SPRITE_SIZE, Vector2 position, float rotation, int id);
Sprite *LoadSingleSprite(FILE *spriteData, Vector2 position, float rotation, int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Sprite **sprites, SafeTexture *textures, int spriteAmount, Shader shader, Color color);
void DrawSingleSprite(Sprite *sprite, SafeTexture *textures, Shader shader, Color color);
void UnloadSprite(Sprite **sprites, int *spriteAmount);
void UnloadSingleSprite(Sprite **sprites, int *spriteAmount, int position, int SPRITE_SIZE);

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
void DrawButton(Button **buttons, SafeTexture *textures, int buttonAmount, Shader shader, Font font, Color color);
void UnloadButton(Button **buttons, int *buttonAmount);

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
void UnloadMessage(Message **messages, int *menssageAmount);
void UnloadSingleMessage(Message **message);
void ChangeTranslation(FILE **translationData, Font font,  Message **messages, int messageAmount, Button **buttons, int buttonAmount, Language language);
void UpdateMessage(FILE *translationData, Font font, Message **messages, int messageAmount, Button **buttons, int buttonAmount);

#endif			// ENTITY_H
