
#include "main.h"
#include <iostream>
#include "stateUpdate.h"

#include "networkRole.h"
#include "collaborationInfo.h"
#include "networkingManager.h"

using namespace RakNet;

Main::Main() {

    // networking
    networkingManager = new NetworkingManager(this);
    collabInfo = runLoby(networkingManager);

    // Start Ogre
    root = configRoot();
    sceneMgr = root->createSceneManager(ST_GENERIC);
    window = root->initialise(true, collabInfo->getGameRoleString());
    //sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

    configResources();

    // Game Loop
    gameLoop = new StateUpdate();

    // SceneNode Manager
    sceneNodeMgr = new SceneNodeManager(sceneMgr);
    gameLoop->addTickable(sceneNodeMgr,"sceneNodeMgr");

    // Create Map
    mapMgr = new MapManager("examplemap.txt",sceneMgr);
    mapMgr->outputMap(sceneMgr->getRootSceneNode());

    // Ship State
    if(collabInfo->getGameRole() == PILOT) {
	    shipState = new ShipState(shipSceneNode);
	    networkingManager->replicate(shipState);
    } else {
    	shipState = 
    		(ShipState*) networkingManager->getReplica("ShipState",true);
    }
    shipState->setX(mapMgr->startx);
    shipState->setY(0);
    shipState->setZ(mapMgr->starty);
    gameLoop->addTickable(shipState, "shipState");

    // Ship Node
    shipSceneNode = sceneNodeMgr->createNode(shipState);
	Entity *shipEntity = sceneNodeMgr->getEntity(shipState);
    if (collabInfo->getGameRole() == PILOT) {
        shipEntity->setVisible(false);
    }
    
    // Camera
    camera = createCamera(shipSceneNode);
    if(collabInfo->getGameRole() == PILOT) {
        camera->setPosition(Vector3(0,0,-5));
    } else if(collabInfo->getGameRole() == NAVIGATOR) {
        camera->setPosition(Vector3(3.5,0,0));
    } else if(collabInfo->getGameRole() == ENGINEER) {
        camera->setPosition(Vector3(-3.5,0,0));
    }
    createViewPort();

    // Collision Manager
	collisionMgr = new CollisionManager(sceneMgr,mapMgr);

    // User Input
    inputState = new InputState(window, false, this,true,true);
    gameLoop->addTickable(inputState,"inputState");

    // Pilot Controls
    if(collabInfo->getGameRole() == PILOT) {
        collisionMgr->addMesh(shipEntity);
        pilotControls = new PilotControls(inputState,camera);
        flying = new Flying( pilotControls, shipState, collisionMgr );
        gameLoop->addTickable(pilotControls,"pilotControls");
        gameLoop->addTickable(flying,"flying");
    }
    
    // Navigator Controls
    if(collabInfo->getGameRole() == NAVIGATOR) {
	    navigatorControls = new NavigatorControls(inputState,camera);
	    gameLoop->addTickable(navigatorControls,"navigatorControls");
    }
    
    // Engineer Controls
    if(collabInfo->getGameRole() == ENGINEER) {
	    engineerControls = new EngineerControls(inputState,camera);
	    gameLoop->addTickable(engineerControls,"engineerControls");
    }

	// GameState
	if(collabInfo->getGameRole() == PILOT) {
	    gameStateMachine = new GameStateMachine(mapMgr,shipState);
	    networkingManager->replicate(gameStateMachine);
	    
    } else {
    	gameStateMachine = 
    		(GameStateMachine*) networkingManager->
    			getReplica("GameStateMachine",true);
    }
	gameLoop->addTickable(gameStateMachine,"gameStateMachine");
	gameParameterMap = new GameParameterMap(gameStateMachine);

	// Print Game State changes
	printState = new PrintState(gameStateMachine);
	gameLoop->addTickable(printState,"printState");

	// Pilot Gun State
	if(collabInfo->getGameRole() == PILOT) {
	    pilotGunState = new GunState(pilotControls,collabInfo->getGameRole());
	    networkingManager->replicate(pilotGunState);
	} else {
    	pilotGunState = (GunState*) networkingManager->
    		getReplica("PilotGunState",true);
    }
    gameLoop->addTickable(pilotGunState,"pilotGunState");
    
    // Navigator Gun State
	if(collabInfo->getGameRole() == NAVIGATOR) {
	    navigatorGunState = new GunState(navigatorControls,collabInfo->getGameRole());
	    networkingManager->replicate(navigatorGunState);
	    gameLoop->addTickable(navigatorGunState,"navigatorGunState");
	} else {
    	if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) {
    	    navigatorGunState = (GunState*) networkingManager->
    	        getReplica("NavigatorGunState",true);
    	        std::cout << "Got nav gun from net" << std::endl;
    	    gameLoop->addTickable(navigatorGunState,"navigatorGunState");
    	}
    }
    
    // Engineer Gun State
	if(collabInfo->getGameRole() == ENGINEER) {
	    engineerGunState = new GunState(engineerControls,collabInfo->getGameRole());
	    networkingManager->replicate(engineerGunState);
	    gameLoop->addTickable(engineerGunState,"engineerGunState");
	} else {
    	if (collabInfo->getNetworkRole() != DEVELOPMENTSERVER) {
    	    engineerGunState = (GunState*) networkingManager->
    		    getReplica("EngineerGunState",true);
    		    std::cout << "Got eng gun from net" << std::endl;
    		gameLoop->addTickable(engineerGunState,"engineerGunState");
        }
    }

	// TODO: start the enemies pointing towards the ship?
	// Swarm Manager
    if (collabInfo->getGameRole() == PILOT) {
	    swarmMgr = new SwarmManager(sceneMgr, sceneNodeMgr, gameParameterMap, mapMgr,
		    shipState,collisionMgr,networkingManager);
    } else {
        swarmMgr = new SwarmManager(sceneMgr, sceneNodeMgr, gameParameterMap,
        	collisionMgr, networkingManager);
    }
	gameLoop->addTickable(swarmMgr, "swarmMgr");

    gameLoop->addTickable(networkingManager,"networkingManager");

	// Bullet Manager
	bulletMgr = new BulletManager(shipState,sceneMgr,pilotGunState,
        engineerGunState,navigatorGunState,collisionMgr,swarmMgr,sceneNodeMgr);
	gameLoop->addTickable(bulletMgr,"bulletManager");

	// Audio
	//soundMgr = new SoundManager();
	//gameLoop->addTickable(soundMgr,"soundManager");
	//audioState = new AudioState(pilotGunState,soundMgr,shipSceneNode);
	//gameLoop->addTickable(audioState,"audioState");

	// Last class to be added to the game loop

    // CEGUI Stuff
    guiMgr = new GuiManager(mapMgr);    
    guiStatusUpdater = new GuiStatusUpdater(guiMgr,gameLoop);
    gameLoop->addTickable(guiStatusUpdater,"guiStatusUpdater");

    // Start Rendering Loop
    
    gameLoop->startLoop();
    //root->startRendering();
    networkingManager->stopNetworking();
}

Root *Main::configRoot()
{
	Root *root = new Root();

    if (!root->restoreConfig())
        root->showConfigDialog();
        
    return root;
}

void Main::configResources()
{
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
                   
    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/fonts", "FileSystem", "fonts"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/imagesets", "FileSystem", "imagesets"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/layouts", "FileSystem", "layouts"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/looknfeel", "FileSystem", "looknfeels"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/lua_scripts", "FileSystem", "lua_scripts"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/schemes", "FileSystem", "schemes"); 

    ResourceGroupManager::getSingleton().addResourceLocation(
                    "cegui/xml_schemas", "FileSystem", "xml_schemas"); 

    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

CollaborationInfo *Main::runLoby(NetworkingManager *networkingManager) {
    
    CollaborationInfo *collabInfo;

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
    
    return collabInfo;
}

Camera *Main::createCamera(SceneNode *shipSceneNode) {

    Camera *camera = sceneMgr->createCamera("mainCam");

    shipSceneNode->attachObject(camera);

    camera->setPosition(Vector3(0,0,0));
    camera->lookAt(Vector3(0,0,1));
    //camera->setFOVy(Radian(2.0943951));
    camera->setNearClipDistance(1);
    camera->setFarClipDistance(1500);
    
    // Lighting
    //sceneMgr->setShadowColour(ColourValue(0.5,0.5,0.5));
    
    // Add some sexy fog
    ColourValue fadeColour(0.1,0.1,0.1);
    sceneMgr->setFog(FOG_LINEAR, fadeColour, 0.0, 0, 300);
    
    Light *sp = sceneMgr->createLight("ShipLight");
    sp->setType(Light::LT_POINT);
    sp->setDiffuseColour(0.6,0.6,1.0);
    sp->setSpecularColour(0.6,0.6,1.0);
    sp->setDirection(Vector3(0,0,1));
    sp->setAttenuation(10000, 0.7, 0.000025, 0.0000045);

    //Light *spot = sceneMgr->createLight("shipSpot");
    //spot->setType(Light::LT_SPOTLIGHT);
    //spot->setDiffuseColour(1.0,1.0,1.0);
    //spot->setSpecularColour(1.0,1.0,1.0);
    //spot->setDirection(Vector3(0,0,1));

    //shipSceneNode->attachObject(spot);
    shipSceneNode->attachObject(sp);
    
    return camera;
}

void Main::createViewPort() {

    Viewport *vp = window->addViewport(camera);
    vp->setBackgroundColour(ColourValue(1,0,0));
    
    camera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()*1.17));
    //camera->setAspectRatio(1.17);

    vp->update();
}

void Main::addCrossHair()
{
    ManualObject* manual = sceneMgr->createManualObject("manual");
    
    manual->setRenderQueueGroup(RENDER_QUEUE_OVERLAY-1);
    manual->setUseIdentityProjection(true);
    manual->setUseIdentityView(true);
    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
    manual->position(-3, 0, 1);
    manual->position(3, 0, 1);
    manual->position(0, -3, 1);
    manual->position(0, +3, 1);
    manual->end();
    
    sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

}




int main()
{
    Main *main = new Main();

    delete main;
}

Main::~Main()
{
}

void Main::exit()
{
    gameLoop->running = false;
}

