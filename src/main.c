// Copyright (C) 2026  Anonymous

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
        SetExitKey(KEY_ESCAPE);
        if(IsKeyDown(KEY_M)) {
            if (IsMusicStreamPlaying(song)) {
                PauseMusicStream(song);
                    
            }
            else {
                ResumeMusicStream(song);
            }
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_PRINT_SCREEN)) {
            TakeScreenshot("../screen.png");
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