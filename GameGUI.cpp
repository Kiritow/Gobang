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


Mix_Music* _bgm=NULL;
Mix_Chunk* _down_1=NULL;
Mix_Chunk* _down_2=NULL;
Mix_Chunk* _down_3=NULL;
SDL_Texture* _button_multi_beserver=NULL;
SDL_Texture* _button_multi_beclient=NULL;
SDL_Texture* _button_multi_back=NULL;
SDL_Texture* _button_menu_singleplayer=NULL;
SDL_Texture* _button_menu_multiplayer=NULL;
SDL_Texture* _button_menu_about=NULL;
SDL_Texture* _button_menu_exit=NULL;

SDL_Texture* _text_singleplayer_lost=NULL;
SDL_Texture* _text_singleplayer_win=NULL;
SDL_Texture* _text_searching_server=NULL;

/// clicked mouseover normal
#define BNAME(ButtonName,Status) _button_##ButtonName##_##Status
#define BNAMEX(ButtonName) SDL_Texture* BNAME(ButtonName,clicked)=NULL;SDL_Texture* BNAME(ButtonName,mouseover)=NULL;SDL_Texture* BNAME(ButtonName,normal)=NULL
#define LoadButton(ButtonName,Status) BNAME(ButtonName,Status)=MyLoadImage(rnd,"img\\button_" #ButtonName "_" #Status ".png",NULL,NULL)
#define LoadButtonX(ButtonName) LoadButton(ButtonName,clicked);LoadButton(ButtonName,mouseover);LoadButton(ButtonName,normal)
BNAMEX(restart);
BNAMEX(cancel);
BNAMEX(givein);

const int _button_w=115;
const int _button_h=55;

class xbutton
{
public:
    SDL_Texture* text_normal;
    SDL_Texture* text_mouseover;
    SDL_Texture* text_clicked;
    int x,y,w,h;
    int centered;
    int status;
    function<void()> _onrelease;
    function<void()> _onclick;
    xbutton()
    {
        status=0;
        w=_button_w;
        h=_button_h;
    }
    bool inRange(int inc_x,int inc_y)
    {
        if(!centered) return inc_x>=x&&inc_x<=x+w&&inc_y>=y&&inc_y<=y+h;
        else
        {
            int nx=x-w/2;
            int ny=y-h/2;
            return inc_x>=nx&&inc_x<=nx+w&&inc_y>=ny&&inc_y<=ny+h;
        }
    }
    int deal(SDL_Event e)
    {
        switch(e.type)
        {
        case SDL_MOUSEMOTION:
            if(inRange(e.motion.x,e.motion.y))
            {
                onMouseOver();
                return 0;
            }
            else
            {
                onOutOfRange();
                return 1;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(inRange(e.button.x,e.button.y)&&status==1)
            {
                onClick();
                return 0;
            }
            else
            {
                onOutOfRange();
                return 1;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(inRange(e.button.x,e.button.y)&&status==2)
            {
                onReleased();
                return 0;
            }
            else
            {
                onOutOfRange();
                return 1;
            }
            break;
        case SDL_QUIT:
            Global::ErrorQuit("SDL_QUIT Received.");
            return 0;
            break;
        }
        return 1;/// Unsolved.
    }
    void onReleased()
    {
        status=0;
        if(_onrelease) _onrelease();
    }
    void onClick()
    {
        status=2;
        if(_onclick) _onclick();
    }
    void onMouseOver()
    {
        printf("[MouseOver] %p\n",this);
        status=1;
    }
    void onOutOfRange()
    {
        printf("[OutRange] %p\n",this);
        status=0;
    }
    void draw(SDL_Renderer* rnd)
    {
        switch(status)
        {
        case 0:
            if(centered) TextureDraw(rnd,text_normal,x-w/2,y-h/2);
            else TextureDraw(rnd,text_normal,x,y);
            break;
        case 1:
            if(centered) TextureDraw(rnd,text_mouseover,x-w/2,y-h/2);
            else TextureDraw(rnd,text_mouseover,x,y);
            break;
        case 2:
            if(centered) TextureDraw(rnd,text_clicked,x-w/2,y-h/2);
            else TextureDraw(rnd,text_clicked,x,y);
            break;
        }
    }
};

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
        SDL_Delay(1500/255);
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
        SDL_Delay(1500/255);
        printf("Overlay %d\n",i);
    }
}

void LoopLoading()
{
    int w,h;
    SDL_Texture* tx_a=RenderText(rnd,systtf.font(),"Loading...",color_white,&w,&h);
    SDL_RenderClear(rnd);
    TextureDraw(rnd,tx_a,WIN_WIDTH/2-w/2,WIN_HEIGHT/2-h/2);
    SDL_RenderPresent(rnd);

    _background=MyLoadImage(rnd,"img\\background.png",NULL,NULL);
    _board=MyLoadImage(rnd,"img\\board.png",NULL,NULL);
    _spot_white=MyLoadImage(rnd,"img\\white_spot.png",NULL,NULL);
    _spot_black=MyLoadImage(rnd,"img\\black_spot.png",NULL,NULL);
    _focus=MyLoadImage(rnd,"img\\focus.png",NULL,NULL);
    _home_gobang=MyLoadImage(rnd,"img\\home.png",NULL,NULL);
    _menu_background=MyLoadImage(rnd,"img\\menu_bg.png",NULL,NULL);
    _menu_text=MyLoadImage(rnd,"img\\menu_text.png",NULL,NULL);
    _menu_light=MyLoadImage(rnd,"img\\menu_highlight.png",NULL,NULL);

    LoadButtonX(restart);
    LoadButtonX(cancel);
    LoadButtonX(givein);

    /// Memory Leak?
    SDL_Surface* _cursor_surf=IMG_Load("img\\mouse.png");
    SDL_Cursor* _cursor=SDL_CreateColorCursor(_cursor_surf,0,0);
    SDL_SetCursor(_cursor);

    _bgm=Mix_LoadMUS("mp3\\bgm.mp3");
    _down_1=Mix_LoadWAV("mp3\\down_1.wav");
    _down_2=Mix_LoadWAV("mp3\\down_2.wav");
    _down_3=Mix_LoadWAV("mp3\\down_3.wav");

    _button_multi_beserver=RenderUTF8(rnd,systtf.font(),"建立房间",color_white,NULL,NULL);
    _button_multi_beclient=RenderUTF8(rnd,systtf.font(),"搜索房间",color_white,NULL,NULL);
    _button_multi_back=RenderUTF8(rnd,systtf.font(),"返回",color_white,NULL,NULL);

    _button_menu_singleplayer=RenderUTF8(rnd,systtf.font(),"单人游戏",color_white,NULL,NULL);
    _button_menu_multiplayer=RenderUTF8(rnd,systtf.font(),"多人游戏",color_white,NULL,NULL);
    _button_menu_about=RenderUTF8(rnd,systtf.font(),"关于",color_white,NULL,NULL);
    _button_menu_exit=RenderUTF8(rnd,systtf.font(),"退出游戏",color_white,NULL,NULL);

    _text_singleplayer_lost=RenderUTF8(rnd,systtf.font(),"你输了~",color_white,NULL,NULL);
    _text_singleplayer_win=RenderUTF8(rnd,systtf.font(),"你赢了！",color_white,NULL,NULL);
    _text_searching_server=RenderUTF8(rnd,systtf.font(),"正在搜索对局...",color_white,NULL,NULL);

    /// Loaded
    SDL_SetWindowTitle(wnd,"GoBang 2016");
}

int LoopMenu()
{
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

int _temp_Really=0;

void LoopSinglePlayer()
{
    int running=1;
    SDL_Event e;
    Game::InitSinglePlayer();

    /// Start Music
    Mix_PlayMusic(_bgm,-1);

    int need_redraw=1;
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
                                }
                            }
                        }
                        need_redraw=1;
                    }
                    else
                    {
                        /// The Mouse Is Out Of Range. Just Update The Screen.

                    }
                }
                break;
            }
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
    SDL_QueryTexture(_button_multi_back,NULL,NULL,&_blank_x1,&_blank_A);
    SDL_QueryTexture(_button_multi_beclient,NULL,NULL,&_blank_x2,&_blank_A);
    SDL_QueryTexture(_button_multi_beserver,NULL,NULL,&_blank_x3,&_blank_A);

    int B=(WIN_HEIGHT-3*_blank_A)/8;
    int C=3*B;

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
            case SDL_MOUSEMOTION:
                {
                    break;
                }
            case SDL_MOUSEBUTTONDOWN:
                {
                    /// Check Which Button It Hits.
                    if(isInRect(e.button.x,e.button.y,WIN_WIDTH/2-_blank_x3/2,C,WIN_WIDTH/2+_blank_x3/2,C+_blank_A))
                    {
                        /// "Be Server" Clicked.
                        printf("[Be Server] Clicked.\n");
                        return 1;
                    }
                    else if(isInRect(e.button.x,e.button.y,WIN_WIDTH/2-_blank_x2/2,C+_blank_A+B,WIN_WIDTH/2+_blank_x2/2,C+_blank_A+B+_blank_A))
                    {
                        /// "Be Client" Clicked.
                        printf("[Be Client] Clicked.\n");
                        return 2;
                    }
                    else if(isInRect(e.button.x,e.button.y,WIN_WIDTH/2-_blank_x1/2,C+_blank_A*2+B*2,WIN_WIDTH/2+_blank_x1/2,C+_blank_A*2+B*2+_blank_A))
                    {
                        printf("[Back] Clicked.\n");
                        return 0;
                    }
                    /// else... just do nothing.
                }
                break;
            }
        }

        SDL_RenderClear(rnd);
        TextureDraw(rnd,_background,0,0);
        TextureDraw(rnd,_button_multi_beserver,WIN_WIDTH/2-_blank_x3/2,C);
        TextureDraw(rnd,_button_multi_beclient,WIN_WIDTH/2-_blank_x2/2,C+_blank_A+B);
        TextureDraw(rnd,_button_multi_back,WIN_WIDTH/2-_blank_x1/2,C+_blank_A*2+B*2);
        SDL_RenderPresent(rnd);
        need_update=0;
    }

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
