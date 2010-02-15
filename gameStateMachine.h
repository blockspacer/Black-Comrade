
#ifndef GAME_STATE_MACHINE_H
#define GAME_STATE_MACHINE_H

#include "ITickable.h"
#include "mapManager.h"
#include "shipState.h"
#include "replicaObject.h"
#include "damageState.h"

enum GameState { GS_STEALTH, GS_ATTACK, GS_FLEE, GS_GAME_OVER, GS_END };

class GameStateMachine : public ITickable, public ReplicaObject
{
private:
	GameState gameState;
        GameState oldState;

	MapManager *mapManager;
	ShipState *shipState;
        DamageState *damageState;
	bool mIsNewState;

       void checkWaypoints();
       void checkHealth();
	
public:
        GameStateMachine();
	GameStateMachine(MapManager *mapManager, ShipState *shipState, DamageState *damageState);
	void tick();
	
	GameState currentGameState();
	bool isNewState();

        virtual RakNet::RakString GetName(void) const;
        virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters);
        virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters);
};

#endif
