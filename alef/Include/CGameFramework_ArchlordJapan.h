#ifndef __CLASS_GAME_FRAMEWORK_ARCHLORD_JAPAN_H__
#define __CLASS_GAME_FRAMEWORK_ARCHLORD_JAPAN_H__




#include "CGameFramework.h"



class CGameFramework_ArchlordJapan : public CGameFramework
{
private :
	
public :
	CGameFramework_ArchlordJapan( void );
	virtual ~CGameFramework_ArchlordJapan( void );

public :
	virtual BOOL	OnGameStart			( void );
	virtual BOOL	OnGameLoop			( void );
	virtual BOOL	OnGameEnd			( void );
};




#endif