// AgpmPvPPacket.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 20.


#include "AgpmPvP.h"

// lWin, lLose, cPvPMode, cPvPStatus ´Â lCID ±âÁØÀÌ´Ù.
// cWin : TRUE(CID °¡ ½Â), FALSE(lTargetCID °¡ ½Â)
PVOID AgpmPvP::MakePvPPacket(INT16* pnPacketLength, INT8 cOperation, INT32* plCID, INT32* plTargetCID, INT32* plWin, INT32* plLose, 
							 INT8* pcPvPMode, INT8* pcPvPStatus, INT8* pcWin, CHAR* szGuildID, CHAR* szMessage)
{
	INT8 cType = AGPMPVP_PACKET_TYPE;
	return m_csPacket.MakePacket(TRUE, pnPacketLength, cType,
					&cOperation,
					plCID,
					plTargetCID,
					plWin,
					plLose,
					pcPvPMode,
					pcPvPStatus,
					pcWin,
					szGuildID,
					szMessage);
}

PVOID AgpmPvP::MakePvPInfoPacket(INT16* pnPacketLength, INT32* plCID, INT32* plWin, INT32* plLose, INT8* pcPvPMode, INT8* pcPvPStatus)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_PVP_INFO,
					plCID,
					NULL,
					plWin,
					plLose,
					pcPvPMode,
					pcPvPStatus,
					NULL, NULL,	NULL);
}

PVOID AgpmPvP::MakePvPResultPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT32* plWin, INT32* plLose,
								   INT8* pcPvPMode, INT8* pcPvPStatus, INT8* pcWin)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_PVP_INFO,
					plCID,
					plTargetCID,
					plWin,
					plLose,
					pcPvPMode,
					pcPvPStatus,
					pcWin,
					NULL, NULL);

}

PVOID AgpmPvP::MakeAddFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_ADD_FRIEND,
					plCID,
					plTargetCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeAddEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT8* pcPvPMode)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_ADD_ENEMY,
					plCID,
					plTargetCID,
					NULL, NULL,
					pcPvPMode,
					NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeRemoveFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_REMOVE_FRIEND,
					plCID,
					plTargetCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeRemoveEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_REMOVE_ENEMY,
					plCID,
					plTargetCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeInitFriendPacket(INT16* pnPacketLength, INT32* plCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_INIT_FRIEND,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeInitEnemyPacket(INT16* pnPacketLength, INT32* plCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_INIT_ENEMY,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeUpdateFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_UPDATE_FRIEND,
					plCID,
					plTargetCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeUpdateEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT8* pcPvPMode)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_UPDATE_ENEMY,
					plCID,
					plTargetCID,
					NULL, NULL,
					pcPvPMode,
					NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeAddFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_ADD_FRIEND_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeAddEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT8* pcPvPMode)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_ADD_ENEMY_GUILD,
					plCID,
					NULL, NULL, NULL,
					pcPvPMode,
					NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeRemoveFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_REMOVE_FRIEND_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeRemoveEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_REMOVE_ENEMY_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeInitFriendGuildPacket(INT16* pnPacketLength, INT32* plCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_INIT_FRIEND_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeInitEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_INIT_ENEMY_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeUpdateFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_UPDATE_FRIEND_GUILD,
					plCID,
					NULL, NULL, NULL, NULL, NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeUpdateEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT8* pcPvPMode)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_UPDATE_ENEMY_GUILD,
					plCID,
					NULL, NULL, NULL,
					pcPvPMode,
					NULL, NULL,
					szGuildID,
					NULL);
}

PVOID AgpmPvP::MakeCannotUseTeleportPacket(INT16 *pnPacketLength, INT32 lCID)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_CANNOT_USE_TELEPORT,
					&lCID,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeSystemMessagePacket(INT16* pnPacketLength, INT32* plCode, CHAR* szData1, CHAR* szData2, INT32* plData1, INT32* plData2)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_SYSTEM_MESSAGE,
					plCode,
					NULL,
					plData1,
					plData2,
					NULL, NULL, NULL,
					szData1, szData2);
}

// 2005.07.28. steeple
PVOID AgpmPvP::MakeRequestDeadTypePacket(INT16* pnPacketLength, INT32 lCID, INT32 lResurrectionType)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_REQUEST_DEAD_TYPE,
					&lCID,
					&lResurrectionType,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

// 2005.07.28. steeple
PVOID AgpmPvP::MakeResponseDeadTypePacket(INT16* pnPacketLength, INT32 lCID, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType)
{
	return MakePvPPacket(pnPacketLength,
					AGPMPVP_PACKET_RESPONSE_DEAD_TYPE,
					&lCID,
					&lTargetCID,
					&lDropExpRate,
					&lTargetType,
					NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmPvP::MakeRaceBattlePacket(INT16* pnPacketLength, INT8 cStatus, INT32 lRemained, INT32 lHumanPoint, INT32 lOrcPoint, INT32 lMoonelfPoint)
{
	return MakePvPPacket(pnPacketLength,
						 AGPMPVP_PACKET_RACE_BATTLE,
						 &lRemained,
						 &lHumanPoint,
						 &lOrcPoint,
						 &lMoonelfPoint,
						 NULL,
						 &cStatus,
						 NULL,
						 NULL,
						 NULL
						 );
}