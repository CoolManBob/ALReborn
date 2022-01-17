#ifndef __CLASS_GAME_FRAMEWORK_ARCHLORD_KOREA_H__
#define __CLASS_GAME_FRAMEWORK_ARCHLORD_KOREA_H__




#include "CGameFramework.h"



class CGameFramework_ArchlordKorea : public CGameFramework
{
private :
	
public :
	CGameFramework_ArchlordKorea( void );
	virtual ~CGameFramework_ArchlordKorea( void );

public :
	virtual BOOL	OnGameStart			( void );
	virtual BOOL	OnGameLoop			( void );
	virtual BOOL	OnGameEnd			( void );

public :
	virtual BOOL	ParseCommandLine	( void );
};




#endif