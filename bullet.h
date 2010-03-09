#ifndef _BULLET_H 
#define _BULLET_H

#include <OGRE/Ogre.h>
#include <string>
#include <iostream>
#include <math.h>
#include "enemy.h"
#include "playerStats.h"

using namespace Ogre;
using namespace std;

class Bullet
{
    private:
        SceneNode *bulletNode;
        string name;
        string rname;
        SceneManager *sceneMgr;


        Vector3 direction;
        Vector3 deathSpark;
        int velocity;

               
   public:
        double distanceTravelled;
        double distanceToTravel;

        Bullet(SceneNode *bulletNode,
            SceneManager *sceneMgr,
            string name,
            string rname,
            Vector3 direction, 
            int velocity,
            double dtt,
            PlayerStats *playerStats); // Distance to travel

        ~Bullet();

        void updateLocation();
        Vector3 getDeathSpark();
        
        Enemy *enemy;
        bool hitEnemy;
        bool hitShip;

        PlayerStats *playerStats;
};

#endif

