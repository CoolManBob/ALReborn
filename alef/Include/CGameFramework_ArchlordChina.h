#ifndef __CLASS_GAME_FRAMEWORK_ARCHLORD_CHINA_H__
#define __CLASS_GAME_FRAMEWORK_ARCHLORD_CHINA_H__




#include "CGameFramework.h"



class CGameFramework_ArchlordChina : public CGameFramework
{
private :
	
public :
	CGameFramework_ArchlordChina( void );
	virtual ~CGameFramework_ArchlordChina( void );

public :
	virtual BOOL	OnGameStart			( void );
	virtual BOOL	OnGameLoop			( void );
	virtual BOOL	OnGameEnd			( void );
};




#endif