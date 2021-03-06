
#include "stateUpdate.h"
#include "const.h"
#include "constManager.h"

#include <iostream>

StateUpdate::StateUpdate() 
    : timeSinceLastEvent(0)
    , count(0)
    , running(true)
    ,latestSlack(0)
    , fps(0)
{}

bool StateUpdate::frameRenderingQueued (const FrameEvent &evt)
{   
    if(running) {

        timeSinceLastEvent += evt.timeSinceLastFrame;
        
        if(timeSinceLastEvent > ConstManager::getFloat("tick_period")) {
        	std::cout << (timeSinceLastEvent - ConstManager::getFloat("tick_period")) << std::endl;
            timeSinceLastEvent = 0;
            tick();
        }
    }
    
    return running;
}

void StateUpdate::startLoop()
{
	WindowEventUtilities weu = WindowEventUtilities();
    Root *root = Root::getSingletonPtr();
    
    Timer timer;
    
    long oldtime;
    long newtime;
    long renderTime;
    long looptime;
    long render;
    long sleep;
    
    bool loop = true;
    while(loop) {
        //std::cout << timer.getMilliseconds() - oldtime << std::endl;
    	oldtime = timer.getMilliseconds();
    	loop = this->tick();
    	weu.messagePump();
        renderTime = timer.getMilliseconds();
    	root->renderOneFrame();
    	newtime = timer.getMilliseconds();
        render = newtime - renderTime;
        //std::cout << "Rendertime: " << render << std::endl;
    	looptime = newtime - oldtime;
    	sleep = (long)(1000 * ConstManager::getFloat("tick_period")) - looptime;
    	//sleep = (sleep > 0) ? sleep : 0;
    	latestSlack = sleep;
    	float time = sleep + looptime;
    	fps = (int)(1 / time);
    	while(timer.getMilliseconds() < newtime + sleep);
    }
}

void StateUpdate::addTickable(ITickable* t, string name)
{
    tickables.push_back(t);
    names.insert(pair<ITickable*,string>(t,name));
}

long StateUpdate::getSlack()
{
    return latestSlack;
}

int StateUpdate::getFps()
{
    return fps;
}

// Called once every TICK_PERIOD seconds
bool StateUpdate::tick() 
{
    Timer timer;

    for(std::vector<ITickable*>::const_iterator it=tickables.begin();it!=tickables.end();++it) {
        if (running == false) break;

        ITickable *t = *it;
        std::map<ITickable *,string>::const_iterator findname = names.find(t);
        string n = findname->second;

        // std::cout << n << ": ";
        long t1 = timer.getMilliseconds();
        t->tick();
        long t2 = timer.getMilliseconds();
        long ticktime = t2-t1;
        // std::cout << ticktime << "\n";
    }

	return running;
}
