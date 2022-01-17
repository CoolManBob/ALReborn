// AtomicFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MainFrm.h"
#include "MyEngine.h"
#include "AtomicFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtomicFilterDlg dialog


CAtomicFilterDlg::CAtomicFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAtomicFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAtomicFilterDlg)
	m_bBlockingObject = FALSE;
	m_bEventFilter = FALSE;
	m_bGeometry = FALSE;
	m_bGrass = FALSE;
	m_bObjectAll = FALSE;
	m_bObjectOthers = FALSE;
	m_bRidableObject = FALSE;
	m_bSystemObject = FALSE;
	m_strEventCombo = _T("");
	//}}AFX_DATA_INIT
}


void CAtomicFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtomicFilterDlg)
	DDX_Check(pDX, IDC_BLOCKINGOBJECT, m_bBlockingObject);
	DDX_Check(pDX, IDC_EVENTFILTER, m_bEventFilter);
	DDX_Check(pDX, IDC_SHOWGEOMETRY, m_bGeometry);
	DDX_Check(pDX, IDC_GRASS, m_bGrass);
	DDX_Check(pDX, IDC_OBJECT_ALL, m_bObjectAll);
	DDX_Check(pDX, IDC_OTHERS, m_bObjectOthers);
	DDX_Check(pDX, IDC_RIDABLEOBJECT, m_bRidableObject);
	DDX_Check(pDX, IDC_SYSTEM_OBJECT, m_bSystemObject);
	DDX_CBString(pDX, IDC_EVENTCOMBO, m_strEventCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAtomicFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CAtomicFilterDlg)
	ON_CONTROL_RANGE( BN_CLICKED , IDC_SYSTEM_OBJECT , IDC_SHOWGEOMETRY , OnClickButton )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtomicFilterDlg message handlers

BOOL CAtomicFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox	* pCombo = ( CComboBox * ) GetDlgItem( IDC_EVENTCOMBO );
	ASSERT( NULL != pCombo );

	CString	str;
#define ADD_COMBO_DATA( index , pName )			\
	str.Format( "%d , %s" , index , pName );	\
	pCombo->AddString( str );			

	ADD_COMBO_DATA(	APDEVENT_FUNCTION_SPAWN					, "SPAWN"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_FACTOR				, "FACTOR"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_VEHICLE				, "VEHICLE"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_SCHEDULE				, "SCHEDULE"			);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_HIDDEN				, "HIDDEN"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_SHOP					, "SHOP"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_INFORMATION			, "INFORMATION"			);
	ADD_COMBO_DATA(	APDEVENT_FUNCTION_TELEPORT				, "TELEPORT"			);
	ADD_COMBO_DATA(	APDEVENT_FUNCTION_NPCTRADE				, "NPCTRADE"			);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_CONVERSATION			, "CONVERSATION"		);
	ADD_COMBO_DATA(	APDEVENT_FUNCTION_NATURE				, "NATURE"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_STATUS				, "STATUS"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_ACTION				, "ACTION"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_SKILL					, "SKILL"				);
	ADD_COMBO_DATA(	APDEVENT_FUNCTION_SHRINE				, "SHRINE"				);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_UVU_REWARD			, "UVU_REWARD"			);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_ITEM_REPAIR			, "ITEM_REPAIR"			);
	ADD_COMBO_DATA(	APDEVENT_FUNCTION_MASTERY_SPECIALIZE	, "MASTERY_SPECIALIZE"	);
	//ADD_COMBO_DATA(	APDEVENT_FUNCTION_BINDING				, "BINDING"				);

	pCombo->SetCurSel( 0 );

	if( m_bObjectAll )
	{
		GetDlgItem( IDC_SYSTEM_OBJECT	)->EnableWindow( TRUE );
		GetDlgItem( IDC_RIDABLEOBJECT	)->EnableWindow( TRUE );
		GetDlgItem( IDC_BLOCKINGOBJECT	)->EnableWindow( TRUE );
		GetDlgItem( IDC_EVENTFILTER		)->EnableWindow( TRUE );
		GetDlgItem( IDC_GRASS			)->EnableWindow( TRUE );
		GetDlgItem( IDC_OTHERS			)->EnableWindow( TRUE );
		GetDlgItem( IDC_EVENTCOMBO		)->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem( IDC_SYSTEM_OBJECT	)->EnableWindow( FALSE );
		GetDlgItem( IDC_RIDABLEOBJECT	)->EnableWindow( FALSE );
		GetDlgItem( IDC_BLOCKINGOBJECT	)->EnableWindow( FALSE );
		GetDlgItem( IDC_EVENTFILTER		)->EnableWindow( FALSE );
		GetDlgItem( IDC_GRASS			)->EnableWindow( FALSE );
		GetDlgItem( IDC_OTHERS			)->EnableWindow( FALSE );
		GetDlgItem( IDC_EVENTCOMBO		)->EnableWindow( FALSE );
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAtomicFilterDlg::OnOK() 
{
	UpdateData( TRUE );
	m_nSelectedEventID = atoi( ( LPCTSTR ) m_strEventCombo );
	
	CDialog::OnOK();
}


void CAtomicFilterDlg::OnClickButton( UINT nID )
{
	CButton * pButton = ( CButton * ) GetDlgItem( nID );
	ASSERT( NULL != pButton );

	// Toggle button state
	BOOL bState;
	bState = pButton->GetCheck();

	switch( nID )
	{
	case IDC_SHOWGEOMETRY	:
	case IDC_GRASS			:
	case IDC_OTHERS			:
		break;
	case IDC_RIDABLEOBJECT	:
	case IDC_BLOCKINGOBJECT	:
		{
			( ( CButton * ) GetDlgItem( IDC_RIDABLEOBJECT	) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_BLOCKINGOBJECT	) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_EVENTFILTER		) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_GRASS			) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_OTHERS			) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_SYSTEM_OBJECT	) ) ->SetCheck( FALSE	);

			( ( CButton * ) GetDlgItem( nID					) ) ->SetCheck( TRUE	);
		}
		break;
	case IDC_EVENTFILTER	:
		{
			( ( CButton * ) GetDlgItem( IDC_RIDABLEOBJECT	) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_BLOCKINGOBJECT	) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_EVENTFILTER		) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_GRASS			) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_OTHERS			) ) ->SetCheck( FALSE	);
			( ( CButton * ) GetDlgItem( IDC_SYSTEM_OBJECT	) ) ->SetCheck( FALSE	);

			( ( CButton * ) GetDlgItem( nID					) ) ->SetCheck( TRUE	);
		}
		break;
	case IDC_SYSTEM_OBJECT	:
		{
			( ( CButton * ) GetDlgItem( IDC_RIDABLEOBJECT	) ) ->SetCheck( TRUE	);
			( ( CButton * ) GetDlgItem( IDC_BLOCKINGOBJECT	) ) ->SetCheck( TRUE	);
			( ( CButton * ) GetDlgItem( IDC_EVENTFILTER		) ) ->SetCheck( TRUE	);
			( ( CButton * ) GetDlgItem( IDC_GRASS			) ) ->SetCheck( TRUE	);
			( ( CButton * ) GetDlgItem( IDC_OTHERS			) ) ->SetCheck( TRUE	);
		}
		break;
	case IDC_OBJECT_ALL		:
		if( bState )
		{
			GetDlgItem( IDC_SYSTEM_OBJECT	)->EnableWindow( TRUE );
			GetDlgItem( IDC_RIDABLEOBJECT	)->EnableWindow( TRUE );
			GetDlgItem( IDC_BLOCKINGOBJECT	)->EnableWindow( TRUE );
			GetDlgItem( IDC_EVENTFILTER		)->EnableWindow( TRUE );
			GetDlgItem( IDC_GRASS			)->EnableWindow( TRUE );
			GetDlgItem( IDC_OTHERS			)->EnableWindow( TRUE );
			GetDlgItem( IDC_EVENTCOMBO		)->EnableWindow( TRUE );
		}
		else
		{
			GetDlgItem( IDC_SYSTEM_OBJECT	)->EnableWindow( FALSE );
			GetDlgItem( IDC_RIDABLEOBJECT	)->EnableWindow( FALSE );
			GetDlgItem( IDC_BLOCKINGOBJECT	)->EnableWindow( FALSE );
			GetDlgItem( IDC_EVENTFILTER		)->EnableWindow( FALSE );
			GetDlgItem( IDC_GRASS			)->EnableWindow( FALSE );
			GetDlgItem( IDC_OTHERS			)->EnableWindow( FALSE );
			GetDlgItem( IDC_EVENTCOMBO		)->EnableWindow( FALSE );
		}

		break;
	}
}
