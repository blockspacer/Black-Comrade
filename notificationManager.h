
#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "ITickable.h"
#include "gameStateMachine.h"
#include "shipState.h"
#include "mapManager2.h"
#include "damageState.h"
#include "systemManager.h"
#include "tutorial.h"
#include "objective.h"

#include "constManager.h"
#include <vector>
#include <limits>
#include <cstdlib>
#include <string>
#include <sstream>

#include "replicaObject.h"
#include "collaborationInfo.h"

#include "notificationType.h"
#include "notification.h"

class NotificationManager : public ITickable, public ReplicaObject
{
private:
    NotificationType nextType;
    Notification *notification;
    Notification *lastNotification;

    std::vector<Notification> queue;
    std::map <NotificationType,int> recency;

    CollaborationInfo *collabInfo;
    GameStateMachine *stateMachine;
    GameState lastStateNotified;
    TutorialState lastTutorialStateNotified;
    MapManager *mapManager;
    ShipState *shipState;
    DamageState *damageState;
    SystemManager *systemManager;
    Objective *objective;

    Tutorial *tutorial;
    bool mIsNewNotification;

    int tickcount;
    int maxDelay;

    bool controlsDisplayed;
    bool stateChangePending;

    void checkComments();
    void checkGameState();
    void checkTutorialState();
    void checkShipPosition();
    void checkHealth();
    void checkCharges();
    void checkCountdown();

    bool isTimely(NotificationType notification, int delaySinceMe, int delaySinceLast);
    int getTimeSince(NotificationType notification);
    int getTimeSinceLast();

    void initializeRecencies();
    void updateRecencies();

    void prepareNotification();
    
    std::string pick(std::string sound, int num);
	
public:
    NotificationManager();
    NotificationManager(CollaborationInfo *collabInfo, GameStateMachine *stateMachine,
                        MapManager *mapManager, ShipState *shipState,
                        DamageState *damageState, SystemManager *systemManager,
                        Tutorial *tutorial, Objective *objective);
    void tick();
	
    Notification *getCurrentNotification();
    bool hasNewNotification();

    void setCollaborationInfo(CollaborationInfo *collabInfo);
    void setTutorial(Tutorial *tutorial);

    virtual RakNet::RakString GetName(void) const;
    virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters);
    virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters);
};

#endif
