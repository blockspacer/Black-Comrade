#include "collaborationInfo.h"

using namespace std;

CollaborationInfo::CollaborationInfo(string nick, NetworkRole networkRole, GameRole gameRole) :
    nick(nick),
    networkRole(networkRole),
    gameRole(gameRole)
{
}

string CollaborationInfo::getNick() { return nick; }

NetworkRole CollaborationInfo::getNetworkRole() { return networkRole; }

GameRole CollaborationInfo::getGameRole() { return gameRole; }
