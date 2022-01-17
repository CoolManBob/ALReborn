#include "stdafx.h"
#include "..\EffTool.h"
#include "dlgneweffbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNewEffBase::CDlgNewEffBase(CWnd* pParent /*=NULL*/) : CDialog(CDlgNewEffBase::IDD, pParent),
 m_bDummyObj(FALSE)
{
	m_nEffBaseType = 0;
}

void CDlgNewEffBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewEffBase)
	DDX_Radio(pDX, IDC_RADIO_BOARD, m_nEffBaseType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgNewEffBase, CDialog)
	//{{AFX_MSG_MAP(CDlgNewEffBase)
	ON_BN_CLICKED(IDC_RADIO_BOARD, OnRadioBoard)
	ON_BN_CLICKED(IDC_RADIO_LIGHT, OnRadioLight)
	ON_BN_CLICKED(IDC_RADIO_MFRM, OnRadioMfrm)
	ON_BN_CLICKED(IDC_RADIO_OBJ, OnRadioObj)
	ON_BN_CLICKED(IDC_RADIO_PSYS, OnRadioPsys)
	ON_BN_CLICKED(IDC_RADIO_PSYS_SBH, OnRadioPsysSbh)
	ON_BN_CLICKED(IDC_RADIO_SOUND, OnRadioSound)
	ON_BN_CLICKED(IDC_RADIO_TAIL, OnRadioTail)
	ON_BN_CLICKED(IDC_RADIO_TERRAINB, OnBnClickedRadioTerrainb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewEffBase message handlers

BOOL CDlgNewEffBase::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_dlgEffBase.Create( CDlg_EffBase::IDD, this );
	m_dlgEffBase.ShowWindow(SW_HIDE );
	m_dlgEffRenderBase.Create( CDlg_EffRenderBase::IDD, this );
	m_dlgEffRenderBase.ShowWindow(SW_HIDE );

	ShowWindow( SW_NORMAL );
	
	CWnd*	pWndOk = GetDlgItem( IDOK );
	CWnd*	pWndCancel = GetDlgItem( IDCANCEL );

	GetWindowRect(&m_rcThis);

	pWndOk->GetWindowRect(&m_rcOk);
	ScreenToClient(&m_rcOk);

	pWndCancel->GetWindowRect(&m_rcCancel);
	ScreenToClient(&m_rcCancel);

	vLayOutWnd();
		
	return TRUE;
}

void CDlgNewEffBase::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	m_dlgEffBase.UpdateData();
	m_dlgEffRenderBase.UpdateData();

	switch( m_nEffBaseType )
	{
	case AgcdEffBase::E_EFFBASE_BOARD:
		{
			if( strlen(m_dlgEffRenderBase.m_szTex) < 1 )
			{
				AfxMessageBox( _T("Texture 가 지정되지 안았습니다.") );
				return;
			}
		}
		break;
	case AgcdEffBase::E_EFFBASE_PSYS:
		{
			if( strlen(m_dlgEffRenderBase.m_szTex) < 1 )
			{
				AfxMessageBox( _T("Texture 가 지정되지 안았습니다.") );
				return;
			}
		}
		break;
	case AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:
		{
			if( strlen(m_dlgEffRenderBase.m_szTex) < 1 )
			{
				AfxMessageBox( _T("Texture 가 지정되지 안았습니다.") );
				return;
			}
		}
		break;
	case AgcdEffBase::E_EFFBASE_TAIL:
		{
			if( strlen(m_dlgEffRenderBase.m_szTex) < 1 )
			{
				AfxMessageBox( _T("Texture 가 지정되지 안았습니다.") );
				return;
			}
		}
		break;
	case AgcdEffBase::E_EFFBASE_OBJECT:
		{
			if( IDYES != AfxMessageBox(_T("dummy object 로 생성하시겠습니까?"), MB_YESNO) )
			{
				m_bDummyObj = FALSE;

				LPCTSTR	FILTER = _T("(*.rws)|*.rws|");
				TCHAR	currDir[MAX_PATH]	= "";
				::GetCurrentDirectory(MAX_PATH, currDir);

				CString	strpath(currDir);
				strpath += AgcuEffPath::bGetPath_Clump();
				strpath += _T("*.rws");
				CFileDialog dlgFile(TRUE,NULL,strpath,OFN_READONLY,FILTER,this);

				if( IDOK == dlgFile.DoModal() )
				{
					m_strFName = dlgFile.GetFileName();
					ToWnd( m_strFName.GetBuffer() );
					UpdateData(FALSE);
				}
				else
				{
					::SetCurrentDirectory(currDir);
					ErrToWnd("spline 파일이 지정되지 안았습니다.");
					return;
				}
				
				::SetCurrentDirectory(currDir);
			}
			else
				m_bDummyObj = TRUE;
		}
		break;
	case AgcdEffBase::E_EFFBASE_LIGHT:
		break;
	case AgcdEffBase::E_EFFBASE_SOUND:
		ToWnd( _T("sound file이 지정되지 안았습니다.") );
		break;
	case AgcdEffBase::E_EFFBASE_MOVINGFRAME:
		break;
	case AgcdEffBase::E_EFFBASE_TERRAINBOARD:
		{
			if( strlen(m_dlgEffRenderBase.m_szTex) < 1 )
			{
				AfxMessageBox( _T("Texture 가 지정되지 안았습니다.") );
				return;
			}
		}
		break;
	default :
		m_nEffBaseType = -1;
		return;
	}
	
	CDialog::OnOK();
}

void CDlgNewEffBase::OnCancel() 
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}

void CDlgNewEffBase::OnRadioBoard() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioLight() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioMfrm() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioObj() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioPsys() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioPsysSbh() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioSound() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnRadioTail() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::OnBnClickedRadioTerrainb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	vLayOutWnd();
}

void CDlgNewEffBase::vLayOutWnd(void)
{	
	BOOL	bBase	= FALSE;
	BOOL	bRenderBase	= FALSE;
	
	m_dlgEffBase.ShowWindow(SW_HIDE );
	m_dlgEffRenderBase.ShowWindow(SW_HIDE );

	switch( m_nEffBaseType )
	{
	case AgcdEffBase::E_EFFBASE_BOARD:
	case AgcdEffBase::E_EFFBASE_PSYS:
	case AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:
	case AgcdEffBase::E_EFFBASE_TAIL:
	case AgcdEffBase::E_EFFBASE_TERRAINBOARD:
		m_dlgEffBase.GetDlgItem( IDC_CHECK_BILLBOARD )->EnableWindow(TRUE);
		m_dlgEffBase.GetDlgItem( IDC_CHECK_BILLBOARDY )->EnableWindow(TRUE);

		m_dlgEffRenderBase.GetDlgItem( IDC_STATIC_TEX )->EnableWindow(TRUE);
		m_dlgEffRenderBase.GetDlgItem( IDC_STATIC_MASK )->EnableWindow(TRUE);
		m_dlgEffRenderBase.GetDlgItem( IDC_EDIT_TEX )->EnableWindow(TRUE);
		m_dlgEffRenderBase.GetDlgItem( IDC_EDIT_MASK )->EnableWindow(TRUE);
		m_dlgEffRenderBase.GetDlgItem( IDC_BUTTON_TEX )->EnableWindow(TRUE);
		m_dlgEffRenderBase.GetDlgItem( IDC_BUTTON_MASK )->EnableWindow(TRUE);

		bBase		= TRUE;
		bRenderBase	= TRUE;
		break;
	case AgcdEffBase::E_EFFBASE_OBJECT:
		m_dlgEffRenderBase.GetDlgItem( IDC_STATIC_TEX )->EnableWindow(FALSE);
		m_dlgEffRenderBase.GetDlgItem( IDC_STATIC_MASK )->EnableWindow(FALSE);
		m_dlgEffRenderBase.GetDlgItem( IDC_EDIT_TEX )->EnableWindow(FALSE);
		m_dlgEffRenderBase.GetDlgItem( IDC_EDIT_MASK )->EnableWindow(FALSE);
		m_dlgEffRenderBase.GetDlgItem( IDC_BUTTON_TEX )->EnableWindow(FALSE);
		m_dlgEffRenderBase.GetDlgItem( IDC_BUTTON_MASK )->EnableWindow(FALSE);
		bBase		= TRUE;
		bRenderBase = TRUE;
		break;
	case AgcdEffBase::E_EFFBASE_LIGHT:
	case AgcdEffBase::E_EFFBASE_SOUND:
	case AgcdEffBase::E_EFFBASE_MOVINGFRAME:
		m_dlgEffBase.GetDlgItem( IDC_CHECK_BILLBOARD )->EnableWindow(FALSE);
		m_dlgEffBase.GetDlgItem( IDC_CHECK_BILLBOARDY )->EnableWindow(FALSE);
		bBase	= TRUE;
		break;
	default:
		return;
	};

	if( !bBase && !bRenderBase )
	{
		this->MoveWindow(&this->m_rcThis);
		GetDlgItem( IDOK )->MoveWindow(&this->m_rcOk);
		GetDlgItem( IDCANCEL )->MoveWindow(&this->m_rcCancel);
		
		return;
	};

	m_dlgEffBase.ShowWindow(SW_SHOW);
	if( bRenderBase )
		m_dlgEffRenderBase.ShowWindow(SW_SHOW );

	CRect	rcThis = m_rcThis;
	CRect	rcEffBase;
	m_dlgEffBase.GetWindowRect(&rcEffBase);

	CRect	rcEffRenderBase;
	
	if( bRenderBase )
		m_dlgEffRenderBase.GetWindowRect(&rcEffRenderBase);

	//ScreenToClient(&rcThis);
	rcThis.bottom += rcEffBase.Height();
	if( bRenderBase )
		rcThis.bottom += rcEffRenderBase.Height();
	this->MoveWindow( rcThis );

	CWnd*	pWndOk = GetDlgItem( IDOK );
	CRect	rcOk = m_rcOk;
	CWnd*	pWndCancel = GetDlgItem( IDCANCEL );
	CRect	rcCancel = m_rcCancel;
	INT		nWidth = rcOk.Width();
	INT		nHeight = rcOk.Height();

	CWnd*	pWndRadioTail = GetDlgItem( IDC_RADIO_TERRAINB );
	CRect	rcRadioTail;
	pWndRadioTail->GetWindowRect( &rcRadioTail );

	CRect	rcNewPos;
	rcNewPos.left = rcRadioTail.left;
	rcNewPos.top = rcRadioTail.bottom + 10;
	rcNewPos.right = rcNewPos.left + rcEffBase.Width();
	rcNewPos.bottom = rcNewPos.top + rcEffBase.Height();
	ScreenToClient( &rcNewPos );
	m_dlgEffBase.MoveWindow(&rcNewPos);

	if( !bRenderBase )
	{
		rcOk.left = rcNewPos.left;
		rcOk.top = rcNewPos.bottom + 2;
		rcOk.right = rcOk.left + nWidth;
		rcOk.bottom = rcOk.top + nHeight;

		ScreenToClient( &rcCancel );
		rcCancel.top = rcOk.top;
		rcCancel.bottom = rcOk.bottom;

		pWndOk->MoveWindow( &rcOk );
		pWndCancel->MoveWindow( &rcCancel );
		return;
	}
	
	rcNewPos.top = rcNewPos.bottom + 1;
	rcNewPos.right = rcNewPos.left + rcEffRenderBase.Width();
	rcNewPos.bottom = rcNewPos.top + rcEffRenderBase.Height();
	m_dlgEffRenderBase.MoveWindow(&rcNewPos);

	m_dlgEffRenderBase.ShowWindow(SW_SHOW );
	
	rcOk.left = rcNewPos.left;
	rcOk.top = rcNewPos.bottom + 2;
	rcOk.right = rcOk.left + nWidth;
	rcOk.bottom = rcOk.top + nHeight;

	rcCancel.top = rcOk.top;
	rcCancel.bottom = rcOk.bottom;

	pWndOk->MoveWindow( &rcOk );
	pWndCancel->MoveWindow( &rcCancel );
};