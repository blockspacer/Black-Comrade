#ifndef _MAPMANAGER2_H
#define _MAPMANAGER2_H

#include <OGRE/Ogre.h>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <errno.h>
#include <algorithm>
#include <math.h>
#include "waypoint.h"
#include "mapTile.h"
#include "constManager.h"
#include "const.h"
#include "mapPieceChoices.h"
#include "lightAndObjectsManager.h"
#include "sceneNodeManager.h"
#include "connectionPiece.h"

#include <boost/math/distributions/uniform.hpp>
#include <boost/random.hpp>

//#define MAPROOT "/home/user-pool/users/jw7082/mapparts/"

using namespace std;
using namespace Ogre;

class MapManager {

private:

    boost::mt19937 rng;

    LightAndObjectsManager *lightMgr;

    int objx,objy; // Location of the objective tile
    int chosenIndex;
    string MAPROOT;
	SceneManager *sceneManager;
    SceneNodeManager *sceneNodeMgr;
    std::vector<Entity*> mapEntities;
    std::vector<ConnectionPiece*> connPieces;
    std::vector<Waypoint*> waypoints;
    std::vector<int> chosenPieces;
    MapPieceChoices *pieceChoices;
    bool objective;

    void loadMap(char *file);
    void createTile(string dir, std::vector<int> connections, int x, int y, int pieceToChoose = -1);
    void setSpawnPoints();
    void attachLight( Real x, Real z);
    int getNextChosenPiece();
    int startx,starty; // Index location of the start square of the map

public:

    MapTile* mts[Const::MAPSIZE][Const::MAPSIZE]; // Maptile storage

    MapManager(char* file, SceneManager *sceneManager, LightAndObjectsManager *lightMgr, SceneNodeManager *sceneNodeMgr);
    MapManager(char* file, MapPieceChoices *pieceChoices, SceneManager *sceneManager, LightAndObjectsManager *lightMgr, SceneNodeManager *sceneNodeMgr);

    Entity* getEntity(Vector3 *locn);
    //void getMapEntities(Vector3 *locn, Entity** mps );
    MapTile* getMapTile(Vector3 *locn);
    Vector3 getActualPosition(MapTile* tile);
    std::vector<string*> getWaypoints(Vector3 *locn);
    std::vector<Waypoint*> getAllWaypoints();
    std::vector<Vector3*> getSpawnPoints(Vector3 *locn); // Takes a location and returns the ring spawn places
    std::vector<Vector3*> getInitialSpawnPoints(); // Returns location inside the correct tile piece
    void makeConPieces();
    Vector3 getDynamicSpawnPoint(Vector3 *locn);
    Vector3 getObjectivePosition();
    Vector3 getStartingPosition();
    std::vector<Entity*> getMapEntitiesForCollision();
    std::vector<ConnectionPiece*> getMapConnectionPiecesForCollision();
    //mps has 5 space for 5 entity pointers
    void getEntitiesForCollisionFromAPosition(Vector3 *locn, Entity** mps);
    MapPieceChoices *getChosenPieces();
};

#endif
