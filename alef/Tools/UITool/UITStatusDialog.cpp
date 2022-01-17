// UITStatusDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITStatusDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITStatusDialog dialog


UITStatusDialog::UITStatusDialog(AcUIBase *pcsBase, AgcdUIControl *pcsControl, CWnd* pParent /*=NULL*/)
	: CDialog(UITStatusDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITStatusDialog)
	m_strName = _T("");
	m_lX = pcsBase->x;
	m_lY = pcsBase->y;
	m_lW = pcsBase->w;
	m_lH = pcsBase->h;
	m_bVisible = FALSE;
	m_cRed = 0;
	m_cGreen = 0;
	m_cBlue = 0;
	//}}AFX_DATA_INIT

	m_pcsBase = pcsBase;
	m_pcsControl = pcsControl;

	memset(m_astStatus, 0, sizeof(stStatusInfo) * ACUIBASE_STATUS_MAX_NUM);
}


void UITStatusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITStatusDialog)
	DDX_Control(pDX, IDC_UIT_USERDATA_LIST, m_csUserDataList);
	DDX_Control(pDX, IDC_UIT_TEXTURE_LIST, m_csTextureList);
	DDX_Control(pDX, IDC_UIT_STATUS_LIST, m_csStatusList);
	DDX_Text(pDX, IDC_UIT_STATUS_NAME, m_strName);
	DDX_Text(pDX, IDC_UIT_WINDOW_X, m_lX);
	DDX_Text(pDX, IDC_UIT_WINDOW_Y, m_lY);
	DDX_Text(pDX, IDC_UIT_WINDOW_W, m_lW);
	DDX_Text(pDX, IDC_UIT_WINDOW_H, m_lH);
	DDX_Check(pDX, IDC_UIT_WINDOW_VISIBLE, m_bVisible);
	DDX_Text(pDX, IDC_UIT_STRING_COLOR_R, m_cRed);
	DDX_Text(pDX, IDC_UIT_STRING_COLOR_G, m_cGreen);
	DDX_Text(pDX, IDC_UIT_STRING_COLOR_B, m_cBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITStatusDialog, CDialog)
	//{{AFX_MSG_MAP(UITStatusDialog)
	ON_BN_CLICKED(IDC_UIT_ADD_STATUS, OnUITAddStatus)
	ON_BN_CLICKED(IDC_UIT_UPDATE_STATUS, OnUITUpdateStatus)
	ON_BN_CLICKED(IDC_UIT_DELETE_STATUS, OnUITDeleteStatus)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UIT_STATUS_LIST, OnItemchangedUITStatusList)
	ON_BN_CLICKED(IDC_UIT_SET_DEFAULT_STATUS, OnUITSetDefaultStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITStatusDialog message handlers

void UITStatusDialog::OnUITAddStatus() 
{
	UpdateData();

	INT32				lIndex;
	INT32				lTextureIndex;
	AgcdUIUserData *	pstUserData;

	lTextureIndex = m_csTextureList.GetItemData(m_csTextureList.GetCurSel());
	if (lTextureIndex == -1)
	{
		// Texture를 Set하지 않았을땐 어떻게 처리할까?
	}

	for (lIndex = 0; lIndex < ACUIBASE_STATUS_MAX_NUM; ++lIndex)
	{
		if (!m_astStatus[lIndex].m_bSetting)		// 비었으면,
			break;
	}

	if (lIndex >= ACUIBASE_STATUS_MAX_NUM)
	{
		AfxMessageBox("Status Full !!!");
		return;
	}

	m_astStatus[lIndex].m_bSetting = TRUE;
	if ( m_astStatus[lIndex].m_lStatusID < 0 )
		m_astStatus[lIndex].m_lStatusID = lIndex;
	else
	{
		m_astStatus[lIndex].m_lStatusID += ACUIBASE_STATUS_ID_BIT_CONSTANT;

		if (m_astStatus[lIndex].m_lStatusID > ACUIBASE_STATUS_ID_MAX)
			m_astStatus[lIndex].m_lStatusID = lIndex;
	}

	m_astStatus[lIndex].m_rectStatusPos.x = m_lX;
	m_astStatus[lIndex].m_rectStatusPos.y = m_lY;
	m_astStatus[lIndex].m_rectStatusPos.w = m_lW;
	m_astStatus[lIndex].m_rectStatusPos.h = m_lH;

	m_astStatus[lIndex].m_lStatusImageID = lTextureIndex;

	strncpy(m_astStatus[lIndex].m_szStatusName, (LPSTR) (LPCTSTR) m_strName, ACUIBASE_STATUS_MAX_NUM);
	m_astStatus[lIndex].m_szStatusName[ACUIBASE_STATUS_NAME_LENGTH - 1] = 0;

	m_astStatus[lIndex].m_bVisible = m_bVisible;

	m_astStatus[lIndex].m_lStringColor = (0xff << 24) | (m_cRed << 16) | (m_cGreen << 8) | m_cBlue;

	pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());
	if (pstUserData == (PVOID) -1)
		pstUserData = NULL;

	m_apstUserData[lIndex] = pstUserData;

	RefreshData();
}

void UITStatusDialog::OnUITUpdateStatus() 
{
	UpdateData();

	POSITION	pos = m_csStatusList.GetFirstSelectedItemPosition();

	if (pos)
	{
		INT32				lStatusIndex;
		INT32				lIndex;
		stStatusInfo *		pstStatus;
		AgcdUIUserData *	pstUserData;

		lStatusIndex = m_csStatusList.GetItemData(m_csStatusList.GetNextSelectedItem(pos));
		if (lStatusIndex == -1)
		{
			AfxMessageBox("Select Status First !!!");
			return;
		}

		lIndex = m_csTextureList.GetItemData(m_csTextureList.GetCurSel());
		if (lIndex == -1)
		{
		}

		pstStatus = m_astStatus + lStatusIndex;
		if (!pstStatus)
		{
			AfxMessageBox("Error !!!");
			return;
		}

		strncpy(pstStatus->m_szStatusName, m_strName, ACUIBASE_STATUS_NAME_LENGTH);
		pstStatus->m_szStatusName[ACUIBASE_STATUS_NAME_LENGTH - 1] = 0;

		pstStatus->m_rectStatusPos.x = m_lX;
		pstStatus->m_rectStatusPos.y = m_lY;
		pstStatus->m_rectStatusPos.w = m_lW;
		pstStatus->m_rectStatusPos.h = m_lH;

		pstStatus->m_lStatusImageID = lIndex;

		pstStatus->m_bVisible		= m_bVisible;

		pstStatus->m_lStringColor = (0xff << 24) | (m_cRed << 16) | (m_cGreen << 8) | m_cBlue;

		lIndex = pstStatus - m_astStatus;

		pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());
		if (pstUserData == (PVOID) -1)
			pstUserData = NULL;

		m_apstUserData[lIndex] = pstUserData;

		RefreshData();
	}
}

void UITStatusDialog::OnUITDeleteStatus() 
{
	POSITION	pos = m_csStatusList.GetFirstSelectedItemPosition();

	if (pos)
	{
		INT32			lStatusIndex;

		lStatusIndex = m_csStatusList.GetItemData(m_csStatusList.GetNextSelectedItem(pos));
		if (lStatusIndex == -1)
		{
			AfxMessageBox("Select Status First !!!");
			return;
		}

		m_astStatus[lStatusIndex].m_bSetting = FALSE;

		RefreshData();
	}
}

BOOL UITStatusDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_csStatusList.InsertColumn(0, "Name", LVCFMT_LEFT, 100);
	m_csStatusList.InsertColumn(1, "Position", LVCFMT_LEFT, 80);
	m_csStatusList.InsertColumn(2, "Size", LVCFMT_LEFT, 80);
	m_csStatusList.InsertColumn(3, "Texture", LVCFMT_LEFT, 135);
	m_csStatusList.InsertColumn(4, "Default", LVCFMT_LEFT, 10);

	if (!m_pcsControl)
		m_csUserDataList.EnableWindow(FALSE);

	memcpy(m_astStatus, m_pcsBase->m_astStatus, sizeof(stStatusInfo) * ACUIBASE_STATUS_MAX_NUM);

	if (m_pcsControl)
		memcpy(m_apstUserData, m_pcsControl->m_apstUserData, sizeof(AgcdUIUserData *) * ACUIBASE_STATUS_MAX_NUM);
	else
		memset(m_apstUserData, 0, sizeof(AgcdUIUserData *) * ACUIBASE_STATUS_MAX_NUM);

	return RefreshData();
}

BOOL UITStatusDialog::RefreshData()
{
	INT32				lIndex;
	INT32				lListIndex;
	INT32				lImageID;
	stStatusInfo *		pstStatus;
	CHAR				szTemp[64];
	RwTexture *			pstTexture;
	AgcdUIUserData *	pstUserData;
	CHAR *				szTextureName;
	static CHAR *		szNULL = "<NULL>";
	static CHAR *		szNoChange = "<NoChange>";
	static CHAR *		szBlank = "<Blank>";

	m_csStatusList.DeleteAllItems();
	for (lIndex = 0; lIndex < ACUIBASE_STATUS_MAX_NUM; ++lIndex)
	{
		pstStatus = m_astStatus + lIndex;
		if (!pstStatus->m_bSetting)
			continue;

		szTextureName = m_pcsBase->m_csTextureList.GetImageName_ID(pstStatus->m_lStatusImageID);
		lListIndex = m_csStatusList.InsertItem(lIndex, pstStatus->m_szStatusName);
		if (lListIndex == -1)
			return FALSE;

		sprintf(szTemp, "%d,%d", pstStatus->m_rectStatusPos.x, pstStatus->m_rectStatusPos.y);
		m_csStatusList.SetItemText(lListIndex, 1, szTemp);

		sprintf(szTemp, "%d,%d", pstStatus->m_rectStatusPos.w, pstStatus->m_rectStatusPos.h);
		m_csStatusList.SetItemText(lListIndex, 2, szTemp);

		if (szTextureName)
			m_csStatusList.SetItemText(lListIndex, 3, szTextureName);
		else if (pstStatus->m_lStatusImageID < 0)
			m_csStatusList.SetItemText(lListIndex, 3, szNoChange);
		else
			m_csStatusList.SetItemText(lListIndex, 3, szBlank);

		m_csStatusList.SetItemData(lListIndex, lIndex);

		if (lIndex == m_pcsControl->m_pcsBase->m_lDefaultStatusID)
		{
			m_csStatusList.SetItemText(lListIndex, 4, "*");
		}
	}

	m_csTextureList.ResetContent();

	lListIndex = m_csTextureList.AddString(szBlank);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csTextureList.SetItemData(lListIndex, 0);

	lListIndex = m_csTextureList.AddString(szNoChange);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csTextureList.SetItemData(lListIndex, -1);

	for (lIndex = 0; lIndex < m_pcsBase->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsBase->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csTextureList.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csTextureList.SetItemData(lListIndex, lImageID);
	}

	m_csUserDataList.ResetContent();

	lListIndex = m_csUserDataList.AddString(szNULL);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csUserDataList.SetItemDataPtr(lListIndex, NULL);

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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITStatusDialog::OnOK() 
{
	memcpy(m_pcsBase->m_astStatus, m_astStatus, sizeof(stStatusInfo) * ACUIBASE_STATUS_MAX_NUM);

	if (m_pcsControl)
		memcpy(m_pcsControl->m_apstUserData, m_apstUserData, sizeof(AgcdUIUserData *) * ACUIBASE_STATUS_MAX_NUM);
	
	CDialog::OnOK();
}

void UITStatusDialog::OnItemchangedUITStatusList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION	pos = m_csStatusList.GetFirstSelectedItemPosition();

	if (pos)
	{
		INT32				lStatusIndex;
		stStatusInfo *		pstStatus;
		RwTexture *			pstTexture;
		static CHAR *		szNoChange = "<NoChange>";
		static CHAR *		szNULL = "<NULL>";
		static CHAR *		szBlank = "<Blank>";

		lStatusIndex = m_csStatusList.GetItemData(m_csStatusList.GetNextSelectedItem(pos));
		if (lStatusIndex == -1)
		{
			AfxMessageBox("Select Status First !!!");
			return;
		}

		pstStatus = m_astStatus + lStatusIndex;
		if (!pstStatus)
		{
			AfxMessageBox("Error Getting Status Info !!!");
			return;
		}

		m_strName = pstStatus->m_szStatusName;

		m_lX = pstStatus->m_rectStatusPos.x;
		m_lY = pstStatus->m_rectStatusPos.y;
		m_lW = pstStatus->m_rectStatusPos.w;
		m_lH = pstStatus->m_rectStatusPos.h;

		pstTexture = m_pcsBase->m_csTextureList.GetImage_ID(pstStatus->m_lStatusImageID);
		if (pstTexture)
			m_csTextureList.SelectString(-1, RwTextureGetName(pstTexture));
		else if (pstStatus->m_lStatusImageID == 0)
			m_csTextureList.SelectString(-1, szBlank);
		else
			m_csTextureList.SelectString(-1, szNoChange);

		m_bVisible = pstStatus->m_bVisible;

		m_cRed		= (pstStatus->m_lStringColor >> 16) & 0xff;
		m_cGreen	= (pstStatus->m_lStringColor >> 8) & 0xff;
		m_cBlue		= (pstStatus->m_lStringColor >> 0) & 0xff;

		if (m_pcsControl)
		{
			if (m_apstUserData[lStatusIndex])
				m_csUserDataList.SelectString(-1, m_apstUserData[lStatusIndex]->m_szName);
			else
				m_csUserDataList.SetCurSel(-1);
		}

		UpdateData(FALSE);
	}
	
	*pResult = 0;
}

void UITStatusDialog::OnUITSetDefaultStatus() 
{
	POSITION	pos = m_csStatusList.GetFirstSelectedItemPosition();

	if (pos)
	{
		INT32			lStatusIndex;

		lStatusIndex = m_csStatusList.GetItemData(m_csStatusList.GetNextSelectedItem(pos));
		if (lStatusIndex == -1)
		{
			AfxMessageBox("Select Status First !!!");
			return;
		}

		m_pcsBase->SetDefaultStatus(m_astStatus[lStatusIndex].m_lStatusID);

		{
			m_csStatusList.SetItemText(lStatusIndex, 4, "*");
		}
	}
}
