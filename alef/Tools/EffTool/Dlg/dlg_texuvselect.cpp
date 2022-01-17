#include "stdafx.h"
#include "..\EffTool.h"
#include "dlg_texuvselect.h"
#include "../ToolOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlg_TexUVSelect::CDlg_TexUVSelect(RwTexture* pTex, CWnd* pParent /*=NULL*/) : CDialog(CDlg_TexUVSelect::IDD, pParent), 
 m_pTex(pTex),
 m_bInit(FALSE),
 m_fLeft(0.f),
 m_fTop(0.f),
 m_fRight(1.f),
 m_fBottom(1.f),
 m_bMultiInput(FALSE),
 m_nTimeDistance(0),
 m_nBeginIndex(0),
 m_nEndIndex(0)
{
	m_nCol		= 1;
	m_nRow		= 1;
	m_nSelCol	= 1;
	m_nSelRow	= 1;
	m_nMaxCol	= 1;
	m_nMaxRow	= 1;
	if( m_pTex )
	{
		m_nMaxCol = RwRasterGetWidth(RwTextureGetRaster(m_pTex)) / 8;
		m_nMaxRow = RwRasterGetHeight(RwTextureGetRaster(m_pTex)) / 8;
	}
}

void CDlg_TexUVSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_ROW, m_ctrlSpinRow);
	DDX_Control(pDX, IDC_SPIN_COLUMN, m_ctrlSpinCol);
	DDX_Text(pDX, IDC_EDIT_COL, m_nCol);
	DDX_Text(pDX, IDC_EDIT_ROW, m_nRow);
	DDX_Text(pDX, IDC_EDIT_TIMEDISTANCE, m_nTimeDistance);
	DDX_Text(pDX, IDC_EDIT_BEGIN_INDEX, m_nBeginIndex);
	DDX_Text(pDX, IDC_EDIT_END_INDEX, m_nEndIndex);
	DDV_MinMaxUInt(pDX, m_nCol, 1, 32);
	DDV_MinMaxUInt(pDX, m_nRow, 1, 32);
	DDV_MinMaxUInt(pDX, m_nCol, 1, m_nMaxCol);
	DDV_MinMaxUInt(pDX, m_nRow, 1, m_nMaxRow);
	DDV_MinMaxUInt(pDX, m_nTimeDistance, 0, 100000);
	DDV_MinMaxUInt(pDX, m_nBeginIndex, 0, 100);
	DDV_MinMaxUInt(pDX, m_nEndIndex, 0, 100);
	DDX_Check(pDX, IDC_CHECK_MULTI, m_bMultiInput);
}

BEGIN_MESSAGE_MAP(CDlg_TexUVSelect, CDialog)
	//{{AFX_MSG_MAP(CDlg_TexUVSelect)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_COL, OnChangeEditCol)
	ON_EN_CHANGE(IDC_EDIT_ROW, OnChangeEditRow)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHECK_MULTI, OnBnClickedCheckMulti)
	ON_EN_CHANGE(IDC_EDIT_BEGIN_INDEX, OnEnChangeEditBeginIndex)
	ON_EN_CHANGE(IDC_EDIT_END_INDEX, OnEnChangeEditEndIndex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TexUVSelect message handlers
BOOL CDlg_TexUVSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if( !m_pTex )		return TRUE;
		
	m_ctrlSpinCol.SetRange(1, m_nMaxCol);
	m_ctrlSpinRow.SetRange(1, m_nMaxRow);

	if( GetDlgItem(IDC_EDIT_TIMEDISTANCE) )
		GetDlgItem(IDC_EDIT_TIMEDISTANCE)->EnableWindow( m_bMultiInput );
	if( GetDlgItem(IDC_EDIT_BEGIN_INDEX) )
		GetDlgItem(IDC_EDIT_BEGIN_INDEX)->EnableWindow( m_bMultiInput );
	if( GetDlgItem(IDC_EDIT_END_INDEX) )
		GetDlgItem(IDC_EDIT_END_INDEX)->EnableWindow( m_bMultiInput );

	CWnd*	pWnd_Rect	= GetDlgItem(IDC_STATIC_TEX);
	if( !pWnd_Rect )	return TRUE;

	CSize	sizeImg( RwRasterGetWidth(RwTextureGetRaster(m_pTex))*2, RwRasterGetHeight(RwTextureGetRaster(m_pTex))*2 );

	CRect	rcWnd;
	GetWindowRect(&rcWnd);
	CRect	rcRect;
	pWnd_Rect->GetWindowRect(&rcRect);
	CSize	sizeOffset( rcWnd.Width()-rcRect.Width(), rcWnd.Height()-rcRect.Height() );

	ScreenToClient(&rcRect);
	if( rcRect.Width() < sizeImg.cx )
	{
		rcWnd.right += sizeImg.cx - rcRect.Width();
	}

	rcWnd.bottom	+= sizeImg.cy - rcRect.Height();

	rcRect.right	= rcRect.left + sizeImg.cx;
	rcRect.bottom	= rcRect.top + sizeImg.cy;

	MoveWindow(&rcWnd);
	pWnd_Rect->MoveWindow(&rcRect);

	CGlobalVar::bGetInst().bGetPtrSelectUV()->m_hWnd = pWnd_Rect->GetSafeHwnd();
	CGlobalVar::bGetInst().bGetPtrSelectUV()->m_pTex = m_pTex;
	CGlobalVar::bGetInst().bGetPtrSelectUV()->m_nCol = m_nCol;
	CGlobalVar::bGetInst().bGetPtrSelectUV()->m_nRow = m_nRow;

	SetTimer( 0, 10, NULL );
	
	return m_bInit = TRUE;
}

void CDlg_TexUVSelect::OnOK()
{
	CGlobalVar::bGetInst().bGetPtrSelectUV()->Clear();
	CDialog::OnOK();
};

void CDlg_TexUVSelect::OnCancel()
{
	CGlobalVar::bGetInst().bGetPtrSelectUV()->Clear();
	CDialog::OnCancel();
};

void CDlg_TexUVSelect::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here	
	CGlobalVar::bGetInst().bGetPtrSelectUV()->Clear();
	KillTimer(0);
}

void CDlg_TexUVSelect::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd*	pWnd_Rect	= GetDlgItem(IDC_STATIC_TEX);
	if( !pWnd_Rect )		return;

	float fWidth = CRenderWare::GetInst().GetViewpotWidth();
	float fHeight = CRenderWare::GetInst().GetViewpotHeight();
	g_MyEngine.bRenderTexForUVSelect(  pWnd_Rect->GetSafeHwnd(), m_pTex, fWidth, fHeight, m_nRow, m_nCol, m_nSelRow, m_nSelCol );
	//CRenderWare::GetInst().RenderOutLine( m_pTex, fWidth, fHeight, m_nRow, m_nCol, m_nSelRow, m_nSelCol );
	
	CDialog::OnTimer(nIDEvent);
}

void CDlg_TexUVSelect::OnChangeEditCol() 
{
	// TODO: Add your control notification handler code here
	if( !m_bInit )		return;
		
	UpdateData();

	vChkValidIndex();
}

void CDlg_TexUVSelect::OnChangeEditRow() 
{
	// TODO: Add your control notification handler code here
	if( !m_bInit )		return;
		
	UpdateData();

	vChkValidIndex();
}

void CDlg_TexUVSelect::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd*	pWnd_Rect	= GetDlgItem(IDC_STATIC_TEX);
	if( !pWnd_Rect )
		return;

	CPoint	cpt(0,0);
	::GetCursorPos(&cpt);
	pWnd_Rect->ScreenToClient(&cpt);
	
	CRect	crc;
	pWnd_Rect->GetClientRect(&crc);

	if( !crc.PtInRect( cpt ) )	return;

	m_nSelCol = cpt.x / (crc.Width()/m_nCol) + 1;
	m_nSelRow = cpt.y / (crc.Height()/m_nRow) + 1;

	float	fstepx	= 1.f / (float)(m_nCol);
	float	fstepy	= 1.f / (float)(m_nRow);

	m_fLeft		= (float)(m_nSelCol-1)*fstepx;
	m_fTop		= (float)(m_nSelRow-1)*fstepy;
	m_fRight	= m_fLeft + fstepx;
	m_fBottom	= m_fTop + fstepy;

	CString	strVal;
	strVal.Format("uvrect : %.4f,%.4f,%.4f,%.4f", m_fLeft, m_fTop, m_fRight, m_fBottom);
	SetWindowText(strVal);

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlg_TexUVSelect::OnBnClickedCheckMulti()
{
	UpdateData(TRUE);
	
	if( GetDlgItem(IDC_EDIT_TIMEDISTANCE) )
		GetDlgItem(IDC_EDIT_TIMEDISTANCE)->EnableWindow( m_bMultiInput );
	if( GetDlgItem(IDC_EDIT_BEGIN_INDEX) )
		GetDlgItem(IDC_EDIT_BEGIN_INDEX)->EnableWindow( m_bMultiInput );
	if( GetDlgItem(IDC_EDIT_END_INDEX) )
		GetDlgItem(IDC_EDIT_END_INDEX)->EnableWindow( m_bMultiInput );

	vChkValidIndex();
}

void CDlg_TexUVSelect::OnEnChangeEditBeginIndex()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	if( !m_bInit )		return;

	UpdateData();

	vChkValidIndex();
}

void CDlg_TexUVSelect::OnEnChangeEditEndIndex()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	if( !m_bInit )
		return;

	UpdateData();

	vChkValidIndex();
}

void CDlg_TexUVSelect::vChkValidIndex()
{
	if( !m_bMultiInput )
		return;

	const UINT nmin	= 0;
	const UINT nmax	= m_nCol*m_nRow-1;

	BOOL bGood	= TRUE;

	if( m_nBeginIndex > nmax )
	{
		m_nBeginIndex	= 
		m_nEndIndex		= nmax;

		bGood = FALSE;
	}
	else if(m_nEndIndex < nmin)
	{
		m_nBeginIndex	= 
		m_nEndIndex		= nmin;
		
		bGood = FALSE;
	}

	if( m_nBeginIndex < nmin )
	{
		m_nBeginIndex = nmin;
		bGood = FALSE;
	}
	if( m_nBeginIndex > m_nEndIndex )
	{
		m_nBeginIndex = m_nEndIndex;
		bGood = FALSE;
	}

	if( m_nEndIndex > nmax )
	{
		m_nEndIndex = nmax;
		bGood = FALSE;
	}
	if( m_nEndIndex < m_nBeginIndex )
	{
		m_nEndIndex = m_nBeginIndex;
		bGood = FALSE;
	}

	if( !bGood )
	{
		ToWnd( "잘못된 beginIndex 또는 endIndex 입니다." );
		UpdateData(FALSE);
	}
};