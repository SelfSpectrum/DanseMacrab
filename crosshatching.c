#include "raylib.h"

// Cross-hatch pattern shader
const char *crossHatchPatternVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char *crossHatchPatternFragmentShader = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;

    uniform vec2 resolution;
    uniform float crossHatchScale;
    uniform float crossHatchThreshold;

    void main()
    {
        // Calculate the cross-hatch pattern
        vec2 uv = TexCoord * resolution;
        float pattern = mod(floor(uv.x * crossHatchScale) + floor(uv.y * crossHatchScale), 2.0);
        
        // Apply threshold to determine visibility
        if (pattern < crossHatchThreshold)
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black color
        }
        else
        {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
        }
    }
)";

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Cross-Hatch Transition Example");

    // Load shader
    Shader shader = LoadShaderCode(crossHatchPatternVertexShader, crossHatchPatternFragmentShader);

    // Shader parameters
    float crossHatchScale = 20.0f; // Adjust to control density of cross-hatch pattern
    float crossHatchThreshold = 0.5f; // Adjust to control visibility of cross-hatch pattern
    SetShaderValue(shader, GetShaderLocation(shader, "crossHatchScale"), &crossHatchScale, UNIFORM_FLOAT);
    SetShaderValue(shader, GetShaderLocation(shader, "crossHatchThreshold"), &crossHatchThreshold, UNIFORM_FLOAT);

    // Transition variables
    float transitionValue = 0.0f;
    bool fadeIn = true;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        if (fadeIn)
        {
            transitionValue += GetFrameTime(); // Adjust transition speed as needed
            if (transitionValue >= 1.0f)
            {
                transitionValue = 1.0f;
                fadeIn = false;
            }
        }
        else
        {
            transitionValue -= GetFrameTime(); // Adjust transition speed as needed
            if (transitionValue <= 0.0f)
            {
                transitionValue = 0.0f;
                fadeIn = true;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Use shader to draw cross-hatch pattern
        BeginShaderMode(shader);
        SetShaderValue(shader, GetShaderLocation(shader, "resolution"), (float[]){(float)screenWidth, (float)screenHeight}, UNIFORM_VEC2);
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionValue)); // Fade to black
        EndShaderMode();

        EndDrawing();
    }

    // Unload resources
    UnloadShader(shader);
    CloseWindow();

    return 0;
}

