
#ifndef ENEMY_STATE_H
#define ENEMY_STATE_H

// Includes all Ogre classes
#include <OGRE/Ogre.h>

#include "ITickable.h"
#include "rayQuery.h"

using namespace Ogre;

class EnemyState : public ITickable {
    
private:
    
    SceneNode *eSceneNode;
    SceneManager *mSceneMgr;
    RayQuery *rRayQuery;
    
    static const float FVEL = 0.3;
    //lookahead in ticks
    static const float LOOKA = 30;

    
public:
    Vector3 *position;
    Real roll;
    Real pitch;
    Real yaw;
    
    EnemyState(SceneNode *neSceneNode, SceneManager *mSceneMgr );
    
    void tick();
    
    void updateOgre();
};

#endif