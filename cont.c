#include "raylib.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Procedural Pixel Contour with Shaders");

    // Load texture
    Texture2D texture = LoadTexture("./resources/gfx/cards00.png");
    Vector2 textureSize = {texture.width, texture.height};
    // Load shader
    Shader shader = LoadShader(0, "contour.fs");

    SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), texture);
    SetShaderValueV(shader, GetShaderLocationAttrib(shader, "textureSize"), &textureSize, 1, 2);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        BeginShaderMode(shader);
        DrawTexture(texture, 0, 0, WHITE);
        EndShaderMode();

        EndDrawing();
    }

    // Unload resources
    UnloadShader(shader);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}

