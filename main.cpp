#include "config.h"
#include "InitManager.h"
#include "GameGUI.h"
#include <fstream>
using namespace std;


int main()
{
    printf("Checking Debug Mode...\n");
    FILE* fp=fopen("isDebug.txt","r");
    if(fp==NULL)
    {
        HideConsole();
    }
    else fclose(fp);
    printf("Initializing...\n");
    int font_open_ret=systtf.openFont("msyh.ttf",20);
    if(font_open_ret<0)
    {
        /// Cannot open msyh.ttf
        return 1;
    }
    wnd=SDL_CreateWindow("Loading...",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIN_WIDTH,WIN_HEIGHT,SDL_WINDOW_SHOWN);
    rnd=SDL_CreateRenderer(wnd,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_TARGETTEXTURE);
    //LoopFrontPage();
    LoopLoading();
    int _loop=1;
    while(_loop)
    {
        switch(LoopMenu())
        {
        case 0:/// Exit Game
            _loop=0;
            break;
        case 1:/// Single Player
            LoopSinglePlayer();
            break;
        case 2:/// Multi Player
            LoopMultiPlayer();
            break;
        case 3:/// About
            LoopAbout();
            break;
        }
    }
    LoopExiting();
    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(wnd);
    return 0;
}
