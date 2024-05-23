#ifndef GRAPHIC_H
#define GRAPHIC_H

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
	//Texture2D texture;      // INFO: Deprecated: Texture from where the sample will come
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
Animable *LoadAnimable(const char *animSheet, bool repeat, int index, Vector2 offset);
void ParseAnimable(char *line, Animable *anim);
void UpdateAnimable(Animable *anim);
void DrawAnimable(Animable *anim, Shader shader);
void UnloadAnimable(Animable *anim);
// Animations
void LoadAnimation(int id, Vector2 offset);
void UnloadAnimation(Animable **animationArray);
// Sprite work
void LoadSprite(const char *spriteSheet, int *spritePosition, int SPRITE_SIZE);
Sprite *LoadSingleSprite(int id);
Sprite *ParseSprite(char *line);
void DrawSprite(Shader shader, Sprite **sprites, int *spritePosition, Color color);
void UnloadSprite(Sprite **sprites, int *spritePosition);
void UnloadSingleSprite(Sprite **sprites, int position);
// Buttons stuff
void LoadButton(const char *buttonSheet);
Button *ParseButton(char *line);
void DrawButton(Shader shader);
void UnloadButton(Button **buttonArray);
// Text and translations and stuff
Message *LoadMessage(int id);			// To load a text line with the corresponding translation
void RenderMessage(Font font);
void UnloadMessage(Message **message);

#endif			// ENTITY_H
