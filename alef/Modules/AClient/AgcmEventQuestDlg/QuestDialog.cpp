// QuestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "agcmeventquestdlg.h"
#include "QuestDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuestDialog dialog


CQuestDialog::CQuestDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestDialog::IDD, pParent)
{
	TRACE("");
	//{{AFX_DATA_INIT(CQuestDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CQuestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestDialog, CDialog)
	//{{AFX_MSG_MAP(CQuestDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuestDialog message handlers
