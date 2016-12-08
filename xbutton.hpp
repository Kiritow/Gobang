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
