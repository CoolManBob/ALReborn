// UITActionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UITActionDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITActionDialog dialog

extern CHAR *g_aszUIActionType[];
extern CHAR *g_szArgumentNULL;

extern CHAR *g_aszUIActionUIType[];
extern CHAR *g_aszUIActionMessageType[];

UITActionDialog::UITActionDialog(AgcdUI *pcsUI, AgcdUIAction *pstAction, CWnd* pParent /*=NULL*/)
	: CDialog(UITActionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITActionDialog)
	m_strArgDescription = _T("");
	m_bSaveCurrentStatus = FALSE;
	m_bGroupAction = FALSE;
	m_strMessage = _T("");
	m_strSound = _T("");
	m_bSmooth = TRUE;
	m_lPosX = 0;
	m_lPosY = 0;
	//}}AFX_DATA_INIT

	m_pstAction	= pstAction;
	m_pcsUI		= pcsUI;

	m_bUserChangeActionType	= FALSE;
}


void UITActionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITActionDialog)
	DDX_Control(pDX, IDC_UIT_ACTION_ATTACH_UI, m_csUIAttach);
	DDX_Control(pDX, IDC_UIT_ACTION_UI_POSY, m_csPositionY);
	DDX_Control(pDX, IDC_UIT_ACTION_UI_POSX, m_csPositionX);
	DDX_Control(pDX, IDC_UIT_SMOOTH_UI, m_csUISmooth);
	DDX_Control(pDX, IDC_UIT_ACTION_SOUND, m_csSound);
	DDX_Control(pDX, IDC_UIT_ACTION_DISPLAY_LIST, m_csDisplayList);
	DDX_Control(pDX, IDC_UIT_ACTION_UD_LIST, m_csUserDataList);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_MS, m_csMessage);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_COLOR_R, m_csMessageColorR);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_COLOR_G, m_csMessageColorG);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_COLOR_B, m_csMessageColorB);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_COLOR_A, m_csMessageColorA);
	DDX_Control(pDX, IDC_UIT_ACTION_MS_TYPE, m_csMessageType);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_TARGET3, m_csStatusTarget3);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_TARGET2, m_csStatusTarget2);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_CURRENT3, m_csStatusCurrent3);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_CURRENT2, m_csStatusCurrent2);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_TARGET1, m_csStatusTarget1);
	DDX_Control(pDX, IDC_UIT_GROUP_ACTION, m_csUIGroupAction);
	DDX_Control(pDX, IDC_UIT_SAVE_STATUS, m_csSaveCurrentStatus);
	DDX_Control(pDX, IDC_UIT_ARGS_5, m_csArguments5);
	DDX_Control(pDX, IDC_UIT_ARGS_4, m_csArguments4);
	DDX_Control(pDX, IDC_UIT_ARGS_3, m_csArguments3);
	DDX_Control(pDX, IDC_UIT_ARGS_2, m_csArguments2);
	DDX_Control(pDX, IDC_UIT_ARGS_1, m_csArguments1);
	DDX_Control(pDX, IDC_UIT_FUNCTIONS, m_csFunctionList);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL_STATUS_CURRENT1, m_csStatusCurrent1);
	DDX_Control(pDX, IDC_UIT_ACTION_CONTROL, m_csControlList);
	DDX_Control(pDX, IDC_UIT_ACTION_UI_TYPE, m_csUITypeList);
	DDX_Control(pDX, IDC_UIT_ACTION_UI, m_csUIList);
	DDX_Control(pDX, IDC_UIT_ACTION_TYPE, m_csActionTypeList);
	DDX_Text(pDX, IDC_UIT_ARG_DESCRIPTION, m_strArgDescription);
	DDX_Check(pDX, IDC_UIT_SAVE_STATUS, m_bSaveCurrentStatus);
	DDX_Check(pDX, IDC_UIT_GROUP_ACTION, m_bGroupAction);
	DDX_Text(pDX, IDC_UIT_ACTION_MS_MS, m_strMessage);
	DDX_Text(pDX, IDC_UIT_ACTION_SOUND, m_strSound);
	DDX_Check(pDX, IDC_UIT_SMOOTH_UI, m_bSmooth);
	DDX_Text(pDX, IDC_UIT_ACTION_UI_POSX, m_lPosX);
	DDX_Text(pDX, IDC_UIT_ACTION_UI_POSY, m_lPosY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITActionDialog, CDialog)
	//{{AFX_MSG_MAP(UITActionDialog)
	ON_CBN_SELCHANGE(IDC_UIT_ACTION_TYPE, OnSelchangeUITActionType)
	ON_CBN_SELCHANGE(IDC_UIT_ACTION_CONTROL, OnSelchangeUITActionControl)
	ON_CBN_SELCHANGE(IDC_UIT_FUNCTIONS, OnSelchangeUITFunctions)
	ON_CBN_SELCHANGE(IDC_UIT_ACTION_MS_TYPE, OnSelchangeUitActionMsType)
	ON_BN_CLICKED(IDC_UIT_ACTION_ADD_VARIABLE, OnUITActionAddVariable)
	ON_CBN_SELCHANGE(IDC_UIT_ACTION_UD_LIST, OnSelchangeUITActionUDList)
	ON_BN_CLICKED(IDC_UIT_SOUND_BROWSE, OnUITSoundBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITActionDialog message handlers

void UITActionDialog::OnSelchangeUITActionType() 
{
	INT32		lIndex;

	m_csUIList.EnableWindow(FALSE);
	m_csUITypeList.EnableWindow(FALSE);
	m_csUIGroupAction.EnableWindow(FALSE);
	m_csUISmooth.EnableWindow(FALSE);
	m_csPositionX.EnableWindow(FALSE);
	m_csPositionY.EnableWindow(FALSE);
	m_csUIAttach.EnableWindow(FALSE);
	m_csControlList.EnableWindow(FALSE);
	m_csStatusCurrent1.EnableWindow(FALSE);
	m_csStatusCurrent2.EnableWindow(FALSE);
	m_csStatusCurrent3.EnableWindow(FALSE);
	m_csStatusTarget1.EnableWindow(FALSE);
	m_csStatusTarget2.EnableWindow(FALSE);
	m_csStatusTarget3.EnableWindow(FALSE);
	m_csSaveCurrentStatus.EnableWindow(FALSE);
	m_csFunctionList.EnableWindow(FALSE);
	m_csArguments1.EnableWindow(FALSE);
	m_csArguments2.EnableWindow(FALSE);
	m_csArguments3.EnableWindow(FALSE);
	m_csArguments4.EnableWindow(FALSE);
	m_csArguments5.EnableWindow(FALSE);
	m_csMessageType.EnableWindow(FALSE)		;
	m_csMessageColorA.EnableWindow(FALSE)	;
	m_csMessageColorB.EnableWindow(FALSE)	;
	m_csMessageColorG.EnableWindow(FALSE)	;
	m_csMessageColorR.EnableWindow(FALSE)	;
	m_csMessage.EnableWindow(FALSE)			;
	m_csUserDataList.EnableWindow(FALSE)	;
	m_csDisplayList.EnableWindow(FALSE)	;
	m_csSound.EnableWindow(FALSE)	;

	lIndex = m_csActionTypeList.GetItemData(m_csActionTypeList.GetCurSel());
	switch (lIndex)
	{
	case AGCDUI_ACTION_UI:
		if (m_bUserChangeActionType)
			m_bSmooth = TRUE;

		m_csUIList.EnableWindow();
		m_csUITypeList.EnableWindow();
		m_csUIGroupAction.EnableWindow();
		m_csUISmooth.EnableWindow();
		m_csPositionX.EnableWindow();
		m_csPositionY.EnableWindow();
		m_csUIAttach.EnableWindow();
		break;

	case AGCDUI_ACTION_CONTROL:
		m_csControlList.EnableWindow();
		m_csStatusCurrent1.EnableWindow();
		m_csStatusCurrent2.EnableWindow();
		m_csStatusCurrent3.EnableWindow();
		m_csStatusTarget1.EnableWindow();
		m_csStatusTarget2.EnableWindow();
		m_csStatusTarget3.EnableWindow();
		m_csSaveCurrentStatus.EnableWindow();
		break;

	case AGCDUI_ACTION_FUNCTION:
		m_csFunctionList.EnableWindow();
		m_csArguments1.EnableWindow();
		m_csArguments2.EnableWindow();
		m_csArguments3.EnableWindow();
		m_csArguments4.EnableWindow();
		m_csArguments5.EnableWindow();
		break;

	case AGCDUI_ACTION_MESSAGE:
		{
			m_csMessageType.EnableWindow()		;

			INT32 lSelectIndex = m_csMessageType.GetCurSel();

			if ( AGCDUI_ACTION_MESSAGE_CHATTING_WINDOW == lSelectIndex )
			{
				m_csMessageColorR.EnableWindow( TRUE );
				m_csMessageColorG.EnableWindow( TRUE );
				m_csMessageColorB.EnableWindow( TRUE );
				m_csMessageColorA.EnableWindow( TRUE );
			}
			else 
			{
				m_csMessageColorR.EnableWindow( FALSE );
				m_csMessageColorG.EnableWindow( FALSE );
				m_csMessageColorB.EnableWindow( FALSE );
				m_csMessageColorA.EnableWindow( FALSE );		
			}

			m_csUserDataList.EnableWindow()		;
			m_csDisplayList.EnableWindow()		;
			m_csMessage.EnableWindow()			;
		}
		break;

	case AGCDUI_ACTION_SOUND:
		m_csSound.EnableWindow()			;
		break;
	}

	UpdateData(FALSE);
}

void UITActionDialog::OnSelchangeUITActionControl() 
{
	INT32			lIndex;
	INT32			lListIndex;
	AgcdUIControl *	pcsControl;
	static CHAR *	szNone = "<None>";
	static CHAR *	szAllStatus = "<All>";

	m_csStatusCurrent1.ResetContent();
	m_csStatusCurrent2.ResetContent();
	m_csStatusCurrent3.ResetContent();
	m_csStatusTarget1.ResetContent();
	m_csStatusTarget2.ResetContent();
	m_csStatusTarget3.ResetContent();

	lIndex = m_csControlList.GetCurSel();
	if (lIndex == CB_ERR)
		return;

	pcsControl = (AgcdUIControl *) m_csControlList.GetItemDataPtr(lIndex);
	if (!pcsControl)
		return;

	/*
	lListIndex = m_csStatusCurrent1.AddString(szNone);
	lListIndex = m_csStatusCurrent2.AddString(szNone);
	lListIndex = m_csStatusCurrent3.AddString(szNone);
	lListIndex = m_csStatusTarget1.AddString(szNone);
	lListIndex = m_csStatusTarget2.AddString(szNone);
	lListIndex = m_csStatusTarget3.AddString(szNone);
	*/

	lListIndex = m_csStatusCurrent1.AddString(szAllStatus);
	lListIndex = m_csStatusCurrent2.AddString(szAllStatus);
	lListIndex = m_csStatusCurrent3.AddString(szAllStatus);
	lListIndex = m_csStatusTarget1.AddString(szAllStatus);
	lListIndex = m_csStatusTarget2.AddString(szAllStatus);
	lListIndex = m_csStatusTarget3.AddString(szAllStatus);

	m_csStatusCurrent1.SetItemData(lListIndex, -1);
	m_csStatusCurrent2.SetItemData(lListIndex, -1);
	m_csStatusCurrent3.SetItemData(lListIndex, -1);
	m_csStatusTarget1.SetItemData(lListIndex, -1);
	m_csStatusTarget2.SetItemData(lListIndex, -1);
	m_csStatusTarget3.SetItemData(lListIndex, -1);

	/*
	m_csStatusCurrent1.SetItemData(lListIndex, 0);
	m_csStatusCurrent2.SetItemData(lListIndex, 0);
	m_csStatusCurrent3.SetItemData(lListIndex, 0);
	m_csStatusTarget1.SetItemData(lListIndex, 0);
	m_csStatusTarget2.SetItemData(lListIndex, 0);
	m_csStatusTarget3.SetItemData(lListIndex, 0);
	*/

	for (lIndex = 0; lIndex < ACUIBASE_STATUS_MAX_NUM; ++lIndex)
	{
		lListIndex = m_csStatusCurrent1.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		lListIndex = m_csStatusCurrent2.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		lListIndex = m_csStatusCurrent3.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		lListIndex = m_csStatusTarget1.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		lListIndex = m_csStatusTarget2.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		lListIndex = m_csStatusTarget3.AddString(pcsControl->m_pcsBase->m_astStatus[lIndex].m_szStatusName);
		if (lListIndex == CB_ERR)
		{
			AfxMessageBox("Error Adding Status Name !!!");
			continue;
		}

		m_csStatusCurrent1.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
		m_csStatusCurrent2.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
		m_csStatusCurrent3.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
		m_csStatusTarget1.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
		m_csStatusTarget2.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
		m_csStatusTarget3.SetItemData(lListIndex, pcsControl->m_pcsBase->m_astStatus[lIndex].m_lStatusID);
	}
}

BOOL UITActionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lIndex2;
	INT32				lListIndex;
	AgcdUIFunction *	pstFunction;
	AgcdUI *			pcsUI;
	AgcdUIControl *		pcsControl;
	static CHAR *		szNone			= "<None>";
	static CHAR *		szAllStatus		= "<All>";
	static CHAR *		szNameUI		= "====== UI ======";
	static CHAR *		szNameControl	= "==== Control ===";
	AgcdUIUserData *	pstUserData;

	m_apcsArguments[0] = &m_csArguments1;
	m_apcsArguments[1] = &m_csArguments2;
	m_apcsArguments[2] = &m_csArguments3;
	m_apcsArguments[3] = &m_csArguments4;
	m_apcsArguments[4] = &m_csArguments5;

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetFunctionCount(); ++lIndex)
	{
		pstFunction = g_pcsAgcmUIManager2->GetFunction(lIndex);
		if (pstFunction)
		{
			lListIndex = m_csFunctionList.AddString(pstFunction->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csFunctionList.SetItemDataPtr(lListIndex, pstFunction);
		}
	}

	for (lIndex = 0; lIndex < AGCDUI_MAX_ACTION; ++lIndex)
	{
		if (g_aszUIActionType[lIndex])
		{
			lListIndex = m_csActionTypeList.AddString(g_aszUIActionType[lIndex]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csActionTypeList.SetItemData(lListIndex, lIndex);
		}
	}

	m_csUIAttach.AddString(szNone);

	lIndex = 0;
	for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex))
	{
		lListIndex = m_csUIList.AddString(pcsUI->m_szUIName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUIList.SetItemDataPtr(lListIndex, pcsUI);

		lListIndex = m_csUIAttach.AddString(pcsUI->m_szUIName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUIAttach.SetItemDataPtr(lListIndex, pcsUI);
	}

	for (lIndex = 0; lIndex < AGCDUI_MAX_ACTION_UI; ++lIndex)
	{
		lListIndex = m_csUITypeList.AddString(g_aszUIActionUIType[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUITypeList.SetItemData(lListIndex, lIndex);
	}

	if (m_pstAction->m_eType == AGCDUI_ACTION_CONTROL)
	{
		pcsControl = m_pstAction->m_uoAction.m_stControl.m_pcsControl;
		if (pcsControl && pcsControl->m_pcsParentUI != m_pcsUI)
		{
			lListIndex = m_csControlList.AddString(pcsControl->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csControlList.SetItemDataPtr(lListIndex, pcsControl);
		}
	}

	if (m_pcsUI)
	{
		lIndex = 0;
		for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex))
		{
			lListIndex = m_csControlList.AddString(pcsControl->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csControlList.SetItemDataPtr(lListIndex, pcsControl);
		}
	}

	for (lIndex2 = 0; lIndex2 < 5; ++lIndex2)
	{
		m_apcsArguments[lIndex2]->ResetContent();
		lListIndex = m_apcsArguments[lIndex2]->AddString(g_szArgumentNULL);
		lListIndex = m_apcsArguments[lIndex2]->AddString(szNameUI);

		lIndex = 0;
		for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex))
		{
			lListIndex = m_apcsArguments[lIndex2]->AddString(pcsUI->m_szUIName);
			m_apcsArguments[lIndex2]->SetItemDataPtr(lListIndex, pcsUI);
		}

		lListIndex = m_apcsArguments[lIndex2]->AddString(szNameControl);

		if (m_pcsUI)
		{
			lIndex = 0;
			for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex))
			{
				lListIndex = m_apcsArguments[lIndex2]->AddString(pcsControl->m_szName);

				m_apcsArguments[lIndex2]->SetItemDataPtr(lListIndex, pcsControl);
			}
		}
	}


	m_csActionTypeList.SelectString(-1, g_aszUIActionType[m_pstAction->m_eType]);

	m_csMessage.LimitText(AGCDUIMANAGER2_MAX_ACTION_MESSAGE - 1);

	// Message
	for ( lIndex = 0 ; lIndex < AGCDUI_MAX_ACTION_MESSAGE ; ++lIndex )
	{
		lListIndex = m_csMessageType.AddString(g_aszUIActionMessageType[lIndex])	;
		m_csMessageType.SetItemData( lListIndex, lIndex )							;
	}
	m_csMessageType.SelectString( -1, g_aszUIActionMessageType[0])					;
	m_csMessageColorR.SetWindowText( "255" )											;
	m_csMessageColorG.SetWindowText( "255" )											;
	m_csMessageColorB.SetWindowText( "255" )											;
	m_csMessageColorA.SetWindowText( "255" )											;

	// Message의 Variable에 쓰일 UserData (DisplayMap은 OnChange에서
	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (!pstUserData)
			continue;

		lListIndex = m_csUserDataList.AddString(pstUserData->m_szName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUserDataList.SetItemDataPtr(lListIndex, pstUserData);
	}

	switch (m_pstAction->m_eType)
	{
	case AGCDUI_ACTION_UI:
		if (m_pstAction->m_uoAction.m_stUI.m_pcsUI)
		{
			m_csUIList.SelectString(-1, m_pstAction->m_uoAction.m_stUI.m_pcsUI->m_szUIName);
			m_csUITypeList.SelectString(-1, g_aszUIActionUIType[m_pstAction->m_uoAction.m_stUI.m_eType]);
			m_bGroupAction = m_pstAction->m_uoAction.m_stUI.m_bGroupAction;
			m_bSmooth = m_pstAction->m_uoAction.m_stUI.m_bSmooth;
			m_lPosX = m_pstAction->m_uoAction.m_stUI.m_lPosX;
			m_lPosY = m_pstAction->m_uoAction.m_stUI.m_lPosY;

			if (m_pstAction->m_uoAction.m_stUI.m_pcsAttachUI)
				m_csUIAttach.SelectString(-1, m_pstAction->m_uoAction.m_stUI.m_pcsAttachUI->m_szUIName);
			else
				m_csUIAttach.SetCurSel(-1);
		}
		else
		{
			m_csUIList.SetCurSel(-1);
			m_csUITypeList.SetCurSel(-1);
			m_bGroupAction = FALSE;
			m_bSmooth = FALSE;
			m_csUIAttach.SetCurSel(-1);
		}

		break;

	case AGCDUI_ACTION_CONTROL:
		if (m_pstAction->m_uoAction.m_stControl.m_pcsControl)
		{
			stStatusInfo *	pstStatus;
			m_csControlList.SelectString(-1, m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_szName);

			OnSelchangeUITActionControl();

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lCurrentStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 1)
				m_csStatusCurrent1.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lCurrentStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 1)
				m_csStatusCurrent1.SelectString(-1, szAllStatus);
			else
				m_csStatusCurrent1.SelectString(-1, szNone);

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lCurrentStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 2)
				m_csStatusCurrent2.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lCurrentStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 2)
				m_csStatusCurrent2.SelectString(-1, szAllStatus);
			else
				m_csStatusCurrent2.SelectString(-1, szNone);

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lCurrentStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 3)
				m_csStatusCurrent3.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lCurrentStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 3)
				m_csStatusCurrent3.SelectString(-1, szAllStatus);
			else
				m_csStatusCurrent3.SelectString(-1, szNone);

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lTargetStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 1)
				m_csStatusTarget1.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lTargetStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 1)
				m_csStatusTarget1.SelectString(-1, szAllStatus);
			else
				m_csStatusTarget1.SelectString(-1, szNone);

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lTargetStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 2)
				m_csStatusTarget2.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lTargetStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 2)
				m_csStatusTarget2.SelectString(-1, szAllStatus);
			else
				m_csStatusTarget2.SelectString(-1, szNone);

			pstStatus = m_pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lTargetStatus);
			if (pstStatus && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 3)
				m_csStatusTarget3.SelectString(-1, pstStatus->m_szStatusName);
			else if (m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lTargetStatus == -1 && m_pstAction->m_uoAction.m_stControl.m_lStatusNum >= 3)
				m_csStatusTarget3.SelectString(-1, szAllStatus);
			else
				m_csStatusTarget3.SelectString(-1, szNone);

			m_bSaveCurrentStatus = m_pstAction->m_uoAction.m_stControl.m_bSaveCurrent;
		}
		else
		{
			m_csControlList.SetCurSel(-1);

			OnSelchangeUITActionControl();

			m_csStatusCurrent1.SetCurSel(-1);
			m_csStatusCurrent2.SetCurSel(-1);
			m_csStatusCurrent3.SetCurSel(-1);
			m_csStatusTarget1.SetCurSel(-1);
			m_csStatusTarget2.SetCurSel(-1);
			m_csStatusTarget3.SetCurSel(-1);
		}

		break;

	case AGCDUI_ACTION_FUNCTION:
		if (m_pstAction->m_uoAction.m_stFunction.m_pstFunction)
			m_csFunctionList.SelectString(-1, m_pstAction->m_uoAction.m_stFunction.m_pstFunction->m_szName);
		else
			m_csFunctionList.SetCurSel(-1);

		for (lIndex = 0; lIndex < 5; ++lIndex)
		{
			if (m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType == AGCDUI_ARG_TYPE_UI)
				m_apcsArguments[lIndex]->SelectString(-1, m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pcsUI ? m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pcsUI->m_szUIName : g_szArgumentNULL);
			else if (m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType == AGCDUI_ARG_TYPE_CONTROL)
				m_apcsArguments[lIndex]->SelectString(-1, m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pstControl ? m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pstControl->m_szName : g_szArgumentNULL);
			else
				m_apcsArguments[lIndex]->SelectString(-1, g_szArgumentNULL);
		}

		break;

	case AGCDUI_ACTION_MESSAGE:
		{
			m_csMessageType.SelectString( -1, g_aszUIActionMessageType[m_pstAction->m_uoAction.m_stMessage.m_eMessageType]);
			m_strMessage = m_pstAction->m_uoAction.m_stMessage.m_szMessage;

			UINT8 lColor[4] = { 255, 255, 255, 255 };
			lColor[0] = (UINT8)( (m_pstAction->m_uoAction.m_stMessage.m_lColor & 0xff000000) >> 24 );	// alpha
			lColor[1] = (UINT8)( (m_pstAction->m_uoAction.m_stMessage.m_lColor & 0x00ff0000) >> 16 );	// red
			lColor[2] = (UINT8)( (m_pstAction->m_uoAction.m_stMessage.m_lColor & 0x0000ff00) >> 8 );	// green
			lColor[3] = (UINT8)( (m_pstAction->m_uoAction.m_stMessage.m_lColor & 0x000000ff) );			// blue

			INT32 lTemp[2] = { 0, 0 };
			if ( 0 == lColor[1] )	m_csMessageColorR.SetWindowText( "0" );
			else					m_csMessageColorR.SetWindowText( _fcvt( lColor[1], 0, &lTemp[0], &lTemp[1] ) )		;

			if ( 0 == lColor[2] )   m_csMessageColorG.SetWindowText( "0" );
			else					m_csMessageColorG.SetWindowText( _fcvt( lColor[2], 0, &lTemp[0], &lTemp[1] ) )		;

			if ( 0 == lColor[3] )	m_csMessageColorB.SetWindowText( "0" );
			else					m_csMessageColorB.SetWindowText( _fcvt( lColor[3], 0, &lTemp[0], &lTemp[1] ) )		;

			if ( 0 == lColor[0] )	m_csMessageColorA.SetWindowText( "0" );
			else					m_csMessageColorA.SetWindowText( _fcvt( lColor[0], 0, &lTemp[0], &lTemp[1] ) )		;
		}

		break;

	case AGCDUI_ACTION_SOUND:
		m_strSound = m_pstAction->m_uoAction.m_stSound.m_szSound;

		break;
	}
	
	UpdateData(FALSE);

	OnSelchangeUITActionType();

	m_bUserChangeActionType = TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITActionDialog::OnOK() 
{
	INT32				lListIndex;
	INT32				lListIndex2;
	AgcdUIActionType	eType;
	AgcdUIControl *		pcsControl;

	UpdateData();

	eType = (AgcdUIActionType) m_csActionTypeList.GetItemData(m_csActionTypeList.GetCurSel());
	m_pstAction->m_eType = eType;
	switch (eType)
	{
	case AGCDUI_ACTION_UI:
		lListIndex = m_csUIList.GetCurSel();
		m_pstAction->m_uoAction.m_stUI.m_pcsUI = lListIndex == CB_ERR ? NULL : (AgcdUI *) m_csUIList.GetItemDataPtr(lListIndex);

		lListIndex = m_csUITypeList.GetCurSel();
		m_pstAction->m_uoAction.m_stUI.m_eType = lListIndex == CB_ERR ? AGCDUI_ACTION_UI_NONE : (AgcdUIActionUIType) m_csUITypeList.GetItemData(lListIndex);

		m_pstAction->m_uoAction.m_stUI.m_bGroupAction = m_bGroupAction;
		m_pstAction->m_uoAction.m_stUI.m_bSmooth = m_bSmooth;

		m_pstAction->m_uoAction.m_stUI.m_lPosX = m_lPosX;
		m_pstAction->m_uoAction.m_stUI.m_lPosY = m_lPosY;

		lListIndex = m_csUIAttach.GetCurSel();
		m_pstAction->m_uoAction.m_stUI.m_pcsAttachUI = lListIndex == CB_ERR ? NULL : (AgcdUI *) m_csUIAttach.GetItemDataPtr(lListIndex);

		break;

	case AGCDUI_ACTION_CONTROL:
		lListIndex = m_csControlList.GetCurSel();

		if (lListIndex == CB_ERR)
			pcsControl = NULL;
		else
			pcsControl = (AgcdUIControl *) m_csControlList.GetItemDataPtr(lListIndex);

		if (pcsControl)
			m_pstAction->m_uoAction.m_stControl.m_pcsUI = pcsControl->m_pcsParentUI;

		m_pstAction->m_uoAction.m_stControl.m_pcsControl = pcsControl;

		m_pstAction->m_uoAction.m_stControl.m_lStatusNum = 0;

		lListIndex = m_csStatusCurrent1.GetItemData(m_csStatusCurrent1.GetCurSel());
		lListIndex2 = m_csStatusTarget1.GetItemData(m_csStatusTarget1.GetCurSel());
		if (lListIndex != CB_ERR || lListIndex2 != CB_ERR)
		{
			m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lCurrentStatus = lListIndex;
			m_pstAction->m_uoAction.m_stControl.m_astStatus[0].m_lTargetStatus = lListIndex2;
			m_pstAction->m_uoAction.m_stControl.m_lStatusNum = 1;
		}

		lListIndex = m_csStatusCurrent2.GetItemData(m_csStatusCurrent2.GetCurSel());
		lListIndex2 = m_csStatusTarget2.GetItemData(m_csStatusTarget2.GetCurSel());
		if (lListIndex != CB_ERR || lListIndex2 != CB_ERR)
		{
			m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lCurrentStatus = lListIndex;
			m_pstAction->m_uoAction.m_stControl.m_astStatus[1].m_lTargetStatus = lListIndex2;
			m_pstAction->m_uoAction.m_stControl.m_lStatusNum = 2;
		}

		lListIndex = m_csStatusCurrent2.GetItemData(m_csStatusCurrent3.GetCurSel());
		lListIndex2 = m_csStatusTarget2.GetItemData(m_csStatusTarget3.GetCurSel());
		if (lListIndex != CB_ERR || lListIndex2 != CB_ERR)
		{
			m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lCurrentStatus = lListIndex;
			m_pstAction->m_uoAction.m_stControl.m_astStatus[2].m_lTargetStatus = lListIndex2;
			m_pstAction->m_uoAction.m_stControl.m_lStatusNum = 3;
		}

		m_pstAction->m_uoAction.m_stControl.m_bSaveCurrent = m_bSaveCurrentStatus;

		break;

	case AGCDUI_ACTION_FUNCTION:
		ApBase *	pcsBase;
		INT32		lIndex;

		lListIndex = m_csFunctionList.GetCurSel();
		m_pstAction->m_uoAction.m_stFunction.m_pstFunction = lListIndex == -1 ? NULL : (AgcdUIFunction *) m_csFunctionList.GetItemDataPtr(lListIndex);

		for (lIndex = 0; lIndex < 5; ++lIndex)
		{
			lListIndex = m_apcsArguments[lIndex]->GetCurSel();
			pcsBase = (ApBase *) m_apcsArguments[lIndex]->GetItemDataPtr(lListIndex);
			if (pcsBase == (PVOID) -1)
				pcsBase = NULL;
			if (!pcsBase)
			{
				m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType = AGCDUI_ARG_TYPE_NONE;
			}
			else
			{
				if (pcsBase->m_eType == APBASE_TYPE_UI)
				{
					m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType = AGCDUI_ARG_TYPE_UI;
					m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pcsUI = lListIndex == -1 ? NULL : (AgcdUI *) m_apcsArguments[lIndex]->GetItemDataPtr(lListIndex);
				}
				else if (pcsBase->m_eType == APBASE_TYPE_UI_CONTROL)
				{
					m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType = AGCDUI_ARG_TYPE_CONTROL;
					m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_pstControl = lListIndex == -1 ? NULL : (AgcdUIControl *) m_apcsArguments[lIndex]->GetItemDataPtr(lListIndex);
				}
				else
					m_pstAction->m_uoAction.m_stFunction.m_astArgs[lIndex].m_eType = AGCDUI_ARG_TYPE_NONE;
			}
		}

		break;

	case AGCDUI_ACTION_MESSAGE:
		{
			lListIndex = m_csMessageType.GetCurSel();

			m_pstAction->m_uoAction.m_stMessage.m_eMessageType = (AgcdUIActionMessageType)m_csMessageType.GetItemData( lListIndex );
				
			UINT8 lColor[4] = { 255, 255, 255, 255 }	;
			CHAR szTemp[32]								;
			memset( szTemp, 0, sizeof( CHAR ) * 32 )	;
			
			m_csMessageColorR.GetWindowText( szTemp, 31 )		;
			lColor[0] = atoi( szTemp )							;

			m_csMessageColorG.GetWindowText( szTemp, 31 )		;
			lColor[1] = atoi( szTemp )							;

			m_csMessageColorB.GetWindowText( szTemp, 31 )		;
			lColor[2] = atoi( szTemp )							;

			m_csMessageColorA.GetWindowText( szTemp, 31 )		;
			lColor[3] = atoi( szTemp )							;
		
			m_pstAction->m_uoAction.m_stMessage.m_lColor = ( ( lColor[3] << 24 ) | ( lColor[0] << 16 ) | ( lColor[1] << 8 ) | lColor[2] );
			
			m_csMessage.GetWindowText( m_pstAction->m_uoAction.m_stMessage.m_szMessage, AGCDUIMANAGER2_MAX_ACTION_MESSAGE - 1 );
		}
		
		break;

	case AGCDUI_ACTION_SOUND:
		strcpy(m_pstAction->m_uoAction.m_stSound.m_szSound, m_strSound);

		break;
	}
	
	CDialog::OnOK();
}

void UITActionDialog::OnSelchangeUITFunctions() 
{
	INT32				lListIndex;
	AgcdUIFunction *	pstFunction = NULL;

	lListIndex = m_csFunctionList.GetCurSel();
	if (lListIndex != CB_ERR)
		pstFunction = (AgcdUIFunction *) m_csFunctionList.GetItemDataPtr(lListIndex);

	if (!pstFunction)
		m_strArgDescription = "";
	else
	{
		m_strArgDescription.Format("Arg No : %d\r\nArg1 : %s\r\nArg2 : %s\r\nArg3 : %s\r\nArg4 : %s\r\nArg5 : %s\r\n",
								   pstFunction->m_lNumData,
								   pstFunction->m_aszData[0] ? pstFunction->m_aszData[0] : "None",
								   pstFunction->m_aszData[1] ? pstFunction->m_aszData[1] : "None",
								   pstFunction->m_aszData[2] ? pstFunction->m_aszData[2] : "None",
								   pstFunction->m_aszData[3] ? pstFunction->m_aszData[3] : "None",
								   pstFunction->m_aszData[4] ? pstFunction->m_aszData[4] : "None");
	}

	UpdateData(FALSE);
}

void UITActionDialog::OnSelchangeUitActionMsType() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );

	INT32 lSelectIndex = m_csMessageType.GetCurSel();

	if ( AGCDUI_ACTION_MESSAGE_CHATTING_WINDOW == lSelectIndex )
	{
		m_csMessageColorR.EnableWindow( TRUE );
		m_csMessageColorG.EnableWindow( TRUE );
		m_csMessageColorB.EnableWindow( TRUE );
		m_csMessageColorA.EnableWindow( TRUE );
	}
	else 
	{
		m_csMessageColorR.EnableWindow( FALSE );
		m_csMessageColorG.EnableWindow( FALSE );
		m_csMessageColorB.EnableWindow( FALSE );
		m_csMessageColorA.EnableWindow( FALSE );		
	}
}

void UITActionDialog::OnUITActionAddVariable() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	CString				strTemp;
	INT32				lCurPos;

	UpdateData(TRUE);

	pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());
	if (pstUserData == (VOID *) -1)
		pstUserData = NULL;

	pstDisplay = (AgcdUIDisplay *) m_csDisplayList.GetItemDataPtr(m_csDisplayList.GetCurSel());
	if (pstDisplay == (VOID *) -1)
		pstDisplay = NULL;

	if (!pstDisplay)
		return;

	lCurPos = m_csMessage.CharFromPos(m_csMessage.GetCaretPos());
	if (lCurPos > m_strMessage.GetLength())
		return;

	strTemp = m_strMessage.Mid(lCurPos);
	m_strMessage = m_strMessage.Left(lCurPos);

	m_strMessage += AGCMUIMANAGER2_VARIABLE_START;
	if (pstUserData)
		m_strMessage += pstUserData->m_szName;
	m_strMessage += AGCMUIMANAGER2_VARIABLE_DELIMITER;
	m_strMessage += pstDisplay->m_szName;
	m_strMessage +=AGCMUIMANAGER2_VARIABLE_END;

	m_strMessage += strTemp;

	UpdateData(FALSE);
}

void UITActionDialog::OnSelchangeUITActionUDList() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNONE = "<None>";

	pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());

	if (pstUserData == (VOID *) -1)
		return;

	m_csDisplayList.ResetContent();

	lListIndex = m_csDisplayList.AddString(szNONE);
	m_csDisplayList.SetItemDataPtr(lListIndex, NULL);
	m_csDisplayList.SetCurSel(lListIndex);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetDisplayCount(); ++lIndex)
	{
		pstDisplay = g_pcsAgcmUIManager2->GetDisplay(lIndex);
		if (!pstDisplay)
			continue;

		if (pstUserData && !(pstUserData->m_eType & pstDisplay->m_ulDataType))
			continue;

		lListIndex = m_csDisplayList.AddString(pstDisplay->m_szName);
		if (lListIndex == CB_ERR)
			return;

		m_csDisplayList.SetItemDataPtr(lListIndex, pstDisplay);
	}
}

void UITActionDialog::OnUITSoundBrowse() 
{
	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.wav", this);

	if (dlgFile.DoModal() == IDOK)
	{
		m_strSound = dlgFile.GetFileName();
	}

	UpdateData(FALSE);
}
