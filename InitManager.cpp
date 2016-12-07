#include "InitManager.h"

InitManager_SDL::InitManager_SDL()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        Global::ErrorQuit("Failed to init SDL2.");
    }
}

InitManager_SDL::~InitManager_SDL()
{
    SDL_Quit();
}

InitManager_IMG::InitManager_IMG()
{
    if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)<0)
    {
        Global::ErrorQuit("Failed to init SDL2 Image.");
    }
}

SDL_Surface* InitManager_IMG::loadimage(const char* Filename)
{
    return IMG_Load(Filename);
}

InitManager_IMG::~InitManager_IMG()
{
    IMG_Quit();
}

InitManager_TTF::InitManager_TTF()
{
    if(TTF_Init()<0)
    {
        Global::ErrorQuit("Failed to init SDL2 TTF.");
    }
    _font=NULL;
}

int InitManager_TTF::openFont(const char* FileName,int Size)
{
    _font=TTF_OpenFont(FileName,Size);
    if(_font==NULL) return -1;
    else return 0;
}

TTF_Font* InitManager_TTF::font()
{
    return _font;
}

int InitManager_TTF::closeFont()
{
    TTF_CloseFont(_font);
    _font=NULL;
    return 0;
}

InitManager_TTF::~InitManager_TTF()
{
    if(_font) closeFont();
}

InitManager_Mix::InitManager_Mix()
{
    if(Mix_Init(MIX_INIT_MP3)<0)
    {
        Global::ErrorQuit("Failed to Init Mixer.");
    }
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,512)<0)
    {
        Global::ErrorQuit("Failed to OpenAudio");
    }
}

InitManager_Mix::~InitManager_Mix()
{
    Mix_CloseAudio();
    Mix_Quit();
}

InitManager_SDL syssdl;
InitManager_IMG sysimg;
InitManager_TTF systtf;
InitManager_Mix sysmix;

InitTextureManager::InitTextureManager()
{

}

InitTextureManager::~InitTextureManager()
{
    while(vec.size())
    {
        SDL_DestroyTexture(vec.back());
        vec.pop_back();
    }
}

void InitTextureManager::add(SDL_Texture* texture)
{
    vec.push_back(texture);
}

void InitTextureManager::pop()
{
    if(vec.size()) vec.pop_back();
}

InitTextureManager systext;
