
#ifndef MAIN_H
#define MAIN_H

// Includes all Ogre classes
#include <OGRE/Ogre.h>

#include "networkingManager.h"
#include "networkRole.h"
#include "stateUpdate.h"
#include "shipState.h"
#include "inputState.h"
#include "gunState.h"
#include "Kbhit.h"
#include "pilotControls.h"
#include "accelerationState.h"
#include "motionState.h"
#include "soundManager.h"
#include "mapManager.h"
#include "audioState.h"
#include "miniGameManager.h"
#include "IExit.h"
#include "bulletManager.h"
#include "collisionManager.h"
#include "gameStateMachine.h"
#include "gameParameterMap.h"
#include "printState.h"
#include "swarmManager.h"
#include "flying.h"
#include "navigatorControls.h"
#include "engineerControls.h"
#include "sceneNodeManager.h"

//include networking stuff
#include <string.h>
#include <string>
#include <cstdio>

using namespace Ogre;
using namespace std;
using namespace RakNet;

class Main : public IExit {
private:
    Root *root;
    Camera *camera;
    SceneManager *sceneMgr;
    SceneNodeManager *sceneNodeMgr;
    RenderWindow *window;
    NetworkingManager *networkingManager;
    bool isServer;
    CollaborationInfo *collabInfo;
    SoundManager *soundMgr;
    AccelerationState *as;
    MotionState *ms;
    Flying *flying;
    InputState *inputState;
    AccelerationState *accelerationState;
    MotionState *motionState;
    AudioState *audioState;
    MiniGameManager *miniGameMgr;
    BulletManager *bulletMgr;
    CollisionManager *collisionMgr;
    GameStateMachine *gameStateMachine;
    GameParameterMap *gameParameterMap;
    PrintState *printState;
    
    PilotControls *pilotControls;
    NavigatorControls *navigatorControls;
    EngineerControls *engineerControls;
   
    GunState *pilotGunState;
    GunState *engineerGunState;
    GunState *navigatorGunState;
    
    SceneNode *mapNode;
    
    StateUpdate *gameLoop;
    SceneNode *shipSceneNode;
    ShipState *shipState;
    
    MapManager *mapMgr;
    SwarmManager *swarmMgr;
    
    Camera *createCamera(SceneNode *shipSceneNode);
    void createScene();
    void createViewPort();
    CollaborationInfo *runLoby(NetworkingManager *networkingManager);
    
    Root *configRoot();
    void configResources();
    
public:
    Main();
    ~Main();
    
    void exit();
};

#endif
