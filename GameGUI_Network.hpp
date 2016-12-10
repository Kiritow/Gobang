/** Previous Context: GameGUI.cpp */

class serversockNX : public serversock
{
public:
    int getfd()
    {
        return sfd;
    }
};

char _netbuild_buff[1024];


void LoopMultiPlayerServerInGame(sock* ps)
{

}


/// Thread Method. Called by LoopMultiPlayerServer
int NetBuildServer(void* runFlags)
{
    struct pack_type
    {
        sock* ps;
    };
    pack_type* ppack=(pack_type*)runFlags;


    return 1;
//    udpsock s;
//    int ret=s.useport(58343);
//    printf("UDP Client Listening at 58343... ret=%d\n",ret);
//
//    printf("About to set recv time out...\n");
//    {
//        struct timeval outtime;
//        /// We don't know why, but on Windows, 1 Second means 1000.
//        #if (defined _WINSOCK2_H || defined _WINSOCK_H)
//        outtime.tv_sec=5*1000;
//        outtime.tv_usec=0;
//        #else
//        outtime.tv_sec=5;
//        outtime.tv_usec=0;
//        #endif
//
//        ret=setsockopt(s.getfd(),SOL_SOCKET,SO_RCVTIMEO,(const char*)&outtime,sizeof(outtime));
//    }
//    printf("Recv Time out Set. (ret=%d)\n",ret);
//
//    while(((int*)procRunFlag)[0])
//    {
//        printf("Listening...\n");
//        memset(_netbuild_buff,0,1024);
//        ret=s.udprecv(_netbuild_buff,1024);
//        printf("Recv returns %d\n",ret);
//    }
//
//    ((int*)procRunFlag)[1]=0;
//    return 1;
//
//    struct pack_type
//    {
//        serversockNX* ps;
//        int flags[2];
//        SDL_mutex* plock;
//    };
//    pack_type* ppack=(pack_type*)runFlags;
//
//    printf("About to start Server via TCP...\n");
//    int ret=ppack->ps->bind(58343);
//    printf("Bind return %d\n",ret);
//    ret=ppack->ps->listen(1);
//    printf("Listen return %d\n",ret);
//
//    printf("Set recv time out...\n");
//
//    {
//        struct timeval outtime;
//        /// We don't know why, but on Windows, 1 Second means 1000.
//        #if (defined _WINSOCK2_H || defined _WINSOCK_H)
//        outtime.tv_sec=5*1000;
//        outtime.tv_usec=0;
//        #else
//        outtime.tv_sec=5;
//        outtime.tv_usec=0;
//        #endif
//
//        ret=setsockopt(ppack->ps->getfd(),SOL_SOCKET,SO_RCVTIMEO,(const char*)&outtime,sizeof(outtime));
//    }
//
//    printf("Recv Time out Set. (ret=%d)\n",ret);
//
//    SDL_LockMutex(ppack->plock);
//    ppack->flags[1]=1;/// 0 --> 1, ready to accept;
//    SDL_UnlockMutex(ppack->plock);
//
//    sock s=ppack->ps->accept();
//
//    int isok=0;
//    SDL_LockMutex(ppack->plock);
//    if(ppack->flags[1]==1)
//    {
//        isok=1;
//        ppack->flags[1]=2;
//    }
//    SDL_UnlockMutex(ppack->plock);
//
//    ppack->flags[0]=0;
//    return isok;
}

void LoopMultiPlayerServer()
{
    struct pack_type
    {
        sock* ps;
        int _run_flag;
        int _control_flag;
    };
    pack_type pack;
    pack.ps=NULL;
    pack._run_flag=1;
    pack._control_flag=0;
    SDL_Thread* td=SDL_CreateThread(NetBuildServer,"Server (TCP&UDP)",&pack);
    int status;
    SDL_WaitThread(td,&status);
}


/// Thread Method. Called By LoopMultiPlayerClient.
char _netfind_buff[1024];
struct _netfind_pack
{
    string servername;
    int serverport;
    string serverip;

    int _flag_running;
};
int NetFindServer(void* runFlag)
{
    _netfind_pack* ppack=(_netfind_pack*)runFlag;

    udpsock usock;
    int ret=usock.setbrocast();
    if(ret<0)
    {
        printf("Cannot Set UDP Socket to Broadcast Mode.\n");
        ppack->_flag_running=0;
        return 1;
    }
    printf("About to set UDP Socket recv timeout limits..\n");

        struct timeval outtime;
        /// We don't know why, but on Windows, 1 Second means 1000.
        #if (defined _WINSOCK2_H || defined _WINSOCK_H)
        outtime.tv_sec=5*1000;
        outtime.tv_usec=0;
        #else
        outtime.tv_sec=5;
        outtime.tv_usec=0;
        #endif

        ret=setsockopt(usock.getfd(),SOL_SOCKET,SO_RCVTIMEO,(const char*)&outtime,sizeof(outtime));

    printf("Time Limit Set to 5 Seconds. (ret=%d)\n",ret);

    usock.settarget("255.255.255.255",58342);
    printf("UDP Broadcast Target Set to 255.255.255.255 (58342) .\n");

    MapBox box;
    box.add("DETECT-SERVER","1.0");
    string s=box.toString();
    ret=usock.udpsend(s.c_str(),s.size());
    printf("Broadcast Sent. udpsend returns %d\n",ret);
    memset(_netfind_buff,0,1024);
    ret=usock.udprecv(_netfind_buff,1024);
    printf("Broadcast Recv. udprecv returns %d\n",ret);
    if(ret<=0)
    {
        ppack->_flag_running=0;
        return 2;
    }

    box.clear();

    /// Parse It.
    {
        string ans(_netfind_buff);
        int sz=ans.size();
        char* tp=(char*)ans.c_str();
        char* p=tp;
        while(p<tp+sz)
        {
            char* q=strstr(p,":");
            char* t=strstr(p,";");
            if(!q||!t) break;
            box.add(base64_decode(ans.substr(p-tp,q-p)),base64_decode(ans.substr(q-tp+1,t-q-1)));
            p=t+1;
        }
    }

    if(box.haskey("SNAME")&&box.haskey("SPORT"))
    {
        ppack->servername=box["SNAME"];
        sscanf(box.getvalp("SPORT")->c_str(),"%d",&(ppack->serverport));
        sockaddr_in tmp=usock.getlastrecvfrom();
        ppack->serverip=inet_ntoa(tmp.sin_addr);
    }
    else
    {
        ppack->_flag_running=0;
        return 3;
    }

    printf("NetFindServer Finish\n");
    ppack->_flag_running=0;
    return 0;
}


void LoopMultiPlayerClientInGame(sock* ps)
{

}

void LoopMultiPlayerClient()
{
    int need_update=1;
    SDL_Event e;

    printf("Starting Thread...\n");
    _netfind_pack pack;
    pack._flag_running=1;
    SDL_Thread* tid=SDL_CreateThread(NetFindServer,"FindServer",&pack);

    while(pack._flag_running)
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
    printf("Thread returns %d\n",status);

    if(status!=0)
    {
        printf("Failed on Finding MultiPlayer Game.\n");
        return;
    }

    printf("About to connect to %s:%d\n",pack.serverip.c_str(),pack.serverport);
    sock s;
    int ret=s.connect(pack.serverip.c_str(),pack.serverport);
    if(ret<0)
    {
        printf("Failed to connect to server with TCP.\n");
        return ;
    }

    LoopMultiPlayerClientInGame(&s);

    /// Connection Build.
}


/** Next Context: GameGUI.cpp */
