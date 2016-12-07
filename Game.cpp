#include "Game.h"

namespace External
{

bool win(int xx,int yy,int color,int board_in[BOARD_SIZE_Y][BOARD_SIZE_X])
{
    int x=xx+4;
    int y=yy+4;
    printf("In External::win(%d,%d,%d...) \n",x+4,y+4,color);
    int a,b;
    int board[25][25];
    memset(board,0,sizeof(board));
    for(int i=0;i<15;i++)
        for(int j=0;j<15;j++)
            board[i+4][j+4]=board_in[i][j];

    for(a=x-4;a<=x+4;a++)
		if(board[y][a]==color&&board[y][a+1]==color&&board[y][a+2]==color&&board[y][a+3]==color&&board[y][a+4]==color)
			{return true;}
	for(b=y-4;b<=y+4;b++)
    {
        printf("Checking... %d %d %d\n",b,x,board[b][x]);
        if(board[b][x]==color&&board[b+1][x]==color&&board[b+2][x]==color&&board[b+3][x]==color&&board[b+4][x]==color)
			{return true;}
    }
	for(a=x-4,b=y-4;a<=x+4;a++,b++)
		if(board[b][a]==color&&board[b+1][a+1]==color&&board[b+2][a+2]==color&&board[b+3][a+3]==color&&board[b+4][a+4]==color)
			{return true;}
	for(a=x-4,b=y+4;a<=x+4;a++,b--)
		if(board[b][a]==color&&board[b+1][a-1]==color&&board[b+2][a-2]==color&&board[b+3][a-3]==color&&board[b+4][a-4]==color)
			{return true;}

    printf("No Winner.\n");
    return false;
}

}/// End of namespace External

namespace Game
{
    /// 0 Clean 1 White 2 Black
    int _m_board[BOARD_SIZE_Y][BOARD_SIZE_X];
    int _m_last_color;
    int _m_last_x;
    int _m_last_y;
    void InitSinglePlayer()
    {
        memset(_m_board,0,sizeof(_m_board));
    }
    void SetSpot(int ColorID,int BoardY,int BoardX)
    {
        _m_board[BoardY][BoardX]=ColorID;
    }
    int GetSpot(int BoardY,int BoardX)
    {
        return _m_board[BoardY][BoardX];
    }
    void SetCheckWinner(int x,int y,int color)
    {
        _m_last_x=x;
        _m_last_y=y;
        _m_last_color=color;
    }
    int CheckWinner()
    {
        //printf("Calling External Function: win(%d,%d,%d...) \n",_m_last_y,_m_last_x,_m_last_color);
        if(External::win(_m_last_x,_m_last_y,_m_last_color,_m_board))
        {
            //printf("Check Winner : Win Detected.\n");
            return _m_last_color;
        }
        else
        {
            //printf("Check Winner: No winner detected.\n");
            return 0;
        }
    }
}
