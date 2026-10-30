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

#include <stdio.h>
#if __STDC_VERSION__ <= 202311L
#include <stdbool.h>
#endif
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "ffmpeg.h"
#include "config.h"



int main(void)
{
    const int width = 800;
    const int height = 450;
    
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Spinner");
    ChangeDirectory(GetApplicationDirectory());
    GuiLoadStyleDefault();
    InitAudioDevice();
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadfile(L, "./../etc/config.lua") || lua_pcall(L, 0, 0, 0)) {
        printf("Errors were found:\n%s\n", lua_tostring(L, -1));
    }
    
    Texture2D logo = LoadTexture(logoPath);
    Texture2D background = LoadTexture(backgroundPath);
    Image icon = LoadImage(iconPath);
    Music song = LoadMusicStream(songPath);
    Font tannenberg = LoadFontEx(fontPath, fontSize, 0, 0);


    PlayMusicStream(song);
    SetWindowIcon(icon);
    
    int frameWidth = logo.width;
    int frameHeight = logo.height;
    
    // Source rectangle (part of the texture to use for drawing)
    Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };
    
    // Destination rectangle (screen rectangle where drawing part of texture)
    Rectangle destRec = { width/2.0f, height/2.0f, frameWidth*2.0f, frameHeight*2.0f };
    
    // Origin of the texture (rot/scale point) it is relative to dest rec size
    Vector2 origin = { (float)frameWidth, (float)frameHeight };
    Vector2 textOrigin = {-800, 400};
    
    float textSpeed = 50.0f;
    int rotation = 0;

    bool showHelpMenu = false;
    bool showOptionsMenu = false;
    bool *loopMusic = &song.looping;
    bool recording = false;
    int activeButton = 1;
    int* currentActiveButton = &activeButton;
    FFMPEG *ffmpeg;
    
    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        // keybinds
        SetExitKey(KEY_NULL);
        if(IsKeyPressed(KEY_SPACE))
        {
            if (IsMusicStreamPlaying(song))
            {
                PauseMusicStream(song);
            }
            else
            {
                ResumeMusicStream(song);
            }
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_PRINT_SCREEN))
        {
            TakeScreenshot("../tmp/screen.png");
        }
        if (!showOptionsMenu && IsKeyPressed(KEY_ESCAPE))
        {
            showOptionsMenu = true;
        }
        else if (showOptionsMenu && IsKeyPressed(KEY_ESCAPE))
        {
            showOptionsMenu = false;
        }
        if (!showHelpMenu && IsKeyPressed(KEY_F1))
        {
            showHelpMenu = true;
        }
        else if (showHelpMenu && IsKeyPressed(KEY_F1))
        {
            showHelpMenu = false;
        }
        if (!recording && IsKeyPressed(KEY_R))
        {
            ffmpeg = ffmpeg_start_rendering("../tmp/screen.mp4", 800, 450, 60, "../tmp/screen.mp3");
        }
        else if (recording && IsKeyPressed(KEY_R))
        {
            ffmpeg_end_rendering(ffmpeg, 1);
        }
        // Update loop
        rotation++;
        UpdateMusicStream(song);
        textOrigin.x += GetFrameTime() * 6.0 * textSpeed;

        if (textOrigin.x >= width + 800) textOrigin.x = -800;
      
        // drawing stuff to the screen
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexture(background, 0, 0, WHITE);
            DrawTexturePro(logo, sourceRec, destRec, origin, (float)rotation, WHITE);
            DrawTextEx(tannenberg, text, textOrigin, fontSize, 2, RAYWHITE);
            if (showOptionsMenu) 
            {
                DrawRectangle(600, 0, 400, height, RAYWHITE);
                GuiLabel((Rectangle){650, 5, 100, 20}, "Logo");
                if (GuiButton((Rectangle){650, 30, 100, 20}, "open file"))
                {
                    // logo = LoadTexture("../res/hyperborea.png");
                    GuiTextInputBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Change Logo Texture", "change the logo texture on the screen", logoPath, fontSize, "Set\nCancel", currentActiveButton, 0);
                    
                }
                GuiLabel((Rectangle){650, 55, 100, 20}, "Background");
                GuiButton((Rectangle){650, 80, 100, 20}, "open file");
                GuiLabel((Rectangle){650, 105, 60, 20}, "Music");
                GuiButton((Rectangle){650, 130, 100, 20}, "open file");
                GuiToggle((Rectangle){650, 155, 100, 20}, "Loop music", loopMusic);
                GuiLabel((Rectangle){650, 180, 60, 20}, "font");
                GuiButton((Rectangle){650, 205, 100, 20}, "open file");
                if (GuiButton((Rectangle){650, 230, 100, 20}, "Change Text"))
                {
                    // GuiMessageBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Message Box", "This is a GuiMessageBox!", "Set\nCancel", currentActiveButton);
                    GuiTextInputBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Change text", "change the scrolling text on the screen", text, fontSize, "Set\nCancel", currentActiveButton, 0);
                }
                GuiSpinner((Rectangle){650, 255, 100, 20}, "Font size", &fontSize, 0, 100, true);
            }
            else if (showHelpMenu)
            {
                GuiMessageBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "About", "A utility to display your favorite logo and spin it around.", "Ok", 0);
            }
        EndDrawing();
    }

    // deinit
    UnloadTexture(logo);        // Texture unloading
    UnloadTexture(background);
    UnloadMusicStream(song);
    CloseAudioDevice();
    CloseWindow();               
    return 0;
}

