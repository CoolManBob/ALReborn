// MainWindow.h: interface for the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
#define AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "AgcEngine.h"
#include "rtimport.h"
#include "AgcdEffect.h"

class MainWindow : public AgcWindow  
{
public:
	INT16		m_nCharacter	;
	INT16		m_nPCType		;
	INT16		m_nArmorPart	;

	RpClump		*m_pSkyBox;
	FLOAT		m_fSpeed;

	// Magoja Test
	RwImage		*m_pBmp		;
	RwRaster	*m_pRaster	;

	INT32		m_nImageX;
	INT32		m_nImageY;

	BOOL			m_bLButtonDown		;
	BOOL			m_bRButtonDown		;
	RsMouseStatus	m_PrevMouseStatus	;
	RpAtomic		*m_pSelectedAtomic	;
	RwBBox			PickBox				;

	BOOL		m_bLCtrl				;
	BOOL		m_bRCtrl				;

	BOOL		m_bLShift				;

	RwFrame		*m_pPlayerFrame			;

	BOOL		m_bMove					;
	BOOL		m_bTurn					;

	FLOAT		m_fMoveDirection		;
	FLOAT		m_fTurnDegree			;

	FLOAT		m_fTurn					;
	FLOAT		m_fTilt					;

	UINT32		m_ulPrevTime			;

	FLOAT		m_fCameraPan			;
	FLOAT		m_fCameraPanInitial		;
	FLOAT		m_fCameraPanUnit		;

public:
	BOOL TransformWeapon(INT32 lCID, INT32 lType);
	void CameraTurn(FLOAT fDegree);
	BOOL InitCharacters();
	static RpCollisionTriangle *MainWindow::CollisionWorldFirstCallback(RpIntersection *pstIntersection,
							RpWorldSector *pstSector, 
							RpCollisionTriangle *pstCollTriangle,
							RwReal fDistance, 
							void *pvData);
	BOOL MovePlayer(RwFrame *pstCharacterFrame, FLOAT fTurn, UINT32 ulDeltaTime, BOOL bMove = TRUE, BOOL bTurn = TRUE, FLOAT fMoveDirection = 0);
	MainWindow();
	virtual ~MainWindow();

	BOOL OnInit			(					);	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseMove	( RsMouseStatus *ms	);
	BOOL OnLButtonUp	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	BOOL OnRButtonUp	( RsMouseStatus *ms	);
	BOOL OnMouseWheel	( BOOL bForward		);
	void OnPreRender	( RwRaster *raster	);	// 월드 렌더하기전.
	void OnRender		( RwRaster *raster	);	// 렌더는 여기서;
	void OnPostRender	( RwRaster *raster	);	// 렌더는 여기서;
	void OnClose		(					);	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
	BOOL OnKeyDown		( RsKeyStatus *ks	);
	BOOL OnKeyUp		( RsKeyStatus *ks	);
	BOOL OnIdle			(UINT32 ulClockCount);


	void HighlightRender( void				);

	void RenderAlphaSortedAtomics(void);
	void SetRenderCallBackForAllAtomic();
	void DestroyAlphaAtomicsList(void);
	static RwReal AtomicGetCameraDistance(RpAtomic *atomic);
	static void AtomicAddToSortedList(SortAtomic *entry);
	static RpAtomic* AtomicSetRenderCallback(RpAtomic *atomic, void *data);
	static RpAtomic* AlphaSortedAtomicRenderCallback(RpAtomic *atomic);
	static RpWorldSector* WorldSectorSetRenderCallBack( RpWorldSector* sector , void *data );

};

#endif // !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
