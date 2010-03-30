#ifndef _ENGINEER_CONTROLS_H 
#define _ENGINEER_CONTROLS_H

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include <iostream>
#include "ITickable.h"
#include "inputState.h"
#include "gunnerControls.h"

using namespace Ogre;
using namespace std;

class EngineerControls : public ITickable, public GunnerControls
{
    private:
       
       InputState *inputState;
       
        bool isChangeShield;
        bool isChangeWeapons;
        bool isTransferShields;
        bool isTransferWeapons;
  
    public:
        virtual void tick();
        virtual bool isShield();
        virtual bool isWeapons();
        virtual bool transferShields();
        virtual bool transferWeapons();

        EngineerControls(InputState *inputState, Camera *cam);
        ~EngineerControls();
};

#endif

