#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	// Initialization
	const int screenWidth = 800;
	const int screenHeight = 450;
	InitWindow(screenWidth, screenHeight, "Crossfade Music Example");
	InitAudioDevice();
	char *names[2] = { "Title - Danse Macrab\0" , "Battle - Carcinitation\0" };
	char playingName[128];
	// Load music streams
	Music music1 = LoadMusicStream("./resources/sfx/title.mp3");
	Music music2 = LoadMusicStream("./resources/sfx/battle.mp3");

	// Start playing the first music stream
	PlayMusicStream(music1);
	PlayMusicStream(music2);

	// Set initial volume levels
	SetMusicVolume(music1, 1.0f); // Full volume for the first music
	SetMusicVolume(music2, 0.0f); // Mute the second music initially

	float crossfadeFactor;

	// Crossfade duration (in seconds)
	const float crossfadeDuration = 3.0f;
	float timer = 0;
	bool shiftCF = true;
	sprintf(playingName, "Now playing... %s", names[0]);

	SetTargetFPS(60);
	// Main game loop
	while (!WindowShouldClose()) {
		// Gradually decrease the volume of the first music stream and increase the volume of the second music stream
		UpdateMusicStream(music1);	// Update music buffer with new stream data
		UpdateMusicStream(music2);	// Update music buffer with new stream data
		timer += GetFrameTime();
		crossfadeFactor = Clamp(timer / crossfadeDuration, 0.0f, 1.0f);
		if (crossfadeFactor <= 1.0f) { // Calculate volume adjustment factor based on time elapsed
			// Adjust volume levels
			SetMusicVolume(music1, shiftCF ? 1.0f - crossfadeFactor : crossfadeFactor);
			SetMusicVolume(music2, shiftCF ? crossfadeFactor : 1.0f - crossfadeFactor);
		}
		if (IsKeyPressed(KEY_Z)) {
			timer = 0;
			shiftCF = !shiftCF;
			sprintf(playingName, "Now playing... %s", shiftCF ? names[0] : names[1]);
		}
		// Draw
		BeginDrawing();
			ClearBackground(BLACK);
			DrawText("Press ESC to exit", 10, 10, 20, RAYWHITE);
			DrawText(playingName, 10, 30, 20, RAYWHITE);
		EndDrawing();
	}
	// Unload music streams
	StopMusicStream(music1);
	StopMusicStream(music2);
	UnloadMusicStream(music1);
	UnloadMusicStream(music2);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}

