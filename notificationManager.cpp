
#include "notificationManager.h"

#include <iostream>

NotificationManager::NotificationManager(CollaborationInfo *collabInfo, GameStateMachine *stateMachine,
                                         MapManager *mapManager, ShipState *shipState,
                                         DamageState *damageState, SystemManager *systemManager,
                                         Tutorial *tutorial)
    : collabInfo(collabInfo)
    , notification(new Notification(NT_NONE,"","",0))
    , lastNotification(new Notification(NT_NONE,"","",0))
    , stateMachine(stateMachine)
    , mapManager(mapManager)
    , shipState(shipState)
    , damageState(damageState)
    , systemManager(systemManager)
    , mIsNewNotification(false)
    , lastStateNotified(GS_END)
    , nextType(NT_NONE)
    , controlsDisplayed(false)
    , tickcount(2000)
    , tutorial(tutorial)
    , lastTutorialStateNotified(TS_END)
{
    initializeRecencies();
}

NotificationManager::NotificationManager()
    : notification(new Notification(NT_NONE,"","",0))
    , lastNotification(new Notification(NT_NONE,"","",0))
    , nextType(NT_NONE)
    , collabInfo(0)
    , stateMachine(0)
    , mapManager(0)
    , shipState(0)
    , damageState(0)
    , systemManager(0)
    , tickcount(2000)
    , mIsNewNotification(false)
    , controlsDisplayed(false)
{
    initializeRecencies();
}

void NotificationManager::setTutorial(Tutorial *newTutorial) {
    tutorial = newTutorial;
}

void NotificationManager::initializeRecencies() {
    maxDelay = std::numeric_limits<int>::max();
    recency.insert(std::pair<NotificationType,int>(NT_TUT_START,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_TUT_OPEN_CONSOLE,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_TUT_CLOSE_CONSOLE,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_TUT_REPAIR,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_TUT_WAITING,maxDelay));

    recency.insert(std::pair<NotificationType,int>(NT_CONTROLS,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_ENGINES_CRITICAL,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_WEAPONS_CRITICAL,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_HULL_CRITICAL,maxDelay));

    recency.insert(std::pair<NotificationType,int>(NT_WEAPON_CHARGE_STUCK,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_SHIELD_CHARGE_STUCK,maxDelay));

    recency.insert(std::pair<NotificationType,int>(NT_UNDER_ATTACK,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_COMMENT_ONE,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_COMMENT_TWO,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_COMMENT_THREE,maxDelay));

    recency.insert(std::pair<NotificationType,int>(NT_OBJECTIVE_SEEK,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_OBJECTIVE_DESTROY,maxDelay));
    recency.insert(std::pair<NotificationType,int>(NT_OBJECTIVE_ESCAPE,maxDelay));
}

void NotificationManager::updateRecencies() {

    std::map<NotificationType,int>::const_iterator itr;
    for (itr = recency.begin() ; itr != recency.end() ; itr++) {
        if (itr->second != maxDelay) recency[itr->first] = itr->second + 1;
    }
}

void NotificationManager::tick()
{
    if (collabInfo == 0) {
        std::cerr << "NotificationManager has no collabinfo" << std::endl;
        exit(-1);
    }

    mIsNewNotification = false;

    if (mapManager == 0) {

        // Check for any local notifications
        if (tickcount < 1 / ConstManager::getFloat("tick_period")) {
            tickcount ++;
            return;
        } else {
            tickcount = 0;
            updateRecencies();
            checkTutorialState();
        }
        if (lastNotification->getType() != nextType) {
            // There's a new notification

            mIsNewNotification = true;

        }

        prepareNotification();

        if (nextType != NT_NONE) lastNotification = notification;

        // Print the notification to the terminal for now. Will be removed
        // when linked in with GUI/sound
        if (getCurrentNotification()->getType() != NT_NONE) {
            std::cout << getCurrentNotification()->getConsoleText();
        }
        return;
    }

    if (tickcount < 1 / ConstManager::getFloat("tick_period")) {
        tickcount ++;
    } else {
        tickcount = 0;
        updateRecencies();
        checkComments(); 
        checkShipPosition();
        checkCharges();
        checkHealth();
        checkTutorialState();
        checkGameState();

        prepareNotification();

        // Print the notification to the terminal for now. Will be removed
        // when linked in with GUI/sound
        if (getCurrentNotification()->getType() != NT_NONE) {
            std::cout << getCurrentNotification()->getConsoleText();
        }
    }

}

std::string NotificationManager::pick(std::string sound, int num) {

	num = rand() % num + 1;
	std::stringstream temp;
	temp << "sound_" << sound << num;
	return temp.str();

}

void NotificationManager::prepareNotification() {
    std::stringstream consoleText;
    string soundNameConst = "";
    int soundLength = 0;
    bool local = false;
    srand ( time(NULL) );
    std::string gameRole = collabInfo->getGameRoleString();
    std::transform(gameRole.begin(), gameRole.end(), gameRole.begin(), ::tolower);
    switch(nextType) {
        case NT_CONTROLS:
            consoleText << "Welcome aboard, " << collabInfo->getNick() << "."
                        << "\nWe know you're going to make a great " << gameRole << "."
                        << "\n\nWatch this space for further instructions." << std::endl;
            break;
        case NT_UNDER_ATTACK:
            soundNameConst = pick("IncomingSwarms",2);
            if (collabInfo->getGameRole() != ENGINEER)
                consoleText << "They're coming for us! You may want to ask your engineer to increase weapon power..." << std::endl;
            else
                consoleText << "They're coming for us! You may want to increase weapon power..." << std::endl;
            break;
        case NT_FLEE:
        	soundNameConst = pick("ObjectiveDestroyed",2);
            if (collabInfo->getGameRole() == PILOT)
                consoleText << "You did it! Now you need to ask your navigator to find the exit, and escape." << std::endl;
            else if (collabInfo->getGameRole() == NAVIGATOR)
                consoleText << "You did it! Now use your large map to find the exit, and get the hell out of here." << std::endl;
            else if (collabInfo->getGameRole() == ENGINEER)
                consoleText << "You did it! You may want to increase engine power so we can escape quickly." << std::endl;
            break;
        case NT_OBJECTIVE_SEEK:
        	soundNameConst = "sound_ClearedToProceed";
            consoleText << "You should be ready for your mission now. Your first objective is to find the Black Comrade..." << std::endl;
            break;
        case NT_ENGINES_CRITICAL:
        	soundNameConst = pick("EngineDamage",3);
            consoleText << "Engines are critical! Repair them quickly, or you'll be immobilised." << std::endl;
            break;
        case NT_WEAPONS_CRITICAL:
        	soundNameConst = pick("WeaponDamage",5);
            consoleText << "Weapons are critical! Repair them quickly, or you'll be defenceless." << std::endl;
            break;
        case NT_HULL_CRITICAL:
            soundNameConst = pick("HullDamage",4);
            consoleText << "The hull is almost breached! Repair it quickly, or it's game over, men." << std::endl;
            break;
        case NT_SENSORS_CRITICAL:		// this one may need to be plugged in
        	soundNameConst = pick("SensorDamage",2);
        	consoleText << "Our sensors are almost destroyed! Repair them quickly, or we'll be flying blind!" << std::endl;
        case NT_COMMENT_ONE:
            consoleText << "Keep up the good work, soldiers." << std::endl;
            break;
        case NT_COMMENT_TWO:
            consoleText << "We're all counting on you!" << std::endl;
            break;
        case NT_COMMENT_THREE:
            consoleText << "Let's show those commies who's boss." << std::endl;
            break;
        case NT_TUT_START:
            consoleText << "Let's give you a quick reminder of how this ship works..." << std::endl;
            break;
        case NT_TUT_OPEN_CONSOLE:
            consoleText << "You can repair the ship using the console."
                        << "\n\nTry opening the console now by pressing the ESCAPE key." << std::endl;
            local = true;
            break;
        case NT_TUT_CLOSE_CONSOLE:
            consoleText << "Looks like you're familiar with the console now."
                        << "\n\nTry closing it by pressing the ESCAPE key again." << std::endl;
            local = true;
            break;
        case NT_TUT_REPAIR:
            consoleText << "The ship's systems are damaged. Pick one and repair it using the console.\n";
            local = true;
            break;
        case NT_TUT_WAITING:
            consoleText << "Great! We'll just wait while the other players finish getting to grips with the ship...\n";
            local = true;
            break;
        case NT_WEAPON_CHARGE_STUCK:
        	soundNameConst = "sound_WeaponChargeDepleted";
            consoleText << "Weapons are out of charge!\n";
            local = true;
            break;
        case NT_SHIELD_CHARGE_STUCK:
        	soundNameConst = "sound_ShieldsDown";
            consoleText << "Shields are out of charge!\n";
            local = true;
            break;
        case NT_COUNTDOWN_5:
        	soundNameConst = "sound_Count54321";
        	break;
    	case NT_COUNTDOWN_10:
    		soundNameConst = "sound_Count10";
    		break;
		case NT_COUNTDOWN_15:
    		soundNameConst = "sound_Count15";
    		break;
    	case NT_COUNTDOWN_30:
    		soundNameConst = "sound_Count30";
    		break;
    	case NT_COUNTDOWN_60:
    		soundNameConst = "sound_Count60";
    		break;
		case NT_MISSION_COMPLETE:
			soundNameConst = "sound_MissionComplete";
			break;
		case NT_DISOBEY:
			soundNameConst = pick("Disobey",2);
			break;
		case NT_GAME_OVER:
			soundNameConst = "sound_GameOver";
			break;
		case NT_NEAR_BC:
			soundNameConst = "sound_NearingBC";
			break;
		case NT_SHIP_ON:
			soundNameConst = "sound_Startup";
			break;
    }
    notification = new Notification(nextType,consoleText.str(),soundNameConst,soundLength);
    if (local) notification->makeLocal();
    if (nextType != NT_NONE) {
        lastNotification = notification;
        nextType = NT_NONE;
        recency[nextType] = 0;
    }
}

bool NotificationManager::isTimely(NotificationType type, int delaySinceMe, int delaySinceLast) {
    return (getTimeSince(type) > delaySinceMe &&
            getTimeSinceLast() > (delaySinceLast + lastNotification->getSoundLength()));
}

int NotificationManager::getTimeSince(NotificationType type) {
    std::map<NotificationType, int>::const_iterator itr;
    itr = recency.find(type);
    if (itr == recency.end()) {
        return 0;
    } else {
        return itr->second;
    }
}

int NotificationManager::getTimeSinceLast() {
    int last = std::numeric_limits<int>::max();
    std::map<NotificationType,int>::const_iterator itr;
    for (itr = recency.begin() ; itr != recency.end() ; itr++) {
        if (itr->second < last) last = itr->second;
    }
    return last;
}

void NotificationManager::checkGameState() {
    GameState gameState = stateMachine->currentGameState();
    if (lastStateNotified == gameState) return;
    NotificationType newNotification = lastNotification->getType();
    switch(gameState) {
        case GS_TUTORIAL:
            if (!controlsDisplayed)
                newNotification = NT_CONTROLS;
                controlsDisplayed = true;
            break;
        case GS_STEALTH:
            newNotification = NT_OBJECTIVE_SEEK;
            break;
        case GS_ATTACK:
            newNotification = NT_UNDER_ATTACK;
            break;
        case GS_FLEE:
            newNotification = NT_FLEE;
            break;
    }
    if(lastNotification->getType() != newNotification) {
        if (isTimely(newNotification,0,1)) {
            mIsNewNotification = true;
            nextType = newNotification;
            lastStateNotified = gameState;
        }
    }
}

void NotificationManager::checkTutorialState() {
    TutorialState tutorialState = tutorial->getState();
    //if (lastTutorialStateNotified == tutorialState) return;
    NotificationType newNotification = lastNotification->getType();
    switch(tutorialState) {
        case TS_OPEN_CONSOLE:
            newNotification = NT_TUT_OPEN_CONSOLE;
            break;
        case TS_CLOSE_CONSOLE:
            newNotification = NT_TUT_CLOSE_CONSOLE;
            break;
        case TS_START:
            newNotification = NT_TUT_START;
            break;
        case TS_REPAIR_SYSTEMS:
            newNotification = NT_TUT_REPAIR;
            break;
        case TS_WAITING_FOR_OTHERS:
            newNotification = NT_TUT_WAITING;
            break;
    }

    if(lastNotification->getType() != newNotification) {
        if (isTimely(newNotification,0,0)) {
            mIsNewNotification = true;
            nextType = newNotification;
            lastTutorialStateNotified = tutorialState;
        }
    }
}

void NotificationManager::checkShipPosition() {
    //TODO - Base any notifications on map tiles?
}

void NotificationManager::checkComments() {
    srand ( time(NULL) );
    int irand = rand() % 3 + 1;

    if (tutorial->getState() != TS_END) return;

    NotificationType newNotification = lastNotification->getType();
    switch(irand) {
        case 1:
            newNotification = NT_COMMENT_ONE;
            break;
        case 2:
            newNotification = NT_COMMENT_TWO;
            break;
        case 3:
            newNotification = NT_COMMENT_THREE;
            break;
    }
    if(lastNotification->getType() != newNotification) {
        if (isTimely(newNotification,180,30)) {
            mIsNewNotification = true;
            nextType = newNotification;
        }
    }
}

void NotificationManager::checkHealth() {
    NotificationType newNotification = lastNotification->getType();
    if (damageState->getHullHealth() <= 20) {
        newNotification = NT_HULL_CRITICAL;
    } else if (damageState->getEngineHealth() <= 20) {
        newNotification = NT_ENGINES_CRITICAL;
    } else if (damageState->getWeaponHealth() <= 20) {
        newNotification = NT_WEAPONS_CRITICAL;
    }
    if(lastNotification->getType() != newNotification) {
        if (isTimely(newNotification,20,8)) {
            mIsNewNotification = true;
            nextType = newNotification;
        }
    }
}

void NotificationManager::checkCharges() {
    NotificationType newNotification = lastNotification->getType();
    GameState gameState = stateMachine->currentGameState();
    if (systemManager->areWeaponsStuck()
        && (gameState == GS_ATTACK || gameState == GS_BLACK_COMRADE))
        newNotification = NT_WEAPON_CHARGE_STUCK;
    else if (systemManager->areShieldsStuck()
        && (gameState == GS_ATTACK || gameState == GS_BLACK_COMRADE))
        newNotification = NT_SHIELD_CHARGE_STUCK;

    //std::cout << newNotification << "\n";
    if(lastNotification->getType() != newNotification) {
        if (isTimely(newNotification,1000,1)) {
            mIsNewNotification = true;
            nextType = newNotification;
        }
    }
}

void NotificationManager::setCollaborationInfo(CollaborationInfo *mCollabInfo) {
    collabInfo = mCollabInfo;
}
	
Notification *NotificationManager::getCurrentNotification() {
	return notification;
}

bool NotificationManager::hasNewNotification() {
	return mIsNewNotification;
}

RakNet::RakString NotificationManager::GetName(void) const {return RakNet::RakString("NotificationManager");}

RM3SerializationResult NotificationManager::Serialize(SerializeParameters *serializeParameters) {
    serializeParameters->outputBitstream[0].Write(lastNotification->getType());
    if (lastNotification->isLocal()) return RM3SR_DO_NOT_SERIALIZE;
    return RM3SR_BROADCAST_IDENTICALLY;
}

void NotificationManager::Deserialize(RakNet::DeserializeParameters *deserializeParameters) {
    deserializeParameters->serializationBitstream[0].Read(nextType);
}
