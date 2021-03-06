#include "replicaObject.h"
#include "ReplicaManager3.h"
#include <iostream>

using namespace RakNet;

	void ReplicaObject::WriteAllocationID(RakNet::BitStream *allocationIdBitstream) const {
		allocationIdBitstream->Write(GetName());
	}

	void ReplicaObject::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{
	}

	bool ReplicaObject::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
		return true;
	}

	void ReplicaObject::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{
	}

	bool ReplicaObject::DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
		return true;
	}

	void ReplicaObject::DeallocReplica(RakNet::Connection_RM3 *sourceConnection) {
        delete this;
	}

	RM3SerializationResult ReplicaObject::Serialize(SerializeParameters *serializeParameters)	{
		return RM3SR_BROADCAST_IDENTICALLY;
	}

	void ReplicaObject::Deserialize(RakNet::DeserializeParameters *deserializeParameters) {

	}

	void ReplicaObject::SerializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{

	}

	void ReplicaObject::DeserializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *acceptingConnection) {

	}

	void ReplicaObject::SerializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{

	}

	void ReplicaObject::DeserializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *rejectingConnection) {

	}

	RM3ConstructionState ReplicaObject::QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_ClientConstruction(destinationConnection);
	}

	bool ReplicaObject::QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_ClientConstruction(sourceConnection);
	}

	RM3QuerySerializationResult ReplicaObject::QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_ClientSerializable(destinationConnection);
	}

	RM3ActionOnPopConnection ReplicaObject::QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_Client(droppedConnection);
	}
