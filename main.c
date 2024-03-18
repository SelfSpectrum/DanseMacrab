#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#define ANIM_SIZE 32
#define TEX_SIZE 8
//---
// Let's set some structs to work with along the gaem
//---
typedef struct Entity Entity;
struct Entity {
    int health;
    Vector2 position;
    Vector2 size;
    //Animable *anim;           // Might be useful? Is this even the correct way? No idea
};
typedef struct Animable Animable;
struct Animable {
    unsigned int frame;       // Frame needed to change to the next event
    unsigned int currentFrame;// Current frame of animation
    FILE *data;               // File that contains the animation data
    Texture2D texture;        // Texture from where the sample will come
    Quaternion origin;        // Rectangle of origin to get the texture sample
    Quaternion dest;          // Rectangle of destination to place the texture sample
    Vector2 position;         // Position of the destination???
    float rotation;           // Rotation of the texture sample
    Quaternion color;         // Tint of the texture sample
    Quaternion deltaOrigin;
    Quaternion deltaDest;
    Vector2 deltaPos;
    float deltaRotation;
    bool shader;              // Draw inside shader mode?
    bool repeat;              // Upon finishing, rewind animation?
};
typedef struct Dialog Dialog;
struct Dialog {
    int id;
    char name[64];
    char line1[64];
    char line2[64];
    char line3[64];
    int next;
    char file[64];
    int emotion;
};

typedef enum GameState GameState;
enum GameState {
    TITLE = 0,
    MAINMENU = 1
};

// TODO: Perhaps a good way to clean code is by turning the whole dual camera system into an struct and some functions
// TODO: Need to implement the CircularBuffer along its functions

//------------------------------------------------------------------------------------
// Function declarations
//------------------------------------------------------------------------------------
void LoadDialog(int record, Dialog *dialog);
void ParseDialog(char *line, Dialog *dialog);
Animable *LoadAnimable(const char *animSheet, bool repeat);
void ParseAnimable(char *line, Animable *anim, bool loadTexture);
void UpdateAnimable(Animable *anim, Shader shader);
void DrawAnimable(Animable *anim, Shader shader, Vector2 offset);
void UnloadAnimable(Animable *anim);
void LoadAnimation(int id);
void UnloadAnimation();
void ButtonX();
void ButtonZ();
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

GameState state = TITLE;                                      // INFO: Contains the current state of the game
Animable *anims[ANIM_SIZE] = { NULL };                        // INFO: Animation handling and rendering
FILE *animsData;                                              // INFO: Big file with every single independent animation data
Texture2D[]

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int virtualScreenWidth = 320;
    const int virtualScreenHeight = 180;

    const int screenWidth = 960;
    const int screenHeight = 540;

    const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

    InitWindow(screenWidth, screenHeight, "Danse Macrab");

    SetExitKey(KEY_NULL);       // INFO: Disable KEY_ESCAPE to close window, X-button still works
    bool exitWindowRequested = false;   // Flag to request window to exit
    bool exitWindow = false;    // Flag to set window to exit

    Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
    Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { -virtualRatio, -virtualRatio, screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
    Vector2 origin = { 0.0f, 0.0f };

    animsData = fopen("./resources/anims/animations.tsv", "r");

    //Animable *test = LoadAnimable("./resources/anims/mainMenu/crab.tsv", true);                   // TODO: Delete test and load a whole anim with anims

    Shader shader = LoadShader(0, "contour.fs");
    //SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), texture);  // INFO: General structure of how to load a texture

    int animCount;

    Dialog dialog = { 0, "Test", "Null", "NULL", "null", 1, "volfe" };

    SetTargetFPS(60);           // INFO: Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!exitWindow) {
        //---------------------------------------------------------------------------------
        // INFO: Update: This is for calculations and events which do not affect Texture or Drawing in a direct way
        //---------------------------------------------------------------------------------
        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;      // Detect if X-button or KEY_ESCAPE have been pressed to close window
        if (exitWindowRequested) {
            // A request for close window has been issued, we can save data before closing
            // or just show a message asking for confirmation
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) exitWindow = true;
            else if (IsKeyPressed(KEY_N) || IsKeyPressedRepeat(KEY_ESCAPE)) exitWindowRequested = false;
        }
        else {
            //UpdateAnimable(test, shader);
            for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
                if (anims[animCount] != NULL) UpdateAnimable(anims[animCount], shader);
            }
            if (IsKeyPressed(KEY_ENTER)) {
                LoadDialog(dialog.next, &dialog);
                //printf("Id: %d\tNext: %d\tFile: %s\n%s\n%s\n%s\n%s\n", dialog.id, dialog.next, dialog.file, dialog.name, dialog.line1, dialog.line2, dialog.line3);
            }
            if (IsKeyPressed(KEY_Z)) {
                ButtonZ();
            }
            if (IsKeyPressed(KEY_X)) {
                ButtonX();
            }
        }
        //----------------------------------------------------------------------------------
        // INFO: Texture: In this texture mode I create an smaller version of the game which is later rescaled in the draw mode
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);
            ClearBackground(BLACK);
            BeginMode2D(worldSpaceCamera);
                if (exitWindowRequested) {
                    DrawText("Are you sure you want to exit program? [Y/N]", 40, 90, 8, WHITE);
                }
                else {
                    //DrawAnimable(test, shader);     // TODO: A way to give offset to every anim in a smart way, useful when abilities
                    for (animCount = 0; animCount < ANIM_SIZE; animCount++) {
                        if (anims[animCount] != NULL) DrawAnimable(anims[animCount], shader, (Vector2) { 0.0f });
                    }
                    if (dialog.id) {
                        DrawText(dialog.name, 64, 120, 8, WHITE);
                        DrawText(dialog.line1, 64, 140, 8, WHITE);
                        DrawText(dialog.line2, 64, 150, 8, WHITE);
                        DrawText(dialog.line3, 64, 160, 8, WHITE);
                    }
                }
            EndMode2D();
        EndTextureMode();
        //----------------------------------------------------------------------------------
        // INFO: Draw: Take the texture in lower resolution and rescale it to a bigger res, all this while preserving pixel perfect
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(screenSpaceCamera);
                DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
            EndMode2D();
            DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);
    UnloadRenderTexture(target);
    UnloadAnimation();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void LoadDialog(int record, Dialog *dialog) {
    char line[256];
    char direction[128];
    sprintf(direction, "./resources/dialog/%s.tsv", dialog->file);
    //printf("%s\n", direction);
    FILE *file = fopen(direction, "r");                       // TODO: Load and unload files is not a good idea, find a way to don't
    if (file == NULL) {
        printf("Error opening the file!\n");
        return;
    }
    int i = 1;
    while (fgets(line, sizeof(line), file)) {
        if (i == record) {
            ParseDialog(line, dialog);
            break;
        }
        i++;
    }
    dialog->id = i;
    fclose(file);
}
void ParseDialog(char *line, Dialog *dialog) {
    char *token;
    char *saveptr;
    //Parse name
    token = strtok_r(line, "	", &saveptr);
    strcpy(dialog->name, token);
    //Parse line1
    token = strtok_r(NULL, "	", &saveptr);
    strcpy(dialog->line1, token);
    //Parse line2
    token = strtok_r(NULL, "	", &saveptr);
    strcpy(dialog->line2, token);
    //Parse line3
    token = strtok_r(NULL, "	", &saveptr);
    strcpy(dialog->line3, token);
    // Parse int next
    token = strtok_r(NULL, "	", &saveptr);
    dialog->next = atof(token);
    // Parse file to open in the next dialog
    token = strtok_r(NULL, "	", &saveptr);
    strcpy(dialog->file, token);
    // Parse the emotion to be displayed on the portrait
    token = strtok_r(NULL, "	", &saveptr);
    dialog->emotion = atof(token);
}

Animable *LoadAnimable(const char *animSheet, bool repeat) {
    Animable *anim = (Animable*) malloc(sizeof(Animable));          // Dynamic allocation since many animables might be created and destroyed in quick successions, don't forget to free later
    if (anim != NULL) {
        char line[256];       // Line from the file that contains all the struct data
        FILE *file = fopen(animSheet, "r");
        if (file == NULL) {     // If the file has problems to open, lack of memory won't be a problem, I hope
            printf("INFO: ANIMABLE: Error opening the animation file %s!\n", animSheet);
            fclose(file);
            return NULL;
        }
        if (fgets(line, sizeof(line), file)) {
            ParseAnimable(line, anim, true);
            printf("INFO: ANIMABLE: Animable loaded succesfully\n");
            anim->currentFrame = 0;
            anim->data = file;
            anim->repeat = repeat;
            return anim;
        }
        else {
            printf("INFO: ANIMABLE: File does not contains usable data!\n");
            fclose(file);
            return NULL;
        }
    }
    return NULL;
}
void ParseAnimable(char *line, Animable *anim, bool loadTexture) {
    char *token;      // 
    char *saveptr;    // 
    token = strtok_r(line, "	", &saveptr); // A line of animation folllow the following pattern: u4 str f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 i4
    anim->frame = (unsigned int) atoi(token);
    token = strtok_r(NULL, "	", &saveptr);
    if (loadTexture) {
        //if (anim->texture.id > 0) UnloadTexture(anim->texture);     // INFO: Used to be able to change texture during execution, but had problems
        anim->texture = LoadTexture(token);
    }
    token = strtok_r(NULL, "	", &saveptr);
    anim->origin.w = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->origin.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->origin.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->origin.z = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->dest.w = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->dest.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->dest.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->dest.z = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->position.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->position.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->rotation = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->color.w = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->color.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->color.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->color.z = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaOrigin.w = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaOrigin.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaOrigin.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaOrigin.z = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaDest.w = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaDest.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaDest.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaDest.z = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaPos.x = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaPos.y = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->deltaRotation = atof(token);
    token = strtok_r(NULL, "	", &saveptr);
    anim->shader = (bool) atoi(token);
}
void UpdateAnimable(Animable *anim, Shader shader) {
    if (anim != NULL) {
        char line[256];
        anim->origin = QuaternionAdd(anim->origin, anim->deltaOrigin);
        anim->dest = QuaternionAdd(anim->dest, anim->deltaDest);
        anim->position = Vector2Add(anim->position, anim->deltaPos);
        anim->rotation += anim->deltaRotation;
        anim->currentFrame++;
        if (anim->currentFrame >= anim->frame) {
            if (anim->frame != 0) {
                fgets(line, sizeof(line), anim->data);
                ParseAnimable(line, anim, false);
            }
            else if (anim->repeat) {
                anim->currentFrame = 0;
                rewind(anim->data);
                fgets(line, sizeof(line), anim->data);
                ParseAnimable(line, anim, false);
            }
            else UnloadAnimable(anim);
        }
        if (anim->shader) SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), anim->texture);
    }
}
void DrawAnimable(Animable *anim, Shader shader, Vector2 offset) {
    if (anim != NULL) {
        //printf("%u\n", anim->currentFrame);   // TODO: A good way of view the frame count as debug inside game
        if (anim->shader) BeginShaderMode(shader);
            DrawTexturePro(anim->texture,
                           (Rectangle) { anim->origin.w, anim->origin.x, anim->origin.y, anim->origin.z },
                           (Rectangle) { anim->dest.w, anim->dest.x, anim->dest.y, anim->dest.z },
                           Vector2Add(anim->position, offset),
                           anim->rotation,
                           (Color) { (unsigned char) anim->color.w, (unsigned char) anim->color.x, (unsigned char) anim->color.y, (unsigned char) anim->color.z });
        if (anim->shader) EndShaderMode();
    }
}
void UnloadAnimable(Animable *anim) {
    if (anim != NULL) {
        fclose(anim->data);
        UnloadTexture(anim->texture);
        free(anim);
        printf("INFO: ANIMABLE: Animable unloaded succesfully\n");
    }
}
void LoadAnimation(int id) {
    if (animsData == NULL) {
        printf("ERROR: ANIMATION: Error opening animation file\n");
        return;
    }
    int i = 0;
    int j;
    char line[256];
    char *token;
    char *saveptr;
    bool repeat;
    rewind(animsData);
    while (fgets(line, sizeof(line), animsData)) {
        if (i == id) {
            token = strtok_r(line, "	", &saveptr);
            printf("INFO: ANIMATION: Loading %s in the %d register\n", token, i);
            while (token != NULL) {
                for (j = 0; j < ANIM_SIZE; j++) {
                    if (anims[j] == NULL) {
                        token = strtok_r(NULL, "	", &saveptr);
                        printf("INFO: ANIMATION: Direction %s\n", token);
                        repeat = strtok_r(NULL, "	", &saveptr);
                        printf("INFO: ANIMATION: Repeat %d\n", repeat);
                        anims[j] = LoadAnimable(token, repeat);
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
void UnloadAnimation(void) {
    int i;
    for (i = 0; i < ANIM_SIZE; i++) {
      if (anims[i] != NULL) {
          UnloadAnimable(anims[i]);
          anims[i] = NULL;
      }
  }
}
void ButtonX(void) {
    switch (state) {
        case TITLE:
            UnloadAnimation();
        case MAINMENU:
            break;
        default:
            break;
    }
}
void ButtonZ(void) {
    switch (state) {
        case TITLE:
            LoadAnimation(0);
        case MAINMENU:
            break;
        default:
            break;
    }
}
