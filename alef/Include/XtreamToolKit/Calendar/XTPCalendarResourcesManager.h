// XTPCalendarResourcesManager.h: interface for the
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTPCALENDAR_RESOURCES_MANAGER_H_)
#define _XTPCALENDAR_RESOURCES_MANAGER_H_
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarResource.h"

//===========================================================================
//class _XTP_EXT_CLASS
class CXTPCalendarResourcesManager;
class CCalendarResourcesDlg;

//===========================================================================
// Summary: This class is a wrapper for CXTPCalendarResource class.
//          It is needed for easier processing in scope of CXTPCalendarResourcesManager
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarResourceDescription : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarResourceDescription)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-------------------------------------------------------------------------
	CXTPCalendarResourceDescription();

	BOOL    m_bEnabled;         // This flag indicates is resource enabled.
	BOOL    m_bGenerateName;    // This flag indicates that resource name should be generated from the schedule(s) name(s).

	CXTPCalendarResourcePtr m_ptrResource;  // A smart pointer to the resource object.

protected:
};

//===========================================================================
// Summary:
//     This class is used as helper to manage calendar resources (and schedules)
//     data configurations.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarResourcesManager : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarResourcesManager)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-------------------------------------------------------------------------
	CXTPCalendarResourcesManager();

	//-------------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-------------------------------------------------------------------------
	virtual ~CXTPCalendarResourcesManager();

	//-------------------------------------------------------------------------
	// Summary:
	//     This enum specify a data provider flags.
	//-------------------------------------------------------------------------
	enum XTPEnumCalendarDataProviderFlags
	{
		xtpCalendarDPFUnknown               = 0,     // Zero value flag
		xtpCalendarDPF_CreateIfNotExists    = 0x001, // Call Create method of data provider if Open method returns FALSE.
		xtpCalendarDPF_SaveOnDestroy        = 0x010, // Call Save method of data provider on Resources Manager destroy.
		xtpCalendarDPF_CloseOnDestroy       = 0x020, // Call Close method of data provider on Resources Manager destroy.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Add a new data provider to data providers collection using
	//     specified Calendar connection string.
	// Parameters:
	//     pcszConnectionString - A text Calendar connection string.
	//     eDPFlags             - Additional flags to control data provider creation/Opening and destruction/Closing.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     SetDataProvider, XTPEnumCalendarDataProviderFlags,
	//     CXTPCalendarControl::CreateDataProvider, CXTPCalendarData overview.
	//-----------------------------------------------------------------------
	BOOL AddDataProvider(LPCTSTR pcszConnectionString, int eDPFlags = xtpCalendarDPF_CreateIfNotExists);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set a new data provider to data providers collection using
	//     specified Calendar connection string.
	// Parameters:
	//     pcszConnectionString - A text Calendar connection string.
	//     eDPFlags             - Additional flags to control data provider creation/Opening and destruction/Closing.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     AddDataProvider, XTPEnumCalendarDataProviderFlags,
	//     CXTPCalendarControl::CreateDataProvider, CXTPCalendarData overview.
	//-----------------------------------------------------------------------
	BOOL SetDataProvider(int nIndex, LPCTSTR pcszConnectionString, int eDPFlags = xtpCalendarDPF_CreateIfNotExists);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve data providers count in the collection.
	// Returns:
	//     Data providers count.
	// See Also:
	//     GetDataProvider, AddDataProvider, SetDataProvider, RemoveDataProvider.
	//-----------------------------------------------------------------------
	int GetDataProvidersCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve data provider at specified index.
	// Parameters:
	//     nIndex - Zero based data provider index in the collection.
	// Returns:
	//     Data provider at specified index.
	// See Also:
	//     GetDataProvidersCount, AddDataProvider, SetDataProvider, RemoveDataProvider.
	//-----------------------------------------------------------------------
	CXTPCalendarData* GetDataProvider(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove data provider at specified index.
	// Parameters:
	//     nIndex - Zero based data provider index in the collection.
	// See Also:
	//     GetDataProvidersCount, GetDataProvider, AddDataProvider, SetDataProvider.
	//-----------------------------------------------------------------------
	void RemoveDataProvider(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find data provider by connection string.
	// Parameters:
	//     pcszConnectionString - A text Calendar connection string.
	//     pnIndex              - A pointer to receive data provider index in the collection.
	// Returns:
	//     Pointer to a data provider with specified connection string if such
	//     presents in the collection, NULL otherwise.
	// See Also: GetDataProvider_ConnStr
	//-----------------------------------------------------------------------
	CXTPCalendarData* GetDataProvider(LPCTSTR pcszConnectionString, int* pnIndex = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve a text Calendar connection string for data provider at
	//     specified index.
	// Parameters:
	//     nIndex - Zero based data provider index in the collection.
	// Returns:
	//     A text Calendar connection string.
	// See Also: GetDataProvider
	//-----------------------------------------------------------------------
	CString GetDataProvider_ConnStr(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Add a new data resource to calendar resources collection.
	// Parameters:
	//     pcszName - Resource name string.
	//     bEnabled - Is resource enabled.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     GetResourcesCount, GetResource, RemoveResource, MoveResource.
	//-----------------------------------------------------------------------
	BOOL AddResource(LPCTSTR pcszName, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve calendar data resources count in the collection.
	// Returns:
	//     Calendar data resources count in the collection.
	// See Also:
	//     AddResource, GetResource, RemoveResource, MoveResource.
	//-----------------------------------------------------------------------
	int GetResourcesCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve resource at specified index.
	// Parameters:
	//     nIndex - Zero based resource index in the collection.
	// Returns:
	//     Calendar resource at specified index.
	// See Also:
	//     AddResource, GetResourcesCount, RemoveResource, MoveResource.
	//-----------------------------------------------------------------------
	CXTPCalendarResourceDescription* GetResource(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove resource at specified index.
	// Parameters:
	//     nIndex - Zero based resource index in the collection.
	// See Also:
	//     AddResource, GetResourcesCount, GetResource, MoveResource.
	//-----------------------------------------------------------------------
	void RemoveResource(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Move resource to a new position in the collection.
	// Parameters:
	//     nIndex    - Current zero based resource index in the collection.
	//     nNewIndex - A new index to move resource to.
	// See Also:
	//     AddResource, GetResourcesCount, GetResource, RemoveResource.
	//-----------------------------------------------------------------------
	void MoveResource(int nIndex, int nNewIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data providers and resources configuration
	//     from or to an storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// See Also:
	//     SaveCfg, LoadCfg.
	//-----------------------------------------------------------------------
	void DoExchangeCfg(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method writes data providers and resources configuration to
	//     a specified file.
	// Parameters:
	//     pcszFile - A file name to store configuration.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also: LoadCfg, DoExchangeCfg.
	//-----------------------------------------------------------------------
	BOOL SaveCfg(LPCTSTR pcszFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads data providers and resources configuration from
	//     a specified file.
	// Parameters:
	//     pcszFile - A file name to load configuration.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also: SaveCfg, DoExchangeCfg.
	//-----------------------------------------------------------------------
	BOOL LoadCfg(LPCTSTR pcszFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set the active configuration (enabled resources) to calendar control.
	// Parameters:
	//     pCalendar - A pointer to calendar control.
	// See Also:
	//     CXTPCalendarControl::SetResources
	//-----------------------------------------------------------------------
	void ApplyToCalendar(CXTPCalendarControl* pCalendar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove all resources and all data providers.
	// Remarks:
	//     Data providers will be saved and closed if this was specified for them.
	// See Also:
	//     XTPEnumCalendarDataProviderFlags
	//-----------------------------------------------------------------------
	void RemoveAll();

protected:
	CXTPCalendarPtrCollectionT<CXTPCalendarData> m_arDataProviders;             // Data providers collection.
	CXTPCalendarPtrCollectionT<CXTPCalendarResourceDescription> m_arResources;  // Resources collection.

	//-----------------------------------------------------------------------
	// Summary:
	//     This method save data providers and resources configuration to
	//     a storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to.
	// See Also: _Load, LoadCfg, SaveCfg, DoExchangeCfg.
	//-----------------------------------------------------------------------
	void _Save(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads data providers and resources configuration from
	//     a storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize from.
	// See Also: _Save, LoadCfg, SaveCfg, DoExchangeCfg.
	//-----------------------------------------------------------------------
	void _Load(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to Save and Close data provider if these
	//     actions were specified in AddDataProvider or SetDataProvider
	//     methods.
	// Parameters:
	//     pData - A pointer to data provider object.
	// See Also:
	//     AddDataProvider, SetDataProvider, XTPEnumCalendarDataProviderFlags.
	//-----------------------------------------------------------------------
	void _SaveCloseDPifNeed(CXTPCalendarData* pData);

protected:

};


#endif // !defined(_XTPCALENDAR_RESOURCES_MANAGER_H_)
