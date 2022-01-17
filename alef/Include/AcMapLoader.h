// AcMapLoader.h: interface for the AcMapLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_)
#define AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AuList.h"
#include "MagUnpackManager.h"

#define	ACMAPLOADER_MAX_QUEUE	512
#define	ACMAPLOADER_PACK_QUEUE	12

class AgcmMap		;
class ApWorldSector	;

class AcMapLoader  
{
protected:
	class	LoadBox
	{
	public:
		ApWorldSector	*	pSector	;
		INT32				nDetail	;	// 로드될 디테일..

		LoadBox() : pSector( NULL ) , nDetail( -1 ) {}
	};

	unsigned long 	m_hThread	;
	BOOL			m_bEnd		;

	//LoadBox			m_aSectors[ACMAPLOADER_MAX_QUEUE];
	
	//INT16			m_nIn		;
	//INT16			m_nOut		;

	AuList< LoadBox >	m_listSectorQueue;

	ApMutualEx		m_csMutex	;

	BOOL			DoExistSectorInfo( ApWorldSector * pSector, INT32 nDetail );
	BOOL			AddLoadSectorInfo( ApWorldSector * pSector, INT32 nDetail );

	// unpack block 설정..
	struct	UnpackBlock
	{
		CMagUnpackManager	*	pUnpackManager	;	// 언팩 메니져 포인터..
		INT32					nDetail			;	// Detail/Rough
		UINT					uBlockIndex		;	// 현재 로드중인 섹터 블럭..
		UINT					uLastAccessTime	;	// 마지막으로 억세스한시간..

		UnpackBlock():pUnpackManager(NULL),uBlockIndex(0),uLastAccessTime(0),nDetail(0) {}
		~UnpackBlock() { if( pUnpackManager ) delete pUnpackManager; }
	};

	UnpackBlock		m_aUnpackBlock[ ACMAPLOADER_PACK_QUEUE ];
	INT32			m_nUnpackCount							;//몇개 들어갔나.

public:
	CMagUnpackManager	*	GetUnpackManager( INT32 nDetail , UINT nBlockIndex , char * pFilename );
	void					FlushUnpackManager();
	
	AcMapLoader();
	virtual ~AcMapLoader();

	/*
	BOOL		StartThread();
	BOOL		StopThread();

	static VOID	Process(PVOID pvArgs);
	*/

	BOOL		LoadDWSector	( ApWorldSector * pvSector , INT32 nDetail = -1 );
	BOOL		RemoveQueue		( ApWorldSector * pvSector , INT32 nDetail = -1 );	// 해당 섹터의 로딩 큐를 모두 제거한다.
																					// nDetail 이 -1이면 모두제거, 디테일명시면 해당 디테일 제거.
	BOOL		OnIdleLoad		();

	BOOL		IsEmpty			();
	void		EmptyQueue		();	// 큐에 있는 로딩 내용을 삭제한다..
	INT32		GetWaitCount	(){ return m_listSectorQueue.GetCount(); };
};

#endif // !defined(AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_)
