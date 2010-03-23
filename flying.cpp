#include "flying.h"
#include "const.h"

Flying::Flying(PilotControls *sc, ShipState *shipState, DamageState *damageState, CollisionManager *colMgr, SystemManager *systemManager, bool useCollisions, double iXPos, double iYPos, double iZPos):
    colMgr(colMgr),
    sc(sc),
    shipState(shipState),
    damageState(damageState),
    systemManager(systemManager),
    useCollisions(useCollisions),
    zVel(0.0),
    xVel(0.0),
    yVel(0.0),
    flyPitch(0.0),
    flyYaw(0.0),
    flyRoll(0.0),
    pitch(0.0),
    yaw(0.0),
    roll(0.0),
    hitCountDown(0),
    addRoll(0.0),
    addPitch(0.0),
    addYaw(0.0),
    yawMom(0.0)
{
    position = new Vector3(iXPos, iYPos, iZPos );
}

Flying::~Flying()
{}

void Flying::updateAngels()
{
    if( hitCountDown == 0)
    {
        flyPitch += (0.004*sc->forward());
        if( flyPitch > 0.4 ) flyPitch = 0.4;
        flyRoll += (0.01*sc->side());
        if( flyRoll > 1.0 ) flyRoll = 1.0;
        yawMom += (0.0015*sc->yaw());
    }
}

void Flying::updatePosition()
{
    //Collision col = colMgr->collideWithMapPiece((Entity*)shipState->shipSceneNode->getAttachedObject(1) );
    Collision col = colMgr->shipMapCollision(position);
    if(col.isCollided && useCollisions)
    {
        vFactor = 0.05;
        hitCountDown = static_cast<int> (100.0*col.penetration[0]);
        //reflected vel need to be fixed
        xVel += col.penetration[0] * col.normals[0];
        yVel += col.penetration[0] * col.normals[1];
        zVel += col.penetration[0] * col.normals[2];

        damageState->damage(col.penetration[0]);
    }

    if( hitCountDown == 0 )
    {
        updateAngels();
        //hack considering not all. Works fine though
        if( damageState->getEngineHealth() > 0 )
        {
            double engineRate = systemManager->getEngineRate();
            double xzFor =  (engineRate+0.1)*EngineForce*sin(flyPitch);
            xVel += xzFor*sin(flyYaw);
            zVel += xzFor*cos(flyYaw);
            double xzSide = (engineRate+0.1)*SideForce*sin(flyRoll);
            xVel -= xzSide*sin(flyYaw+1.57079633);
            zVel -= xzSide*cos(flyYaw+1.57079633);
    
            yVel += 0.025* sc->up();
        }
    
        addRoll = 0.0;
        addYaw = 0.0;
        addPitch = 0.0;
    }
    else
    {
        double r = 1.0 *rand()/RAND_MAX-0.5;
        addRoll = r * vFactor;
        addYaw = r * vFactor;
        addPitch = r * vFactor;
        vFactor *= 0.93;
        hitCountDown--;
    }

    xVel *= 0.96;
    zVel *= 0.96;
    yVel *= 0.9;

    flyPitch *= 0.98;
    flyRoll *= 0.98;
    yawMom *= 0.9;

    flyYaw += yawMom;

    position->x += xVel;
    position->z += zVel;
    position->y += yVel;

    pitch = flyPitch + addPitch;
    roll = flyRoll + addRoll;

    yaw = flyYaw + addYaw;
}

void Flying::updateShipState()
{
    shipState->setX(position->x);
    shipState->setY(position->y);
    shipState->setZ(position->z);
    
    shipState->yaw = yaw;
    shipState->pitch = -pitch;
    shipState->roll = -roll;
}

void Flying::tick()
{
    shipState->setSpeed(getSpeed());
    updatePosition();
    updateShipState();
}

double Flying::getSpeed()
{
    return sqrt( xVel*xVel+zVel*zVel);
}
