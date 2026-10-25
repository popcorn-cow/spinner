// Copyright (C)  2026  Anonymous.
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


int main(void)
{
    const int width = 800;
    const int height = 450;

    InitWindow(width, height, "Spinner");
    ChangeDirectory(GetApplicationDirectory());
    InitAudioDevice();
    
    Texture2D blacksun = LoadTexture("../res/sunwheel.png");
    Texture2D hyperborea = LoadTexture("../res/hyperborea.png");
    Music song = LoadMusicStream("../res/h4rdjump1988.mp3");
    Font tannenberg = LoadFont("../res/Tannenbergfett.ttf");
    // Font tannenberg = LoadFont("../res/Tannenbergfett.otf");
    
    
    GuiLoadStyleDefault();
    GuiSetFont(tannenberg);
    PlayMusicStream(song);

    int frameWidth = blacksun.width;
    int frameHeight = blacksun.height;
    
    // Source rectangle (part of the texture to use for drawing)
    Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };
    
    // Destination rectangle (screen rectangle where drawing part of texture)
    Rectangle destRec = { width/2.0f, height/2.0f, frameWidth*2.0f, frameHeight*2.0f };
    
    // Origin of the texture (rot/scale point) it is relative to dest rec size
    Vector2 origin = { (float)frameWidth, (float)frameHeight };
    Vector2 textOrigin = {-800, 400};
    
    float textSpeed = 50.0f;
    int rotation = 0;
    
    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // keybinds
        SetExitKey(KEY_ESCAPE | KEY_Q);
        if(IsKeyDown(KEY_M)) {
            if (IsMusicStreamPlaying(song)) {
                PauseMusicStream(song);
                    
            }
            else {
                ResumeMusicStream(song);
            }
        }

        // Update loop
        rotation++;
        UpdateMusicStream(song);
        textOrigin.x += GetFrameTime() * 6.0 * textSpeed;

        if (textOrigin.x >= width + 800) textOrigin.x = -800;
      
        // drawing stuff to the screen
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(hyperborea, 0, 0, WHITE);
            DrawTexturePro(blacksun, sourceRec, destRec, origin, (float)rotation, WHITE);
            DrawTextEx(tannenberg, "We must secure the existence of our people and a future for white children, because the beauty of the White Aryan woman must not perish from the earth.", textOrigin, 20, 2, RAYWHITE);
        EndDrawing();
    }

    // deinit
    UnloadTexture(blacksun);        // Texture unloading
    UnloadTexture(hyperborea);
    UnloadMusicStream(song);
    CloseAudioDevice();
    CloseWindow();               
    return 0;
}