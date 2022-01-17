
#include "MyEngine.h"
#include "UITileList_PluginBase.h"

#include "AgcmGrass.h"

// Maptool Grass Format
#define	GRASS_FILE_HEADER	0x10000009
#define	GRASS_FILE_VERSION	0x00000001

#define	GRASS_FILE_NAME	"MAP\\DATA\\GRASS\\GR%04d.MGF"

struct	stGrassSaveData	// ΩÍ¿Ã∫Í«“∂ß æ∏..
{
	INT32 nGrassID	;
	FLOAT fX		;
	FLOAT fZ		;
	FLOAT fXRot		;
	FLOAT fYRot		;
	FLOAT fScale	;

	stGrassSaveData():nGrassID( 0 ),fX( 0.0f ),fZ( 0.0f ),fXRot( 0.0f ),fYRot( 0.0f ),fScale( 0.0f ){}
};

/////////////////////////////////////////////////////////////////////////////
// CPlugin_Grass window

class CPlugin_Grass : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_Grass();

// Attributes
public:
	CTreeCtrl		m_TreeGrass			;

	CButton			m_buttonGrassSet	;

	CRect			m_rectAddGrass		;
	CRect			m_rectSelectGrass	;

	INT32			m_iCurWorkState		;					// 0 - √ﬂ∞° , 1 - º±≈√
	INT32			m_iCurSelectGrassID	;
	CString			m_strCurSelectGrass ;

	INT32			m_iCurSectorGrassNum;

	BOOL			m_bDrawGrass;
	INT32			m_iGrassDensity;
	INT32			m_iGrassRotMin;
	INT32			m_iGrassRotMax;
	INT32			m_iGrassRotYMin;
	INT32			m_iGrassRotYMax;
	FLOAT			m_fGrassScaleMin;
	FLOAT			m_fGrassScaleMax;

	BOOL			m_bValidPosition;
	RwV3d			m_ptSelectPos;
	FLOAT			m_fSelectRadius;

	BOOL			LoadGrassOctreeFile();

// Operations
public:
	virtual	BOOL OnSelectedPlugin		();
	virtual	BOOL OnDeSelectedPlugin		();
	
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual	BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );

	virtual BOOL OnWindowRender			();

	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );

	virtual BOOL OnLoadData				();
	virtual BOOL OnSaveData				();
	virtual BOOL OnQuerySaveData		( char * pStr );

	static BOOL	__DivisionLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	__DivisionSaveCallback ( DivisionInfo * pDivisionInfo , PVOID pData );

	BOOL	LoadGrass( DivisionInfo * pDivisionInfo );
	BOOL	SaveGrass( DivisionInfo * pDivisionInfo , char * pDestFolder = NULL );

	virtual void OnChangeSectorGeometry	( ApWorldSector * pSector );

	void	ChangeGrassSet();
	void	PositionValidation(RwV3d*	pPos);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_Grass)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlugin_Grass();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_Grass)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void	OnPaint();
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
