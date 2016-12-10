#pragma once
#include "config.h"

#define BOARD_SIZE_X 15
#define BOARD_SIZE_Y 15

namespace Game
{
    void InitSinglePlayer();
    void SetSpot(int ColorID,int BoardY,int BoardX);
    int GetSpot(int BoardY,int BoardX);
    int CancelLast();
    /// Check Winner: Returns 1 on white wins, 2 on black wins.
    int CheckWinner();
    void SetCheckWinner(int x,int y,int color);
}

