#ifndef _FLYING_H
#define _FLYING_H

#include <OGRE/Ogre.h>
#include <math.h>
#include "ITickable.h"
#include "pilotControls.h"
#include "collisionManager.h"
#include "shipState.h"
#include "damageState.h"
#include "systemManager.h"
#include "sceneNodeManager.h"

using namespace Ogre;
using namespace std;

class Flying : public ITickable
{
    private:
    	
    	// Decent
    	float elivation;
    	int timeToNextDamage;
    
        void updateShipState();
        
        bool useCollisions;
    
        int hitCountDown;
        CollisionManager *colMgr;
        PilotControls *sc;
        ShipState *shipState;
        DamageState *damageState;
        SystemManager *systemManager;
        SceneNodeManager *snMgr;
        double vFactor;

        PlayerStats *pilotStats;

        double zVel;
        double xVel;
        double yVel;

        double addPitch;
        double addRoll;
        double addYaw;
        double yawMom;

        double flyPitch;
        double flyYaw;
        double flyRoll;

        double averageSpeed;
        int lastAverage;
        int averageDelay;

        bool flyingDisabled;

        int numSpeedsTaken;
        
        static const double EngineForce = 0.5;
        static const double SideForce = 0.13;
        
        double damage;

        void updateAverageSpeed();
        void updateAngels();

        // double getDrag(string dir);
        void updatePosition();
        // void changeVector();
    public:
        Vector3 *position; //
        // Vector3 *velocity; // ( Units per tick )
        //Vector3 *angularVelocity; // pitch yaw roll ( Radians per tick )
        //Vector3 *orientation; // pitch yaw roll ( Radians )
        double pitch;
        double yaw;
        double roll;

        Flying( SceneNodeManager *snMgr, PilotControls *sc, ShipState *shipState,
                DamageState *damageState, CollisionManager *colMgr,
                SystemManager *systemManager, bool collisions,
                double x, double y, double z, double yaw,
                PlayerStats *pilotStats);

        ~Flying();

        double getSpeed();

        void enableFlying();

        virtual void tick();
};

#endif

