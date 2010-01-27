#include "swarmManager.h"
#include "const.h"

SwarmManager::SwarmManager(SceneManager *sceneMgr, GameParameterMap *gamePM, MapManager *mapMgr, ShipState *shipState) :
    sceneMgr(sceneMgr),
    gamePM(gamePM),
    mapMgr(mapMgr),
    id(0),
    dynSwarmSize(0),
    swarmTick(0),
    shipState(shipState)
{

    activeSwarms = new vector<Swarm*>();

    vector<Vector3*> wps = mapMgr->getInitialSpawnPoints();

    Vector3 *v;
    for(vector<Vector3*>::const_iterator it=wps.begin();it!=wps.end(); ++it) {
        v = *it;
        vector<Vector3*> rings = mapMgr->getSpawnPoints(v);

        Vector3 *spawnPoint;
        for(vector<Vector3*>::const_iterator ite=rings.begin();ite!=rings.end(); ++ite) {
            spawnPoint = *ite;
            Vector3 sp = Vector3(spawnPoint->x,spawnPoint->y,spawnPoint->z);
            createSwarm(1,sp);
            //cout << "Created initial swarm..." << endl;
        }
    }
    
}

SwarmManager::~SwarmManager()
{
}

void SwarmManager::createSwarm(int size, Vector3 location)
{
    Swarm *s = new Swarm(size,id,location,sceneMgr,0,0,0,shipState);
    activeSwarms->push_back(s);
    id++;
}

void SwarmManager::tick() 
{
    int sp = gamePM->getParameter("SPAWN");

    if(sp>0) {
        if(dynSwarmSize<sp) {
            swarmTick++;
            if(swarmTick>Const::SPAWN_DELAY) {
                swarmTick = 0;
                Vector3 spawnPoint = mapMgr->getDynamicSpawnPoint(shipState->position);
                createSwarm(1,spawnPoint);
                dynSwarmSize++;
            }
        }
    }

    // Here we are updating the locations of the swarms and the enemies within
    for(int i=0;i<activeSwarms->size();i++) {
        Swarm *s = activeSwarms->at(i);
        s->tick();
    } 
}
