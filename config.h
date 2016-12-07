#pragma once
#include "basic_config.h"

#include "sdl_engine.h"

namespace Global
{
    void ErrorQuit(const char* ErrorMessage);
}

extern SDL_Window* wnd;
extern SDL_Renderer* rnd;

#define WIN_WIDTH 1000
#define WIN_HEIGHT 600

#define BD_LEFT_UX 32
#define BD_LEFT_UY 32
#define BD_X_DIS 32
#define BD_Y_DIS 32

void HideConsole();
void ShowConsole();
