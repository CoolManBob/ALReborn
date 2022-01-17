#if !defined(AFX_AGCDEFFTEX_H__EB8A6053_2F48_44B5_B3D8_B234DFCBC275__INCLUDED_)
#define AFX_AGCDEFFTEX_H__EB8A6053_2F48_44B5_B3D8_B234DFCBC275__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffUtil.h"
#include "ApMemory.h"
#include "rwcore.h"
#include <vector>
using std::vector;

#include "AgcuEff2ApMemoryLog.h"

//---------------------------- AgcdEffTex --------------------------
class AgcdEffTex : public ApMemory<AgcdEffTex, 300>
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffTex();
	AgcdEffTex(const RwChar* szTex, const RwChar* szMask=0, RwInt32 nIndex=0);
	virtual ~AgcdEffTex();

	RwInt32				bAddRef();
	RwInt32				bRelease();

	RwInt32				bLoadTex( void );

	const RwTexture*	bGetPtrTex		( VOID )const	{	return m_pTex;		}
	RwTexture*			bGetPtrTex		( VOID )		{	return m_pTex;		}
	RwInt32				bGetIndex		( VOID )const	{	return m_nIndex;	}
	const RwChar*		bGetTexName		( VOID )const	{	return m_szTex;		}
	const RwChar*		bGetMaskName	( VOID )const	{	return m_szMask;	}
	RwChar*				bGetTexName		( VOID )		{	return m_szTex;		}
	RwChar*				bGetMaskName	( VOID )		{	return m_szMask;	}

	void				bSetIndex		( RwInt32 nIndex )		{ m_nIndex = nIndex; };
	void				bSetTexName		( const RwChar* szTex );
	void				bSetMaskName	( const RwChar* szMask = NULL );
	void				bSetTexMaskName	( const RwChar* szTex, const RwChar* szMask=NULL );

	bool				bCmpFileName	(const RwChar* szTex, const RwChar* szMask);

	bool operator < ( const AgcdEffTex& cmp )const{ return (m_nIndex <  cmp.m_nIndex); };
	bool operator ==( const AgcdEffTex& cmp )const{ return (m_nIndex == cmp.m_nIndex); };
	
	RwInt32				bToFile(FILE* fp);
	RwInt32				bFromFile(FILE* fp);

private:
	AgcdEffTex(const AgcdEffTex& cpy);
	AgcdEffTex& operator = (const AgcdEffTex& cpy);

public:
	RwInt32		m_nIndex;
	RwChar		m_szTex[EFF2_FILE_NAME_MAX];
	RwChar		m_szMask[EFF2_FILE_NAME_MAX];
	RwInt32		m_nRefCnt;
	RwTexture*	m_pTex;
};
typedef AgcdEffTex EFFTEX, *PEFFTEX, *LPEFFTEX;

//---------------------------- AgcdEffTexInfo --------------------------
class AgcdEffTexInfo : public ApMemory<AgcdEffTexInfo, 800>
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffTexInfo();
	~AgcdEffTexInfo();

	const RwTexture* const	bGetPtrTex		( VOID ) const			{	return m_lpAgcdEffTex ? m_lpAgcdEffTex->bGetPtrTex() : NULL;	}
	RwTexture*				bGetPtrTex		( VOID )				{	return m_lpAgcdEffTex ? m_lpAgcdEffTex->bGetPtrTex() : NULL;	}
	RwInt32					bGetIndex		( VOID ) const			{	return m_nIndex;	}

	void					bSetIndex		( RwInt32 nIndex	)	{ m_nIndex = nIndex; };
	RwInt32					bSetPtrEffTex	( LPEFFTEX lpEffTex );

private:
	AgcdEffTexInfo(const AgcdEffTexInfo& cpy)				{cpy;};
	AgcdEffTexInfo& operator = (const AgcdEffTexInfo& cpy)	{cpy; return *this;};

public:
	RwInt32		m_nIndex;
	LPEFFTEX	m_lpAgcdEffTex;
};

#endif