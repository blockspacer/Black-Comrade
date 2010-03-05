#include "preGame.h"

PreGame::PreGame(SceneManager *sceneMgr, Ogre::RenderWindow *window, InputState *inputState, 
        GuiManager *guiMgr, NetworkingManager *networkingMgr)
    : sceneMgr(sceneMgr)
    , window(window)
    , inputState(inputState)
    , guiMgr(guiMgr)
    , networkingMgr(networkingMgr)
{
    preGameLoop = new StateUpdate();

    Camera *camera = sceneMgr->createCamera("preGameCam");
    Viewport *vp = window->addViewport(camera);
    vp->setBackgroundColour(ColourValue(0,0,0));
    camera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()*1.17));

    vp->update();

    preGameLoop->addTickable(inputState,"inputState");
    preGameLoop->addTickable(this,"preGame");

    collabInfo = new CollaborationInfo("",NO_NETWORK_ROLE,NO_GAME_ROLE);

    CEGUI::ImagesetManager::getSingleton().create("GameRoleMenu.imageset");
    CEGUI::ImagesetManager::getSingleton().create("NetworkRoleMenu.imageset");
}

CollaborationInfo* PreGame::run() {

    std::cout << "***Input is now capured by the main window!***\n"; 

    showMainMenu();
    preGameLoop->startLoop();

    networkingMgr->lobby->chooseNick("Player");

    window->removeAllViewports();

    return collabInfo;
}

void PreGame::showMainMenu() {
    std::cout << "Main menu displayed...\n";

    currentMenu = PGM_MAIN_MENU;
    showNetworkRoleMenu();
}

void PreGame::showNetworkRoleMenu() {
    CEGUI::FrameWindow *overlay     = guiMgr->addStaticImage("NetworkRoleMenu",     0.5, 0.5,   1.0,  1.0,    "NetworkRoleMenu",      "NetworkRoleMenu" );
    std::cout << "Pick network role [ D | S | C ]\n";
    currentMenu = PGM_CHOOSE_NETWORK_ROLE;
}

void PreGame::processNetworkRoleMenu() {
    NetworkRole desiredRole = NO_NETWORK_ROLE;

    if (inputState->isKeyDown(OIS::KC_D)) {
        desiredRole = DEVELOPMENTSERVER;
    } else if (inputState->isKeyDown(OIS::KC_S)) {
        desiredRole = SERVER;
    } else if (inputState->isKeyDown(OIS::KC_C))	 {
        desiredRole = CLIENT;
    } else if (inputState->isKeyDown(OIS::KC_ESCAPE)) {
        std::exit(-1);
    }
    if (desiredRole != NO_NETWORK_ROLE) {
        if (!networkingMgr->startNetworking(desiredRole)) {
            std::cout << "Could not start networking." << std::endl;
            std::exit(0);
        } else showGameRoleMenu();
    }
}

void PreGame::showGameRoleMenu() {
    CEGUI::FrameWindow *overlay     = guiMgr->addStaticImage("GameRoleMenu", 0.5, 0.5, 1.0, 1.0, "GameRoleMenu","GameRoleMenu" );
    std::cout << "Pick game role [ P | E | N ]\n";
    currentMenu = PGM_CHOOSE_GAME_ROLE;
}

void PreGame::processGameRoleMenu() {

    collabInfo = runLobby();
    
    if (collabInfo->getGameRole() != NO_GAME_ROLE) {
        clearMenuUI();
        exit();
    }
    if (inputState->isKeyDown(OIS::KC_P)) {
        networkingMgr->lobby->chooseGameRole(PILOT);
    } else if (inputState->isKeyDown(OIS::KC_N)) {
        networkingMgr->lobby->chooseGameRole(NAVIGATOR);
    } else if (inputState->isKeyDown(OIS::KC_E))	 {
        networkingMgr->lobby->chooseGameRole(ENGINEER);
    } else if (inputState->isKeyDown(OIS::KC_ESCAPE)) {
        std::exit(0);
    }
}

void PreGame::clearMenuUI() {
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow("NetworkRoleMenu");
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow("GameRoleMenu");
}

void PreGame::processMainMenu() {

}

void PreGame::tick() {
    switch (currentMenu) {
        case (PGM_MAIN_MENU):
            processMainMenu();
            break;
        case (PGM_CHOOSE_NETWORK_ROLE):
            processNetworkRoleMenu();
            break;
        case (PGM_CHOOSE_GAME_ROLE):
            processGameRoleMenu();
            break;
    }

}

void PreGame::exit() {
    preGameLoop->running = false;
}

CollaborationInfo *PreGame::runLobby() {
    networkingMgr->tick();
    return networkingMgr->collabInfo;
}
