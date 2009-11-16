
#include "shipState.h"

#include <iostream>

ShipState::ShipState(SceneNode *shipSceneNode, IMotionState *motionState) :
    position(new Vector3(0.0,0.0,-500.0)),
    roll(0.0),
    pitch(0.0),
    yaw(0.0),
    shipSceneNode(shipSceneNode),
    motionState(motionState)
{}

void ShipState::tick()
{
    position->x += motionState->xVelocity();
    position->y += motionState->yVelocity();
    position->z += motionState->zVelocity();
    
    roll = motionState->roll();
    pitch = motionState->pitch();
    yaw = motionState->yaw();
    
    updateOgre();
}

void ShipState::updateOgre()
{
    //std::cout << position->x << "," << position->y << "," << position->z << std::endl;

	shipSceneNode->resetOrientation();

    shipSceneNode->setPosition(*position);
    
    Radian roll(this->roll);
    Radian pitch(this->pitch);
    Radian yaw(this->yaw);
    
    shipSceneNode->yaw(yaw);
    shipSceneNode->roll(roll);
    shipSceneNode->pitch(pitch);
        
    //std::cout << shipSceneNode->getPosition().z << std::endl;
}

	RakNet::RakString ShipState::GetName(void) const {return RakNet::RakString("ShipState");}

	void ShipState::WriteAllocationID(RakNet::BitStream *allocationIdBitstream) const {
		allocationIdBitstream->Write(GetName());
	}

	RM3SerializationResult ShipState::Serialize(SerializeParameters *serializeParameters)	{
		serializeParameters->outputBitstream[0].Write(position->x);
		serializeParameters->outputBitstream[0].Write(position->y);
		serializeParameters->outputBitstream[0].Write(position->z);
		serializeParameters->outputBitstream[0].Write(roll);
		serializeParameters->outputBitstream[0].Write(pitch);
		serializeParameters->outputBitstream[0].Write(yaw);

		return RM3SR_BROADCAST_IDENTICALLY;
	}

	void ShipState::Deserialize(RakNet::DeserializeParameters *deserializeParameters) {
		deserializeParameters->serializationBitstream[0].Read(position->x);	
		deserializeParameters->serializationBitstream[0].Read(position->y);	
		deserializeParameters->serializationBitstream[0].Read(position->z);	
		deserializeParameters->serializationBitstream[0].Read(roll);
		deserializeParameters->serializationBitstream[0].Read(pitch);
		deserializeParameters->serializationBitstream[0].Read(yaw);

		print();
	}

	void ShipState::print() {
		printf("ShipState is now (X:%.1f Y:%.1f Z:%.1f), (R:%.1f P:%.1f Y:%.1f)\n", position->x, position->y, position->z, roll, pitch, yaw);
	}

	ShipState::ShipState()
       {
		position = new Vector3(0.0,0.0,-500.0);
		roll=0.0;
		pitch=0.0;
		yaw=0.0;
	}

