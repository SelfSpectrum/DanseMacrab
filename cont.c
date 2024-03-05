#include "raylib.h"
#include <stdio.h>

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Procedural Pixel Contour with Shaders");

    // Load texture
    Texture2D texture = LoadTexture("./resources/gfx/cards00.png");
    //float textureSize[2] = {texture.width, texture.height};
    // Load shader
    Shader shader = LoadShader(0, "contour.fs");

    SetShaderValueTexture(shader, GetShaderLocationAttrib(shader, "textureSampler"), texture);
    //SetShaderValue(shader, GetShaderLocationAttrib(shader, "textureSize"), textureSize, SHADER_UNIFORM_VEC2);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);

            BeginShaderMode(shader);
                DrawTexture(texture, 0, 0, WHITE);
            EndShaderMode();

            DrawFPS(10, 10);
        EndDrawing();
    }

    // Unload resources
    UnloadShader(shader);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}

