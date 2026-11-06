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

#if __STDC_VERSION__ <= 202311L
#include <stdbool.h>
#endif
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define LUA_ENABLED 0
#if LUA_ENABLED
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif
#include "ffmpeg.h"
#include "tinyfiledialogs.h"


char const *lFilterPatterns[6] = {"*.png", "*.mp3", "*.wav", "*.ogg", "*.flac", "*.webp" };
const int width = 800;
const int height = 450;

int main(void)
{
    
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Spinner");
    ChangeDirectory(GetApplicationDirectory());
    GuiLoadStyleDefault();
    InitAudioDevice();
    #if LUA_ENABLED
    
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // if (luaL_dofile(L, "./../etc/config.lua") == LUA_OK) {
    //     lua_getglobal(L, "logoPath");
    //     lua_ logoPathLua[256] = lua_tostring(L, -1);
    //     printf("recieved: %s", logoPathLua);
    //     printf("[C] Executed string s\n");
    // } else {
    //     printf("[C] Error reading script\n");
    //     luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
    // }
    if (luaL_loadfile(L, "./../etc/config.lua") || lua_pcall(L, 0, 0, 0)) {
        printf("Errors were found:\n%s\n", lua_tostring(L, -1));
    }
    #endif

    char iconPath[256] = "../icon.png";
    char backgroundPath[256] = "../res/soviet.png";
    char logoPath[256] = "../res/leftysoy-entrance-emblem.png"; // changed it because i don't want to get banned. i will probably still get banned cause it's le bad or something.
    char songPath[256] = "../res/Sidewalks_And_Skeletons_Crawlspace_Witch_House_ncs.mp3";
    // static char fontpath[] = "../res/tannenbergfett.ttf";
    char fonts[3][256] = {"../res/DotGothic16-Regular.ttf", "../res/anonymous_pro_bold.ttf", "../res/TannenbergFett.ttf"};
    char text[256] = "workers of the world, unite!";
    int fontSize = 60;
    char userInputBuffer[256];
    Texture2D logo = LoadTexture(logoPath);
    Texture2D background = LoadTexture(backgroundPath);
    Image icon = LoadImage(iconPath);
    Music song = LoadMusicStream(songPath);
    Font tannenberg = LoadFontEx(fonts[1], fontSize, 0, 0);
    RenderTexture2D screen = LoadRenderTexture(width, height);
    Image screenImage;

    PlayMusicStream(song);
    SetWindowIcon(icon);
    
    int frameWidth = logo.width;
    int frameHeight = logo.height;
    
    Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight}; // Source rectangle (part of the texture to use for drawing)
    Rectangle destRec = { width/2.0f, height/2.0f, frameWidth*2.0f, frameHeight*2.0f }; // Destination rectangle (screen rectangle where drawing part of texture)
    Vector2 origin = { (float)frameWidth, (float)frameHeight }; // Origin of the texture (rot/scale point) it is relative to dest rec size
    Vector2 textOrigin = {-800, 400};    
    float textSpeed = 50.0f;
    int rotation = 0;

    bool showHelpMenu = false;
    bool showOptionsMenu = false;
    // bool *loopMusic = &song.looping;
    bool recording = false;
    bool showEditTextMenu = false;
    bool showEditLogoPathMenu = false;
    bool showEditBackgroundPathMenu = false;
    bool showEditSongPathMenu = false;
    bool showEditFontPathMenu = false;
    int activeButton = 1;
    float songVolume = 1.0;
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
        if (IsKeyPressed(KEY_S) && IsKeyPressed(KEY_LEFT_SHIFT))
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
        if ((!showHelpMenu || !showOptionsMenu) && !recording && IsKeyPressed(KEY_R))
        {
            GuiDisable();
            recording = true;
            char *saveVideoFilePath = tinyfd_saveFileDialog("save recorded video", "../res/", 1, lFilterPatterns, "video");
            if (saveVideoFilePath == NULL) {
                tinyfd_messageBox("Notice", "Filename is not specified, saving in default location \x28./../tmp/screen.mp4\x29", "ok", "info", 1);
                ffmpeg = ffmpeg_start_rendering("./../tmp/screen.mp4", width, height, 60, songPath);
            }
            else
            {
                strncpy(userInputBuffer, saveVideoFilePath, sizeof(userInputBuffer));
                ffmpeg = ffmpeg_start_rendering(userInputBuffer, width, height, 60, songPath);
            }
        }
        else if (recording && IsKeyPressed(KEY_R))
        {   
            recording = false;
            ffmpeg_end_rendering(ffmpeg, recording);
            ffmpeg = 0;
            GuiEnable();
        }

        // Update loop
        rotation++;
        UpdateMusicStream(song);
        textOrigin.x += GetFrameTime() * 6.0 * textSpeed;
        if (textOrigin.x >= width + 800) textOrigin.x = -800;
        
        // drawing stuff to the screen
        BeginDrawing();
            ClearBackground(RAYWHITE);
            if (!recording) {
                
                DrawTexture(background, 0, 0, WHITE);
                DrawTexturePro(logo, sourceRec, destRec, origin, (float)rotation, WHITE);
                DrawTextEx(tannenberg, text, textOrigin, fontSize, 2, RAYWHITE);
            }
            else {
                BeginTextureMode(screen);
                    DrawTexture(background, 0, 0, WHITE);
                    DrawTexturePro(logo, sourceRec, destRec, origin, (float)rotation, WHITE);
                    DrawTextEx(tannenberg, text, textOrigin, fontSize, 2, RAYWHITE);
                    screenImage = LoadImageFromTexture(screen.texture);
                    ffmpeg_send_frame_flipped(ffmpeg, screenImage.data, screenImage.width, screenImage.height);
                EndTextureMode();    
            }
            if (showOptionsMenu) 
            {
                DrawRectangle(600, 0, 400, height, RAYWHITE);
                GuiLabel((Rectangle){650, 5, 100, 20}, "Logo");
                GuiToggle((Rectangle){650, 30, 100, 20}, "open file", &showEditLogoPathMenu);
                if (showEditLogoPathMenu)
                {
                    char *logoFilePathBuffer = tinyfd_openFileDialog("Choose a logo to spin", "../res/", 1, lFilterPatterns, "pictures", 0);
                    // GuiTextInputBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Change Logo Texture", "change the logo texture on the screen", userInputBuffer, fontSize, "Set\nCancel", &activeButton, 0);
                    if (logoFilePathBuffer == NULL)
                    {
                        TraceLog(LOG_ERROR, "You must select a file.");
                        tinyfd_messageBox("error", "You must select a file.", "ok", "error", 1);
                        continue;
                    }
                    else
                    {
                        strncpy(logoPath, logoFilePathBuffer, sizeof(logoPath));
                        logo = LoadTexture(logoPath);
                        // memset(logoFilePathBuffer, '\0', 256);
                        continue;
                    }
                }
                
                GuiLabel((Rectangle){650, 55, 100, 20}, "Background");
                GuiToggle((Rectangle){650, 80, 100, 20}, "open file", &showEditBackgroundPathMenu);
                if (showEditBackgroundPathMenu)
                {
                    
                    char *backgroundFilePathBuffer = tinyfd_openFileDialog("Choose a background", "../res/", 1, lFilterPatterns, "pictures", 0);
                    if (backgroundFilePathBuffer == NULL)
                    {
                        TraceLog(LOG_ERROR, "You must select a file.");
                        tinyfd_messageBox("error", "You must select a file.", "ok", "error", 1);
                        continue;
                    }
                    else
                    {
                        strncpy(backgroundPath, backgroundFilePathBuffer, sizeof(backgroundPath));
                        background = LoadTexture(backgroundPath);
                        // memset(backgroundFilePathBuffer, '\0', 256);
                        continue;
                    }

                }
                GuiLabel((Rectangle){650, 105, 60, 20}, "Music");
                GuiToggle((Rectangle){650, 130, 100, 20}, "open file", &showEditSongPathMenu);
                if (showEditSongPathMenu)
                {
                    char *songFilePathBuffer = tinyfd_openFileDialog("Choose a song to play", "../res/", 1, lFilterPatterns, "songs", 0);
                    if (songFilePathBuffer == NULL)
                    {
                        TraceLog(LOG_ERROR, "You must select a file.");
                        tinyfd_messageBox("error", "You must select a file.", "ok", "error", 1);
                        continue;
                    }
                    else
                    {
                        strncpy(songPath, songFilePathBuffer, sizeof(songPath));
                        song = LoadMusicStream(songPath);
                        PlayMusicStream(song);
                        // memset(songFilePathBuffer, '\0', 256);
                        continue;
                    }

                }
                GuiToggle((Rectangle){650, 155, 100, 20}, "Loop music", &song.looping);
                if (GuiValueBoxFloat((Rectangle){650, 180, 100, 20}, "Volume", TextFormat("%f", songVolume), &songVolume, 0))
                {
                    SetMusicVolume(song, songVolume);
                }
                
                // GuiValueBox(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode); // Value Box control, updates input text with numbers

                
                GuiLabel((Rectangle){650, 205, 60, 20}, "font");
                GuiToggleSlider((Rectangle){650, 230, 100, 20}, "Tannenberg Fett; Anonymous Code Pro;DotGothic", &activeButton);
                if (showEditFontPathMenu)
                {
                    // GuiTextInputBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Change font", "change the font currently displayed", fontPath, fontSize, "Set\nCancel", &activeButton, 0);
                    
                }
                GuiToggle((Rectangle){650, 255, 100, 20}, "Change Text", &showEditTextMenu);
                if (showEditTextMenu)
                {
                    // GuiMessageBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Message Box", "This is a GuiMessageBox!", "Set\nCancel", currentActiveButton);
                    GuiTextInputBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "Change text", "change the scrolling text on the screen", text, fontSize, "Set\nCancel", &activeButton, 0);
                }
                GuiSpinner((Rectangle){650, 280, 100, 20}, "Font size", &fontSize, 0, 100, true);
            }
            else if (showHelpMenu)
            {
                GuiMessageBox((Rectangle){width / 2 - 250, height /2 - 100, 500, 200}, "About", "A utility to display your favorite logo and spin it around.\n1. F1 - Show a help menu\n2. Shift+S - Take a screenshot\n3. R - Record a video\n4. ESCAPE - Open an options menu\n5. SPACE - Pause music", "Ok", 0);
            }
            DrawTexture(screen.texture, 0, 0, WHITE);
        EndDrawing();
    }

    // deinit
    UnloadTexture(logo);        // Texture unloading
    UnloadImage(screenImage);
    UnloadRenderTexture(screen);
    UnloadTexture(background);
    UnloadMusicStream(song);
    CloseAudioDevice();
    CloseWindow();               
    return 0;
}