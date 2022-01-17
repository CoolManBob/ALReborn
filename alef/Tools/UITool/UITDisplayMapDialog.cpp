// UITDisplayMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UITDisplayMapDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITDisplayMapDialog dialog

CHAR *g_aszDisplayType[AGCDUI_MAX_DISPLAYMAP] = 
{
	"None",
	"Static",
	"Variable",
};

extern CHAR *	g_aszTextHAlign[];
extern CHAR *	g_aszTextVAlign[];


UITDisplayMapDialog::UITDisplayMapDialog(AgcdUIControl *pcsControl, CWnd* pParent /*=NULL*/)
	: CDialog(UITDisplayMapDialog::IDD, pParent)
	, m_bNumberComma(FALSE)
{
	//{{AFX_DATA_INIT(UITDisplayMapDialog)
	m_strCaption = _T("");
	m_cRed = 0;
	m_cGreen = 0;
	m_cBlue = 0;
	m_fScale = 0.0f;
	m_bShadow = FALSE;
	m_cAlpha = 0;
	//}}AFX_DATA_INIT

	m_pcsControl = pcsControl;
}


void UITDisplayMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITDisplayMapDialog)
	DDX_Control(pDX, IDC_UIT_FONT_VALIGN, m_csFontVAlign);
	DDX_Control(pDX, IDC_UIT_FONT_HALIGN, m_csFontHAlign);
	DDX_Control(pDX, IDC_UIT_FONT_TYPE, m_csFontList);
	DDX_Control(pDX, IDC_UIT_USER_DATA_LIST, m_csUserDataList);
	DDX_Control(pDX, IDC_UIT_DISPLAY_LIST, m_csVariableList);
	DDX_Control(pDX, IDC_UIT_CAPTION, m_csCaption);
	DDX_Control(pDX, IDC_UIT_DISPLAYMAP_TYPE, m_csDisplayType);
	DDX_Text(pDX, IDC_UIT_CAPTION, m_strCaption);
	DDX_Text(pDX, IDC_UIT_FONT_RED, m_cRed);
	DDX_Text(pDX, IDC_UIT_FONT_GREEN, m_cGreen);
	DDX_Text(pDX, IDC_UIT_FONT_BLUE, m_cBlue);
	DDX_Text(pDX, IDC_UIT_FONT_RED2, m_fScale);
	DDX_Check(pDX, IDC_UIT_FONT_SHADOW, m_bShadow);
	DDX_Text(pDX, IDC_UIT_FONT_ALPHA, m_cAlpha);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_UIT_FONT_COMMA, m_bNumberComma);
}


BEGIN_MESSAGE_MAP(UITDisplayMapDialog, CDialog)
	//{{AFX_MSG_MAP(UITDisplayMapDialog)
	ON_BN_CLICKED(IDC_UIT_UPDATE_DISPLAY_MAP, OnUITUpdateDisplayMap)
	ON_CBN_SELCHANGE(IDC_UIT_DISPLAYMAP_TYPE, OnSelchangeUITDisplaymapType)
	ON_CBN_SELCHANGE(IDC_UIT_USER_DATA_LIST, OnSelchangeUITUserDataList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITDisplayMapDialog message handlers

BOOL UITDisplayMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplayMap *	pstDisplayMap;
	FontType *			pstFont;
	INT32				lImageFont;
	CHAR				szTemp[128];
	static CHAR *		szNONE = "<None>";
	static CHAR *		szUSERDATA = "<User Data>";

	for (lIndex = 0; lIndex < AGCDUI_MAX_DISPLAYMAP; ++lIndex)
	{
		lListIndex = m_csDisplayType.AddString(g_aszDisplayType[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csDisplayType.SetItemData(lListIndex, lIndex);
	}

	lListIndex = m_csUserDataList.AddString(szNONE);
	m_csUserDataList.SetItemDataPtr(lListIndex, NULL);
	m_csUserDataList.SetCurSel(lListIndex);

	lListIndex = m_csUserDataList.AddString(szUSERDATA);
	m_csUserDataList.SetItemDataPtr(lListIndex, (VOID *) 1);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (!pstUserData)
			continue;

		lListIndex = m_csUserDataList.AddString(pstUserData->m_szName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUserDataList.SetItemDataPtr(lListIndex, pstUserData);
	}

	lIndex = 0;
	for (pstFont = g_pcsAgcmFont->GetSequenceFont(&lIndex); pstFont; pstFont = g_pcsAgcmFont->GetSequenceFont(&lIndex))
	{
		sprintf(szTemp, "%d : %s %d", pstFont->lID, pstFont->szType, pstFont->lSize);
		lListIndex = m_csFontList.AddString(szTemp);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csFontList.SetItemData(lListIndex, pstFont->lID);
	}

	lIndex = 0;
	for (lImageFont = g_pcsAgcmFont->GetSequenceImageFont(&lIndex); lImageFont != -1; lImageFont = g_pcsAgcmFont->GetSequenceImageFont(&lIndex))
	{
		sprintf(szTemp, "%d : ImageFont %d", AGCM_FONT_MAX_TYPE_NUM + lImageFont, lImageFont);
		lListIndex = m_csFontList.AddString(szTemp);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csFontList.SetItemData(lListIndex, AGCM_FONT_MAX_TYPE_NUM + lImageFont);
	}

	for (lIndex = 0; lIndex < ACUIBASE_MAX_HALIGN; ++lIndex)
	{
		if (!g_aszTextHAlign[lIndex])
			continue;

		lListIndex = m_csFontHAlign.AddString(g_aszTextHAlign[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csFontHAlign.SetItemData(lListIndex, lIndex);
	}

	for (lIndex = 0; lIndex < ACUIBASE_MAX_VALIGN; ++lIndex)
	{
		if (!g_aszTextVAlign[lIndex])
			continue;

		lListIndex = m_csFontVAlign.AddString(g_aszTextVAlign[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csFontVAlign.SetItemData(lListIndex, lIndex);
	}

	pstDisplayMap = &m_pcsControl->m_stDisplayMap;

	m_csDisplayType.SelectString(-1, g_aszDisplayType[pstDisplayMap->m_eType]);
	if (pstDisplayMap->m_eType == AGCDUI_DISPLAYMAP_TYPE_STATIC)
	{
		m_strCaption = pstDisplayMap->m_uoDisplay.m_szStatic;
	}
	else if (pstDisplayMap->m_eType == AGCDUI_DISPLAYMAP_TYPE_VARIABLE)
	{
		if (pstDisplayMap->m_uoDisplay.m_stVariable.m_bControlUserData)
			m_csUserDataList.SelectString(-1, szUSERDATA);
		else if (pstDisplayMap->m_uoDisplay.m_stVariable.m_pstUserData)
			m_csUserDataList.SelectString(-1, pstDisplayMap->m_uoDisplay.m_stVariable.m_pstUserData->m_szName);

		OnSelchangeUITUserDataList();

		if (pstDisplayMap->m_uoDisplay.m_stVariable.m_pstDisplay)
			m_csVariableList.SelectString(-1, pstDisplayMap->m_uoDisplay.m_stVariable.m_pstDisplay->m_szName);
	}

	m_csFontHAlign.SelectString(-1, g_aszTextHAlign[pstDisplayMap->m_stFont.m_eHAlign]);
	m_csFontVAlign.SelectString(-1, g_aszTextVAlign[pstDisplayMap->m_stFont.m_eVAlign]);

	sprintf(szTemp, "%d", pstDisplayMap->m_stFont.m_lType + (pstDisplayMap->m_stFont.m_bImageNumber ? AGCM_FONT_MAX_TYPE_NUM : 0));
	m_csFontList.SelectString(-1, szTemp);

	m_cAlpha	= (pstDisplayMap->m_stFont.m_ulColor & 0xff000000) >> 24;
	m_cRed		= (pstDisplayMap->m_stFont.m_ulColor & 0x00ff0000) >> 16;
	m_cGreen	= (pstDisplayMap->m_stFont.m_ulColor & 0x0000ff00) >> 8;
	m_cBlue		= (pstDisplayMap->m_stFont.m_ulColor & 0x000000ff) >> 0;

	m_fScale	= pstDisplayMap->m_stFont.m_fScale;

	m_bShadow	= pstDisplayMap->m_stFont.m_bShadow;
	m_bNumberComma	= m_pcsControl->m_pcsBase->m_bStringNumberComma;

	OnSelchangeUITDisplaymapType();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITDisplayMapDialog::OnUITUpdateDisplayMap() 
{
	if (!m_pcsControl)
		return;

	INT32				lIndex;
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	AgcdUIDisplayMap *	pstDisplayMap = &m_pcsControl->m_stDisplayMap;

	UpdateData();

	lIndex = m_csFontList.GetItemData(m_csFontList.GetCurSel());
	if (lIndex == CB_ERR)
		lIndex = 0;
	pstDisplayMap->m_stFont.m_lType		= lIndex % AGCM_FONT_MAX_TYPE_NUM;

	pstDisplayMap->m_stFont.m_bImageNumber	= lIndex >= AGCM_FONT_MAX_TYPE_NUM;

	lIndex = m_csFontHAlign.GetItemData(m_csFontHAlign.GetCurSel());
	if (lIndex == CB_ERR)
		lIndex = 0;
	pstDisplayMap->m_stFont.m_eHAlign	= (AcUIBaseHAlign) lIndex;

	lIndex = m_csFontVAlign.GetItemData(m_csFontVAlign.GetCurSel());
	if (lIndex == CB_ERR)
		lIndex = 0;
	pstDisplayMap->m_stFont.m_eVAlign	= (AcUIBaseVAlign) lIndex;

	pstDisplayMap->m_stFont.m_ulColor	= (m_cAlpha << 24) | (m_cRed << 16) | (m_cGreen << 8) | m_cBlue;
	pstDisplayMap->m_stFont.m_fScale	= m_fScale;
	pstDisplayMap->m_stFont.m_bShadow	= m_bShadow;
	m_pcsControl->m_pcsBase->m_bStringNumberComma	= m_bNumberComma;

	g_pcsAgcmUIManager2->SetControlDisplayFont(m_pcsControl,
					pstDisplayMap->m_stFont.m_lType,
					pstDisplayMap->m_stFont.m_ulColor,
					pstDisplayMap->m_stFont.m_fScale,
					pstDisplayMap->m_stFont.m_bShadow,
					pstDisplayMap->m_stFont.m_eHAlign,
					pstDisplayMap->m_stFont.m_eVAlign,
					pstDisplayMap->m_stFont.m_bImageNumber);

	lIndex = m_csDisplayType.GetItemData(m_csDisplayType.GetCurSel());
	if (lIndex == CB_ERR)
		lIndex = 0;

	if (lIndex == AGCDUI_DISPLAYMAP_TYPE_NONE)
	{
		g_pcsAgcmUIManager2->ResetControlDisplayMap(m_pcsControl);
	}
	else if (lIndex == AGCDUI_DISPLAYMAP_TYPE_STATIC)
	{
		g_pcsAgcmUIManager2->SetControlDisplayMap(m_pcsControl, (LPSTR) (LPCTSTR) m_strCaption);
	}
	else if (lIndex == AGCDUI_DISPLAYMAP_TYPE_VARIABLE)
	{
		BOOL	bControlUserData;

		pstDisplay = (AgcdUIDisplay *) m_csVariableList.GetItemDataPtr(m_csVariableList.GetCurSel());
		pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());


		if (pstUserData == (VOID *) -1)
			pstUserData = NULL;

		if (pstUserData == (VOID *) 1)
		{
			pstUserData = m_pcsControl->m_pstUserData;

			bControlUserData = TRUE;
		}
		else
			bControlUserData = FALSE;

		if (pstDisplay == (VOID *) -1)
			g_pcsAgcmUIManager2->ResetControlDisplayMap(m_pcsControl);
		else
			g_pcsAgcmUIManager2->SetControlDisplayMap(m_pcsControl, pstUserData, pstDisplay, bControlUserData);
	}

	g_pcsAgcmUIManager2->CountUserData();
}

void UITDisplayMapDialog::OnSelchangeUITDisplaymapType() 
{
	INT32				lIndex;

	lIndex = m_csDisplayType.GetItemData(m_csDisplayType.GetCurSel());

	m_csCaption.EnableWindow(FALSE);
	m_csVariableList.EnableWindow(FALSE);
	m_csUserDataList.EnableWindow(FALSE);

	if (lIndex == AGCDUI_DISPLAYMAP_TYPE_STATIC)
	{
		m_csCaption.EnableWindow(TRUE);
	}
	else if (lIndex == AGCDUI_DISPLAYMAP_TYPE_VARIABLE)
	{
		m_csVariableList.EnableWindow(TRUE);
		m_csUserDataList.EnableWindow(TRUE);
	}
}

void UITDisplayMapDialog::OnSelchangeUITUserDataList() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNONE = "<None>";

	pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());

	if (pstUserData == (VOID *) -1)
		pstUserData = NULL;

	if (pstUserData == (VOID *) 1)
		pstUserData = m_pcsControl->m_pstUserData;

	m_csVariableList.ResetContent();

	lListIndex = m_csVariableList.AddString(szNONE);
	m_csVariableList.SetItemDataPtr(lListIndex, NULL);
	m_csVariableList.SetCurSel(lListIndex);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetDisplayCount(); ++lIndex)
	{
		pstDisplay = g_pcsAgcmUIManager2->GetDisplay(lIndex);
		if (!pstDisplay)
			continue;

		if (pstUserData && !(pstUserData->m_eType & pstDisplay->m_ulDataType))
			continue;

		lListIndex = m_csVariableList.AddString(pstDisplay->m_szName);
		if (lListIndex == CB_ERR)
			return;

		m_csVariableList.SetItemDataPtr(lListIndex, pstDisplay);
	}
}
