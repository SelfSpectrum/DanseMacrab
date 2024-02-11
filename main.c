#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>
//---
// Let's set some structs to work with along the gaem
//---
typedef struct Entity Entity;
struct Entity {
  int health;
  Color color;
  Vector2 position;
  Vector2 size;
};
typedef struct Dialog Dialog;
struct Dialog {
  int id;
  char *data[4];
  int next;
};
void PlayerUpdate(Entity *player);
void ParseDialog(char *line, Dialog *dialog);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  Entity player = {10, {0, 0, 0, 255}, {0, 0}, {16, 16}};
  Dialog dialog;
  char *line;
  FILE *file = fopen("./resources/dialog/data.csv", "r");
  if (file == NULL) {
    printf("Error opening the file!\n");
    return 1;
  }
  int record = 0;
  int target = 1;
  const int screenWidth = 800;
  const int screenHeight = 450;
  //Camera2D camera = {};
  InitWindow(screenWidth, screenHeight, "Raylib");

  SetExitKey(KEY_NULL);       // Disable KEY_ESCAPE to close window, X-button still works
  
  bool exitWindowRequested = false;   // Flag to request window to exit
  bool exitWindow = false;    // Flag to set window to exit

  SetTargetFPS(60);           // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  
  while (fgets(line, sizeof(line), file)) {
    target++;
    if (record == target) ParseDialog(line, &dialog);
  }

  while (!exitWindow) {
    // Update
    //---------------------------------------------------------------------------------
    // Player stuff
    PlayerUpdate(&player);
    // Detect if X-button or KEY_ESCAPE have been pressed to close window
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;
    
    if (exitWindowRequested) {
      // A request for close window has been issued, we can save data before closing
      // or just show a message asking for confirmation

      if (IsKeyPressed(KEY_Y) || IsKeyPressedRepeat(KEY_ESCAPE)) exitWindow = true;
      else if (IsKeyPressed(KEY_N)) exitWindowRequested = false;
    }
    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawRectangleV(player.position, player.size, player.color);
      DrawText(dialog.data[0], 160, 400, 20, BLACK);
      DrawText(dialog.data[1], 160, 440, 20, BLACK);
      if (exitWindowRequested) {
        DrawText("Are you sure you want to exit program? [Y/N]", 160, 200, 20, LIGHTGRAY);
      }
      else DrawText("Try to close the window to get confirmation message!", 120, 200, 20, LIGHTGRAY);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void PlayerUpdate(Entity *player) {
  if (IsKeyDown(KEY_W)) player->position.y -= 1;
  if (IsKeyDown(KEY_A)) player->position.x -= 1;
  if (IsKeyDown(KEY_S)) player->position.y += 1;
  if (IsKeyDown(KEY_D)) player->position.x += 1;
}
void ParseDialog(char *line, Dialog *dialog) {
  char *token;
  char *saveptr;
  int i;
  //Parse text
  for (i = 0; i < 5; i++) {
    token = strtok_r(NULL, ",", &saveptr);
    if (token[i] == '"') {
      // Handle quoted field
      char *end_quote = strchr(token + 1, '"');
      if (end_quote != NULL) {
        // Copy the quoted string without the quotes
        strncpy(dialog->data[i], token + 1, end_quote - token - 1);
        dialog->data[i][end_quote - token - 1] = '\0';
      } else {
        // Error: unterminated quoted field
        strcpy(dialog->data[i], "ERROR");
      }
    } else {
      strcpy(dialog->data[i], token);
    }
  }
  // Parse the value
  token = strtok_r(NULL, ",", &saveptr);
  dialog->next = atof(token);
}
