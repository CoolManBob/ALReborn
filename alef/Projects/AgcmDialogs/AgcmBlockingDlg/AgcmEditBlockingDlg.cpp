// AgcmEditBlockingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
//#include "AuRWUtil.h"
#include "AgcmBlockingDlg.h"
#include "AgcmEditBlockingDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEditBlockingDlg dialog

AgcmEditBlockingDlg::AgcmEditBlockingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEditBlockingDlg::IDD, pParent)
	, m_fEditX(0)
	, m_fEditY(0)
	, m_fEditZ(0)
{
	//{{AFX_DATA_INIT(AgcmEditBlockingDlg)
	m_strBlockingType = _T("");
	//}}AFX_DATA_INIT
}

void AgcmEditBlockingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEditBlockingDlg)
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_NONE, m_csBtNone);
	DDX_Control(pDX, IDC_COMBO_CUR_INDEX, m_csCurIndex);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_UP, m_csBtUp);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_SCALE, m_csBtScale);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_RIGHT, m_csBtRight);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_POSIT, m_csBtTranslate);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_HEIGHT, m_csBtHeight);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_AT, m_csBtAt);
	DDX_Control(pDX, IDC_CHECK_EDIT_BLOCKING_ALL, m_csBtAll);
	DDX_Text(pDX, IDC_EDIT_BLOCKING_TYPE, m_strBlockingType);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_OFFSET_X, m_fEditX);
	DDX_Text(pDX, IDC_OFFSET_Y, m_fEditY);
	DDX_Text(pDX, IDC_OFFSET_Z, m_fEditZ);
}


BEGIN_MESSAGE_MAP(AgcmEditBlockingDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEditBlockingDlg)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_SCALE, OnCheckEditBlockingScale)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_POSIT, OnCheckEditBlockingPosit)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_RIGHT, OnCheckEditBlockingRight)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_UP, OnCheckEditBlockingUp)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_AT, OnCheckEditBlockingAt)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_ALL, OnCheckEditBlockingAll)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_HEIGHT, OnCheckEditBlockingHeight)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_TYPE, OnButtonChangeType)
	ON_BN_CLICKED(IDC_CHECK_EDIT_BLOCKING_NONE, OnCheckEditBlockingNone)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_COPY_AND_INSERT, OnButtonCopyAndInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BLOCK_UPDATE, &AgcmEditBlockingDlg::OnBnClickedBlockUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_CUR_INDEX, OnSelchangeComboCurIndex)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEditBlockingDlg message handlers
BOOL AgcmEditBlockingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitaliizeBlockingList();
	InitializeEditControl();
//	m_csBtNone.SetCheck(BST_CHECKED);
//	OnCheckEditBlockingNone();
	SetBlockingType();

	m_bClick = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID AgcmEditBlockingDlg::InitaliizeBlockingList()
{
	CHAR		szIndex[256];
	AuBLOCKING	*pstBlocking	= AgcmBlockingDlg::GetInstance()->GetBlocking();
	INT16		nMaxBlocking	= AgcmBlockingDlg::GetInstance()->GetMaxBlocking();
	INT16		nCurIndex		= AgcmBlockingDlg::GetInstance()->GetCurBlockingIndex();

	m_csCurIndex.ResetContent();

	INT16 nIndex;
	for(nIndex = 0; nIndex < nMaxBlocking; ++nIndex)
	{
		if(pstBlocking[nIndex].type == AUBLOCKING_TYPE_NONE)
			break;

		sprintf(szIndex, "%d", nIndex);
		m_csCurIndex.InsertString(nIndex, szIndex);
	}

	if(nIndex >= nCurIndex)
	{
		m_csCurIndex.SetCurSel(nCurIndex);
	}
	else
	{
		OutputDebugString("AgcmEditBlockingDlg::InitaliizeBlockingList() Error (1) !!!\n");
	}
}

BOOL AgcmEditBlockingDlg::SetBlockingType()
{
	AuBLOCKING* pBlock = AgcmBlockingDlg::GetInstance()->GetCurrentBlocking();

	switch( pBlock->type )
	{
	case AUBLOCKING_TYPE_BOX:
		{
			m_strBlockingType = "BOX";

			RwV3d	vCenter; 
			RwV3dAdd( &vCenter, (RwV3d*)&pBlock->data.box.inf, (RwV3d*)&pBlock->data.box.sup );
			RwV3dScale( &vCenter, &vCenter, 0.5f );
			SetPos( vCenter.x, vCenter.y, vCenter.z );
		}
		break;

	case AUBLOCKING_TYPE_SPHERE:
		{
			m_strBlockingType = "SPHERE";
			SetPos( pBlock->data.sphere.center.x, pBlock->data.sphere.center.y, pBlock->data.sphere.center.z );
		}
		break;

	case AUBLOCKING_TYPE_CYLINDER:
		{
			m_strBlockingType = "CYLINDER";
			SetPos( pBlock->data.cylinder.center.x, pBlock->data.cylinder.center.y, pBlock->data.cylinder.center.z );
		}
		break;

	case AUBLOCKING_TYPE_NONE:
	default:
		{
			OutputDebugString("AgcmEditBlockingDlg::SetBlockingType() Error (1) !!!\n");
			return FALSE;
		}
		break;
	}

	UpdateData(FALSE);

	return TRUE;
}

VOID AgcmEditBlockingDlg::InitializeEditControl()
{
	CheckDlgButton(IDC_CHECK_EDIT_BLOCKING_NONE, BST_CHECKED);
	OnCheckEditBlockingNone();
}

VOID AgcmEditBlockingDlg::InitializeEditControl_None()
{
	m_csBtNone.EnableWindow(FALSE);
	m_csBtScale.EnableWindow(FALSE);
	m_csBtTranslate.EnableWindow(FALSE);

	m_csBtRight.EnableWindow(FALSE);
	m_csBtUp.EnableWindow(FALSE);
	m_csBtAt.EnableWindow(FALSE);
	m_csBtAll.EnableWindow(FALSE);
	m_csBtHeight.EnableWindow(FALSE);

	m_csBtNone.SetCheck(BST_UNCHECKED);
	m_csBtScale.SetCheck(BST_UNCHECKED);
	m_csBtTranslate.SetCheck(BST_UNCHECKED);

	m_csBtRight.SetCheck(BST_UNCHECKED);
	m_csBtUp.SetCheck(BST_UNCHECKED);
	m_csBtAt.SetCheck(BST_UNCHECKED);
	m_csBtAll.SetCheck(BST_UNCHECKED);
	m_csBtHeight.SetCheck(BST_UNCHECKED);
}

VOID AgcmEditBlockingDlg::InitializeEditControl_Box()
{
	m_csBtNone.EnableWindow(TRUE);
	m_csBtScale.EnableWindow(TRUE);
	m_csBtTranslate.EnableWindow(TRUE);

	m_csBtRight.EnableWindow(TRUE);
	m_csBtUp.EnableWindow(TRUE);
	m_csBtAt.EnableWindow(TRUE);
	m_csBtAll.EnableWindow(TRUE);
	m_csBtHeight.EnableWindow(FALSE);

	m_csBtNone.SetCheck(BST_UNCHECKED);
	m_csBtScale.SetCheck(BST_CHECKED);
	m_csBtTranslate.SetCheck(BST_UNCHECKED);

	m_csBtRight.SetCheck(BST_UNCHECKED);
	m_csBtUp.SetCheck(BST_UNCHECKED);
	m_csBtAt.SetCheck(BST_UNCHECKED);
	m_csBtAll.SetCheck(BST_CHECKED);
	m_csBtHeight.SetCheck(BST_UNCHECKED);
}

VOID AgcmEditBlockingDlg::InitializeEditControl_Sphere()
{
	m_csBtNone.EnableWindow(TRUE);
	m_csBtScale.EnableWindow(TRUE);
	m_csBtTranslate.EnableWindow(TRUE);
	
	m_csBtRight.EnableWindow(FALSE);
	m_csBtUp.EnableWindow(FALSE);
	m_csBtAt.EnableWindow(FALSE);
	m_csBtAll.EnableWindow(TRUE);
	m_csBtHeight.EnableWindow(FALSE);

	m_csBtNone.SetCheck(BST_UNCHECKED);
	m_csBtScale.SetCheck(BST_CHECKED);
	m_csBtTranslate.SetCheck(BST_UNCHECKED);

	m_csBtRight.SetCheck(BST_UNCHECKED);
	m_csBtUp.SetCheck(BST_UNCHECKED);
	m_csBtAt.SetCheck(BST_UNCHECKED);
	m_csBtAll.SetCheck(BST_CHECKED);
	m_csBtHeight.SetCheck(BST_UNCHECKED);
}

VOID AgcmEditBlockingDlg::InitializeEditControl_Cylinder()
{
	m_csBtNone.EnableWindow(TRUE);
	m_csBtScale.EnableWindow(TRUE);
	m_csBtTranslate.EnableWindow(TRUE);

	m_csBtRight.EnableWindow(FALSE);
	m_csBtUp.EnableWindow(FALSE);
	m_csBtAt.EnableWindow(FALSE);
	m_csBtAll.EnableWindow(TRUE);
	m_csBtHeight.EnableWindow(TRUE);

	m_csBtNone.SetCheck(BST_UNCHECKED);
	m_csBtScale.SetCheck(BST_CHECKED);
	m_csBtTranslate.SetCheck(BST_UNCHECKED);

	m_csBtRight.SetCheck(BST_UNCHECKED);
	m_csBtUp.SetCheck(BST_UNCHECKED);
	m_csBtAt.SetCheck(BST_UNCHECKED);
	m_csBtAll.SetCheck(BST_CHECKED);
	m_csBtHeight.SetCheck(BST_UNCHECKED);
}

VOID AgcmEditBlockingDlg::InitializeEditControl_Translate()
{
	m_csBtNone.EnableWindow(TRUE);
	m_csBtScale.EnableWindow(TRUE);
	m_csBtTranslate.EnableWindow(TRUE);

	m_csBtRight.EnableWindow(TRUE);
	m_csBtUp.EnableWindow(TRUE);
	m_csBtAt.EnableWindow(TRUE);
	m_csBtAll.EnableWindow(TRUE);
	m_csBtHeight.EnableWindow(FALSE);

	m_csBtNone.SetCheck(BST_UNCHECKED);
	m_csBtScale.SetCheck(BST_UNCHECKED);
	m_csBtTranslate.SetCheck(BST_CHECKED);

	m_csBtRight.SetCheck(BST_UNCHECKED);
	m_csBtUp.SetCheck(BST_UNCHECKED);
	m_csBtAt.SetCheck(BST_UNCHECKED);
	m_csBtAll.SetCheck(BST_CHECKED);
	m_csBtHeight.SetCheck(BST_UNCHECKED);
}


void AgcmEditBlockingDlg::OnOK() 
{
	// TODO: Add extra validation here
	AgcmBlockingDlg::GetInstance()->CloseEditBlocking();
	
//	CDialog::OnOK();
}

void AgcmEditBlockingDlg::OnCheckEditBlockingScale() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_SCALE))
	{
		switch(AgcmBlockingDlg::GetInstance()->GetCurrentBlocking()->type)
		{
		case AUBLOCKING_TYPE_BOX:
			{
				InitializeEditControl_Box();
			}
			break;

		case AUBLOCKING_TYPE_SPHERE:
			{
				InitializeEditControl_Sphere();
			}
			break;

		case AUBLOCKING_TYPE_CYLINDER:
			{
				InitializeEditControl_Cylinder();
			}
			break;

		default:
			{
				OutputDebugString("AgcmEditBlockingDlg::OnCheckEditBlockingScale() Error (1) !!!\n");
			}
			break;
		}
	}
	else
	{
		CheckDlgButton(IDC_CHECK_EDIT_BLOCKING_SCALE, BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingPosit() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_POSIT))
	{
		InitializeEditControl_Translate();
	}
	else
	{
		CheckDlgButton(IDC_CHECK_EDIT_BLOCKING_POSIT, BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingRight() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_RIGHT))
	{
		m_csBtRight.SetCheck(BST_CHECKED);

		m_csBtUp.SetCheck(BST_UNCHECKED);
		m_csBtAt.SetCheck(BST_UNCHECKED);
		m_csBtAll.SetCheck(BST_UNCHECKED);
		m_csBtHeight.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_csBtRight.SetCheck(BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingUp() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_UP))
	{
		m_csBtUp.SetCheck(BST_CHECKED);

		m_csBtRight.SetCheck(BST_UNCHECKED);
		m_csBtAt.SetCheck(BST_UNCHECKED);
		m_csBtAll.SetCheck(BST_UNCHECKED);
		m_csBtHeight.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_csBtUp.SetCheck(BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingAt() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_AT))
	{
		m_csBtAt.SetCheck(BST_CHECKED);

		m_csBtRight.SetCheck(BST_UNCHECKED);
		m_csBtUp.SetCheck(BST_UNCHECKED);
		m_csBtAll.SetCheck(BST_UNCHECKED);
		m_csBtHeight.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_csBtAt.SetCheck(BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingAll() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_ALL))
	{
		m_csBtAll.SetCheck(BST_CHECKED);

		m_csBtRight.SetCheck(BST_UNCHECKED);
		m_csBtUp.SetCheck(BST_UNCHECKED);
		m_csBtAt.SetCheck(BST_UNCHECKED);
		m_csBtHeight.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_csBtAll.SetCheck(BST_CHECKED);
	}
}

void AgcmEditBlockingDlg::OnCheckEditBlockingHeight() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_HEIGHT))
	{
		m_csBtHeight.SetCheck(BST_CHECKED);

		m_csBtRight.SetCheck(BST_UNCHECKED);
		m_csBtUp.SetCheck(BST_UNCHECKED);
		m_csBtAt.SetCheck(BST_UNCHECKED);
		m_csBtAll.SetCheck(BST_UNCHECKED);
	}
	else
	{
		m_csBtHeight.SetCheck(BST_CHECKED);
	}
}

BOOL AgcmEditBlockingDlg::Create(CWnd *pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class	
	return CDialog::Create(IDD, pParentWnd);
}

BOOL AgcmEditBlockingDlg::IsScale()
{
	if(!m_csBtScale.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_SCALE);
}

BOOL AgcmEditBlockingDlg::IsTranslate()
{
	if(!m_csBtTranslate.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_POSIT);
}

BOOL AgcmEditBlockingDlg::IsRight()
{
	if(!m_csBtRight.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_RIGHT);
}

BOOL AgcmEditBlockingDlg::IsUp()
{
	if(!m_csBtScale.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_UP);
}

BOOL AgcmEditBlockingDlg::IsAt()
{
	if(!m_csBtAt.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_AT);
}

BOOL AgcmEditBlockingDlg::IsAll()
{
	if(!m_csBtAll.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_ALL);
}

BOOL AgcmEditBlockingDlg::IsHeight()
{
	if(!m_csBtHeight.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_HEIGHT);
}

BOOL AgcmEditBlockingDlg::ClearEdit()
{
	m_fEditX = 0.f;
	m_fEditY = 0.f;
	m_fEditZ = 0.f;

	UpdateData( FALSE );

	return TRUE;
}

void AgcmEditBlockingDlg::SetPos( float fX, float fY, float fZ )
{
	m_fEditX = fX;
	m_fEditY = fY;
	m_fEditZ = fZ;

	UpdateData( FALSE );
}

void AgcmEditBlockingDlg::OnSelchangeComboCurIndex() 
{
	// TODO: Add your control notification handler code here
	AgcmBlockingDlg::GetInstance()->SetCurrentBlockingIndex( m_csCurIndex.GetCurSel() );
	InitializeEditControl();
	SetBlockingType();
}

void AgcmEditBlockingDlg::OnButtonChangeType() 
{
	// TODO: Add your control notification handler code here
	if(!AgcmBlockingDlg::GetInstance()->OpenInsertBlocking(AgcmBlockingDlg::GetInstance()->GetCurBlockingIndex()))
		return;

	SetBlockingType();
	InitializeEditControl();
}

void AgcmEditBlockingDlg::OnCheckEditBlockingNone() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_NONE))
	{
		m_csBtNone.SetCheck(BST_CHECKED);
		m_csBtScale.SetCheck(BST_UNCHECKED);
		m_csBtTranslate.SetCheck(BST_UNCHECKED);

		m_csBtRight.SetCheck(BST_UNCHECKED);
		m_csBtUp.SetCheck(BST_UNCHECKED);
		m_csBtAt.SetCheck(BST_UNCHECKED);
		m_csBtAll.SetCheck(BST_UNCHECKED);
		m_csBtHeight.SetCheck(BST_UNCHECKED);

		m_csBtRight.EnableWindow(FALSE);
		m_csBtUp.EnableWindow(FALSE);
		m_csBtAt.EnableWindow(FALSE);
		m_csBtAll.EnableWindow(FALSE);
		m_csBtHeight.EnableWindow(FALSE);
	}
	else
	{
		CheckDlgButton(IDC_CHECK_EDIT_BLOCKING_NONE, BST_CHECKED);
	}
}

BOOL AgcmEditBlockingDlg::IsNone()
{
	if(!m_csBtNone.IsWindowEnabled())
		return FALSE;

	return IsDlgButtonChecked(IDC_CHECK_EDIT_BLOCKING_NONE);
}

void AgcmEditBlockingDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	AgcmBlockingDlg::GetInstance()->CloseEditBlocking();
	
	//CDialog::OnClose();
}

void AgcmEditBlockingDlg::OnButtonInsert() 
{
	// TODO: Add your control notification handler code here
	AgcmBlockingDlg::GetInstance()->SetNewBlocking(-1);
}

void AgcmEditBlockingDlg::OnButtonCopyAndInsert() 
{
	// TODO: Add your control notification handler code here
	AgcmBlockingDlg::GetInstance()->SetNewBlocking(m_csCurIndex.GetCurSel());
}

void AgcmEditBlockingDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	UINT16	nCurIndex = m_csCurIndex.GetCurSel();

	CHAR	szText[256];
	sprintf(szText, "%d번째 BLOCKING을 삭제하시겠습니까?", nCurIndex);

	if(MessageBox(szText, "Delete", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		if(AgcmBlockingDlg::GetInstance()->DeleteBlocking(nCurIndex))
		{
			InitaliizeBlockingList();
		}
	}
}

void AgcmEditBlockingDlg::OnBnClickedBlockUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	RwV3d	vec = { m_fEditX, m_fEditY, m_fEditZ };
	AgcmBlockingDlg::GetInstance()->TransBlock( vec, FALSE );

	m_bClick  = TRUE;

	UpdateData( FALSE );
}

BOOL AgcmEditBlockingDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
		KeyDown( pMsg->wParam );

	return CDialog::PreTranslateMessage(pMsg);
}

void AgcmEditBlockingDlg::KeyDown( WPARAM wParam )
{
	//RwV3d	vec = { 0.f, 0.f, 0.f };	
	//switch( wParam )
	//{
	//case VK_NUMPAD8:	//y-up
	//	vec.y += 1.f;
	//	break;
	//case VK_NUMPAD2:	//y-down
	//	vec.y -= 1.f;
	//	break;
	//case VK_NUMPAD4:	//x-down
	//	vec.x -= 1.f;
	//	break;
	//case VK_NUMPAD6:	//y-up
	//	vec.x += 1.f;
	//	break;
	//case VK_NUMPAD7:	//z-up
	//	vec.z -= 1.f;
	//	break;
	//case VK_NUMPAD9:	//z-down
	//	vec.z += 1.f;
	//	break;
	//}

	//AgcmBlockingDlg::GetInstance()->UpdateBlock( vec );
}

void AgcmEditBlockingDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
