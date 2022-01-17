#include "stdafx.h"
#include "..\EffTool.h"
#include "dlg_neweffanim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlg_NewEffAnim::CDlg_NewEffAnim(AgcdEffBase* pBase, CWnd* pParent /*=NULL*/) : CDialog(CDlg_NewEffAnim::IDD, pParent), m_pEffBase(pBase)
{
	m_nSelBtn = 0;
}

void CDlg_NewEffAnim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_COLR, m_nSelBtn);
}

BEGIN_MESSAGE_MAP(CDlg_NewEffAnim, CDialog)
	//{{AFX_MSG_MAP(CDlg_NewEffAnim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlg_NewEffAnim::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	vValidateChk();
	
	return TRUE;
}

void CDlg_NewEffAnim::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData(TRUE);

	//E_EFFANIM_RTANIM
	if( m_nSelBtn == (INT)AgcdEffAnim::E_EFFANIM_RTANIM )
	{
		//set RtAnim File
		LPCTSTR	FILTER = _T("(*.rws)|*.rws|");
		TCHAR	currDir[MAX_PATH]	= "";
		::GetCurrentDirectory(MAX_PATH, currDir);

		CString	strpath(currDir);
		strpath += AgcuEffPath::bGetPath_Anim();
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
			ErrToWnd("에니메이션 파일이 지정되지 안았습니다.");
			return;
		}
		
		::SetCurrentDirectory(currDir);
	}
	else if( m_nSelBtn == (INT)AgcdEffAnim::E_EFFANIM_RPSPLINE )		//E_EFFANIM_RPSPLINE
	{
		//set RpSpline File
		LPCTSTR	FILTER = _T("(*.rws)|*.rws|");
		TCHAR	currDir[MAX_PATH]	= "";
		::GetCurrentDirectory(MAX_PATH, currDir);

		CString	strpath(currDir);
		strpath += AgcuEffPath::bGetPath_Anim();
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

	CDialog::OnOK();
}

void CDlg_NewEffAnim::vValidateChk(void)
{
	if( !m_pEffBase )		return;

	CWnd*	pWndColr	= GetDlgItem(IDC_RADIO_COLR);
	CWnd*	pWndTutv	= GetDlgItem(IDC_RADIO_TUTV);
	CWnd*	pWndMissile	= GetDlgItem(IDC_RADIO_MISSILE);
	CWnd*	pWndLinear	= GetDlgItem(IDC_RADIO_LINEAR);
	CWnd*	pWndRev		= GetDlgItem(IDC_RADIO_REV);
	CWnd*	pWndRot		= GetDlgItem(IDC_RADIO_ROT);
	CWnd*	pWndSpline	= GetDlgItem(IDC_RADIO_SPLINE);
	CWnd*	pWndRtanim	= GetDlgItem(IDC_RADIO_RTANIM);
	CWnd*	pWndScale	= GetDlgItem(IDC_RADIO_SCALE);

	BOOL	bValidColr		= TRUE;
	BOOL	bValidTutv		= TRUE;
	BOOL	bValidMissile	= TRUE;
	BOOL	bValidLinear	= TRUE;
	BOOL	bValidRev		= TRUE;
	BOOL	bValidRot		= TRUE;
	BOOL	bValidSpline	= TRUE;
	BOOL	bValidRtanim	= TRUE;
	BOOL	bValidScale		= TRUE;

	AgcdEffBase::LPEffAniVec& vecEffAni = m_pEffBase->m_vecLPEffAni;
	for( AgcdEffBase::LPEffAniVecItr Itr = vecEffAni.begin(); Itr != vecEffAni.end(); ++Itr  )
	{	
		LPEFFANIM pAnim = (*Itr);
		switch( pAnim->bGetAnimType() )
		{
		case AgcdEffAnim::E_EFFANIM_COLOR:		pWndColr->EnableWindow(bValidColr = FALSE);		break;
		case AgcdEffAnim::E_EFFANIM_TUTV:		pWndTutv->EnableWindow(bValidTutv = FALSE);		break;
		case AgcdEffAnim::E_EFFANIM_MISSILE:	pWndMissile->EnableWindow(bValidMissile = FALSE);	break;
		case AgcdEffAnim::E_EFFANIM_LINEAR:		pWndLinear->EnableWindow(bValidLinear = FALSE);	break;
		case AgcdEffAnim::E_EFFANIM_REVOLUTION:	pWndRev->EnableWindow(bValidRev = FALSE);		break;
		case AgcdEffAnim::E_EFFANIM_ROTATION:	pWndRot->EnableWindow(bValidRot = FALSE);		break;
		case AgcdEffAnim::E_EFFANIM_RPSPLINE:	pWndSpline->EnableWindow(bValidSpline = FALSE);	break;
		case AgcdEffAnim::E_EFFANIM_RTANIM:		pWndRtanim->EnableWindow(bValidRtanim = FALSE);	break;
		case AgcdEffAnim::E_EFFANIM_SCALE:		pWndScale->EnableWindow(bValidScale	= FALSE);	break;
		}
	}

	if( bValidColr		)		m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_COLOR		;
	else if( bValidTutv		)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_TUTV		;
	else if( bValidMissile	)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_MISSILE	;	
	else if( bValidLinear	)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_LINEAR	;	
	else if( bValidRev		)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_REVOLUTION;	
	else if( bValidRot		)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_ROTATION	;
	else if( bValidSpline	)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_RPSPLINE	;
	else if( bValidRtanim	)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_RTANIM	;	
	else if( bValidScale	)	m_nSelBtn	= (INT)AgcdEffAnim::E_EFFANIM_SCALE		;

	UpdateData(FALSE);
};
