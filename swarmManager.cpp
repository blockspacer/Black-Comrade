#include "swarmManager.h"
#include "const.h"

SwarmManager::SwarmManager(SceneManager *sceneMgr, SceneNodeManager *sceneNodeMgr,
	GameParameterMap *gamePM, MapManager *mapMgr, ShipState *shipState,
	CollisionManager* colMgr, NetworkingManager *networkingMgr, Lines *lines) :
    sceneMgr(sceneMgr),
    sceneNodeMgr(sceneNodeMgr),
    gamePM(gamePM),
    mapMgr(mapMgr),
    id(0),
    dynSwarmSize(0),
    swarmTick(0),
    shipState(shipState),
    colMgr(colMgr),
    networkingMgr(networkingMgr),
    lines(lines)
{

    activeSwarms = std::vector<Swarm*>();

    std::vector<Vector3*> wps = mapMgr->getInitialSpawnPoints();

    Vector3 *v;
    for(std::vector<Vector3*>::const_iterator it=wps.begin();it!=wps.end(); ++it) {
        v = *it;
        std::vector<Vector3*> rings = mapMgr->getSpawnPoints(v);

        Vector3 *spawnPoint;
        for(std::vector<Vector3*>::const_iterator ite=rings.begin();ite!=rings.end(); ++ite) {
            spawnPoint = *ite;
            Vector3 sp = Vector3(spawnPoint->x,spawnPoint->y,spawnPoint->z);
            createSwarm(10,sp);
            cout << "Created initial swarm..." << endl;
            return;
        }
    }
    
}

SwarmManager::SwarmManager(SceneManager *sceneMgr, SceneNodeManager *sceneNodeMgr, GameParameterMap *gamePM,
    CollisionManager* colMgr, NetworkingManager *networkingMgr) :
    sceneMgr(sceneMgr),
    sceneNodeMgr(sceneNodeMgr),
    gamePM(gamePM),
    mapMgr(0),
    id(0),
    dynSwarmSize(0),
    swarmTick(0),
    shipState(shipState),
    colMgr(colMgr),
    networkingMgr(networkingMgr)
{
    activeSwarms = std::vector<Swarm*>();
}

SwarmManager::~SwarmManager()
{
}

void SwarmManager::createSwarm(int size, Vector3 location)
{
    Swarm *s = new Swarm(size,id,location,sceneMgr,0,0,0,shipState,sceneNodeMgr
        ,lines,colMgr,mapMgr,gamePM);

    std::vector<Enemy*> ents = s->getAllEnemies();
    Enemy *en;
    for(std::vector<Enemy*>::const_iterator ite=ents.begin();ite!=ents.end();++ite) {
        en = *ite;
        colMgr->addMesh(sceneNodeMgr->getEntity(en));
    }
    
    activeSwarms.push_back(s);
    id++;
}

std::vector<Enemy*> SwarmManager::getAllEnemies()
{
    Swarm *s;
    Enemy *enemy;
    std::vector<Enemy*> out = std::vector<Enemy*>();

    for(std::vector<Swarm*>::const_iterator it=activeSwarms.begin();it!=activeSwarms.end();++it) {
        s = *it;
        std::vector<Enemy*> ents = s->getAllEnemies();

        for(std::vector<Enemy*>::const_iterator ite=ents.begin();ite!=ents.end();++ite) {
            enemy = *ite;
            out.push_back(enemy);
        }
    }

    return out;
}

std::vector<Enemy*> SwarmManager::getReplicatedEnemies() {
    Enemy *enemy;
    std::vector<Enemy*> out = std::vector<Enemy*>();

    std::vector<ReplicaObject*> replicatedEnemies = networkingMgr->getReplicas("Enemy");
    for (std::vector<ReplicaObject*>::const_iterator it=replicatedEnemies.begin();it!=replicatedEnemies.end();++it) {
        enemy = (Enemy*) *it;
        out.push_back(enemy);
    }

    return out;
}

void SwarmManager::updateRemoteSwarms() {
    if (mapMgr == 0) {
        std::vector<ReplicaObject*> replicatedEnemies = networkingMgr->getReplicas("Enemy");
        for (std::vector<ReplicaObject*>::const_iterator it=replicatedEnemies.begin();it!=replicatedEnemies.end();++it) {
            Enemy *enemy = (Enemy*) *it;
            sceneNodeMgr->createNode(enemy);
            if (enemy->health < 0) {
                sceneNodeMgr->deleteNode(enemy);
                delete enemy;
            }
        }
    } else {
        std::vector<Enemy*> allEnemies = getAllEnemies();
        for (std::vector<Enemy*>::const_iterator it = allEnemies.begin(); it!=allEnemies.end();++it) {
            if (!(*it)->isReplicated) networkingMgr->replicate(*it);
        }
    }
}

void SwarmManager::tick() 
{
    updateRemoteSwarms();
    if (mapMgr == 0) return;

    lines->clear();

    int sp = gamePM->getParameter("SPAWN");


    if(sp>0) {
        if(activeSwarms.size()<sp) {
            swarmTick++;
            if(swarmTick>Const::SPAWN_DELAY) {
                swarmTick = 0;
                Vector3 spawnPoint = mapMgr->getDynamicSpawnPoint(shipState->getPosition());
                createSwarm(10,spawnPoint);
                cout << "Swarms: " << activeSwarms.size() << endl;
            }
        }
    }

    // Here we are updating the locations of the swarms and the enemies within
    for(int i=0;i<activeSwarms.size();i++) {
        Swarm *s = activeSwarms.at(i);
        if(s->size==0) {
            delete s;
            activeSwarms.erase(activeSwarms.begin()+(i));
            cout << "Swarms: " << activeSwarms.size() << endl;
        } else {
            s->tick();
        }
    }
}

