#include "GameGUI.h"
#include "Game.h"
#include <functional>
#include <algorithm>
#include <cpplib/cpplib#gsock>
#include <wartime/frame/MapBox.h>
using namespace std;

SDL_Texture* _background=NULL;
SDL_Texture* _board=NULL;
SDL_Texture* _spot_white=NULL;
SDL_Texture* _spot_black=NULL;
SDL_Texture* _home_gobang=NULL;
SDL_Texture* _focus=NULL;
SDL_Texture* _menu_background=NULL;
SDL_Texture* _menu_text=NULL;
SDL_Texture* _menu_light=NULL;
SDL_Texture* _box_white=NULL;
SDL_Texture* _box_black=NULL;
SDL_Texture* _box_blank=NULL;


Mix_Music* _bgm=NULL;
Mix_Chunk* _down_1=NULL;
Mix_Chunk* _down_2=NULL;
Mix_Chunk* _down_3=NULL;
Mix_Chunk* _effect_clicked=NULL;
Mix_Chunk* _effect_mouseover=NULL;

SDL_Texture* _button_menu_singleplayer=NULL;
SDL_Texture* _button_menu_multiplayer=NULL;
SDL_Texture* _button_menu_about=NULL;
SDL_Texture* _button_menu_exit=NULL;
SDL_Texture* _button_glass_basic=NULL;
SDL_Texture* _button_glass_highlight=NULL;

SDL_Texture* _text_singleplayer_lost=NULL;
SDL_Texture* _text_singleplayer_win=NULL;
SDL_Texture* _text_searching_server=NULL;
SDL_Texture* _text_singleplayer_easy=NULL;
SDL_Texture* _text_singleplayer_normal=NULL;
SDL_Texture* _text_singleplayer_hard=NULL;
SDL_Texture* _text_singleplayer_choose=NULL;
SDL_Texture* _text_multi_beserver=NULL;
SDL_Texture* _text_multi_beclient=NULL;
SDL_Texture* _text_multi_back=NULL;


/// clicked mouseover normal
#define BNAME(ButtonName,Status) _button_##ButtonName##_##Status
#define BNAMEX(ButtonName) SDL_Texture* BNAME(ButtonName,clicked)=NULL;SDL_Texture* BNAME(ButtonName,mouseover)=NULL;SDL_Texture* BNAME(ButtonName,normal)=NULL
#define LoadButton(ButtonName,Status) BNAME(ButtonName,Status)=MyLoadImage(rnd,"img\\button_" #ButtonName "_" #Status ".png",NULL,NULL)
#define LoadButtonX(ButtonName) LoadButton(ButtonName,clicked);LoadButton(ButtonName,mouseover);LoadButton(ButtonName,normal)
BNAMEX(restart);
BNAMEX(cancel);
BNAMEX(givein);
BNAMEX(exit);
BNAMEX(withdraw);
BNAMEX(other);

/// Pre-Rendered Buttons.
BNAMEX(creategame);
BNAMEX(joingame);
BNAMEX(multicancel);

const int _button_w=115;
const int _button_h=55;

#include "xbutton.hpp"

auto _play_clicked_basic=[&](xbutton& b)
{
    if(b.status!=2) Mix_PlayChannel(-1,_effect_clicked,0);
};
auto _play_mouseover_basic=[&](xbutton& b)
{
    if(b.status!=1) Mix_PlayChannel(-1,_effect_mouseover,0);
};
#define _play_clicked(ButtonVariableName) [&](){_play_clicked_basic(ButtonVariableName);}
#define _play_mouseover(ButtonVariableName) [&](){_play_mouseover_basic(ButtonVariableName);}

void LoopFrontPage()
{
    SDL_Texture* text=MyLoadImage(rnd,"img\\frontpage_prefer.png",NULL,NULL);
    SDL_Rect rect;
    rect.w=1000;
    rect.h=600;
    rect.x=0;
    rect.y=0;
    for(int i=0;i<=255;i++)
    {
        SDL_RenderClear(rnd);
        printf("SetAlphaMod returns %d\n",SDL_SetTextureAlphaMod(text,i));
        SDL_RenderCopy(rnd,text,NULL,&rect);
        SDL_RenderPresent(rnd);
        SDL_PollEvent(NULL);
        SDL_Delay(800/255);
        printf("Overlay %d\n",i);
    }

    SDL_Delay(1500);

    for(int i=255;i>=0;i--)
    {
        SDL_RenderClear(rnd);
        printf("SetAlphaMod returns %d\n",SDL_SetTextureAlphaMod(text,i));
        SDL_RenderCopy(rnd,text,NULL,NULL);
        SDL_RenderPresent(rnd);
        SDL_PollEvent(NULL);
        SDL_Delay(800/255);
        printf("Overlay %d\n",i);
    }
}

int ProcLoading(void* iStopFlag)
{
    _background=MyLoadImage(rnd,"img\\background.png",NULL,NULL);
    _board=MyLoadImage(rnd,"img\\board.png",NULL,NULL);
    _spot_white=MyLoadImage(rnd,"img\\white_spot.png",NULL,NULL);
    _spot_black=MyLoadImage(rnd,"img\\black_spot.png",NULL,NULL);
    _focus=MyLoadImage(rnd,"img\\focus.png",NULL,NULL);
    _home_gobang=MyLoadImage(rnd,"img\\home.png",NULL,NULL);
    _menu_background=MyLoadImage(rnd,"img\\menu_bg.png",NULL,NULL);
    _menu_text=MyLoadImage(rnd,"img\\menu_text.png",NULL,NULL);
    _menu_light=MyLoadImage(rnd,"img\\menu_highlight.png",NULL,NULL);
    _box_black=MyLoadImage(rnd,"img\\box_black.png",NULL,NULL);
    _box_white=MyLoadImage(rnd,"img\\box_white.png",NULL,NULL);
    _box_blank=MyLoadImage(rnd,"img\\box_blank.png",NULL,NULL);

    LoadButtonX(restart);
    LoadButtonX(cancel);
    LoadButtonX(givein);
    LoadButtonX(other);
    LoadButtonX(exit);
    LoadButtonX(withdraw);

    /// Memory Leak?
    SDL_Surface* _cursor_surf=IMG_Load("img\\mouse.png");
    SDL_Cursor* _cursor=SDL_CreateColorCursor(_cursor_surf,0,0);
    SDL_SetCursor(_cursor);

    _bgm=Mix_LoadMUS("mp3\\bgm.mp3");
    _down_1=Mix_LoadWAV("mp3\\down_1.wav");
    _down_2=Mix_LoadWAV("mp3\\down_2.wav");
    _down_3=Mix_LoadWAV("mp3\\down_3.wav");
    _effect_clicked=Mix_LoadWAV("mp3\\effect_clicked.wav");
    _effect_mouseover=Mix_LoadWAV("mp3\\effect_mouseover.wav");

    _button_menu_singleplayer=RenderUTF8(rnd,systtf.font(),"单人游戏",color_white,NULL,NULL);
    _button_menu_multiplayer=RenderUTF8(rnd,systtf.font(),"多人游戏",color_white,NULL,NULL);
    _button_menu_about=RenderUTF8(rnd,systtf.font(),"关于",color_white,NULL,NULL);
    _button_menu_exit=RenderUTF8(rnd,systtf.font(),"退出游戏",color_white,NULL,NULL);

    _text_singleplayer_lost=RenderUTF8(rnd,systtf.font(),"你输了~",color_white,NULL,NULL);
    _text_singleplayer_win=RenderUTF8(rnd,systtf.font(),"你赢了！",color_white,NULL,NULL);
    _text_searching_server=RenderUTF8(rnd,systtf.font(),"正在搜索对局...",color_white,NULL,NULL);
    _text_singleplayer_choose=RenderUTF8(rnd,systtf.font(),"请选择难度",color_white,NULL,NULL);
    _text_singleplayer_easy=RenderUTF8(rnd,systtf.font(),"小白",color_white,NULL,NULL);
    _text_singleplayer_normal=RenderUTF8(rnd,systtf.font(),"普通",color_white,NULL,NULL);
    _text_singleplayer_hard=RenderUTF8(rnd,systtf.font(),"大师",color_white,NULL,NULL);

    /**
    // A Small Example of PreRendering
    SDL_Texture* newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,rect.w,rect.h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rnd,128,128,128,128);
    SDL_RenderFillRect(rnd,NULL);
    /// Reset Render Target to Window
    SDL_SetRenderTarget(rnd,NULL);
    //SDL_SetTextureAlphaMod(newtext,128);
    */

    /// Pre-Rendering Textures
    int _glass_w,_glass_h;
    _button_glass_basic=MyLoadImage(rnd,"img\\button_glass_basic.png",&_glass_w,&_glass_h);
    _button_glass_highlight=MyLoadImage(rnd,"img\\button_glass_highlight.png",NULL,NULL);

    InitManager_TTF thisf;
    thisf.openFont("msyh.ttf",25);

    /// Pre-Rendering Variables
    SDL_Texture* newtext;
    int _txt_w,_txt_h;

    // CreateGame
    _text_multi_beserver=RenderUTF8(rnd,thisf.font(),"创建游戏",color_white,&_txt_w,&_txt_h);
    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    SDL_RenderCopy(rnd,_button_glass_highlight,NULL,NULL);
    TextureDraw(rnd,_text_multi_beserver,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(creategame,mouseover)=newtext;

    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    TextureDraw(rnd,_text_multi_beserver,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(creategame,clicked)=newtext;

    BNAME(creategame,normal)=newtext;

    // JoinGame
    _text_multi_beclient=RenderUTF8(rnd,thisf.font(),"加入游戏",color_white,&_txt_w,&_txt_h);
    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    SDL_RenderCopy(rnd,_button_glass_highlight,NULL,NULL);
    TextureDraw(rnd,_text_multi_beclient,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(joingame,mouseover)=newtext;

    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    TextureDraw(rnd,_text_multi_beclient,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(joingame,clicked)=newtext;

    BNAME(joingame,normal)=newtext;

    // Multi-Cancel
    _text_multi_back=RenderUTF8(rnd,thisf.font(),"返回",color_white,&_txt_w,&_txt_h);
    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    SDL_RenderCopy(rnd,_button_glass_highlight,NULL,NULL);
    TextureDraw(rnd,_text_multi_back,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(multicancel,mouseover)=newtext;

    newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,_glass_w,_glass_h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd,_button_glass_basic,NULL,NULL);
    TextureDraw(rnd,_text_multi_back,_glass_w/2-_txt_w/2,_glass_h/2-_txt_h/2-5);
    BNAME(multicancel,clicked)=newtext;

    BNAME(multicancel,normal)=newtext;

    /// (Pre-Rendering Finish) Reset Render Target to Window
    SDL_SetRenderTarget(rnd,NULL);

    /// Loaded
    SDL_SetWindowTitle(wnd,"GoBang 2016");

    *(int*)iStopFlag=1;
    return 0;
}

void LoopLoading()
{
    /// Load The Loading Resources.
    int w,h;
    SDL_Texture* tx_a=RenderText(rnd,systtf.font(),"Loading...",color_white,&w,&h);
    SDL_RenderClear(rnd);
    TextureDraw(rnd,tx_a,WIN_WIDTH/2-w/2,WIN_HEIGHT/2-h/2);
    SDL_RenderPresent(rnd);
    ///  Start Background Worker For Loading...
    int finish=0;
    SDL_Thread* td=SDL_CreateThread(ProcLoading,"Loading Thread",&finish);
    SDL_Event e;
    while(!finish)
    {
        SDL_WaitEventTimeout(&e,100);
    }
    SDL_WaitThread(td,NULL);
}

int LoopMenu()
{
    /// Start Music
    Mix_PlayMusic(_bgm,-1);

    int running=1;
    int need_update=1;
    SDL_Event e;
    int light_w,light_h;
    SDL_QueryTexture(_menu_light,NULL,NULL,&light_w,&light_h);
    while(running)
    {
        function<void()> work;
        while(!need_update&&SDL_WaitEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                Global::ErrorQuit("SDL_QUIT Received.");
                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    /// Check If Mouse Is in Rectangle.
                    if(isInRect(e.motion.x,e.motion.y,820-light_w/2,200-light_h/2,820+light_w/2,200+light_h/2))
                    {
                        LoopSinglePlayer();
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,290-light_h/2,820+light_w/2,290+light_h/2))
                    {
                        LoopMultiPlayer();
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,385-light_h/2,820+light_w/2,385+light_h/2))
                    {
                        LoopAbout();
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,470-light_h/2,820+light_w/2,470+light_h/2))
                    {
                        need_update=1;
                        return 0;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                {
                    /// Check If Mouse Is in Rectangle.
                    if(isInRect(e.motion.x,e.motion.y,820-light_w/2,200-light_h/2,820+light_w/2,200+light_h/2))
                    {
                        work=[&]()
                        {
                            TextureDraw(rnd,_menu_light,820-light_w/2+10,200-light_h/2+10);
                        };
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,290-light_h/2,820+light_w/2,290+light_h/2))
                    {
                        work=[&]()
                        {
                            TextureDraw(rnd,_menu_light,820-light_w/2+10,290-light_h/2+10);
                        };
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,385-light_h/2,820+light_w/2,385+light_h/2))
                    {
                        work=[&]()
                        {
                            TextureDraw(rnd,_menu_light,820-light_w/2,385-light_h/2);
                        };
                        need_update=1;
                    }
                    else if(isInRect(e.motion.x,e.motion.y,820-light_w/2,470-light_h/2,820+light_w/2,470+light_h/2))
                    {
                        work=[&]()
                        {
                            TextureDraw(rnd,_menu_light,820-light_w/2,470-light_h/2+10);
                        };
                        need_update=1;
                    }
                }
                break;
            }
        }

        SDL_RenderClear(rnd);
        TextureDraw(rnd,_menu_background,0,0);
        if(work) work();
        TextureDraw(rnd,_menu_text,735,160);

        SDL_RenderPresent(rnd);

        need_update=0;
    }
    return 0;
}

int LoopSinglePlayerFinish(int winner)
{
    int running=1;
    int need_update=1;
    SDL_Event e;

    int _b_restart=2;

    int Y=150;
    int C=55;
    int S=20;
    int L,B;
    SDL_Texture* text;
    if(winner==2)
    {
        text=_text_singleplayer_win;
    }
    else
    {
        text=_text_singleplayer_lost;
    }
    SDL_QueryTexture(text,NULL,NULL,&L,&B);
    int A=(WIN_HEIGHT-2*Y-C-B)/3;

    printf("A=%d Y=%d B=%d L=%d C=%d\n",A,Y,B,L,C);

    xbutton restart;
    restart.y=WIN_HEIGHT-Y-A-C/2;
    restart.x=WIN_WIDTH/2-20-115/2;

    restart.text_clicked=BNAME(restart,clicked);
    restart.text_mouseover=BNAME(restart,mouseover);
    restart.text_normal=BNAME(restart,normal);
    restart.centered=1;
    restart._onrelease=[&]()
    {
        _b_restart=1;
    };


    xbutton cancel;
    cancel.y=WIN_HEIGHT-Y-A-C/2;
    cancel.x=WIN_WIDTH/2+20+115/2;
    cancel.text_clicked=BNAME(cancel,clicked);
    cancel.text_mouseover=BNAME(cancel,mouseover);
    cancel.text_normal=BNAME(cancel,normal);
    cancel.centered=1;
    cancel._onrelease=[&]()
    {
        _b_restart=0;
    };


    SDL_Rect rect;
    rect.w=L;
    rect.h=B;
    rect.x=WIN_WIDTH/2-L/2;
    rect.y=Y+A;

    SDL_Rect rectx;
    rectx.w=max(L+2*S,2*_button_w+20*2+S*2);
    rectx.h=B+C+3*A;
    rectx.x=min(WIN_WIDTH/2-L/2-S,WIN_WIDTH/2-20-_button_w-S);
    rectx.y=Y;

    SDL_Rect rectxx;
    rectxx.x=rectx.x-5;
    rectxx.y=rectx.y-5;
    rectxx.w=rectx.w+10;
    rectxx.h=rectx.h+10;

    ClearMessageQueue();

    while(running)
    {
        while(!need_update&&SDL_WaitEvent(&e))
        {
            if(!restart.deal(e))
            {
                if(_b_restart==1) return 1;
            }
            else if(!cancel.deal(e))
            {
                if(_b_restart==0) return 0;
            }

            need_update=1;
        }
        SDL_RenderClear(rnd);
        restart.draw(rnd);
        cancel.draw(rnd);
        SDL_RenderCopy(rnd,text,NULL,&rect);
        SDL_SetRenderDrawColor(rnd,255,255,255,0);
        SDL_RenderDrawRect(rnd,&rectx);
        SDL_RenderDrawRect(rnd,&rectxx);
        SDL_SetRenderDrawColor(rnd,0,0,0,0);
        SDL_RenderPresent(rnd);
        need_update=0;
    }

    return 0;
}



int LoopGetAIHardness()
{
    /// If you don't need this, then return 1 for normal level.
    //return 1;

    int running=1;
    SDL_Event e;
    int need_update=1;

    int hardness=-1;

    int _text_singleplayer_sz_w,_text_singleplayer_sz_h;
    SDL_QueryTexture(_text_singleplayer_easy,NULL,NULL,&_text_singleplayer_sz_w,&_text_singleplayer_sz_h);

    xbutton choose_easy;
    xbutton choose_normal;
    xbutton choose_hard;

    choose_easy.text_clicked=BNAME(other,clicked);
    choose_easy.text_mouseover=BNAME(other,mouseover);
    choose_easy.text_normal=BNAME(other,normal);
    choose_easy.centered=1;
    SDL_QueryTexture(BNAME(other,clicked),NULL,NULL,&choose_easy.w,&choose_easy.h);
    choose_easy.x=WIN_WIDTH/2;
    choose_easy.y=WIN_HEIGHT/2-choose_easy.h-20;
    choose_easy._onrelease=[&]()
    {
        hardness=0;
    };
    choose_easy._onmouseover=_play_mouseover(choose_easy);
    choose_easy._onclick=_play_clicked(choose_easy);

    choose_normal.text_clicked=BNAME(other,clicked);
    choose_normal.text_mouseover=BNAME(other,mouseover);
    choose_normal.text_normal=BNAME(other,normal);
    choose_normal.centered=1;
    choose_normal.w=choose_easy.w;
    choose_normal.h=choose_easy.h;
    choose_normal.x=WIN_WIDTH/2;
    choose_normal.y=WIN_HEIGHT/2;
    choose_normal._onrelease=[&]()
    {
        hardness=1;
    };
    choose_normal._onmouseover=_play_mouseover(choose_normal);
    choose_normal._onclick=_play_clicked(choose_normal);

    choose_hard.text_clicked=BNAME(other,clicked);
    choose_hard.text_mouseover=BNAME(other,mouseover);
    choose_hard.text_normal=BNAME(other,normal);
    choose_hard.centered=1;
    choose_hard.w=choose_easy.w;
    choose_hard.h=choose_easy.h;
    choose_hard.x=WIN_WIDTH/2;
    choose_hard.y=WIN_HEIGHT/2+choose_hard.h+20;
    choose_hard._onrelease=[&]()
    {
        hardness=2;
    };
    choose_hard._onmouseover=_play_mouseover(choose_hard);
    choose_hard._onclick=_play_clicked(choose_hard);

    SDL_Rect rect;
    rect.w=20+20+choose_easy.w;
    rect.h=30+30+3*choose_easy.h+20*2;
    rect.x=WIN_WIDTH/2-choose_easy.w/2-20;
    rect.y=WIN_HEIGHT/2-choose_easy.h/2*3-20-30;

    SDL_Texture* newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,rect.w,rect.h);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rnd,128,128,128,128);
    SDL_RenderFillRect(rnd,NULL);
    /// Reset Render Target to Window
    SDL_SetRenderTarget(rnd,NULL);
    //SDL_SetTextureAlphaMod(newtext,128);

    while(running)
    {
        while(!need_update&&SDL_WaitEvent(&e))
        {
            choose_easy.deal(e);
            choose_normal.deal(e);
            choose_hard.deal(e);
            need_update=1;
        }

        SDL_RenderClear(rnd);
        TextureDraw(rnd,_background,0,0);
        TextureDraw(rnd,_board,240,45);
        SDL_RenderCopy(rnd,newtext,NULL,&rect);
        choose_easy.draw(rnd);
        choose_normal.draw(rnd);
        choose_hard.draw(rnd);
        TextureDraw(rnd,_text_singleplayer_easy,choose_easy.x-_text_singleplayer_sz_w/2,choose_easy.y-_text_singleplayer_sz_h/2-7);
        TextureDraw(rnd,_text_singleplayer_normal,choose_normal.x-_text_singleplayer_sz_w/2,choose_normal.y-_text_singleplayer_sz_h/2-7);
        TextureDraw(rnd,_text_singleplayer_hard,choose_hard.x-_text_singleplayer_sz_w/2,choose_hard.y-_text_singleplayer_sz_h/2-7);
        SDL_RenderPresent(rnd);
        need_update=0;

        if(hardness!=-1) running=0;
    }

    SDL_DestroyTexture(newtext);
    return hardness;
}



void LoopSinglePlayer()
{
    int hardness=LoopGetAIHardness();
    printf("AI Hardness is %d\n",hardness);

    int running=1;
    SDL_Event e;
    Game::InitSinglePlayer();

    int need_redraw=1;
    int _temp_Really=0;

    xbutton withdraw;
    xbutton givein;
    xbutton backmenu;

    withdraw.text_clicked=BNAME(withdraw,clicked);
    withdraw.text_mouseover=BNAME(withdraw,mouseover);
    withdraw.text_normal=BNAME(withdraw,normal);
    withdraw.x=795;
    withdraw.y=230;
    withdraw.centered=0;
    withdraw._onmouseover=_play_mouseover(withdraw);
    withdraw._onclick=_play_clicked(withdraw);
    withdraw._onrelease=[&]()
    {
        if(Game::CancelLast()) _temp_Really=!_temp_Really;
    };

    givein.text_clicked=BNAME(givein,clicked);
    givein.text_mouseover=BNAME(givein,mouseover);
    givein.text_normal=BNAME(givein,normal);
    givein.x=795;
    givein.y=345;
    givein.centered=0;
    givein._onmouseover=_play_mouseover(givein);
    givein._onclick=_play_clicked(givein);
    givein._onrelease=[&]()
    {
        if(LoopSinglePlayerFinish(!_temp_Really)==0)
        {
            running=0;
            need_redraw=1;
        }
        else
        {
            Game::InitSinglePlayer();
            _temp_Really=0;
        }
    };


    backmenu.text_clicked=BNAME(cancel,clicked);
    backmenu.text_mouseover=BNAME(cancel,mouseover);
    backmenu.text_normal=BNAME(cancel,normal);
    backmenu.x=795;
    backmenu.y=460;
    backmenu.centered=0;
    backmenu._onmouseover=_play_mouseover(backmenu);
    backmenu._onclick=_play_clicked(backmenu);
    backmenu._onrelease=[&]()
    {
        running=0;
    };


    while(running)
    {
        function<void()> work;
        while((!need_redraw)&&SDL_WaitEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                Global::ErrorQuit("SDL_QUIT Received.");
                break;
            case SDL_MOUSEMOTION:
                {
                    double _sensor_range_f=425/28.0;
                    int _board_area_left=290-_sensor_range_f;
                    int _board_area_right=715+_sensor_range_f;
                    int _board_area_up=80-_sensor_range_f;
                    int _board_area_down=505+_sensor_range_f;
                    /// Check If Mouse In "Board Area"
                    if(e.motion.x>_board_area_left&&e.motion.x<_board_area_right&&e.motion.y>_board_area_up&&e.motion.y<_board_area_down)
                    {
                        //printf("Mouse In Range: %d %d ",e.motion.x,e.motion.y);
                        /// The Mouse Is In Range!
                        int _sensor_id_x=(e.motion.x-_board_area_left)/_sensor_range_f/2;
                        int _sensor_id_y=(e.motion.y-_board_area_up)/_sensor_range_f/2;
                        //printf("Sensor: %d %d\n",_sensor_id_y,_sensor_id_x);
                        int curid=Game::GetSpot(_sensor_id_y,_sensor_id_x);
                        if(curid==0)
                        {
                            work=[&]()
                            {
                                /// Draw A Focus On _sensor_id_x and _sensor_id_y
                                int _draw_x=round(290+425/14.05*_sensor_id_x-16)-3;
                                int _draw_y=round(80+425/14.0*_sensor_id_y-16);
                                TextureDraw(rnd,_focus,_draw_x,_draw_y);
                            };
                            need_redraw=1;
                        }
                    }
                    else
                    {
                        /// The Mouse Is Out Of Range. Just Update The Screen.
                        need_redraw=1;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    double _sensor_range_f=425/28.0;
                    int _board_area_left=290-_sensor_range_f;
                    int _board_area_right=715+_sensor_range_f;
                    int _board_area_up=80-_sensor_range_f;
                    int _board_area_down=505+_sensor_range_f;
                    /// Check If Mouse In "Board Area"
                    if(e.motion.x>_board_area_left&&e.motion.x<_board_area_right&&e.motion.y>_board_area_up&&e.motion.y<_board_area_down)
                    {
                        printf("Click In Range: %d %d ",e.motion.x,e.motion.y);
                        /// The Mouse Is In Range!
                        int _sensor_id_x=(e.motion.x-_board_area_left)/_sensor_range_f/2;
                        int _sensor_id_y=(e.motion.y-_board_area_up)/_sensor_range_f/2;
                        //printf("Sensor: %d %d ",_sensor_id_y,_sensor_id_x);
                        int curid=Game::GetSpot(_sensor_id_y,_sensor_id_x);
                        //printf("Spot ID: %d\n",curid);
                        if(curid==0)
                        {
                            if(_temp_Really)
                            {
                                Game::SetSpot(1,_sensor_id_y,_sensor_id_x);
                                _temp_Really=0;
                            }
                            else
                            {
                                Game::SetSpot(2,_sensor_id_y,_sensor_id_x);
                                _temp_Really=1;
                            }
                            //printf("Spot Set to %d: %d %d\n",_temp_Really?2:1,_sensor_id_y,_sensor_id_x);

                            /// Play A Piece Of Music
                            do
                            {
                                int r=rand()%3;
                                if(r==0)Mix_PlayChannel(-1,_down_1,0);
                                else if(r==1)Mix_PlayChannel(-1,_down_2,0);
                                else if(r==2)Mix_PlayChannel(-1,_down_3,0);
                            }while(0);

                            Game::SetCheckWinner(_sensor_id_x,_sensor_id_y,_temp_Really?2:1);
                            int ret=Game::CheckWinner();
                            printf("Check Winner Returns %d\n",ret);
                            if(ret)
                            {
                                if(LoopSinglePlayerFinish(ret)==0)
                                {
                                    /// Return to Main Menu
                                    return;
                                }
                                else
                                {
                                    /// Start A New Game
                                    Game::InitSinglePlayer();
                                    _temp_Really=0;
                                }
                            }
                        }
                        need_redraw=1;
                    }
                    else
                    {
                        /// The Mouse Is Out Of Range. Just Update The Screen.
                        need_redraw=1;
                    }
                }
                break;
            }

            withdraw.deal(e);
            givein.deal(e);
            backmenu.deal(e);
            need_redraw=1;
        }

        SDL_RenderClear(rnd);
        TextureDraw(rnd,_background,0,0);
        TextureDraw(rnd,_board,240,45);
        for(int i=0;i<BOARD_SIZE_Y;i++)
        {
            for(int j=0;j<BOARD_SIZE_X;j++)
            {
                int id=Game::GetSpot(i,j);
                switch(id)
                {
                case 1:
                    TextureDraw(rnd,_spot_white,290+j*425/14-16,80+i*425/14-13);
                    break;
                case 2:
                    TextureDraw(rnd,_spot_black,290+j*425/14-16,80+i*425/14-13);
                    break;
                }
            }
        }
        if(_temp_Really)
        {
            SDL_Rect rect;
            rect.x=50;
            rect.y=85;
            rect.w=150;
            rect.h=150;
            SDL_RenderCopyEx(rnd,_box_white,NULL,&rect,0,NULL,SDL_FLIP_NONE);
            rect.y=400;
            SDL_RenderCopyEx(rnd,_box_blank,NULL,&rect,0,NULL,SDL_FLIP_NONE);
        }
        else
        {
            SDL_Rect rect;
            rect.x=50;
            rect.y=85;
            rect.w=150;
            rect.h=150;
            SDL_RenderCopyEx(rnd,_box_blank,NULL,&rect,0,NULL,SDL_FLIP_NONE);
            rect.y=400;
            SDL_RenderCopyEx(rnd,_box_black,NULL,&rect,0,NULL,SDL_FLIP_NONE);
        }
        withdraw.draw(rnd);
        givein.draw(rnd);
        backmenu.draw(rnd);
        /// Draw the cursor on the board.
        if(work) work();
        SDL_RenderPresent(rnd);
        need_redraw=0;
    }
}

int LoopChooseType()
{
    int running=1;
    SDL_Event e;
    int need_update=1;

    int _blank_A;
    int _blank_x1,_blank_x2,_blank_x3;
    SDL_QueryTexture(_text_multi_back,NULL,NULL,&_blank_x1,&_blank_A);
    SDL_QueryTexture(_text_multi_beclient,NULL,NULL,&_blank_x2,&_blank_A);
    SDL_QueryTexture(_text_multi_beserver,NULL,NULL,&_blank_x3,&_blank_A);

    xbutton creategame;
    xbutton joingame;
    xbutton cancel;

    int selected=0;

    creategame.text_clicked=BNAME(creategame,clicked);
    creategame.text_mouseover=BNAME(creategame,mouseover);
    creategame.text_normal=BNAME(creategame,normal);
    creategame.centered=0;
    creategame.w=180;
    creategame.h=100;
    creategame.x=410;
    creategame.y=125;
    creategame._onmouseover=_play_mouseover(creategame);
    creategame._onclick=_play_clicked(creategame);
    creategame._onrelease=[&]()
    {
        selected=2;
    };

    joingame.text_clicked=BNAME(joingame,clicked);
    joingame.text_mouseover=BNAME(joingame,mouseover);
    joingame.text_normal=BNAME(joingame,normal);
    joingame.centered=0;
    joingame.w=180;
    joingame.h=100;
    joingame.x=410;
    joingame.y=250;
    joingame._onmouseover=_play_mouseover(joingame);
    joingame._onclick=_play_clicked(joingame);
    joingame._onrelease=[&]()
    {
        selected=3;
    };

    cancel.text_clicked=BNAME(multicancel,clicked);
    cancel.text_mouseover=BNAME(multicancel,mouseover);
    cancel.text_normal=BNAME(multicancel,normal);
    cancel.centered=0;
    cancel.w=180;
    cancel.h=100;
    cancel.x=410;
    cancel.y=375;
    cancel._onmouseover=_play_mouseover(cancel);
    cancel._onclick=_play_clicked(cancel);
    cancel._onrelease=[&]()
    {
        selected=1;
    };

    SDL_Texture* newtext=SDL_CreateTexture(rnd,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_TARGET,500,500);
    printf("%s\n",SDL_GetError());
    /// Set Render Target to Texture.
    SDL_SetRenderTarget(rnd,newtext);
    SDL_SetRenderDrawBlendMode(rnd,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(newtext,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rnd,128,128,128,128);
    SDL_RenderFillRect(rnd,NULL);
    /// Reset Render Target to Window
    SDL_SetRenderTarget(rnd,NULL);
    SDL_SetRenderDrawColor(rnd,0,0,0,0);

    while(running)
    {
        while(!need_update&&SDL_WaitEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                /// request to exit...
                Global::ErrorQuit("SDL_QUIT Received.");
                break;
            }
            if(creategame.deal(e))
            if(joingame.deal(e))
            if(cancel.deal(e));

            if(selected)
            {
                running=0;
            }

            need_update=1;
        }

        SDL_RenderClear(rnd);
        TextureDraw(rnd,_background,0,0);
        TextureDraw(rnd,newtext,250,50);
        creategame.draw(rnd);
        joingame.draw(rnd);
        cancel.draw(rnd);
        SDL_RenderPresent(rnd);
        need_update=0;
    }

    SDL_DestroyTexture(newtext);
    return selected-1;

    /// Never Reached
    Global::ErrorQuit("====This Should Never Be Reached.====");
    return 0;
}

void LoopMultiPlayerServer()
{

}

/// Thread Method. Called By LoopMultiPlayerClient.
int NetFindServer(void* runFlag)
{
    sock s;
    int ret=s.connect("123.206.86.73",58699);
    if(ret<0)
    {
        *(int*)runFlag=0;
        return 0;
    }
    char buffer[1024];
    memset(buffer,0,1024);
    unsigned long sz=1024;
    GetComputerName(buffer,&sz);
    printf("Computer Name: %s\n",buffer);
    string fullname=buffer;
    memset(buffer,0,1024);
    GetUserName(buffer,&sz);
    fullname=fullname+"-"+buffer;
    printf("User Name: %s\n",buffer);

    MapBox box;
    box.add("MYNAME",fullname);
    box.add("ALLOW","0");
    box.add("ROOMID","");
    box.add("ACTION","FIND");
    box.add("TIMEOUT","30");

    ret=s.send(box);
    if(ret<0)
    {
        *(int*)runFlag=0;
        return 1;
    }

    ret=s.recv(box);
    if(ret<0)
    {
        *(int*)runFlag=0;
        return 2;
    }

    /// TODO


    return 0;
}

void LoopMultiPlayerClient()
{
    int running=1;
    int need_update=1;
    SDL_Event e;

    printf("Starting Thread...\n");
    SDL_Thread* tid=SDL_CreateThread(NetFindServer,"FindServer",&running);

    while(running)
    {
        while(!need_update&&SDL_WaitEventTimeout(&e,100))
        {
            need_update=1;
        }
        SDL_RenderClear(rnd);
        SDL_RenderCopy(rnd,_background,NULL,NULL);
        SDL_Rect rect;
        SDL_QueryTexture(_text_searching_server,NULL,NULL,&rect.w,&rect.h);
        rect.x=WIN_WIDTH/2-rect.w/2;
        rect.y=WIN_HEIGHT/2-rect.h/2;
        SDL_RenderCopy(rnd,_text_searching_server,NULL,&rect);
        SDL_RenderPresent(rnd);
        need_update=0;
    }

    int status=0;
    SDL_WaitThread(tid,&status);
}

void LoopMultiPlayer()
{
    switch(LoopChooseType())
    {
    case 0:
        return;
    case 1:
        LoopMultiPlayerServer();
        return;
    case 2:
        LoopMultiPlayerClient();
        return;
    }
}

void LoopAbout()
{

}

void LoopExiting()
{

}
