#ifndef _ENEMY_H 
#define _ENEMY_H

#include <OGRE/Ogre.h>
#include <string>
#include <iostream>
#include <math.h>

#include "IDrawable.h"
#include "sceneNodeManager.h"

using namespace Ogre;
using namespace std;

class Enemy : public IDrawable
{
    private:
        Vector3 *position;
        Real roll;
        Real pitch;
        Real yaw;
        SceneManager *sceneMgr;
        int id;
        
    public:
    	int health;
    	bool fire;
    	int fireDelay;
    
        SceneNode *node;
        Enemy(int health, int id);

        Vector3 getDirection();

        Vector3 *getPosition();
        Vector3 *getOrientation();

		Real getRoll();
		Real getPitch();
		Real getYaw();

        std::string getMeshName();
        IDrawable *getParentObject();

        void setPosition(Vector3 v);
        void setOrientation(Real roll, Real pitch, Real yaw);

        int getHealth();

        ~Enemy();
};

#endif
