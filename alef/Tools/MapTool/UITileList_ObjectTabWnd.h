#if !defined(AFX_UITILELIST_OBJECTTABWND_H__0152A0C4_99C4_42FD_AC33_D483CF4D0A81__INCLUDED_)
#define AFX_UITILELIST_OBJECTTABWND_H__0152A0C4_99C4_42FD_AC33_D483CF4D0A81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_ObjectTabWnd.h : header file
//

#include "rwcore.h"
#include "rpworld.h"

#include "ApDefine.h"
#include "ApmObject.h"
#include "ProgressDlg.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CUITileList_ObjectTabWnd window

#define OBJECT_FILE		"Ini\\objectlist.txt"

typedef struct stObjectEntry
{
	unsigned long tid;
	char name[256];
	char file[256];
	RwRGBA stPreLitLum;
	RpClump *pClump;
} stObjectEntry;

#define	OBJECT_BUTTON_HEIGHT	20

class CUITileList_ObjectTabWnd : public CWnd
{
// Construction
public:
	CUITileList_ObjectTabWnd();

	enum
	{
		ID_SNAPTOGRID	,
		ID_SNAPTOHEIGHT	,
		ID_MOVEXAXIS	,
		ID_MOVEZAXIS	,
		ID_ROTATEXAXIS	,
		ID_USE_BRUSHDRAWING	,
		ID_COUNT
	};

	char	m_strSubDataMainDir[ MAX_PATH ];

// Attributes
public:
	int			m_nMaxOID		;
	int			m_nMaxTID		;
	CTreeCtrl	m_wndTreeCtrl	;

	// 오브젝트 변형 옵션들..
	BOOL		m_bSnapToGrid	;
	BOOL		m_bSnapToHeight	;
	BOOL		m_bMoveXAxis	;
	BOOL		m_bMoveZAxis	;
	BOOL		m_bRotateXAxis	;
	BOOL		m_bUseBrush		;

	CButton		m_pctlButton[ ID_COUNT ];
	
	FLOAT		m_fDensity		;	// 타일당 덴져티..
	FLOAT		m_fRotateMin	;
	FLOAT		m_fRotateMax	;
	FLOAT		m_fRotateYMin	;
	FLOAT		m_fRotateYMax	;
	FLOAT		m_fScaleMin		;
	FLOAT		m_fScaleMax		;

	FLOAT		GetDensity	()	{ return m_fDensity		; }
	FLOAT		GetRotateXMin()	{ return m_fRotateMin	; }
	FLOAT		GetRotateXMax()	{ return m_fRotateMax	; }
	FLOAT		GetRotateYMin()	{ return m_fRotateMin	; }
	FLOAT		GetRotateYMax()	{ return m_fRotateMax	; }
	FLOAT		GetScaleMin	()	{ return m_fScaleMin	; }
	FLOAT		GetScaleMax	()	{ return m_fScaleMax	; }

	FLOAT		GetRotateX()	{ return m_fRotateMin	+ ( m_fRotateMax	- m_fRotateMin	) * frand(); }
	FLOAT		GetRotateY()	{ return m_fRotateYMin	+ ( m_fRotateYMax	- m_fRotateYMin	) * frand(); }
	FLOAT		GetScale()		{ return m_fScaleMin	+ ( m_fScaleMax		- m_fScaleMin	) * frand(); }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_ObjectTabWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	class CCategory
	{
	public:
		CString		str;
		HTREEITEM	pos;
	};
	vector< CCategory >	m_listCategory;

	BOOL		IsObjectSelected	();

	BOOL		LoadCategory		();

	INT32		GetObjectTID		();
	BOOL		DeleteObject		( RpClump *pClump);
	BOOL		DeleteTemplate		( int nTID);
	ApdObjectTemplate *	EditTemplate		( int nTID, const char *name, const char *file, RwRGBA *pstColor, int nBlockingType = -1);
	ApdObjectTemplate *	AddTemplate			( const char *name, const char *file, RwRGBA *pstColor, int nBlockingType, HTREEITEM item );
	BOOL		TransformObject		( RpClump *pClump, RsMouseStatus *pstCurrMouse , FLOAT fDeltaX , FLOAT fDeltaY , RwV3d *pos, int flag );
	ApdObject *	AddObject			( RwV3d *pos , UINT32 uAddtionalFlag = 0 , FLOAT fDegreeY = 0.0f );
	ApdObject *	AddObject			( ApdObjectTemplate	* pstApdObjectTemplate , RwV3d *pos , UINT32 uAddtionalFlag = 0 , FLOAT fDegreeY = 0.0f );
	HTREEITEM	SearchItemText		( const char *strName, HTREEITEM root = TVI_ROOT);
	static BOOL	TemplateCreateCB	( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	ObjectCreateCB		( PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL		EditEntry			( HTREEITEM item, const char *name, const char *file );
	BOOL		DeleteEntry			( HTREEITEM item );
	HTREEITEM	AddEntry			( unsigned long tid, const char *name, const char *file, RpClump *pClump, HTREEITEM entry , RwRGBA *pstColor );
	//RpClump *	LoadDFF				( const char *file );
	BOOL		SaveScript			( BOOL bTemplateSave = TRUE , BOOL bScriptSave = TRUE );
	BOOL		SaveTree			( FILE *pFile, HTREEITEM root, int level);

	void		LoadScript			( );
	BOOL		LoadLocalObjectInfo	();
	
	BOOL		LoadOldObjectScript	();
	virtual ~CUITileList_ObjectTabWnd();

	static RpAtomic *	GetClumpInfo(RpAtomic *pstAtomic, PVOID pvData);
	static RpAtomic *	SetClumpInfo(RpAtomic *pstAtomic, PVOID pvData);
	static RpMaterial *	SetMaterialAlpha(RpMaterial *pstMaterial, PVOID pvData);

	/*
	static RpMaterial *	GetAmbientInfo(RpMaterial *pstMaterial, PVOID pvData);
	*/
	static RpMaterial *	SetAmbient(RpMaterial *pstMaterial, PVOID pvData);

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_ObjectTabWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnObjectAddentry();
	afx_msg void OnObjectDeleteentry();
	afx_msg void OnObjectEditentry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnObjectpopupTaketemplateinfo();
};

struct DivisionInfo;

struct DivisionCallbackResult
{
	INT32	nDivisionCount	;
	INT32	nSuccessCount	;
	CProgressDlg dlgProgress;

	DivisionCallbackResult():nDivisionCount( 0 ), nSuccessCount( 0 ){}
};
BOOL	__DivisionObjectLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData );


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_OBJECTTABWND_H__0152A0C4_99C4_42FD_AC33_D483CF4D0A81__INCLUDED_)

/*
// 마고자 (2004-02-09 오후 4:18:16) : 
	오브젝트 분리에 관한정의들.

  오브젝트는 
*/
