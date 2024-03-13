#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>
//---
// Let's set some structs to work with along the gaem
//---
typedef struct Entity Entity; // 41 bytes per entity
struct Entity {
    int health;
    Color color;
    Vector2 position;
    Vector2 size;
};
typedef struct Animable Animable; // 152 < bytes per animable
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
    bool repeat;              // Upon ending, rewind animation?
};
typedef struct Dialog Dialog; // 332 bytes per dialog
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
// TODO: Perhaps a good way to clean code is by turning the whole dual camera system into an struct and some functions
// TODO: Need to implement the CircularBuffer along its functions
//------------------------------------------------------------------------------------
// Function declarations
//------------------------------------------------------------------------------------
void LoadDialog(int record, Dialog *dialog);
void ParseDialog(char *line, Dialog *dialog);
Animable *LoadAnimable(const char *animSheet, bool repeat);
void ParseAnimable(char *line, Animable *anim, bool loadTexture);
void UpdateAnimable(Animable *anim);
void UnloadAnimable(Animable *anim);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int virtualScreenWidth = 320;
    const int virtualScreenHeight = 180;

    const int screenWidth = 960;
    const int screenHeight = 540;

    const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

    InitWindow(screenWidth, screenHeight, "Danse Macrab");

    SetExitKey(KEY_NULL);       // Disable KEY_ESCAPE to close window, X-button still works
    bool exitWindowRequested = false;   // Flag to request window to exit
    bool exitWindow = false;    // Flag to set window to exit

    Dialog dialog = {0,"Test", "Null", "NULL", "null", 1, "volfe"};
    Camera2D worldSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };
    Camera2D screenSpaceCamera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { -virtualRatio, -virtualRatio, screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };
    Vector2 origin = { 0.0f, 0.0f };

    Animable *test = LoadAnimable("./resources/anims/mainMenu/crab.tsv", true);

    //Shader shader = LoadShader(0, "contour.fs");
    //SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), texture);

    SetTargetFPS(60);           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!exitWindow) {
        // Update
        //---------------------------------------------------------------------------------
        // Detect if X-button or KEY_ESCAPE have been pressed to close window
        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;
        if (exitWindowRequested) {
            // A request for close window has been issued, we can save data before closing
            // or just show a message asking for confirmation
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) exitWindow = true;
            else if (IsKeyPressed(KEY_N) || IsKeyPressedRepeat(KEY_ESCAPE)) exitWindowRequested = false;
        }
        else {
            if (IsKeyPressed(KEY_ENTER)) {
                LoadDialog(dialog.next, &dialog);
                //printf("Id: %d\tNext: %d\tFile: %s\n%s\n%s\n%s\n%s\n", dialog.id, dialog.next, dialog.file, dialog.name, dialog.line1, dialog.line2, dialog.line3);
            }
        }
        //----------------------------------------------------------------------------------
        //Texture
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);
            ClearBackground(BLACK);
            BeginMode2D(worldSpaceCamera);
                if (exitWindowRequested) {
                    DrawText("Are you sure you want to exit program? [Y/N]", 40, 90, 8, WHITE);
                }
                else {
                    UpdateAnimable(test);
                    //BeginShaderMode(shader);
                        //DrawTexturePro(texture, textureOrigin, textureDest, texturePos, 0.0f, WHITE);
                    //EndShaderMode();
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
        // Draw
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
    //UnloadShader(shader);
    UnloadRenderTexture(target);
    UnloadAnimable(test);
    //UnloadTexture(texture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void LoadDialog(int record, Dialog *dialog) {
    char line[256];
    char direction[128];
    sprintf(direction, "./resources/dialog/%s.tsv", dialog->file);
    //printf("%s\n", direction);
    FILE *file = fopen(direction, "r");
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
    token = strtok_r(line, "	", &saveptr); // A line of animation folllow the following pattern: u4 str f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 u1 u1 u1 u1
    anim->frame = (unsigned int) atoi(token);
    token = strtok_r(NULL, "	", &saveptr);
    if (loadTexture) {
        //if (anim->texture.id > 0) UnloadTexture(anim->texture); INFO: Used to be able to change texture during execution, but had problems
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
}
void UpdateAnimable(Animable *anim) {
    if (anim != NULL) {
        char line[256];
        DrawTexturePro(anim->texture,
                       (Rectangle) { anim->origin.w, anim->origin.x, anim->origin.y, anim->origin.z },
                       (Rectangle) { anim->dest.w, anim->dest.x, anim->dest.y, anim->dest.z },
                       anim->position,
                       anim->rotation,
                       (Color) { (unsigned char) anim->color.w, (unsigned char) anim->color.x, (unsigned char) anim->color.y, (unsigned char) anim->color.z });
        printf("%u\n", anim->currentFrame);
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
