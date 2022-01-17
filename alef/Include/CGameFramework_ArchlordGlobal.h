#ifndef __CLASS_GAME_FRAMEWORK_ARCHLORD_GLOBAL_H__
#define __CLASS_GAME_FRAMEWORK_ARCHLORD_GLOBAL_H__




#include "CGameFramework.h"



class CGameFramework_ArchlordGlobal : public CGameFramework
{
private :
	
public :
	CGameFramework_ArchlordGlobal( void );
	virtual ~CGameFramework_ArchlordGlobal( void );

public :
	virtual BOOL	OnGameStart			( void );
	virtual BOOL	OnGameLoop			( void );
	virtual BOOL	OnGameEnd			( void );
};




#endif