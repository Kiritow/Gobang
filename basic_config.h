#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

#include <SDL2/SDL.h>
#undef main

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_Mixer.h>

class NonCopyable
{
public:
    NonCopyable()=default;
    ~NonCopyable()=default;
    NonCopyable(const NonCopyable&)=delete;
    NonCopyable& operator = (const NonCopyable&)=delete;
};
