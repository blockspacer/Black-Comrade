#include "bullet.h"

void Bullet::tick()
{

}

Bullet::Bullet(SceneNode *shipSceneNode,SceneManager *sceneMgr) :
    shipSceneNode(shipSceneNode),
    sceneMgr(sceneMgr)
{
    bulletNode = shipSceneNode->createChildSceneNode();
    particle = sceneMgr->createParticleSystem("Bullet", "Bullet");
    bulletNode->attachObject(particle);
}
