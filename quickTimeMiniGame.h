#ifndef QUICKTIME_MINIGAME_H
#define QUICKTIME_MINIGAME_H

#include "IMiniGame.h"
#include "console.h"
#include "inputState.h"
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
#include <cstdlib>
#include <time.h>
#include "shipSystem.h"

class QuickTimeMiniGame : public IMiniGame
{
    private:
        InputState *inputState;
        Console *console;
        
        bool win;
        int broke;

        bool begin;
        bool isEnd;
        int score;
        int ticks;
        int saveTick;
        int gameTick;

        ShipSystem system;
        
    public:
        void tick();
        bool end();
        int getScore();

        ShipSystem getSystem();

        void returnKeyPressed();

        QuickTimeMiniGame(Console *console, InputState *inputState, ShipSystem system);
        ~QuickTimeMiniGame();
};

#endif
