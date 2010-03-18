#include "networkRoleMenu.h"
#include "IMenuScreen.h"

NetworkRoleMenu::NetworkRoleMenu(InputState *inputState,
                                 NetworkingManager *networkingMgr,
                                 GuiManager *guiMgr)
    : inputState(inputState)
    , networkingMgr(networkingMgr)
    , guiMgr(guiMgr)
    , isVisible(false)
    , isEnd(false)
{
    CEGUI::ImagesetManager::getSingleton().create("NetworkRoleMenu.imageset");
    gameRefreshDelay = 200;
    lastRefresh = gameRefreshDelay;
}

void NetworkRoleMenu::tick() {
    // Check for key presses etc
    NetworkRole desiredRole = NO_NETWORK_ROLE;

    if (inputState->isKeyDown(OIS::KC_D)) {
        bool hosted = networkingMgr->hostGame(true);
        if (hosted) isEnd = true;
    } else if (inputState->isKeyDown(OIS::KC_S)) {
        bool hosted = networkingMgr->hostGame(false);
        if (hosted) isEnd = true;
    } else if (inputState->isKeyDown(OIS::KC_C))	 {
        bool joined = networkingMgr->connectToGame(0);
        if (joined) {
            isEnd = true;
        }
        else
        {
            std::cout << "failed" << std::endl;
            //SLEEP(30);
        }
    } else if (inputState->isKeyDown(OIS::KC_ESCAPE)) {
        std::exit(-1);
    }

    if (lastRefresh == gameRefreshDelay) {
        // Refresh games
        std::cout << "Refreshing game list\n";
        refreshGameList();
        lastRefresh = 0;
    } else { lastRefresh ++; }
}

void NetworkRoleMenu::refreshGameList() {
    std::vector<string> games = networkingMgr->findGames();
    std::cout << games.size() << " games found:\n";
    for(std::vector<string>::const_iterator it=games.begin();it!=games.end(); ++it) {
        std::cout << *it << "\n";
    }
}

MenuType::NetworkRoleMenu::nextMenu() {
    return MT_CHOOSE_GAME_ROLE;
}

void NetworkRoleMenu::show() {
    // Show background image etc
    if (isVisible) return;

    guiMgr->addStaticImage("NetworkRoleMenu",0.5, 0.5,1.0, 1.0,"NetworkRoleMenu","NetworkRoleMenu");
    isVisible = true;
}

void NetworkRoleMenu::hide() {
    // Hide background image etc
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow("NetworkRoleMenu");
}

bool NetworkRoleMenu::end() { return isEnd; }

bool NetworkRoleMenu::visible() { return isVisible; }
