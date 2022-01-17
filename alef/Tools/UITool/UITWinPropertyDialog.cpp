// UITWinPropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "MainFrm.h"
#include "UITWinPropertyDialog.h"

#include "UITMessageMapDialog.h"
#include "UITDisplayMapDialog.h"
#include "UITStatusDialog.h"

#include "MyEngine.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWinPropertyDialog dialog


IMPLEMENT_DYNCREATE(UITWinPropertyDialog, CPropertyPage)

UITWinPropertyDialog::UITWinPropertyDialog() : CPropertyPage(UITWinPropertyDialog::IDD)
{
	//{{AFX_DATA_INIT(UITWinPropertyDialog)
	m_strName = _T("");
	m_lX = 0;
	m_lY = 0;
	m_ulW = 0;
	m_ulH = 0;
	m_cRed = 0;
	m_cGreen = 0;
	m_cBlue = 0;
	m_cAlpha = 0;
	//}}AFX_DATA_INIT

	m_pcsBase	= NULL	;
	m_szName	= NULL	;
	m_pcsUI		= NULL	;
	m_pcsControl= NULL	;
}


UITWinPropertyDialog::~UITWinPropertyDialog()
{
}


void UITWinPropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWinPropertyDialog)
	DDX_Control(pDX, IDC_UIT_TEXTURE_LIST, m_csTextureList);
	DDX_Text(pDX, IDC_UIT_WINDOW_NAME, m_strName);
	DDX_Text(pDX, IDC_UIT_WINDOW_X, m_lX);
	DDX_Text(pDX, IDC_UIT_WINDOW_Y, m_lY);
	DDX_Text(pDX, IDC_UIT_WINDOW_W, m_ulW);
	DDX_Text(pDX, IDC_UIT_WINDOW_H, m_ulH);
	DDX_Text(pDX, IDC_UIT_WINDOW_RED, m_cRed);
	DDX_Text(pDX, IDC_UIT_WINDOW_GREEN, m_cGreen);
	DDX_Text(pDX, IDC_UIT_WINDOW_BLUE, m_cBlue);
	DDX_Text(pDX, IDC_UIT_WINDOW_ALPHA, m_cAlpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWinPropertyDialog, CPropertyPage)
	//{{AFX_MSG_MAP(UITWinPropertyDialog)
	ON_BN_CLICKED(IDC_UIT_ADD_TEXTURE, OnUITAddTexture)
	ON_BN_CLICKED(IDC_UIT_EDIT_TEXTURE, OnUITEditTexture)
	ON_BN_CLICKED(IDC_UIT_DELETE_TEXTURE, OnUITDeleteTexture)
	ON_BN_CLICKED(IDC_UIT_SET_TEXTURE, OnUITSetTexture)
	ON_BN_CLICKED(IDC_UIT_DELETE, OnUITDelete)
	ON_BN_CLICKED(IDC_UIT_SIZE_AS_IMAGE, OnUITSizeAsImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWinPropertyDialog message handlers

BOOL UITWinPropertyDialog::OnSetActive()
{
	UpdateDataToDialog(m_szName, m_pcsBase, m_pcsUI, m_pcsControl);

	return TRUE;
}

void UITWinPropertyDialog::OnOK() 
{
	UpdateData();

	if (m_szName && strcmp(m_szName, m_strName) && !g_MainWindow.ChangeName(m_strName))
	{
		AfxMessageBox("Can't Change UI Name !!!");
		return;
	}

	UpdateDataFromDialog();

	g_pMainFrame->UpdateUIList();

	UpdateDataToDialog(m_szName, m_pcsBase, m_pcsUI, m_pcsControl);
//	CPropertyPage::OnOK();
}

void UITWinPropertyDialog::OnCancel() 
{
	UpdateDataToDialog(m_szName, m_pcsBase, m_pcsUI, m_pcsControl);
//	CPropertyPage::OnCancel();
}

void UITWinPropertyDialog::OnUITAddTexture() 
{
	if (!m_pcsBase)
		return;

	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.bmp||*.png||*.gif||*.tif", this);

	if (dlgFile.DoModal() == IDOK)
	{
		CString		strPath;
		POSITION	posFiles = dlgFile.GetStartPosition();

		while (posFiles)
		{
			strPath = dlgFile.GetNextPathName(posFiles);
			m_csTextures.AddImage((LPSTR) (LPCTSTR) strPath.Mid(strPath.ReverseFind('\\') + 1));
		}

		if (!RefreshTextureList())
		{
			AfxMessageBox("Error Refresh TextureList !!!");
		}
	}
}

void UITWinPropertyDialog::OnUITEditTexture() 
{
	if (!m_pcsBase)
		return;

	INT32		lListIndex;
	INT32		lIndex;
	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.bmp||*.png||*.gif||*.tif", this);
	POSITION	pos = m_csTextureList.GetFirstSelectedItemPosition();

	if (pos)
	{
		if (dlgFile.DoModal() == IDOK)
		{
			lListIndex = m_csTextureList.GetNextSelectedItem(pos);
			lIndex = m_csTextureList.GetItemData(lListIndex);

			m_csTextures.SetImage_ID(lIndex, (LPSTR) (LPCTSTR) dlgFile.GetFileName());

			if (!RefreshTextureList())
			{
				AfxMessageBox("Error Refresh TextureList !!!");
			}
		}
	}
}

void UITWinPropertyDialog::OnUITDeleteTexture()
{
	if (!m_pcsBase)
		return;

	INT32		lIndex;
	INT32		lListIndex;
	POSITION	pos = m_csTextureList.GetFirstSelectedItemPosition();

	if (pos)
	{
		lListIndex = m_csTextureList.GetNextSelectedItem(pos);
		lIndex = m_csTextureList.GetItemData(lListIndex);

		m_csTextures.DeleteImage_ID(lIndex);

		if (!RefreshTextureList())
		{
			AfxMessageBox("Error Refresh TextureList !!!");
		}
	}
}

BOOL UITWinPropertyDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_csTextureList.InsertColumn(0, "No", LVCFMT_LEFT, 30);
	m_csTextureList.InsertColumn(1, "Name", LVCFMT_LEFT, 165);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL UITWinPropertyDialog::UpdateDataToDialog(CHAR *szName, AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_szName		= szName;
	m_pcsBase		= pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd())
		return TRUE;

	if (szName)
		m_strName = szName;
	else
		m_strName = "";

	if (pcsBase)
	{
		DWORD			lColor;

		m_lX = pcsBase->x;
		m_lY = pcsBase->y;
		m_ulW = pcsBase->w;
		m_ulH = pcsBase->h;

		m_csTextures = pcsBase->m_csTextureList;
		/*
		for (lIndex = 0; lIndex < pcsBase->GetImageCount(); ++lIndex)
		{
			pstTexture = pcsBase->GetImage(lIndex);

			sprintf(szTemp, "%d", lIndex);
			lListIndex = m_csTextureList.InsertItem(lIndex, szTemp);
			if (lListIndex == -1)
				return FALSE;

			m_csTextureList.SetItemText(lListIndex, 1, RwTextureGetName(pstTexture));
			m_csTextureList.SetItemData(lListIndex, lIndex);

			++m_lTextureCount;
		}
		*/

		lColor = pcsBase->GetRenderColor();

		m_cRed		= (UINT8) ((lColor & 0x00ff0000) >> 16);
		m_cGreen	= (UINT8) ((lColor & 0x0000ff00) >> 8);
		m_cBlue		= (UINT8) ((lColor & 0x000000ff));
		m_cAlpha	= pcsBase->GetRenderAlpha();
	}
	else
	{
		m_lX = 0;
		m_lY = 0;
		m_ulW = 0;
		m_ulH = 0;

		m_csTextureList.DeleteAllItems();

		m_cRed = 0;
		m_cGreen = 0;
		m_cBlue = 0;
		m_cAlpha = 0;
	}

	UpdateData(FALSE);
	RefreshTextureList();

	return TRUE;
}

BOOL UITWinPropertyDialog::UpdateDataFromDialog()
{
	if (!m_pcsBase)
		return FALSE;

	if (!GetSafeHwnd())
		return TRUE;

	UpdateData();

	if (m_szName)
	{
		strncpy(m_szName, m_strName, AGCDUIMANAGER2_MAX_NAME);
		m_szName[AGCDUIMANAGER2_MAX_NAME - 1] = 0;
	}

	if (m_pcsBase)
	{
		m_pcsBase->MoveWindow(m_lX, m_lY, m_ulW, m_ulH);

		m_pcsBase->m_csTextureList = m_csTextures;
		/*
		m_pcsBase->DestroyTextureList();

		lListIndex = m_csTextureList.GetNextItem(-1, LVNI_ALL);
		while (lListIndex != -1)
		{
			m_csTextureList.GetItemText(lListIndex, 1, szName, 128);

			m_pcsBase->AddImage(szName);

			lListIndex = m_csTextureList.GetNextItem(lListIndex, LVNI_ALL);
		}
		*/

		m_pcsBase->SetRenderColor(	(m_cRed << 16) |
									(m_cGreen << 8) |
									(m_cBlue) );
		m_pcsBase->SetRenderAlpha(m_cAlpha);
	}

	return TRUE;
}

BOOL UITWinPropertyDialog::RefreshTextureList()
{
	if (!m_pcsBase)
		return FALSE;

	INT32			lIndex;
	INT32			lListIndex;
	INT32			lTextureID;
	RwTexture *		pstTexture;
	CHAR			szTemp[64];

	m_csTextureList.DeleteAllItems();

	for (lIndex = 0; lIndex < m_csTextures.GetCount(); ++lIndex)
	{
		pstTexture = m_csTextures.GetImage_Index(lIndex, &lTextureID);

		sprintf(szTemp, "%d", lTextureID);
		lListIndex = m_csTextureList.InsertItem(lIndex, szTemp);
		if (lListIndex == -1)
			return FALSE;

		if (pstTexture)
			m_csTextureList.SetItemText(lListIndex, 1, RwTextureGetName(pstTexture));

		m_csTextureList.SetItemData(lListIndex, lTextureID);
	}

	return TRUE;
}
void UITWinPropertyDialog::OnUITSetTexture() 
{
	if (!m_pcsBase)
		return;

	INT32		lImageID;
	POSITION	pos = m_csTextureList.GetFirstSelectedItemPosition();

	if (pos)
	{
		lImageID = m_csTextureList.GetItemData(m_csTextureList.GetNextSelectedItem(pos));
		m_pcsBase->SetDefaultRenderTexture(lImageID);
		m_pcsBase->SetRednerTexture(lImageID);
	}
}

void UITWinPropertyDialog::OnUITDelete() 
{
	if (m_pcsControl)
		g_MainWindow.DeleteControl(m_pcsControl);
	else if (m_pcsUI && m_pcsBase == m_pcsUI->m_pcsUIWindow)
		g_MainWindow.DeleteUI(m_pcsUI);
}

void UITWinPropertyDialog::OnUITSizeAsImage() 
{
	if (!m_pcsBase)
		return;

	RwTexture *	pstTexture = m_pcsBase->m_csTextureList.GetImage_ID(m_pcsBase->GetRenderTexture());
	if (!pstTexture)
		return;

	RwRaster *	pstRaster = RwTextureGetRaster(pstTexture);
	if (!pstRaster)
		return;

	m_pcsBase->MoveWindow(m_pcsBase->x, m_pcsBase->y, RwRasterGetWidth(pstRaster), RwRasterGetHeight(pstRaster));

	UpdateDataToDialog(m_szName, m_pcsBase, m_pcsUI, m_pcsControl);
}
