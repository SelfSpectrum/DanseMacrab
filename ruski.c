#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib [text] example - drawing text");

    int codepoints[32] = { 1056, 1091, 1089, 1089, 1081, 1079, 1080, 10 };
    int ca = 8;

    Font font = LoadFontEx("./resources/fonts/unifont-14.0.04.ttf", 16, codepoints, ca);

    // Main game loop
    while (!WindowShouldClose()) {
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the text using the custom font
        DrawTextCodepoints(font, codepoints, ca, (Vector2){ 20, 20 }, 32, 2, BLACK);

        EndDrawing();
    }

    // De-Initialization
    UnloadFont(font);  // Unload custom font
    CloseWindow();           // Close window and OpenGL context

    return 0;
}
