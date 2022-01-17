// XTPCalendarResourcesManager.cpp: implementation of the
// CXTPCalendarResourcesManager class.
//
// This file is a part of the XTREME CALENDAR MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Resource.h"

//#include "Common/XTPResourceManager.h"
#include "Common/XTPPropExchange.h"

#include "XTPCalendarResourcesManager.h"
#include "XTPCalendarCustomProperties.h"

#include "XTPCalendarControl.h"
#include "XTPCalendarData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=====================================================================
static const LPCTSTR cszCalendarDPF_SaveOnDestroy   = _T("*xtp*CalendarDPF_SaveOnDestroy");
static const LPCTSTR cszCalendarDPF_CloseOnDestroy  = _T("*xtp*CalendarDPF_CloseOnDestroy");

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPCalendarResourceDescription, CCmdTarget)
IMPLEMENT_DYNCREATE(CXTPCalendarResourcesManager, CCmdTarget)

//////////////////////////////////////////////////////////////////////////
CXTPCalendarResourceDescription::CXTPCalendarResourceDescription()
{
	m_bEnabled = FALSE;
	m_bGenerateName = TRUE;

}

//////////////////////////////////////////////////////////////////////////
//class CXTPCalendarResourcesManager
CXTPCalendarResourcesManager::CXTPCalendarResourcesManager()
{
}

CXTPCalendarResourcesManager::~CXTPCalendarResourcesManager()
{
	RemoveAll();
}

void CXTPCalendarResourcesManager::RemoveAll()
{
	int i, nDPCount = GetDataProvidersCount();

	for (i = 0; i < nDPCount; i++)
	{
		CXTPCalendarData* pData = GetDataProvider(i);
		_SaveCloseDPifNeed(pData);
	}

	//------------------------------------
	for (i = nDPCount - 1; i >= 0; i--)
	{
		RemoveDataProvider(i);
	}

	//------------------------------------
	int nRCCount = GetResourcesCount();
	for (i = nRCCount - 1; i >= 0; i--)
	{
		RemoveResource(i);
	}
}

void CXTPCalendarResourcesManager::_SaveCloseDPifNeed(CXTPCalendarData* pData)
{
	if (!pData || !pData->GetCustomProperties())
	{
		ASSERT(FALSE);
		return;
	}

	if (!pData->IsOpen())
	{
		return;
	}

	COleVariant ovarSave((long)0);
	COleVariant ovarClose((long)0);
	BOOL bPropSave = pData->GetCustomProperties()->GetProperty(cszCalendarDPF_SaveOnDestroy, ovarSave);
	BOOL bPropClose = pData->GetCustomProperties()->GetProperty(cszCalendarDPF_CloseOnDestroy, ovarClose);

	_variant_t vtSave = bPropSave ? ovarSave : (long)0;
	_variant_t vtClose = bPropClose ? ovarClose : (long)0;

	if ((long)vtSave)
	{
		pData->Save();
	}
	if ((long)vtClose)
	{
		pData->Close();
	}
}
//=========================================================================
BOOL CXTPCalendarResourcesManager::AddDataProvider(LPCTSTR pcszConnectionString,
												   int eDPFlags)
{
	m_arDataProviders.Add(NULL);
	int nNewIndex = m_arDataProviders.GetCount()-1;

	BOOL bRes = SetDataProvider(nNewIndex, pcszConnectionString, eDPFlags);

	if (!bRes && m_arDataProviders.GetAt(nNewIndex) == NULL)
	{
		m_arDataProviders.RemoveAt(nNewIndex);
	}
	return bRes;
}

BOOL CXTPCalendarResourcesManager::SetDataProvider(int nIndex, LPCTSTR pcszConnectionString, int eDPFlags)
{
	if (nIndex < 0 || nIndex >= m_arDataProviders.GetCount())
	{
		return FALSE;
	}

	CXTPCalendarData* pData = CXTPCalendarControl::CreateDataProvider(pcszConnectionString);
	if (!pData)
	{
		return FALSE;
	}

	CXTPCalendarData* pData_prev = m_arDataProviders.GetAt(nIndex);
	if (pData_prev)
	{
		_SaveCloseDPifNeed(pData_prev);
	}

	//========================================
	m_arDataProviders.SetAt(nIndex, pData);

	//----------------------------------------
	BOOL bOpen = pData->Open();
	if (!bOpen && (eDPFlags & xtpCalendarDPF_CreateIfNotExists))
	{
		bOpen = pData->Create();
	}

	if (pData->GetCustomProperties())
	{
		COleVariant ovarSave((long)0);
		COleVariant ovarClose((long)0);

		if (eDPFlags & xtpCalendarDPF_SaveOnDestroy)
		{
			ovarSave = (long)1;
		}
		if (eDPFlags & xtpCalendarDPF_CloseOnDestroy)
		{
			ovarClose = (long)1;
		}
		VERIFY(pData->GetCustomProperties()->SetProperty(cszCalendarDPF_SaveOnDestroy, ovarSave) );
		VERIFY(pData->GetCustomProperties()->SetProperty(cszCalendarDPF_CloseOnDestroy, ovarClose) );
	}
	return bOpen;
}

int CXTPCalendarResourcesManager::GetDataProvidersCount() const
{
	return m_arDataProviders.GetCount();
}

CXTPCalendarData* CXTPCalendarResourcesManager::GetDataProvider(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arDataProviders.GetCount())
	{
		return m_arDataProviders.GetAt(nIndex);
	}
	return NULL;
}

CXTPCalendarData* CXTPCalendarResourcesManager::GetDataProvider(
	LPCTSTR pcszConnectionString, int* pnIndex)
{
	int nCount = GetDataProvidersCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strConnStr = GetDataProvider_ConnStr(i);
		if (strConnStr.CompareNoCase(pcszConnectionString) == 0)
		{
			if (pnIndex)
			{
				*pnIndex = i;
			}
			return GetDataProvider(i);
		}
	}

	if (pnIndex)
	{
		*pnIndex = -1;
	}
	return NULL;
}

CString CXTPCalendarResourcesManager::GetDataProvider_ConnStr(int nIndex)
{
	CXTPCalendarData* pData = GetDataProvider(nIndex);

	if (pData)
	{
		return pData->GetConnectionString();
	}
	return _T("");
}

void CXTPCalendarResourcesManager::RemoveDataProvider(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arDataProviders.GetCount())
	{
		CXTPCalendarData* pData = m_arDataProviders.GetAt(nIndex);
		if (pData && pData->IsOpen())
		{
			pData->Close();
		}

		m_arDataProviders.RemoveAt(nIndex);
	}
}

//=========================================================================
BOOL CXTPCalendarResourcesManager::AddResource(LPCTSTR pcszName, BOOL bEnabled)
{
	CXTPCalendarResourceDescription* pRCDesc = new CXTPCalendarResourceDescription();
	if (!pRCDesc)
	{
		return FALSE;
	}

	//pRCDesc->m_strName = pcszName;
	pRCDesc->m_bEnabled = bEnabled;
	pRCDesc->m_ptrResource = new CXTPCalendarResource();

	if (!pRCDesc->m_ptrResource)
	{
		return FALSE;
	}
	pRCDesc->m_ptrResource->SetDataProvider((CXTPCalendarData*)NULL);
	pRCDesc->m_ptrResource->SetName(pcszName);

	m_arResources.Add(pRCDesc, FALSE);

	return TRUE;
}

int CXTPCalendarResourcesManager::GetResourcesCount() const
{
	return m_arResources.GetCount();
}

CXTPCalendarResourceDescription*
CXTPCalendarResourcesManager::GetResource(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arResources.GetCount())
	{
		return m_arResources.GetAt(nIndex);
	}

	ASSERT(FALSE);
	return NULL;
}

void CXTPCalendarResourcesManager::RemoveResource(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_arResources.GetCount())
	{
		m_arResources.RemoveAt(nIndex);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CXTPCalendarResourcesManager::MoveResource(int nIndex, int nNewIndex)
{
	if (nIndex < 0 || nIndex >= m_arResources.GetCount())
	{
		ASSERT(FALSE);
		return;
	}
	nNewIndex = max(0, min(nNewIndex, m_arResources.GetCount()-1));

	CXTPCalendarResourceDescription* pRC = m_arResources.GetAt(nIndex, TRUE); // AddRef. RefCnt + 1
	m_arResources.RemoveAt(nIndex); // Release. RefCnt - 1
	m_arResources.InsertAt(nNewIndex, pRC); // RefCnt - is not changed
}

BOOL CXTPCalendarResourcesManager::SaveCfg(LPCTSTR pcszFile)
{
#ifndef _XTP_EXCLUDE_XML
	CXTPPropExchangeXMLNode px(FALSE, 0, _T("CalendarMultiResourcesConfiguration"));

	_Save(&px);

	::SetFileAttributes(pcszFile, FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(pcszFile);

	px.SaveToFile(pcszFile);

	return TRUE;
#else
	return FALSE;
#endif
}

BOOL CXTPCalendarResourcesManager::LoadCfg(LPCTSTR pcszFile)
{
#ifndef _XTP_EXCLUDE_XML
	BOOL bResult = FALSE;
	CXTPPropExchangeXMLNode px(TRUE, 0, _T("CalendarMultiResourcesConfiguration"));

	if (px.LoadFromFile(pcszFile))
	{
		_Load(&px);
		bResult = TRUE;
	}

	return bResult;

#else
	return FALSE;
#endif

}

void CXTPCalendarResourcesManager::DoExchangeCfg(CXTPPropExchange* pPX)
{
	ASSERT(pPX);
	if (!pPX)
	{
		return;
	}

	if (pPX->IsLoading())
	{
		_Load(pPX);
	}
	else
	{
		_Save(pPX);
	}
}

void CXTPCalendarResourcesManager::_Save(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsStoring())
	{
		ASSERT(FALSE);
		return;

	}
	pPX->ExchangeLocale();

	CXTPPropExchangeSection secDPs(pPX->GetSection(_T("DataProviders")));

	int nDPCount = GetDataProvidersCount();
	CXTPPropExchangeEnumeratorPtr ptrEnumDPs(secDPs->GetEnumerator(_T("DataProvider")));
	POSITION posStorage = ptrEnumDPs->GetPosition(nDPCount);

	for (int nDRNr = 0; nDRNr < nDPCount; nDRNr++)
	{
		CXTPCalendarData* pDP = GetDataProvider(nDRNr);
		ASSERT(pDP);
		if (!pDP)
		{
			continue;
		}

		CString strConnStr = pDP->GetConnectionString();

		CXTPPropExchangeSection secDP(ptrEnumDPs->GetNext(posStorage));
		PX_String(&secDP, _T("ConnectionString"), strConnStr);
	}

	//---------------------
	CXTPPropExchangeSection secResources(pPX->GetSection(_T("Resources")));

	int nRCCount = GetResourcesCount();
	CXTPPropExchangeEnumeratorPtr ptrEnumRCs(secResources->GetEnumerator(_T("Resource")));
	posStorage = ptrEnumRCs->GetPosition(nRCCount);

	for (int nRCNr = 0; nRCNr < nRCCount; nRCNr++)
	{
		CXTPCalendarResourceDescription* pRCdesc = GetResource(nRCNr);
		if (!pRCdesc || !pRCdesc->m_ptrResource)
		{
			ASSERT(FALSE);
			continue;
		}

		CXTPPropExchangeSection secRC(ptrEnumRCs->GetNext(posStorage));

		CString strRCName = pRCdesc->m_ptrResource->GetName();
		PX_String(&secRC, _T("Name"), strRCName);
		PX_Bool(&secRC, _T("Enabled"), pRCdesc->m_bEnabled);
		PX_Bool(&secRC, _T("GenerateName"), pRCdesc->m_bGenerateName);

		CXTPCalendarData* pRCdp = pRCdesc->m_ptrResource->GetDataProvider();
		if (pRCdp)
		{
			CString strConnStr = pRCdp->GetConnectionString();
			PX_String(&secRC, _T("ConnectionString"), strConnStr);

			int nSchIdsCount = 0;
			if (pRCdesc->m_ptrResource->GetSchedules())
			{
				nSchIdsCount = (int)pRCdesc->m_ptrResource->GetSchedules()->GetSize();
			}
			PX_Int(&secRC, _T("SchedulesCount"), nSchIdsCount);

			CXTPPropExchangeEnumeratorPtr ptrEnumSchIDs(secRC->GetEnumerator(_T("Schedules")));
			POSITION posSchID = ptrEnumSchIDs->GetPosition(nSchIdsCount);

			for (int k = 0; k < nSchIdsCount; k++)
			{
				CXTPPropExchangeSection secSchID(ptrEnumSchIDs->GetNext(posSchID));
				ULONG ulSchID = pRCdesc->m_ptrResource->GetSchedules()->GetAt(k);
				PX_ULong(&secSchID, _T("ScheduleID"), ulSchID);
			}
		}
	}
}

void CXTPCalendarResourcesManager::_Load(CXTPPropExchange* pPX)
{
	if (!pPX || !pPX->IsLoading())
	{
		ASSERT(FALSE);
		return;

	}

	// Clear previous configuration.
	// Data providers will be saved and closed if this was specified.
	RemoveAll();

	pPX->ExchangeLocale();

	CXTPPropExchangeSection secDPs(pPX->GetSection(_T("DataProviders")));

	//int nDPCount = GetDataProvidersCount();
	CXTPPropExchangeEnumeratorPtr ptrEnumDPs(secDPs->GetEnumerator(_T("DataProvider")));
	POSITION posStorage = ptrEnumDPs->GetPosition();

	while (posStorage)
	{
		CString strConnStr;

		CXTPPropExchangeSection secDP(ptrEnumDPs->GetNext(posStorage));
		PX_String(&secDP, _T("ConnectionString"), strConnStr);

		if (!strConnStr.IsEmpty())
		{
			const int eDPFAll = xtpCalendarDPF_CreateIfNotExists |
				xtpCalendarDPF_SaveOnDestroy |
				xtpCalendarDPF_CloseOnDestroy;
			AddDataProvider(strConnStr, eDPFAll);
		}
	}

	//---------------------
	CXTPPropExchangeSection secResources(pPX->GetSection(_T("Resources")));

	CXTPPropExchangeEnumeratorPtr ptrEnumRCs(secResources->GetEnumerator(_T("Resource")));
	posStorage = ptrEnumRCs->GetPosition();

	while (posStorage)
	{
		CXTPPropExchangeSection secRC(ptrEnumRCs->GetNext(posStorage));
		CString strName;
		PX_String(&secRC, _T("Name"), strName);
		BOOL bAdded = AddResource(strName, FALSE);

		int nRCCount = GetResourcesCount();
		if (!bAdded || !nRCCount)
		{
			ASSERT(FALSE);
			continue;
		}

		CXTPCalendarResourceDescription* pRCdesc = GetResource(nRCCount - 1);
		if (!pRCdesc || !pRCdesc->m_ptrResource)
		{
			ASSERT(FALSE);
			continue;
		}

		PX_Bool(&secRC, _T("Enabled"), pRCdesc->m_bEnabled);
		PX_Bool(&secRC, _T("GenerateName"), pRCdesc->m_bGenerateName);


		//**********
		CString strConnStr;
		PX_String(&secRC, _T("ConnectionString"), strConnStr);
		CXTPCalendarData* pRCdp = GetDataProvider(strConnStr);
		pRCdesc->m_ptrResource->SetDataProvider(pRCdp);

		int nSchIdsCount = 0;
		PX_Int(&secRC, _T("SchedulesCount"), nSchIdsCount);

		if (nSchIdsCount)
		{
			CXTPPropExchangeEnumeratorPtr ptrEnumSchIDs(secRC->GetEnumerator(_T("Schedules")));
			POSITION posSchID = ptrEnumSchIDs->GetPosition();

			while (posSchID)
			{
				CXTPPropExchangeSection secSchID(ptrEnumSchIDs->GetNext(posSchID));
				ULONG ulSchID = 0;
				if (PX_ULong(&secSchID, _T("ScheduleID"), ulSchID))
				{
					if (pRCdesc->m_ptrResource->GetSchedules())
					{
						pRCdesc->m_ptrResource->GetSchedules()->Add(ulSchID);
					}
				}
			}
		}
	}
}

void CXTPCalendarResourcesManager::ApplyToCalendar(CXTPCalendarControl* pCalendar)
{
	CXTPCalendarResources arResources;

	int nRCCount = GetResourcesCount();
	for (int i = 0; i < nRCCount; i++)
	{
		CXTPCalendarResourceDescription* pRCdesc = GetResource(i);
		if (!pRCdesc || !pRCdesc->m_ptrResource)
		{
			ASSERT(FALSE);
			continue;
		}
		if (!pRCdesc->m_bEnabled || !pRCdesc->m_ptrResource->GetDataProvider())
		{
			continue;
		}

		arResources.Add(pRCdesc->m_ptrResource, TRUE);
	}

	if (arResources.GetCount())
	{
		pCalendar->SetResources(&arResources);
	}
}

/////////////////////////////////////////////////////////////////////////////
