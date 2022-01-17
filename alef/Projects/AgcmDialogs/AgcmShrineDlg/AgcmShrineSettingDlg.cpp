// AgcmShrineSettingDlg.cpp : implementation file
//

#include "stdafx.h"

#include "AgpmSkill.h"

#include "AgcmShrineDlg.h"
#include "AgcmShrineSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmShrineSettingDlg dialog


AgcmShrineSettingDlg::AgcmShrineSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmShrineSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmShrineSettingDlg)
	m_strShrineName = _T("");
	m_unShrineLevel = 0;
	m_strSkillName = _T("");
	m_bIsRandomPosition = FALSE;
	m_strActiveCondition = _T("");
	m_strReactiveCondition = _T("");
	m_strUseCondition = _T("");
	m_unMinActiveTime = 0;
	m_unMaxActiveTime = 0;
	m_strPosition = _T("");
	m_unUseInterval = 0;
	//}}AFX_DATA_INIT

	m_pcsAgcmShrineDlg		= NULL;

	m_pcsShrineTemplate		= NULL;
}


void AgcmShrineSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmShrineSettingDlg)
	DDX_Control(pDX, IDC_LIST_SKILL_LIST, m_csListSkillList);
	DDX_Control(pDX, IDC_LIST_POSITION, m_csListPosition);
	DDX_Control(pDX, IDC_COMBO_USE_CONDITION, m_csComboUseCondition);
	DDX_Control(pDX, IDC_COMBO_REACTIVE_CONDITION, m_csComboReactiveCondition);
	DDX_Control(pDX, IDC_COMBO_ACTIVE_CONDITION, m_csComboActiveCondition);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strShrineName);
	DDV_MaxChars(pDX, m_strShrineName, AGPMSHRINE_MAX_NAME);
	DDX_Text(pDX, IDC_EDIT_LEVEL, m_unShrineLevel);
	DDV_MinMaxUInt(pDX, m_unShrineLevel, 0, AGPMSHRINE_MAX_LEVEL);
	DDX_Text(pDX, IDC_EDIT_SKILL_NAME, m_strSkillName);
	DDV_MaxChars(pDX, m_strSkillName, AGPMSKILL_MAX_SKILL_NAME);
	DDX_Check(pDX, IDC_CHECK_RANDOM_POS, m_bIsRandomPosition);
	DDX_Text(pDX, IDC_EDIT_MIN_ACTIVE_TIME, m_unMinActiveTime);
	DDV_MinMaxUInt(pDX, m_unMinActiveTime, 0, 500);
	DDX_Text(pDX, IDC_EDIT_MAX_ACTIVE_TIME, m_unMaxActiveTime);
	DDX_Text(pDX, IDC_EDIT_POSITION, m_strPosition);
	DDX_Text(pDX, IDC_EDIT_USE_INTERVAL, m_unUseInterval);
	DDV_MinMaxUInt(pDX, m_unUseInterval, 0, 500);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmShrineSettingDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmShrineSettingDlg)
	ON_BN_CLICKED(IDOK, OnSave)
	ON_BN_CLICKED(IDC_BUTTON_POS_ADD, OnButtonPosAdd)
	ON_BN_CLICKED(IDC_BUTTON_POS_DELETE, OnButtonPosDelete)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_ADD, OnButtonSkillAdd)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DELETE, OnButtonSkillDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmShrineSettingDlg message handlers

void AgcmShrineSettingDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData();

	// 대화창에 세팅되어 있는 데이타를 모두 저장한다.
	/////////////////////////////////////////////////////////////////////////////

	strncpy(m_pcsShrineTemplate->m_szShrineName, m_strShrineName, AGPMSHRINE_MAX_NAME);

	int i;

	// skill list 저장
	for (i = 0; i < m_csListSkillList.GetCount(); ++i)
	{
		CHAR	buffer[64];
		ZeroMemory(buffer, sizeof(CHAR) * 64);

		m_csListSkillList.GetText(i, buffer);

		CHAR	szSkillName[AGPMSKILL_MAX_SKILL_NAME + 1];

		ZeroMemory(szSkillName, sizeof(CHAR) * (AGPMSKILL_MAX_SKILL_NAME + 1));

		INT16	nLevel = 0;
		sscanf(buffer, "%d", &nLevel);

		int j;
		for (j = 0; j < strlen(buffer); ++j)
		{
			if (buffer[j] == ':')
			{
				strcpy(szSkillName, buffer + j + 1);

				break;
			}
		}

		if (j == 0)
		{
			return;
		}

		strcpy(m_pcsShrineTemplate->m_szSkillName[nLevel - 1], szSkillName);
	}

	// shrine position 저장
	for (int i = 0; i < m_csListPosition.GetCount(); ++i)
	{
		CHAR	buffer[64];
		ZeroMemory(buffer, sizeof(CHAR) * 64);

		m_csListPosition.GetText(i, buffer);

		AuPOS	posTemp;

		sscanf(buffer, "%f;%f;%f", &posTemp.x, &posTemp.y, &posTemp.z);

		m_pcsShrineTemplate->m_posShrine[m_pcsShrineTemplate->m_nNumShrinePos] = posTemp;

		++m_pcsShrineTemplate->m_nNumShrinePos;
	}

	m_pcsShrineTemplate->m_bIsRandomPosition = m_bIsRandomPosition;

	// active condition
	m_pcsShrineTemplate->m_eActiveCondition = (eAgpmShrineActiveCondition) m_csComboActiveCondition.GetItemData(m_csComboActiveCondition.GetCurSel());
	m_pcsShrineTemplate->m_ulMinActiveTimeHR = m_unMinActiveTime;
	m_pcsShrineTemplate->m_ulMaxActiveTimeHR = m_unMaxActiveTime;

	// reactive condition
	m_pcsShrineTemplate->m_eReActiveCondition = (eAgpmShrineReActiveCondition) m_csComboReactiveCondition.GetItemData(m_csComboReactiveCondition.GetCurSel());

	// use condition
	m_pcsShrineTemplate->m_eUseCondition = (eAgpmShrineUseCondition) m_csComboUseCondition.GetItemData(m_csComboUseCondition.GetCurSel());

	CDialog::OnOK();
}

void AgcmShrineSettingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL AgcmShrineSettingDlg::SetModule(AgcmShrineDlg	*pcsModule)
{
	if (!pcsModule)
		return FALSE;

	m_pcsAgcmShrineDlg = pcsModule;

	return TRUE;
}

BOOL AgcmShrineSettingDlg::Init()
{
	// dialog 초기화

	return TRUE;
}

BOOL AgcmShrineSettingDlg::InitData(AgpdShrineTemplate *pcsShrineTemplate)
{
	if (!pcsShrineTemplate)
		return FALSE;

	m_pcsShrineTemplate = pcsShrineTemplate;

	return TRUE;
}

void AgcmShrineSettingDlg::OnButtonPosAdd() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);

	if (m_strPosition.GetLength() > 0)
	{
		m_csListPosition.AddString(m_strPosition);
	}
}

void AgcmShrineSettingDlg::OnButtonPosDelete() 
{
	// TODO: Add your control notification handler code here
	
	if (m_csListPosition.GetCurSel() != LB_ERR)
	{
		m_csListPosition.DeleteString(m_csListPosition.GetCurSel());
	}
}

void AgcmShrineSettingDlg::OnButtonSkillAdd() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_strSkillName.GetLength() > 0 &&
		m_unShrineLevel != 0)
	{
		CString	strBuffer;

		strBuffer.Format(_T("%d:%s"), m_unShrineLevel, m_strSkillName);

		m_csListSkillList.AddString(strBuffer);
	}
}

void AgcmShrineSettingDlg::OnButtonSkillDelete() 
{
	// TODO: Add your control notification handler code here
	
	if (m_csListSkillList.GetCurSel() != LB_ERR)
	{
		m_csListSkillList.DeleteString(m_csListSkillList.GetCurSel());
	}
}

BOOL AgcmShrineSettingDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	/*
	// default 값들을 세팅한다.
	m_strShrineName = m_pcsShrineTemplate->m_szShrineName;

	m_bIsRandomPosition = m_pcsShrineTemplate->m_bIsRandomPosition;
	m_unMinActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMinActiveTimeHR;
	m_unMaxActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMaxActiveTimeHR;

	m_unUseInterval		= (INT16) m_pcsShrineTemplate->m_ulUseIntervalTimeHR;

	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; ++i)
	{
		if (m_pcsShrineTemplate->m_szSkillName[i][0] == NULL)
			break;

		CString	strBuffer;

		strBuffer.Format(_T("%d:%s"), i, m_pcsShrineTemplate->m_szSkillName[i]);

		m_csListSkillList.AddString(strBuffer);
	}

	for (i = 0; i < m_pcsShrineTemplate->m_nNumShrinePos; ++i)
	{
		CString	strBuffer;

		strBuffer.Format(_T("%f;%f;%f"), m_pcsShrineTemplate->m_posShrine[i].x,
										 m_pcsShrineTemplate->m_posShrine[i].y,
										 m_pcsShrineTemplate->m_posShrine[i].z);

		m_csListPosition.AddString(strBuffer);
	}

	m_csComboActiveCondition.SetCurSel(m_pcsShrineTemplate->m_eActiveCondition);
	m_csComboReactiveCondition.SetCurSel(m_pcsShrineTemplate->m_eReActiveCondition);
	m_csComboUseCondition.SetCurSel(m_pcsShrineTemplate->m_eUseCondition);

	UpdateData(FALSE);
	*/
	
	return CDialog::Create(IDD, NULL);
}

BOOL AgcmShrineSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_csComboActiveCondition.ResetContent();
	m_csComboReactiveCondition.ResetContent();
	m_csComboUseCondition.ResetContent();

	INT32	lIndex	= 0;

	lIndex	= m_csComboActiveCondition.AddString("Active always");
	m_csComboActiveCondition.SetItemData(lIndex, AGPMSHRINE_ACTIVE_ALWAYS);
	lIndex	= m_csComboActiveCondition.AddString("Active day only");
	m_csComboActiveCondition.SetItemData(lIndex, AGPMSHRINE_ACTIVE_DAY_ONLY);
	lIndex	= m_csComboActiveCondition.AddString("Active spawn");
	m_csComboActiveCondition.SetItemData(lIndex, AGPMSHRINE_ACTIVE_SPAWN);

	lIndex	= m_csComboReactiveCondition.AddString("Reactive none");
	m_csComboReactiveCondition.SetItemData(lIndex, AGPMSHRINE_REACTIVE_NONE);
	lIndex	= m_csComboReactiveCondition.AddString("Reactive day only");
	m_csComboReactiveCondition.SetItemData(lIndex, AGPMSHRINE_REACTIVE_DAY_ONLY);
	lIndex	= m_csComboReactiveCondition.AddString("Reactive twice last active time");
	m_csComboReactiveCondition.SetItemData(lIndex, AGPMSHRINE_REACTIVE_TWICE_LAST_ACTIVE_TIME);

	lIndex	= m_csComboUseCondition.AddString("Use once");
	m_csComboUseCondition.SetItemData(lIndex, AGPMSHRINE_USE_ONCE);
	lIndex	= m_csComboUseCondition.AddString("Use when active");
	m_csComboUseCondition.SetItemData(lIndex, AGPMSHRINE_USE_WHEN_ACTIVE);
	lIndex	= m_csComboUseCondition.AddString("Use time interval");
	m_csComboUseCondition.SetItemData(lIndex, AGPMSHRINE_USE_TIME_INTERVAL);

	// default 값들을 세팅한다.
	m_strShrineName = m_pcsShrineTemplate->m_szShrineName;

	m_bIsRandomPosition = m_pcsShrineTemplate->m_bIsRandomPosition;
	m_unMinActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMinActiveTimeHR;
	m_unMaxActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMaxActiveTimeHR;

	m_unUseInterval		= (INT16) m_pcsShrineTemplate->m_ulUseIntervalTimeHR;

	int i;

	for (i = 0; i < AGPMSHRINE_MAX_LEVEL; ++i)
	{
		if (m_pcsShrineTemplate->m_szSkillName[i][0] == NULL)
			break;

		CString	strBuffer;

		strBuffer.Format(_T("%d:%s"), i + 1, m_pcsShrineTemplate->m_szSkillName[i]);

		m_csListSkillList.AddString(strBuffer);
	}

	for (int i = 0; i < m_pcsShrineTemplate->m_nNumShrinePos; ++i)
	{
		CString	strBuffer;

		strBuffer.Format(_T("%f;%f;%f"), m_pcsShrineTemplate->m_posShrine[i].x,
										 m_pcsShrineTemplate->m_posShrine[i].y,
										 m_pcsShrineTemplate->m_posShrine[i].z);

		m_csListPosition.AddString(strBuffer);
	}

	m_csComboActiveCondition.SetCurSel(m_pcsShrineTemplate->m_eActiveCondition);
	m_csComboReactiveCondition.SetCurSel(m_pcsShrineTemplate->m_eReActiveCondition);
	m_csComboUseCondition.SetCurSel(m_pcsShrineTemplate->m_eUseCondition);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int AgcmShrineSettingDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class

	/*
	// default 값들을 세팅한다.
	m_strShrineName = m_pcsShrineTemplate->m_szShrineName;

	m_bIsRandomPosition = m_pcsShrineTemplate->m_bIsRandomPosition;
	m_unMinActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMinActiveTimeHR;
	m_unMaxActiveTime	= (INT16) m_pcsShrineTemplate->m_ulMaxActiveTimeHR;

	m_unUseInterval		= (INT16) m_pcsShrineTemplate->m_ulUseIntervalTimeHR;

	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; ++i)
	{
		if (m_pcsShrineTemplate->m_szSkillName[i][0] == NULL)
			break;

		CString	strBuffer;

		strBuffer.Format(_T("%d:%s"), i, m_pcsShrineTemplate->m_szSkillName[i]);

		m_csListSkillList.AddString(strBuffer);
	}

	for (i = 0; i < m_pcsShrineTemplate->m_nNumShrinePos; ++i)
	{
		CString	strBuffer;

		strBuffer.Format(_T("%f;%f;%f"), m_pcsShrineTemplate->m_posShrine[i].x,
										 m_pcsShrineTemplate->m_posShrine[i].y,
										 m_pcsShrineTemplate->m_posShrine[i].z);

		m_csListPosition.AddString(strBuffer);
	}

	m_csComboActiveCondition.SetCurSel(m_pcsShrineTemplate->m_eActiveCondition);
	m_csComboReactiveCondition.SetCurSel(m_pcsShrineTemplate->m_eReActiveCondition);
	m_csComboUseCondition.SetCurSel(m_pcsShrineTemplate->m_eUseCondition);

	UpdateData(FALSE);
	*/

	return CDialog::DoModal();
}
