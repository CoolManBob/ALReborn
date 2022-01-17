// AlefAdminXTExtension.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 02. 17.

#include "StdAfx.h"
#include "AlefAdminManager.h"

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx

CTabCtrlEx::CTabCtrlEx()
{
	// TODO: add construction code here.
	m_bXPBorder = true;
}

CTabCtrlEx::~CTabCtrlEx()
{
	// TODO: add destruction code here.

}

IMPLEMENT_DYNAMIC(CTabCtrlEx, CXTTabCtrl)

BEGIN_MESSAGE_MAP(CTabCtrlEx, CXTTabCtrl)
	//{{AFX_MSG_MAP(CTabCtrlEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//void CTabCtrlEx::OnSelChange()
//{
//	CXTTabCtrl::OnSelChange();
//
//	CWnd* pParent = GetParent();
//	if(pParent)
//	{
//		pParent->SendMessage(TABCTRLEX_SELCHANGE, 0, 0);
//	}
//}

//BOOL CTabCtrlEx::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
//{
//	return CXTTabExBase::ModifyTabStyle(dwRemove, dwAdd, nFlags);
//}


CXTPSetValueGridItem::CXTPSetValueGridItem(CString strCaption, CString strValue) : CXTPPropertyGridItem(strCaption, strValue, NULL)
{
}

void CXTPSetValueGridItem::SetValue(CString strValue)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem::SetValue(strValue);
}





CAlefAdminMacro::CAlefAdminMacro() : m_eMacroType(MACROTYPE_NONE),
			m_eFunctionType(MACROFUNCTIONTYPE_NONE), m_szCommand(0), m_szDescription(0), m_szParameter(0)
{
}

CAlefAdminMacro::~CAlefAdminMacro()
{
	if(m_szCommand)
		delete [] m_szCommand;

	if(m_szDescription)
		delete [] m_szDescription;

	if(m_szParameter)
		delete [] m_szParameter;
}

void CAlefAdminMacro::Execute()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!AlefAdminManager::Instance()->IsInitialized())
		return;

	AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_pCharMacroView->ExecuteMacro(this);
}
