// ToolOption.h: interface for the CToolOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLOPTION_H__E19FDDEA_0EF7_4DAB_A7A7_1DCCB42C299E__INCLUDED_)
#define AFX_TOOLOPTION_H__E19FDDEA_0EF7_4DAB_A7A7_1DCCB42C299E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./XTPPropertyGridItemCustum/XTPGI_EffSet.h"
#include "./XTPPropertyGridItemCustum/XTPGI_UINT.h"

#include "AgcdEffPublicStructs.h"

class CDlgGridToolOption;
class CToolOption  
{
	friend class CDlgGridToolOption;

public:
	enum	ELBTNACT
	{
		e_lbtn_camrot	= 0,
		e_lbtn_movefrmOnVirtualPlane,
		e_lbtn_movefrmOnField,
		e_lbtn_frmrot,
		e_lbtn_targetting,
		e_lbtn_actnum,
	};

	const	TCHAR*const*const	STRELBTNACT;

private:
	CToolOption();

public:
	virtual ~CToolOption();

public:
	static CToolOption& bGetInst(void);

	//Get
	BOOL	bShowGrid			(void)const { return m_bShowGrid			;};
	BOOL	bShowWorldAxis		(void)const { return m_bShowWorldAxis		;};
	BOOL	bShowWorldAxisCone	(void)const { return m_bShowWorldAxisCone	;};
	BOOL	bShowCameraAxis		(void)const { return m_bShowCameraAxis		;};
	BOOL	bShowField			(void)const { return m_bShowField			;};
	BOOL	bShowFieldWire		(void)const { return m_bShowFieldWire		;};
	BOOL	bShowFramesAxis		(void)const { return m_bShowFramesAxis		;};
	BOOL	bShowFrame			(void)const { return m_bShowFrame			;};

	BOOL	bShowEffFrame		(void)const { return m_bShowEffFrame		;};
	BOOL	bShowOnlyOneEff		(void)const { return m_bShowOnlyOneEff		;};

	ELBTNACT bGetLBtnAct		(void)const { return m_eLBtnAct				;};
			
	//caption
	CString	bCaptionShowGrid			(void)const { return _T("Grid"); };
	CString	bCaptionShowWorldAxis		(void)const { return _T("WorldAxis"); };
	CString	bCaptionShowWorldAxisCone	(void)const { return _T("WorldAxisCone"); };
	CString	bCaptionShowCameraAxis		(void)const { return _T("CameraAxis"); };
	CString	bCaptionShowField			(void)const { return _T("Field"); };
	CString	bCaptionShowFieldWire		(void)const { return _T("FieldWire"); };
	CString	bCaptionShowFramesAxis		(void)const { return _T("FramesAxis"); };
	CString	bCaptionShowFrame			(void)const { return _T("Frame"); };

	CString	bCaptionShowEffFrame		(void)const { return _T("EffFrame"); };

	CString bCaptionShowOnlyOneEff		(void)const { return _T("OnlyOne"); };
	CString	bCaptionLBtnAct				(void)const { return _T("LBtnAct"); };
	CString	bCaptionELBtnAct			(void)const { return STRELBTNACT[m_eLBtnAct]; };

private:
	ELBTNACT	m_eLBtnAct;

	BOOL		m_bShowGrid;
	BOOL		m_bShowWorldAxis;
	BOOL		m_bShowWorldAxisCone;
	BOOL		m_bShowCameraAxis;
	BOOL		m_bShowField;
	BOOL		m_bShowFieldWire;
	BOOL		m_bShowFramesAxis;
	BOOL		m_bShowFrame;
	BOOL		m_bShowEffFrame;
	BOOL		m_bShowOnlyOneEff;
};

class CGlobalVar
{
	friend class CDlgGridToolOption;

public:
	enum	E_FRAME
	{
		e_frm_blue			= 0,
		e_frm_red			,
		e_frm_yeloow		,

		e_frm_clump			,

		e_frm_num			,
	};

	const	TCHAR*const*const	STREFRM;

private:
	BOOL			m_bInitEngin;
	stEffUseInfo	m_stEffUseInfo;

	E_FRAME			m_eFrmParent;
	E_FRAME			m_eFrmTarget;
	RwChar			m_szClump[MAX_PATH];
	RpClump*		m_pClump;
	ShowFrmVec		m_showFrmContainer;

	struct stSelectUV
	{
		stSelectUV() : m_hWnd(NULL), m_pTex(NULL), m_nRow(1), m_nCol(1), m_nSelRow(1), m_nSelCol(1)		{	};
		VOID Clear()	{	m_hWnd = NULL; m_pTex = NULL; m_nRow=m_nCol=m_nSelRow=m_nSelCol=1;	};

		HWND		m_hWnd;
		RwTexture*	m_pTex;
		INT			m_nRow;
		INT			m_nCol;
		INT			m_nSelRow;
		INT			m_nSelCol;

	}m_stSelectUVInfo;

	CGlobalVar();
public:
	//Get
	BOOL			bIsInitEngin(void)const { return m_bInitEngin; };
	LPSTEFFUSEINFO	bGetPtrEffUseInfo(void)	{ return &m_stEffUseInfo; };
	stSelectUV*		bGetPtrSelectUV(void)	{ return &m_stSelectUVInfo; };
	RwFrame*		bGetPtrFrame(E_FRAME efrm) { return m_showFrmContainer.at(efrm)->bGetPtrFrm(); };
	RpClump*		bGetPtrEmiterClump()	{ return m_pClump; };

	//Set
	void			bSetInitEngin(BOOL bInit) { m_bInitEngin	= bInit; };

	INT				bOnEngineInitAccessory();
	INT				bOnEngineDestroyAccessory();
	INT				bRenderShowFrm();
	ShowFrm*		bPickShowFrm(POINT ptMouse);
	INT				bSetShowFrmHeight(CEffUt_HFMap<VTX_PNDT1>& heightmap);
	INT				bSetShowFrmScale(RwReal scale);

	INT				bUpdateUseInfo(RwUInt32 ulEffID);

private:
	INT				vInitShowFrm();
	INT				vInitShowClump();
	
public:
	~CGlobalVar();
	static CGlobalVar& bGetInst(void);
};

#include "AgcdEffSet.h"
class AgcdEffSet;
class AgcdEffTex;
class AgcdEffBase;
struct AgcdEffSet::stBaseDependancy;

namespace NSUtFn //NameSpaceUtilFunction
{
	class	CSelectedInfo
	{
		AgcdEffSet*			m_lpCurrSelectedEffSet;
		AgcdEffBase*		m_lpCurrSelectedEffBase;
		AgcdEffAnim*		m_lpCurrSelectedEffAnim;

		//texture	
		RwChar				m_szTex[EFF2_FILE_NAME_MAX];
		RwChar				m_szMask[EFF2_FILE_NAME_MAX];
		CXTPGI_FileName*	m_pCXTPGI_Tex;
		CXTPGI_FileName*	m_pCXTPGI_Mask;
		CXTPGI_FileName*	m_pCXTPGI_Clump;
		CXTPGI_FileName*	m_pCXTPGI_Spline;
		
		CSelectedInfo();
	public:
		~CSelectedInfo();
		static CSelectedInfo& bGetInst();

		void	bOnSelEffSet(AgcdEffSet* pEffSet);
		void	bOnSelEffBase(AgcdEffBase* pEffBase);
		void	bOnSelEffAnim(AgcdEffAnim* pEffAnim);

		void	bOnChangedTex(void);
		void	bOnChangedClump(void);
		void	bOnChangedSpline(void);

		void	bDelAnimItem(RwUInt32 ulTime);

		//etc		
		void	bOnAddCTXPGI_TexMask( CXTPGI_FileName* pCXTPGI_Tex, CXTPGI_FileName* m_pCXTPGI_Mask );
		void	bOnAddCTXPGI_Clump( CXTPGI_FileName* pCXTPGI_Clump );
		BOOL	bFind_CXTPGI_TexMask( CXTPGI_FileName* pCXTPGI_Tex );
		BOOL	bFind_CXTPGI_TexClump( CXTPGI_FileName* pCXTPGI_Clump );
		BOOL	bFind_CXTPGI_Spline( CXTPGI_FileName* pCXTPGI_Spline );

		RwChar*	bGetPtrStrTex() { return m_szTex; };
		RwChar*	bGetPtrStrMask() { return m_szMask; };
		AgcdEffBase*
			bGetPtrSelectedBase() { return m_lpCurrSelectedEffBase; };

		void	bDelEffBaseDpnd(INT delBaseDpndIndex);
	};
};

#endif // !defined(AFX_TOOLOPTION_H__E19FDDEA_0EF7_4DAB_A7A7_1DCCB42C299E__INCLUDED_)
