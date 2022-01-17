// EffectHelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "EffectHelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectHelpDlg dialog


CEffectHelpDlg::CEffectHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectHelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectHelpDlg)
	m_strHelp = _T("");
	//}}AFX_DATA_INIT
}


void CEffectHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectHelpDlg)
	DDX_Text(pDX, IDC_EDIT_HELP, m_strHelp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectHelpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectHelpDlg message handlers
VOID CEffectHelpDlg::InputString(CHAR *szString)
{
	CHAR	szTemp[256];
	sprintf(szTemp, "\r\n%s", szString);

	m_strHelp	+= szTemp;
}

BOOL CEffectHelpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_strHelp = "[Custom Data]\r\n";

	InputString("DestroyMatch=");
	InputString("이펙트 소멸시 지정된 이펙트를 생성합니다.");

	InputString(" ");
	InputString("TargetStruckAnim=");
	InputString("Target character이 맞는 애니매이션을 합니다.");
	InputString("(여러 point 지정가능)");

	InputString(" ");
	InputString("ExtType=");
	InputString("예외 형태를 설정합니다.");
	InputString("1 - 지속형 이펙트");
	InputString("2 - 써드어택");
	InputString("3 - 버프된 상태에서 공격시");
	InputString("4 - 임시");
	InputString("5 - 회피 발동시");
	InputString("6 - 데미지 반사시");
	InputString("7 - 이동속도 변화시");
	InputString("8 - 데미지 흡수시");

	InputString(" ");
	InputString("CheckTimeOn=");
	InputString("이펙트의 생성시기를 정합니다.");
	InputString("(시단위)");

	InputString(" ");
	InputString("CheckTimeOff=");
	InputString("이펙트의 소멸시기를 정합니다.");
	InputString("(시단위)");

	InputString(" ");
	InputString("TargetOption=");
	InputString("타겟의 기준을 설정합니다.");
	InputString("1 - All");
	InputString("2 - First");
	InputString("3 - Chain");

	InputString(" ");
	InputString("CalcDistOption=");
	InputString("타겟과의 거리를 계산해서 Gap을 추가합니다.");
	InputString("(공식 -> Gap(milliseconds) = 미터 * offset)");

	InputString(" ");
	InputString("DirOption=");
	InputString("이펙트의 방향을 설정합니다.");
	InputString("1 - Origin -> Target(Ignore height)");
	InputString("2 - Target -> Origin(Ignore height)");
	InputString("3 - Origin -> Target");
	InputString("4 - Target -> Origin");

	InputString(" ");
	InputString("LinkSkill=");
	InputString("스킬을 발동합니다.(SKILL TID)");



	InputString(" ");
	InputString("ExtTypeCustData=");
	InputString("Exception type에 추가로 사용자 정의를 지정합니다.");
	InputString("Exception type(써드어택) - 레벨로 사용");



	InputString(" ");
	InputString("CheckUser=");
	InputString("Effect를 사용하는 주체를 제한합니다.");
	InputString("1 - PC");
	InputString("2 - NPC");
	InputString("3 - Monster");
	InputString("4 - GM");

	InputString(" ");
	InputString("ActionObj=");
	InputString("Effect를 사용하는 주체의 Action이 필요할 경우 사용합니다.");
	InputString("1 - Hide shield and weapons(생산스킬일 경우)");

	//. 2005. 10. 06 Nonstopdj
	//. 오타수정 -_-
	InputString(" ");
	InputString("NatureCondition=");
	InputString("스카이셋에 따른 효과의 상태를 지정합니다.");
	InputString("0 - Normal <사용하지않는다.>");
	InputString("1 - Cold");

	//@{ 2007/01/03 burumal
	InputString(" ");
	InputString("CastingEffect=");
	InputString("스킬에 사용되는 Effect중 캐스팅에 사용되는 이펙트임을 명시한다.");	
	InputString("1 - 공성시 캐스팅 이펙트는 Disable 된다.");
	//@}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
