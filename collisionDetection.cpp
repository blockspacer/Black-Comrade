
#include "collisionDetection.h"

CollisionDetection::CollisionDetection( bool gernerateMeshes ):
    gernerateMeshes(gernerateMeshes),
    objCollision(NULL)
{
	newtonWorld = NewtonCreate();
    for(int i=0; i < 16; i++) idmatrix[i] = 0.0f;
    idmatrix[0] = 1.0f;
    idmatrix[5] = 1.0f;
    idmatrix[10] = 1.0f;
    idmatrix[15] = 1.0f;
    enemyEnt = NULL;
    shapeID = 5;
}

void CollisionDetection::mySerializeCollisionCallbackFunction(void* file, const void* buffer, int size)
{
    fwrite( buffer, size, 1, (FILE*)file );
}

void CollisionDetection::myDeserializeCollisionCallbackFunction(void* serializeHandle, void* buffer, int size)
{
    fread( buffer, size, 1, (FILE*)serializeHandle );
}

void CollisionDetection::createObjPrimitive( Real x, Real y, Real z, Real radius)
{
    dFloat pos[16];
    for(int i=0; i < 16; i++) pos[i] = 0.0f;
    pos[0] = 1.0f;
    pos[5] = 1.0f;
    pos[10] = 1.0f;
    pos[15] = 1.0f;
    pos[12] = x;
    pos[13] = y;
    pos[14] = z;
    objCollision = NewtonCreateSphere (newtonWorld, radius, radius, radius, shapeID, &pos[0]);
    shapeID++;
    //void* ptoFunc = &(mySerializeCollisionCallbackFunction);
    //NewtonReleaseCollision (newtonWorld, objCollision);
}

bool CollisionDetection::collideEntityWithObj(Entity *e)
{
    if( objCollision != NULL)
    {
        NewtonCollision *eCollision;
        std::map<Entity *,NewtonCollision *>::const_iterator iter = collisionsMap.find(e);
        if(iter != collisionsMap.end())
        {
            eCollision=iter->second;
        }
        else
        {
            cout << "in collideEntityWithObj(e), collision mesh for enitiy e could not be found in (cpp)map" << endl;
            return false;
        }
        dFloat eMatrix[16];
        getMatrix(e,eMatrix, true);
        dFloat contacts[16];
        dFloat normals[16];
        dFloat penetration[16];
        int numCollisionPoints = NewtonCollisionCollide (newtonWorld, 1,
                                    eCollision,   &eMatrix[0],
                                    objCollision, &idmatrix[0],
                                    &contacts[0], &normals[0], &penetration[0], 0);
        if (numCollisionPoints > 0) return true;
        else return false;
    }
    else
    {
        cout << "Objective collision mesh is NULL (most probably not initialised)" << endl;
        return false;
    }
}

dFloat CollisionDetection::objRayCollision(  Vector3 *start, Vector3 *end )
{
    dFloat p0[3];
    dFloat p1[3];
    p0[0] = start->x;
    p0[1] = start->y;
    p0[2] = start->z;

    p1[0] = end->x;
    p1[1] = end->y;
    p1[2] = end->z;

    dFloat normal[3];
    int att[1];

    dFloat t =  NewtonCollisionRayCast(objCollision, p0, p1, normal, att);
    //std::cout << "OBJ T: " << t << std::endl;
    return t;
}

void CollisionDetection::addStaticTreeCollisionMesh( Entity *entity, string name)
{
    bool meshCreated = false;
    NewtonCollision *treeCollision;

    std::stringstream out;
    out << name;

    string fileString = ConstManager::getString("cmesh_file_path");
    fileString += out.str();
    fileString.append(".cmesh");
    cout << fileString<<endl;
    char *fileName = (char*) fileString.c_str();

    if( !gernerateMeshes )
    {
        FILE* meshFile;
        meshFile = fopen(fileName,"r");
        if (meshFile!=NULL)
        {
            treeCollision = NewtonCreateCollisionFromSerialization (newtonWorld, myDeserializeCollisionCallbackFunction, meshFile);
            fclose (meshFile);
            meshCreated = true;
        }
        else
        {
            cout << "Colision detection could not read file.\nAttempting to create mesh from scratch" << endl;
        }
    }

    if( !meshCreated )
    {
        treeCollision = NewtonCreateTreeCollision (newtonWorld, 0);
        NewtonTreeCollisionBeginBuild(treeCollision);

        size_t vertex_count;
        size_t index_count;
        Ogre::Vector3 *vertices;
        unsigned long *indices;

        GetMeshInformation(   entity->getMesh(), vertex_count, vertices, index_count, indices,
                              entity->getParentNode()->getPosition(),
                              entity->getParentNode()->getOrientation(),
                              entity->getParentNode()->_getDerivedScale());

    /*                               Ogre::Vector3(),
                                  Ogre::Quaternion::IDENTITY,
                                  Ogre::Vector3(1,1,1)); */
        dFloat vArray[9];
        int i0, i1, i2;
        for (int i = 0; i < static_cast<int>(index_count); i += 3)
        {
            i0 = indices[i];
            i1 = indices[i+1];
            i2 = indices[i+2];

            vArray[0] = vertices[i0].x;
            vArray[1] = vertices[i0].y;
            vArray[2] = vertices[i0].z;

            vArray[3] = vertices[i1].x;
            vArray[4] = vertices[i1].y;
            vArray[5] = vertices[i1].z;

            vArray[6] = vertices[i2].x;
            vArray[7] = vertices[i2].y;
            vArray[8] = vertices[i2].z;

             NewtonTreeCollisionAddFace(treeCollision, 3, vArray,
                        sizeof(dFloat)*3, i);
        }

        NewtonTreeCollisionEndBuild(treeCollision, 1);

        FILE* meshFile;
        meshFile = fopen(fileName,"w");
        if (meshFile!=NULL)
        {
            NewtonCollisionSerialize(newtonWorld, treeCollision, mySerializeCollisionCallbackFunction, meshFile);
            fclose (meshFile);
        }
        else
        {
            cout << "Colision detection could not write cmesh file." << endl;
        }
    }

    NewtonBody* rigidTree = NewtonCreateBody (newtonWorld, treeCollision);
    NewtonReleaseCollision (newtonWorld, treeCollision);

    NewtonBodySetMatrix (rigidTree, &idmatrix[0]);

    dFloat boxP0[3];
    dFloat boxP1[3];

    //possibly need this if we rely on newton
    //NewtonCollisionCalculateAABB (treeCollision, &idmatrix[0], &boxP0[0], &boxP1[0]);

    collisionsMap.insert(pair<Entity*,NewtonCollision*>(entity,treeCollision));
    bodysMap.insert(pair<Entity*,NewtonBody*>(entity,rigidTree));
}

//only to be used with map pieces where the second element is not transformed
Collision CollisionDetection::staicAndDynamicCollision(Entity *e1, Entity *e2, bool dynamic)
{
    dFloat e1Matrix[16];
    dFloat e2Matrix[16];

    NewtonCollision *e1Collision;
    NewtonCollision *e2Collision;

    std::map<Entity *,NewtonCollision *>::const_iterator iter =
    		collisionsMap.find(e1);
    if(iter != collisionsMap.end()) {
    	e1Collision=iter->second;
    } else {
        cout<<"in mapCollision(e1,e2) the collision mesh for enitiy e1 could not be found in (C++)map"<< endl;
        dFloat contacts[16] = {0.0f};
        dFloat normals[16] = {0.0f};
        dFloat penetration[16] = {0.0f};
    	return Collision(false,normals,contacts,penetration);
    }

    iter = collisionsMap.find(e2);
    if(iter != collisionsMap.end()) {
    	e2Collision=iter->second;
    } else {
        cout<<"in mapCollision(e1,e2) collision mesh for mapEnitiy e2 could not be found in (C++)map"<<endl;
        dFloat contacts[16] = {0.0f};
        dFloat normals[16] = {0.0f};
        dFloat penetration[16] = {0.0f};
    	return Collision(false,normals,contacts,penetration);;
    }
    getMatrix(e1,e1Matrix, true);       //allwarys transform
    getMatrix(e2,e2Matrix, dynamic);    //transfrom second entity only if not static

    dFloat contacts[16];
    dFloat normals[16];
    dFloat penetration[16];
    int numCollisionPoints = NewtonCollisionCollide (newtonWorld, 1,
                                e1Collision, &e1Matrix[0],
                                e2Collision, &e2Matrix[0],
                                &contacts[0], &normals[0], &penetration[0], 0);

    if (numCollisionPoints > 0) {
        return Collision(true,normals,contacts,penetration);
    } else {
        return Collision(false,normals,contacts,penetration);
    }
}


void CollisionDetection::getMatrix(Entity *entity, dFloat *matrix, bool doTransform)
{
	for(int i=0; i < 16; i++) *(matrix+i) = 0.0f;
    *(matrix+0) = 1.0f;
    *(matrix+5) = 1.0f;
    *(matrix+10) = 1.0f;
    *(matrix+15) = 1.0f;

    if( doTransform )
    {
        Matrix4 m4 = entity->getParentSceneNode()->_getFullTransform().transpose();

        for( int i = 0; i < 4; i++)
        {
            for( int j = 0; j < 4; j++ )
            {
                *(matrix+i*4+j) = m4[i][j];
            }
        }
    }
}

/* dFloat CollisionDetection::getRCAgainstNCollision( Vector3 *start, Vector3 *end, NewtonCollision* collideAgainst )
{
    dFloat p0[3];
    dFloat p1[3];
    p0[0] = start->x;
    p0[1] = start->y;
    p0[2] = start->z;

    p1[0] = end->x;
    p1[1] = end->y;
    p1[2] = end->z;

    dFloat normal[3];
    int att[1];

     return NewtonCollisionRayCast( collideAgainst, p0, p1, normal, att);
} */


dFloat CollisionDetection::rayCollideDist( Vector3 *start, Vector3 *end, Entity* collideAgainst )
{
    dFloat p0[3];
    dFloat p1[3];
    p0[0] = start->x;
    p0[1] = start->y;
    p0[2] = start->z;

    p1[0] = end->x;
    p1[1] = end->y;
    p1[2] = end->z;

    dFloat normal[3];
    int att[1];

    NewtonCollision collideAgainstCollision;

    std::map<Entity *,NewtonCollision *>::const_iterator iter =
    		collisionsMap.find(collideAgainst);
    if(iter != collisionsMap.end()) {
        return NewtonCollisionRayCast( iter->second, p0, p1, normal, att);
    } else {
        cout << "in rayCollideDist(start,end,e) collsion mesh for e not found in (cpp)map" << endl;
    	return -1;
    }
}

dFloat CollisionDetection::rayCollideWithTransform( Vector3 *start, Vector3 *end, Entity* collideAgainst )
{
    //cout << collideAgainst << endl;
    Matrix4 m4 = collideAgainst->getParentSceneNode()->_getFullTransform().inverse();
    Vector3 transStart = m4 * (*start);
    Vector3 transEnd = m4 * (*end);
    return rayCollideDist( &transStart, &transEnd, collideAgainst );
    //return -1;
}


dFloat CollisionDetection::rayCollideWithEnemy( Vector3 *start, Vector3 *end, Entity* collideAgainst )
{
    if( enemyEnt != NULL )
    {
        Matrix4 m4 = collideAgainst->getParentSceneNode()->_getFullTransform().inverse();
        Vector3 transStart = m4 * (*start);
        Vector3 transEnd = m4 * (*end);


        dFloat p0[3];
        dFloat p1[3];
        p0[0] = transStart.x;
        p0[1] = transStart.y;
        p0[2] = transStart.z;

        p1[0] = transEnd.x;
        p1[1] = transEnd.y;
        p1[2] = transEnd.z;

        dFloat normal[3];
        int att[1];

        return NewtonCollisionRayCast( enemyEnt, p0, p1, normal, att);
    }
    else return 1.2;
}

void CollisionDetection::createConvexHull( Entity *entity, bool enemy )
{
    size_t vertex_count;
	size_t index_count;
	Ogre::Vector3 *vertices;
	unsigned long *indices;

    GetMeshInformation(   entity->getMesh(), vertex_count, vertices, index_count, indices,
                          Vector3( 0.0, 0.0, 0.0 ),//entity->getParentNode()->getPosition(),
                          Ogre::Quaternion::IDENTITY, //entity->getParentNode()->getOrientation()
                          Ogre::Vector3(1,1,1));//entity->getParentNode()->_getDerivedScale() );//Ogre::Vector3(1,1,1));

    dFloat *vertexCloud = new dFloat[vertex_count*3];
    for( int i = 0; i < vertex_count; i++ )
    {
        vertexCloud[i*3] = vertices[i].x;
        vertexCloud[i*3+1] = vertices[i].y;
        vertexCloud[i*3+2] = vertices[i].z;
    }
    shapeID++;
    NewtonCollision *newCol = NewtonCreateConvexHull (newtonWorld, vertex_count, vertexCloud, 12, 0.0, shapeID, NULL);
    if(enemy)
    {
        enemyEnt = newCol;
    }
    else
    {
        NewtonBody* rigidBodyBox = NewtonCreateBody (newtonWorld, newCol);
        //PROBLEM with the line as this comand should work, but terminates the app;Possibly a c++ issue
        //NewtonReleaseCollision( newtonWorld, enemyCol);
        collisionsMap.insert(pair<Entity*,NewtonCollision*>(entity,newCol));
        bodysMap.insert(pair<Entity*,NewtonBody*>(entity,rigidBodyBox));
    }
}


/* void CollisionDetection::createShipMesh( Entity * e )
{
    NewtonCollision *shipCollision = NewtonCreateBox (newtonWorld, 8.0f, 8.0f, 8.0f,2, NULL);
    NewtonBody* rigidBodyBox = NewtonCreateBody (newtonWorld, shipCollision);
    NewtonReleaseCollision (newtonWorld, shipCollision);

    collisionsMap.insert(pair<Entity*,NewtonCollision*>(e,shipCollision));
 	bodysMap.insert(pair<Entity*,NewtonBody*>(e,rigidBodyBox));
} */


/*
void CollisionDetection::createEnemyMesh( Entity * e )
{
    NewtonCollision *shipCollision = NewtonCreateBox (newtonWorld, 8.0f, 8.0f, 8.0f,3, NULL);
    NewtonBody* rigidBodyBox = NewtonCreateBody (newtonWorld, shipCollision);
    NewtonReleaseCollision (newtonWorld, shipCollision);

    collisionsMap.insert(pair<Entity*,NewtonCollision*>(e,shipCollision));
 	bodysMap.insert(pair<Entity*,NewtonBody*>(e,rigidBodyBox));
} */

void CollisionDetection::GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                unsigned long* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        uint32_t*  pLong = static_cast<unsigned uint32_t*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        // Ogre 1.6 patch (commenting the static_cast...) - index offsets start from 0 for each submesh
        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] /*+ static_cast<unsigned long>(offset)*/;
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) /*+
                    static_cast<unsigned long>(offset)*/;
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

