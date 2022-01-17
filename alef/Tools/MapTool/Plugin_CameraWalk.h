#if !defined(AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_)
#define AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Plugin_CameraWalk.h : header file
//

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "AgcuCamPathWork.h"
#include "AxisManager.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_CameraWalk window

#define CW_MAX_SPLINE_DISPLAY_VERTEX	1000

class CPlugin_CameraWalk : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_CameraWalk();

// Attributes
public:
	/*enum	CAMERA_WALKING_TYPE
	{
		CWT_AIRPLANE		,
		CWT_TOWARDTARGET	,
		CWT_TYPEMAX
	};
	*/

	// 각 컨트롤들의 id 부여
	enum	CONTROL_ID
	{
		CWCID_BUTTONSETTING				= 1001,
		CWCID_BUTTONADD					= 1002,
		CWCID_BUTTONREMOVE				= 1003,
		CWCID_BUTTONSETPLAYTIME			= 1004,
		CWCID_BUTTONPLAY				= 1005,
		CWCID_BUTTONREMOVEALL			= 1007,
		CWCID_BUTTONSAVE				= 1008,
		CWCID_BUTTONLOAD				= 1009,
		CWCID_MAX
	};

	// 각 컨트롤들의 클래스들..
	CButton		m_ctlButtonSetting		;

	CButton		m_ctlButtonAdd			;
	CButton		m_ctlButtonRemove		;
	CButton		m_ctlButtonSetPlayTime	;

	CButton		m_ctlButtonPlay			;
	CButton		m_ctlButtonRemoveAll	;
	CButton		m_ctlButtonSave			;
	CButton		m_ctlButtonLoad			;

	CRect		m_rectProgress			;

public:
	// 폴리건 정보..
	RpAtomic *	m_pSphereAtomic			;
	INT32		m_nSelectedPoint		;

	RpAtomic *	TranslateSphere( RwV3d * pPos );
	void		DrawSphere( RwV3d	* pPos );
	INT32		GetCollisionIndex( RsMouseStatus *ms );
	// -1 이 없는거..

	CAxisManager	m_cAxis;
public:
	// 재생관련 데이타들..
	BOOL		m_bPlay					;	// 플레이중인지..
	UINT32		m_uCurrent				;	// 현재 진행 양..


	AgcuPathWork *	PreparePathWork	( BOOL bTestMode = FALSE );
	AgcuPathWork *	PrepareAngleWork( BOOL bTestMode = FALSE );

	AgcuPathWork *	m_pPathWork			;	// 스플라인정보
	AgcuPathWork *	m_pAngleWork		;	// 앵글정보
	// x값을 앵글값으로 사용함.

	//RwMatrix		m_pCtrlMatrix	[ CW_MAX_SPLINE_NODE ];
	//INT32			m_nCtrlMatrixCount		;

	vector< RwMatrix >	m_vecCtrlMatrix;
	vector< FLOAT >		m_vecProjection;

	UINT32			m_uDuration				;	// 전체길이

	INT32			m_nType					;
	INT32			m_nSSType				;
	BOOL			m_bAccel				;
	BOOL			m_bClosed				;
	BOOL			m_bLoop					;

	BOOL			m_bPrevControlState		;

	RwIm3DVertex	m_pImVertex		[ CW_MAX_SPLINE_DISPLAY_VERTEX ];
	INT32			m_nImVertexCount;

	void		Stop		();
	void		RemoveAll	();
	BOOL		Save		();
	BOOL		Load		();

	void		UpdateVertex();

	// 성진아저씨 사용할 거리계산툴..
	FLOAT		m_fDistance	;	// 거리
	FLOAT		m_fTime		;	// 도달시간..
	UINT32		m_uRunSpeed	;	// 달리기 속도

	void		UpdateDistance( BOOL bForce = FALSE );

	INT32		m_nSelectedAxis		;
	FLOAT		m_fSelectedValue1	;
	FLOAT		m_fSelectedValue2	;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_CameraWalk)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlugin_CameraWalk();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_CameraWalk)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnIdle					( UINT32 ulClockCount );
	virtual BOOL OnWindowRender			();

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDurationSetDlg dialog

class CDurationSetDlg : public CDialog
{
// Construction
public:
	CDurationSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDurationSetDlg)
	enum { IDD = IDD_DURATION_SET_DIALOG };
	UINT	m_uDuration;
	BOOL	m_bType;		// 2004/02/14 안써요~
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDurationSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDurationSetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
// CCameraWalkingSettingDlg 대화 상자입니다.

class CCameraWalkingSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCameraWalkingSettingDlg)

public:
	CCameraWalkingSettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraWalkingSettingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CAMERAWALKSETTING };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_CameraWalk)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAccel	;
	BOOL m_bClosed	;
	BOOL m_bLoop	;

	INT32	m_nType	;
	INT32	m_nSSType;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_)