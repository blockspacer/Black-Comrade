
#include "main.h"
#include <iostream>
#include "stateUpdate.h"

#include "networkRole.h"
#include "collaborationInfo.h"
#include "networkingManager.h"

using namespace RakNet;

Main::Main() {

    networkingManager = new NetworkingManager(this);

    // networking
    startNetworking();

    root = new Root();

    if (!root->restoreConfig())
        root->showConfigDialog();
    
    window = root->initialise(true, collabInfo->getNick() + " - " + collabInfo->getGameRoleString());
    

    sceneMgr = root->createSceneManager(ST_GENERIC);

	ResourceGroupManager::getSingleton().addResourceLocation(
                    ".", "FileSystem", "General");

    // Set the paths to look for various resources
    ResourceGroupManager::getSingleton().addResourceLocation(
                    "models", "FileSystem", "General");

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "sounds", "FileSystem", "General");

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "materials/scripts", "FileSystem", "General");

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "materials/programs", "FileSystem", "General");

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "materials/textures", "FileSystem", "General");
                    
    ResourceGroupManager::getSingleton().addResourceLocation(
                    "particles", "FileSystem", "General"); 
                   
    // Magic Resource line
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    soundMgr = new SoundManager();
    
    mapMgr = new MapManager("examplemap.txt",sceneMgr);

    createCamera();
    createViewPort();
    createScene();
    
    inputState = new InputState(window, false, this);
    
    stateUpdate = new StateUpdate();
    
    stateUpdate->addTickable(inputState);
    
    collisionMgr = new CollisionManager( sceneMgr, mapMgr);

    GameRole myGameRole = collabInfo->getGameRole();
    if (myGameRole == PILOT) {
        pilotStartup();
    }
    else if (myGameRole == ENGINEER) {
        engineerStartup();
    }
    else if (myGameRole == NAVIGATOR) {
        navigatorStartup();
    }

    NetworkRole myNetworkRole = collabInfo->getNetworkRole();
    if (myNetworkRole == SERVER || myNetworkRole == DEVELOPMENTSERVER) {
        serverStartup();
    }
    else if (myNetworkRole == CLIENT) {
        clientStartup();
    }

    audioState = new AudioState(pilotGunState,soundMgr,shipSceneNode);
    miniGameMgr = new MiniGameManager(inputState,pilotControls,sceneMgr);

    gameParameterMap = new GameParameterMap(gameStateMachine);
	
    printState = new PrintState(gameStateMachine);

	//Test swarm
	//Vector3 swarmLocation(mapMgr->startx,0,mapMgr->starty+500);
	//Swarm *swarm = new Swarm(1,1,swarmLocation,sceneMgr,0,0,0);
    swarmMgr = new SwarmManager(sceneMgr,gameParameterMap,mapMgr,shipState, collisionMgr);
    bulletMgr = new BulletManager(shipSceneNode,sceneMgr,pilotGunState,engineerGunState,navigatorGunState,collisionMgr,swarmMgr);

    stateUpdate->addTickable(pilotGunState);
    if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) stateUpdate->addTickable(engineerGunState);
    if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) stateUpdate->addTickable(navigatorGunState);    
    stateUpdate->addTickable(audioState);
    stateUpdate->addTickable(shipState);
    stateUpdate->addTickable(networkingManager);
    stateUpdate->addTickable(bulletMgr);
    stateUpdate->addTickable(soundMgr);
    stateUpdate->addTickable(miniGameMgr);
    stateUpdate->addTickable(printState);
    stateUpdate->addTickable(swarmMgr);
    
    // This should be last to allow events for the inital state 'change'
    stateUpdate->addTickable(gameStateMachine);

    root->addFrameListener(stateUpdate);

	

    // Start Rendering Loop
    root->startRendering();
    networkingManager->stopNetworking();
    
    addCrossHair();

}

void Main::clientStartup() {
    gameStateMachine = (GameStateMachine *) networkingManager->getReplica("GameStateMachine", true);
}

void Main::serverStartup() {
    gameStateMachine = new GameStateMachine(mapMgr,shipState);
    networkingManager->replicate(gameStateMachine);
}

void Main::navigatorStartup() {
    camera->setPosition(0,2,-2);
    
    navControls = new NavigatorControls(inputState,camera);
    navigatorGunState = new GunState(navControls,NAVIGATOR);
    networkingManager->replicate(navigatorGunState);

    shipState = (ShipState*) networkingManager->getReplica("ShipState",true);
    pilotGunState = (GunState *) networkingManager->getReplica("PilotGunState",true);
    engineerGunState = (GunState *) networkingManager->getReplica("EngineerGunState",true);

    stateUpdate->addTickable(navControls);

    shipState->shipSceneNode = shipSceneNode;
    shipState->position = new Vector3(mapMgr->startx,0,mapMgr->starty);
}

void Main::engineerStartup() {
    camera->setPosition(0,-2,-2);

    engControls = new EngineerControls(inputState,camera);
    engineerGunState = new GunState(engControls,ENGINEER);
    networkingManager->replicate(engineerGunState);


    shipState = (ShipState*) networkingManager->getReplica("ShipState",true);
    pilotGunState = (GunState *) networkingManager->getReplica("PilotGunState",true);
    navigatorGunState = (GunState *) networkingManager->getReplica("NavigatorGunState",true);

    stateUpdate->addTickable(engControls);

    shipState->shipSceneNode = shipSceneNode;
    shipState->position = new Vector3(mapMgr->startx,0,mapMgr->starty);
}

void Main::pilotStartup() {
    camera->setPosition(Vector3(0,0,0));
    pilotControls = new PilotControls(inputState);
    as = new AccelerationState(pilotControls);
    ms = new MotionState(as);
    pilotGunState = new GunState(pilotControls,PILOT);
    shipState = new ShipState(shipSceneNode, ms, collisionMgr);
    //enemyState = new EnemyState(enemySceneNode, sceneMgr);

    networkingManager->replicate(shipState);
    networkingManager->replicate(pilotGunState);
    //networkingManager->replicate(enemyState);

    if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) engineerGunState = (GunState *) networkingManager->getReplica("EngineerGunState",true);
    if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) navigatorGunState = (GunState *) networkingManager->getReplica("NavigatorGunState",true);

    stateUpdate->addTickable(pilotControls);
    stateUpdate->addTickable(as);
    stateUpdate->addTickable(ms);

    shipState->position = new Vector3(mapMgr->startx,0,mapMgr->starty);

}



void Main::startNetworking() {
    char ch;
    printf("Start as (c)lient, (s)erver or (d)evelopment server?\n");

    while(true) {
        ch=getch();
        if (ch=='c' || ch=='C')
        {
            collabInfo = networkingManager->startNetworking(CLIENT);
            break;
        }
        else if (ch=='d' || ch=='D')
        {
            collabInfo = networkingManager->startNetworking(DEVELOPMENTSERVER);
            printf("DEVELOPMENT SERVER\n");
            break;
        }
        else if (ch=='s' || ch=='S')
        {
            collabInfo = networkingManager->startNetworking(SERVER);
            break;
        }
    }
}

void Main::createCamera() {

    shipSceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode();

    camera = sceneMgr->createCamera("mainCam");

    shipSceneNode->attachObject(camera);

    camera->setPosition(Vector3(0,0,0));
    camera->lookAt(Vector3(0,0,1));
    //camera->setFOVy(Radian(2.0943951));
    camera->setNearClipDistance(1);
    camera->setFarClipDistance(1500);
}

void Main::createViewPort() {

    Viewport *vp = window->addViewport(camera);
    vp->setBackgroundColour(ColourValue(1,0,0));
    
    camera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()*1.17));
    //camera->setAspectRatio(1.17);
}

void Main::createScene() {

    sceneMgr->setShadowColour(ColourValue(0.5,0.5,0.5));

    sceneMgr->setAmbientLight(ColourValue(1,1,1));
    //sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    
    // Add some sexy fog
    //ColourValue fadeColour(0.1,0.1,0.1);
    //sceneMgr->setFog(FOG_LINEAR, fadeColour, 0.0, 0, 300);
    
    // Creating the light that is attached to the ship
    Light *sp = sceneMgr->createLight("ShipLight");
    sp->setType(Light::LT_POINT);
    sp->setDiffuseColour(1.0,1.0,1.0);
    sp->setSpecularColour(0.2,0.2,0.7);
    sp->setDirection(Vector3(0,0,1));
    //sp->setAttenuation(10000, 0.7, 0.000025, 0.0000045);

    shipSceneNode->attachObject(sp);
    
    //Entity *en = sceneMgr->createEntity("enemy","smallenemy.mesh");
    
    //enemySceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    //enemySceneNode->showBoundingBox(true);
    //enemySceneNode->attachObject(en);

    mapNode = sceneMgr->getRootSceneNode()->createChildSceneNode();

    mapMgr->outputMap(mapNode);
    
    SceneNode *modelNode = shipSceneNode->createChildSceneNode();
    
    Entity *e = sceneMgr->createEntity("ourship", "ourship.mesh");
    modelNode->attachObject(e);
    modelNode->setPosition(0,-7,-5);
}

int main()
{
    Main *main = new Main();

    delete main;
}

void Main::clientShutdown() {

}

void Main::serverShutdown() {

}

void Main::pilotShutdown() {
        cout << "deleting shipstate" << endl;
        delete shipState;
        cout << "deleting pilotGunState" << endl;
        delete pilotGunState;
}

void Main::navigatorShutdown() {

}

void Main::engineerShutdown() {

}

Main::~Main()
{
    cout << "deleting inputState" << endl;
    delete inputState;
    cout << "deleting pilotControls" << endl;
    delete pilotControls;
    cout << "deleting as" << endl;
    delete as;
    cout << "deleting ms" << endl;
    delete ms;
    cout << "deleteing bulletMgr" << endl;
    delete bulletMgr;
    if (collabInfo->getGameRole() == PILOT) {
        pilotShutdown();
    }
    else if (collabInfo->getGameRole() == NAVIGATOR) {
        navigatorShutdown();
    }
    else if (collabInfo->getGameRole() == ENGINEER) {
        engineerShutdown();
    }
    cout << "deleting networkingManger" << endl;
    delete networkingManager;

    cout << "deleting stateUpdate" << endl;
    delete stateUpdate;

    delete soundMgr;

    OGRE_DELETE root;

    //cout << "PLEASE STOP HERE!" << endl;
}

void Main::addCrossHair()
{
    /* lines = new Lines(sceneMgr);
    Vector3 *a = new Vector3(mapMgr->startx,0,mapMgr->starty);
    cout << mapMgr->startx << " " << << endl<< endl;
    lines->addCross( a );
    lines->addCross( a ); */
    
    // ManualObject* manual = sceneMgr->createManualObject("manual");
    // manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
// 
    // 
    // manual->position(mapMgr->startx-100, 0, mapMgr->starty);
    // manual->position(mapMgr->startx+100, 0, mapMgr->starty);
    // manual->position(mapMgr->startx, -100, mapMgr->starty);
    // manual->position(mapMgr->startx, +100, mapMgr->starty);
// 
// 
    // manual->end();
    // sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

}
    

void Main::exit()
{
    stateUpdate->running = false;
}

