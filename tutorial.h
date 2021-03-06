#ifndef TUTORIAL
#define TUTORIAL

#include "ITickable.h"
#include "guiManager.h"
#include "hud.h"
#include "miniGameManager.h"
#include "collaborationInfo.h"
#include "door.h"
#include "systemManager.h"
#include "shipState.h"
#include "tutorial.h"

#include <iostream>

enum TutorialState { TS_PRE, TS_START,
                     TS_SHOW_CONTROLS,
                     TS_MISSION_LOG,
                     TS_AVATARS,
                     TS_HEALTH_BARS,
                     TS_OPEN_DOORS,
                     TS_CHARGE_BARS,
                     TS_POWER_BARS,
                     TS_MINI_RADAR,
                     TS_MINI_MAP,
                     TS_PILOT_ROLE,
                     TS_ENGINEER_ROLE,
                     TS_NAVIGATOR_ROLE,
                     TS_INDIVIDUAL,
                     TS_FIRE_WEAPON,
                     TS_MOVE_SHIP,
                     TS_CHANGE_POWERS,
                     TS_SHOW_MAP, TS_CLOSE_MAP,
                     TS_SHOW_RADAR, TS_CLOSE_RADAR,
                     TS_OPEN_CONSOLE, TS_END_MINIGAME, TS_CLOSE_CONSOLE,
                     TS_REPAIR_SYSTEMS, TS_REPAIR_ENGINES, TS_REPAIR_WEAPONS, TS_REPAIR_SENSORS,
                     TS_PILOT_END, TS_ENGINEER_END, TS_NAVIGATOR_END,
                     TS_WAITING_FOR_OTHERS, TS_END };

class RadarGui;
class Flying;

class Tutorial : public ITickable {
    private:

        GuiManager *guiMgr;
        HUD *hud;
        TutorialState state;
        MiniGameManager *miniGameMgr;
        DamageState *damageState;
        SystemManager *systemMgr;
        ShipState *shipState;
        RadarGui *largeRadar;
        InputState *inputState;
        Flying *flying;

        CollaborationInfo *tutee;

        CollaborationInfo *tutee1;
        CollaborationInfo *tutee2;
        CollaborationInfo *tutee3;

        Door *door;

        int pauseProgress;

        void tickCommonTutorial();
        void tickPilotTutorial();
        void tickEngineerTutorial();
        void tickNavigatorTutorial();

        void checkForCompletion();

        void changeWithPause(TutorialState newState, int pause = 2);

    public:
        TutorialState getState();
        Tutorial(CollaborationInfo *tutee,
                 CollaborationInfo *tutee1, CollaborationInfo *tutee2, CollaborationInfo *tutee3,
                 GuiManager *guiMgr, MiniGameManager *miniGameMgr, DamageState *damageState,
                 SystemManager *systemMgr, ShipState *shipState, Door *door, InputState *inputState);
	virtual void tick();
    void end();

    void setRadar(RadarGui *radar);
    void setHUD(HUD *hud);
    void setFlying(Flying *flying);
};

#endif
