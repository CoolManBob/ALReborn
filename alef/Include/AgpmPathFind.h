#ifndef __AGPMPATHFIND_H__
#define __AGPMPATHFIND_H__

#include <math.h>
#include "AgpdPathFind.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "SpecializeListPathFind.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmPathFindD" )
#else
#pragma comment ( lib , "AgpmPathFind" )
#endif
#endif

class AgpmPathFind : public ApModule {
private:
	ApmMap				*m_papmMap;
	ApMutualEx		m_Mutex;

public:

	CSpecializeListPathFind<int, AgpdPathFindPoint *>	m_cSearchList;
	AgpdPathFindBlock		**m_ppcPathFindBlock;

	AgpdPathFindPoint		m_cTargetPoint;
	AgpdPathFindPoint		m_cStartPoint;

//	int						m_aArraryX[8];
//	int						m_aArraryY[8];

	ApSafeArray<int, 8>		m_aArraryX;
	ApSafeArray<int, 8>		m_aArraryY;

	bool			m_bFound;

	int				m_iWidth;
	int				m_iHalfWidth;
	int				m_iHeight;
	int				m_iHalfHeight;
	unsigned int	m_iRevision;

	AgpmPathFind();
	~AgpmPathFind();

	void initBlock( int iHalfWidth, int iHalfHeight );
	void initSearchNode( int iNodeCount );
	void reset();
	void resetRevision();
	void resetBlock();
	void getPathToExternalList( CSpecializeListPathFind<int, AgpdPathFindPoint *> *pcPathNodeList );
	void initStartPoint();
	void initTargetPoint();
	bool pathFind( float fStartX, float fStartZ, float fEndX, float fEndZ, CSpecializeListPathFind<int, AgpdPathFindPoint *> *pcPathNodeList );
	bool pathFind();

	void lock();
	void unlock();

	BOOL OnAddModule();
};

#endif
