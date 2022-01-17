#pragma once

#include "ApBase.h"
#include "ApModule.h"

enum AgcmBlockingDlgCallbackPoint
{
	AGCMBLOCKINGDLG_CB_ID_INIT_BLOCKING					= 0,
	AGCMBLOCKINGDLG_CB_ID_FINISH_UP_EDITING_BLOCKING	= 1,
	AGCMBLOCKINGDLG_CB_ID_NUM
};

class AFX_EXT_CLASS AgcmBlockingDlg : public ApModule
{
public:
	AgcmBlockingDlg();
	virtual ~AgcmBlockingDlg();

	static AgcmBlockingDlg *GetInstance();

public:
	BOOL			Open(AuBLOCKING *pstOutBlocking, INT16 *pnBlockingNum, INT16 nMaxBlocking, PVOID pvFinishUpCBData);
	BOOL			OpenEditBlocking(BOOL bChangeType = FALSE);
	BOOL			OpenInsertBlocking(UINT16 unNewIndex);	
	BOOL			SetNewBlocking(INT16 nCopyIndex = -1);
	BOOL			DeleteBlocking(INT16 nDeleteIndex);

	BOOL			EditIsNone(); // 사용자 정의
	BOOL			EditIsScale();
	BOOL			EditIsTranslate();

	BOOL			EditIsRight();
	BOOL			EditIsUp();
	BOOL			EditIsAt();
	BOOL			EditIsAll();
	BOOL			EditIsHeight();

	BOOL			IsUpdateClick();
	void			ClearEdit();
	void			GetVector( float& fX, float& fY, float& fZ );

	BOOL			IsOpenEditBlocking()	{	return m_bOpenEditBlocking;	}
	AuBLOCKING*		GetBlocking()			{	return m_pstBlocking;		}
	AuBLOCKING*		GetCurrentBlocking()	{	return m_pstBlocking + m_nCurBlockingIndex;	}
	INT16			GetCurBlockingIndex()	{	return m_nCurBlockingIndex;	}	
	INT16			GetMaxBlocking()		{	return m_nMaxBlocking;		}
	INT16			GetBlockingNum()		{	return *(m_pnBlockingNum);	}

	void			ScaleBlock( const float& fScale );							// y / 10 * m_csEditOffset.m_fScale
	void			ScaleBlock( const RwV3d& vScale );
	void			TransBlock( const RwV2d& vec, const RwCamera* pCamera );	// y * m_csEditOffset.m_fPosit
	void			TransBlock( const RwV3d& vTrans, BOOL bOffset );
	BOOL			UpdateBlock( const RwV3d& vec );
	BOOL			UpdateBlock( const RwV2d& vec, const RwCamera* pCamera );

public:
	BOOL	OnInit();
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	BOOL	SetCallbackInitBlocking(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackFinishUpEditingBlocking(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	VOID	EnumCallbackInitBlocking(PVOID pData);

	// 내부적으로 쓰인당께~
	VOID			SetBlockingType(UINT16 unType);
	VOID			SetCurrentBlockingIndex(INT16 nIndex) {m_nCurBlockingIndex = nIndex;}
	VOID			CloseEditBlocking();

protected:
	BOOL			m_bOpenEditBlocking;
	AuBlockingType	m_eBlockingType;

	AuBLOCKING		*m_pstBlocking;
	INT16			*m_pnBlockingNum;
	INT16			m_nCurBlockingIndex;
	INT16			m_nMaxBlocking;
	PVOID			m_pvFinishUpCBData;
};