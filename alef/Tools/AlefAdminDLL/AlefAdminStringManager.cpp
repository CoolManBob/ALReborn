// AlefAdminStringManager.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2006.03.07
//

#include "stdafx.h"
#include "AlefAdminManager.h"
#include "AlefAdminStringManager.h"

AlefAdminStringManager::AlefAdminStringManager()
{
	m_pcsAgcmUIManager2 = NULL;
}

AlefAdminStringManager::~AlefAdminStringManager()
{
}

void AlefAdminStringManager::Init()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!AlefAdminManager::Instance()->GetAdminModule())
		return;

	m_pcsAgcmUIManager2 = ( AgcmUIManager2* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmUIManager2" );
}

// 리턴값을 그냥 이용하지 말고 카피해서 이용한다.
CString AlefAdminStringManager::GetUIMessage(TCHAR* szKey)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szValue = _T("");
	if( !szKey ) return szValue;

	CHAR* pString = AlefAdminManager::Instance()->GetAdminData()->GetUIMessageString( szKey );
	if( !pString || strlen( pString ) <= 0 ) return szValue;
	
	szValue = pString;
	return szValue;
}

CString AlefAdminStringManager::GetResourceMessage(UINT uID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szValue = _T("");
	szValue.LoadString(uID);
	return szValue;
}
