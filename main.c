#include <stdio.h>
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
//------------------------------------------------------------------------------------
// Function declarations
//------------------------------------------------------------------------------------
void LoadDialog(int record, Dialog *dialog);
void ParseDialog(char *line, Dialog *dialog);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 320;
    const int screenHeight = 180;

    InitWindow(screenWidth, screenHeight, "Danse Macrab");

    SetExitKey(KEY_NULL);       // Disable KEY_ESCAPE to close window, X-button still works
    bool exitWindowRequested = false;   // Flag to request window to exit
    bool exitWindow = false;    // Flag to set window to exit

    Dialog dialog = {0,"Test", "Null", "NULL", "null", 1, "volfe"};
    Camera2D camera = { {0, 0}, {0, 0}, 0.0f, 1.0f };

    Texture2D texture = LoadTexture("./resources/gfx/bigSprites00.png");
    Rectangle textureOrigin = {0, 0, 320, 180};
    Rectangle textureDest = {0, 0, 320, 180};
    Vector2 texturePos = {0, 0};
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
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            if (exitWindowRequested) {
                DrawText("Are you sure you want to exit program? [Y/N]", 160, 200, 20, WHITE);
            }
            else {
                DrawTexturePro(texture, textureOrigin, textureDest, texturePos, 0.0f, WHITE);
                if (dialog.id) {
                    DrawText(dialog.name, 160, 300, 20, WHITE);
                    DrawText(dialog.line1, 160, 320, 20, WHITE);
                    DrawText(dialog.line2, 160, 340, 20, WHITE);
                    DrawText(dialog.line3, 160, 360, 20, WHITE);
                }
            }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void LoadDialog(int record, Dialog *dialog) {
    char line[512];
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
