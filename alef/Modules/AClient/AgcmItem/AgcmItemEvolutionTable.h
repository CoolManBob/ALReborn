#ifndef __CLASS_AGCM_ITEM_EVOLUTION_TABLE_H__
#define __CLASS_AGCM_ITEM_EVOLUTION_TABLE_H__



#include "ContainerUtil.h"
#include "ApDefine.h"


struct stItemEvolutionEntry
{
	INT32											m_nScionTID;
	INT32											m_nSlayerTID;
	INT32											m_nOrbiterTID;
	INT32											m_nSummonerTID;

	stItemEvolutionEntry( void )
	{
		m_nScionTID = -1;
		m_nSlayerTID = -1;
		m_nOrbiterTID = -1;
		m_nSummonerTID = -1;
	}
};


class AgcmItemEvolutionTable
{
private :
	ContainerMap< INT32, stItemEvolutionEntry >		m_mapEvolutionTable;

public :
	AgcmItemEvolutionTable( void );
	~AgcmItemEvolutionTable( void );

public :
	BOOL			OnLoadTable						( char* pTableFileName );

public :
	INT32			GetEvolutionTID					( INT32 nCommonTID, AuCharClassType eClassType );

private :
	BOOL			_IsDigitString					( char* pString );
	BOOL			_ParseTableLine					( char* pBuffer, char* pSlayer, char* pOrbiter, char* pScion, char* pSummoner );
};



#endif