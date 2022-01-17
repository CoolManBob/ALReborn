#ifndef		_AGCMBATTLEGROUND_H_
#define		_AGCMBATTLEGROUND_H_

#include "AgcModule.h"
#include "AgppBattleGround.h"

class AgpmBattleGround;
class AgpmFactors;
class AgpmCharacter;
class AgcmCharacter;
class AgpdCharacter;
class AgcmChatting2;
class AgcmUIManager2;
class AgcmUISiegeWar;	//메세지창을 공유한다

class AgcmBattleGround : public AgcModule
{
public:
	AgcmBattleGround();
	virtual ~AgcmBattleGround();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle( UINT32 ulClockCount );

	BOOL	IsBattleGournd( AgpdCharacter* pChar );
	BOOL	IsMyBattleGround();

	void	StartBattleGround();
	void	EndBattleGround();

	void	StartEventBattleGround();
	void	EndEventBattleGround();

	BOOL	IsOtherRace( AgpdCharacter* pOther );

	//start - update(time, monster spon) - result

	//Send Packet

	//Receive Packet

	//Callback

private:
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	void BattleGroundNotice(PACKET_BATTLEGROUND* pvPacket);
	void _BattleGroundEffectText( const char* szMessage );
	void _EpicZoneEffectText( const char* szMessage , DWORD dwColor );

private:
	AgpmBattleGround*	m_pcsAgpmBattleGround;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgpmFactors*		m_pcsAgpmFactors;
	AgcmChatting2*		m_pcsAgcmChatting;
	AgcmUIManager2*		m_pcsAgcmUIManager2;
	AgcmUISiegeWar*		m_pcsAgcmUISiegeWar;
};

#endif