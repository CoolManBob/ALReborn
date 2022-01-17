// AgcmShrineDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "AgcmShrineDlg.h"
#include "AgcmShrineSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAgcmShrineDlgApp

/*
BEGIN_MESSAGE_MAP(CAgcmShrineDlgApp, CWinApp)
	//{{AFX_MSG_MAP(CAgcmShrineDlgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// CAgcmShrineDlgApp construction

AgcmShrineDlg::AgcmShrineDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_nCreatedShrineIndex	= 1;

	SetModuleName("AgcmShrineDlg");
	
	m_pcsAgpmShrine			= NULL;
	m_pcsApmEventManager	= NULL;
	m_pcsApmEventManagerDlg	= NULL;
}

AgcmShrineDlg::~AgcmShrineDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

BOOL AgcmShrineDlg::OnAddModule()
{	
	m_pcsAgpmShrine			= (AgpmShrine *)			GetModule("AgpmShrine");
	m_pcsApmEventManager	= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg	= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");

	if (!m_pcsAgpmShrine ||
		!m_pcsApmEventManager ||
		!m_pcsApmEventManagerDlg)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog((ApmEventDialog *) this, APDEVENT_FUNCTION_SHRINE))
		return FALSE;

	if (!m_pcsAgpmShrine->SetCallbackAddShrineEvent(CBAddShrineEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmShrineDlg::Open(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return FALSE;

	AgcmShrineSettingDlg	dlgShrine;

	AgpdShrineTemplate	*pcsShrineTemplate = NULL;

	if (pcsEvent->m_pvData)
	{
		pcsShrineTemplate = ((AgpdShrine *) pcsEvent->m_pvData)->m_pcsTemplate;
	}
	else
	{
		pcsShrineTemplate = m_pcsAgpmShrine->AddShrineTemplate(m_nCreatedShrineIndex++);
	}

	if (!pcsShrineTemplate)
		return FALSE;

	dlgShrine.InitData(pcsShrineTemplate);

	if (dlgShrine.DoModal() == IDOK)
	{
		if (!pcsEvent->m_pvData)
		{
			AgpdShrine	*pcsShrine = m_pcsAgpmShrine->AddShrine(pcsShrineTemplate->m_lID, pcsShrineTemplate->m_lID);
			if (!pcsShrine)
			{
				m_pcsAgpmShrine->RemoveShrineTemplate(pcsShrineTemplate->m_lID);
				return FALSE;
			}

			/*
			if (!m_pcsAgpmShrine->StreamWriteTemplate("ShrineTemplate.ini"))
			{
				m_pcsAgpmShrine->RemoveShrineTemplate(pcsShrineTemplate->m_lID);
				m_pcsAgpmShrine->RemoveShrine(pcsShrine->m_lID);

				return FALSE;
			}
			*/

			pcsEvent->m_pvData = pcsShrine;
		}
	}
	else
	{
		m_pcsAgpmShrine->RemoveShrineTemplate(pcsShrineTemplate->m_lID);

		if (pcsEvent->m_pvData)
			m_pcsAgpmShrine->RemoveShrine(((AgpdShrine *) pcsEvent->m_pvData)->m_lID);
	}

	return TRUE;
}

/*
BOOL AgcmShrineDlg::Open(AgpdShrineTemplate *pcsShrineTemplate)
{
	if (!pcsShrineTemplate)
		return FALSE;

	AgcmShrineSettingDlg	dlgShrine;

	if (!dlgShrine.SetModule(this))
		return FALSE;

	if (!dlgShrine.Init())
		return FALSE;

	if (!dlgShrine.InitData(pcsShrineTemplate))
		return FALSE;

	if (dlgShrine.DoModal() == IDOK)
	{
	}

	return TRUE;
}
*/

BOOL AgcmShrineDlg::Close()
{
	return TRUE;
}

BOOL AgcmShrineDlg::CBAddShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pstEvent		= (ApdEvent *)				pData;
	AgcmShrineDlg		*pThis			= (AgcmShrineDlg *)			pClass;
	AgpdShrineTemplate	*pcsTemplate	= (AgpdShrineTemplate *)	pCustData;

	// shrine을 추가한다.
	AgpdShrine			*pcsShrine		= pThis->m_pcsAgpmShrine->AddShrine(pcsTemplate->m_lID, pcsTemplate->m_lID);
	if (!pcsShrine)
		return FALSE;

	pstEvent->m_pvData = pcsShrine;

	return TRUE;
}