// XTPSyntaxEditLexColorFileReader.h: interface for the CXTLexColorFileReader class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITLEXCOLORFILEREADER_H__)
#define __XTPSYNTAXEDITLEXCOLORFILEREADER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditColorTheme;

//-----------------------------------------------------------------------
// Summary:
//      This enumeration describes allowable configuration managers flags.
//-----------------------------------------------------------------------
enum XTPSyntaxEditCfgFlags
{
	xtpEditCfgFileAdd       = 0x0001, // file should be added
	xtpEditCfgFileRemove    = 0x0002, // file should be removed
};

//===========================================================================
// Summary:
//      This storage class is designed to contain information from the color
//      settings file section. It helps easy retrieve color information.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditColorInfo
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      pTheme : [in] Pointer to CXTPSyntaxEditColorTheme object (theme
	//                      this color information belongs to)
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorInfo(CXTPSyntaxEditColorTheme* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      strClassName : [in] Lex Class name
	//      pTheme : [in] Pointer to CXTPSyntaxEditColorTheme object (theme
	//                      this color information belongs to)
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorInfo(const CString& strClassName, CXTPSyntaxEditColorTheme* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditColorTheme() object, handles cleanup and
	//      de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditColorInfo ();

	//-----------------------------------------------------------------------
	// Summary:
	//      Adds parameter.
	// Parameters:
	//      strName     : [in] Parameter name.
	//      strValue    : [in] Parameter value.
	//-----------------------------------------------------------------------
	void AddParam(const CString& strName, const CString& strValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns string of parameter value.
	// Parameters:
	//      strName     : [in] Parameter name.
	//      bDynamic    : [in] If TRUE - search parameter value in parent
	//                         themes (dynamically), otherwise it is searched
	//                         only in the current theme.
	// Returns:
	//      CString object with parameter value.
	//-----------------------------------------------------------------------
	const CString GetParam(const CString& strName, BOOL bDynamic = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns hexadecimal value.
	// Parameters:
	//      strName     : [in] Parameter name.
	//      bDynamic    : [in] If TRUE - search parameter value in parent
	//                         themes (dynamically), otherwise it is searched
	//                         only in the current theme.
	// Returns:
	//      DWORD parameter value.
	//-----------------------------------------------------------------------
	DWORD GetHexParam(const CString& strName, BOOL bDynamic = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns firs parameter position.
	// Returns:
	//      POSITION value.
	//-----------------------------------------------------------------------
	POSITION GetFirstParamNamePosition();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns next parameter position.
	// Parameters:
	//      pos : [in] position.
	// Returns:
	//      POSITION value.
	//-----------------------------------------------------------------------
	const CString GetNextParamName(POSITION& pos);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this function to get lex class name.
	// Returns:
	//      A lex class name
	//-----------------------------------------------------------------------
	const CString GetClassName() const;

private:
	CString m_strClassName;     // lex class name
	CMapStringToString m_mapParams; // name/value parameters map
	CXTPSyntaxEditColorTheme* m_pTheme; // pointer to the theme this color information belongs to
};

//===========================================================================
//===========================================================================
class CXTPSyntaxEditColorInfoArray : public
			CArray<CXTPSyntaxEditColorInfo*, CXTPSyntaxEditColorInfo*>
{
	typedef CArray<CXTPSyntaxEditColorInfo*, CXTPSyntaxEditColorInfo*> TBase;
public:
	CXTPSyntaxEditColorInfoArray(){};
	virtual ~CXTPSyntaxEditColorInfoArray(){
		RemoveAll();
	};

	void RemoveAll()
	{
		for(int i = 0; i < GetSize(); i++) {
			if(GetAt(i)) {
				delete GetAt(i);
			}
		}
		TBase::RemoveAll();
	}
};


class CXTPSyntaxEditColorThemesManager;
class CXTPSyntaxEditPropertiesTheme;

//===========================================================================
// Summary:
//      This class provides functionality to read configuration information
//      from configuration files with predefined structure. Its information
//      describes color settings for corresponding lexical items for given
//      language.
//      You construct instance of CXTPSyntaxEditColorTheme by calling
//      constructor. Then you should call Load() member function
//      to read data from persistent storage (file for this implementation),
//      CXTPSyntaxEditLexColorInfo structures.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditColorTheme
{
	friend class CXTPSyntaxEditPropertiesTheme;

	//===========================================================================
	// Summary:
	//      Determines custom array type to store CXTPSyntaxEditColorInfo structures.
	// See also:
	//      CXTPSyntaxEditColorInfo.
	//===========================================================================
	typedef CMap<CString, LPCTSTR, CXTPSyntaxEditColorInfo *, CXTPSyntaxEditColorInfo *> CXTPSyntaxEditMapLexColorInfo;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pThemesManager : [in] Pointer to the parent themes manager.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorTheme(CXTPSyntaxEditColorThemesManager* pThemesManager);

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditColorTheme() object, handles cleanup and
	//      de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditColorTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//      Parses associated color schema file and fills internal color info
	//      structures.
	// Parameters:
	//      csFileName : [in] configuration file name string.
	//-----------------------------------------------------------------------
	virtual void Load(const CString& csFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns name of the parent schema.
	// Returns:
	//      CString containing name of the parent schema.
	//-----------------------------------------------------------------------
	const CString GetParentThemeName();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns parent schema.
	// Returns:
	//      Pointer to CXTPSyntaxEditColorTheme object if exists;
	//      NULL lf parent schema doesn't exist.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorTheme* GetParentTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns color information.
	// Parameters:
	//      strLexClass         : [in] Lex Class name string
	//      strThemeFilename    : [in] configuration file name string.
	//      bDynamic            : [in] If TRUE - search color info in parent
	//                                 themes (dynamically), otherwise it is
	//                                 searched only in this theme.
	// Returns:
	//      Pointer to CXTPSyntaxEditColorInfo object.
	// Remarks:
	//      strThemeFilename is required to avoid calling recursion.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorInfo* GetColorInfo(const CString& strLexClass,
											const CString& strThemeFilename,
											BOOL bDynamic = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns configuration file name.
	// Returns:
	//      CString containing file name.
	//-----------------------------------------------------------------------
	CString GetFileName();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to save colors to configuration file.
	// Parameters:
	//      strParentThemeName  - The parent theme name.
	//      pColorInfoArray_new - Colors information array.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: GetFileName
	//-----------------------------------------------------------------------
	BOOL WriteCfgFile(CString strParentThemeName,
					  CXTPSyntaxEditColorInfoArray* pColorInfoArray_new);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Cleans up collection.
	//-----------------------------------------------------------------------
	void Cleanup();

	//-----------------------------------------------------------------------
	// Summary:
	//      Parses a single section and creates CXTPSyntaxEditColorInfo
	//      object from it.
	// Parameters:
	//      csSection : [in] section name (in configuration file) string.
	// Returns:
	//      Pointer to CXTPSyntaxEditColorInfo with resulting colors set.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorInfo * ParseSection(const CString& csSection);

private:
	CStringList m_arSections;   // All section names from the theme config file
	CString m_csFileName;       // filename of the corresponding theme config file
	CXTPSyntaxEditMapLexColorInfo m_mapLexColorInfo;    // internal map - lex class name to color info
	CXTPSyntaxEditColorThemesManager* m_pThemesManager; // pointer to the parent themes manager
};

AFX_INLINE CString CXTPSyntaxEditColorTheme::GetFileName() {
	return m_csFileName;
}

//===========================================================================
// Summary: This class provides functionality to store set of all color themes
//          that would be written from configuration files. For short it is
//          custom collection class.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditColorThemesManager
{
	typedef CMap<CString, LPCTSTR, CXTPSyntaxEditColorTheme*, CXTPSyntaxEditColorTheme*> CXTPSyntaxEditMapThemes;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorThemesManager();
	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditColorThemesManager() object,
	//      handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditColorThemesManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads theme by given theme name and configuration file.
	// Parameters:
	//      strThemeName    : [in] theme name string.
	//      strThemeFilename: [in] configuration file name.
	//-----------------------------------------------------------------------
	void LoadTheme(const CString& strThemeName, const CString& strThemeFilename);

	//-----------------------------------------------------------------------
	// Summary:
	//      Adds new entity to "theme name - configuration file" map.
	// Parameters:
	//      strThemeName    : [in] theme name string.
	//      strThemeFilename: [in] configuration file name.
	//-----------------------------------------------------------------------
	void AddThemeInfo(const CString& strThemeName, const CString& strThemeFilename);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns color theme by given name.
	// Parameters:
	//      strThemeName    : [in] theme name string.
	// Returns:
	//      Pointer to CXTPSyntaxEditColorTheme.
	// See also:
	//      class CXTPSyntaxEditColorTheme
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorTheme* GetTheme(const CString& strThemeName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns CStringArray with themes names.
	// Returns:
	//      CStringArray with themes names.
	//-----------------------------------------------------------------------
	CStringArray& GetThemes();

	//-----------------------------------------------------------------------
	// Summary:
	//      Reloads configuration settings from the specified configuration file.
	// Parameters:
	//      csFileName : [in] configuration file name.
	//      nCfgFlags   : [in] action flags.
	// Returns:
	//      Reloaded Theme name if found, or empty string if theme was not found.
	// Remarks:
	//      Does nothing if specified file is not found in the collection of
	//      files from the main configuration file.
	// See also:
	//      enum XTPSyntaxEditCfgFileRemove.
	//-----------------------------------------------------------------------
	CString ReloadFile(const CString& csFileName, int nCfgFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//      Remove all loaded data.
	//-----------------------------------------------------------------------
	void RemoveAll();

private:
	CXTPSyntaxEditMapThemes     m_mapThemes;        // Themes map
	CStringArray        m_arThemeNames;     // Themes names array
	CMapStringToString  m_mapFileToTheme;   // Theme name to configuration file name map
};



typedef CXTPSyntaxEditLexTextSchema CXTPSyntaxEditTextSchema;
typedef CXTPSmartPtrInternalT<CXTPSyntaxEditLexTextSchema> CXTPSyntaxEditLexTextSchemaPtr;

typedef CXTPSyntaxEditLexTextSchemaPtr CXTPSyntaxEditTextSchemaPtr;

//===========================================================================
// Summary:
//      This class provides functionality to store set of all color themes
//      that would be written from configuration files. For short it is
//      custom collection class.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditTextSchemesManager
{
	typedef CMap<CString, LPCTSTR,
				CXTPSyntaxEditTextSchemaPtr,
				CXTPSyntaxEditTextSchemaPtr&> CXTPSyntaxEditMapSchemes;


public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextSchemesManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditTextSchemesManager() object,
	//      handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditTextSchemesManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Add information about text scheme.
	// Parameters:
	//      strSchemeName       : [in] Schema name.
	//      strSchemeFilename   : [in] Schema file name.
	//-----------------------------------------------------------------------
	void AddTextSchemeInfo(XTP_EDIT_SCHEMAFILEINFO& info);

	//-----------------------------------------------------------------------
	// Summary:
	//      Implements common logic on creating new text scheme object and
	//      loading it from file or resources.
	// Parameters:
	//      strSchemeName       : [in] Schema name.
	//      nResourceID         : [in] Schema HTML resource ID.
	//      strSchemeFilename   : [in] Schema file name.
	//-----------------------------------------------------------------------
	void LoadTextScheme(XTP_EDIT_SCHEMAFILEINFO& info);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns schema by its name.
	// Parameters:
	//      strSchemeName       : [in] schema name.
	// Returns:
	//      CXTPSyntaxEditTextSchemaPtr object.
	// See also:
	//      CXTPSyntaxEditTextSchemaPtr.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextSchemaPtr GetSchema(const CString& strSchemeName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns schema name by its file name.
	// Parameters:
	//      csFileName  : [in] file name.
	// Returns:
	//      CString object.
	//-----------------------------------------------------------------------
	const CString GetSchemaName(const CString& csFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns file name by its schema name.
	// Parameters:
	//      strSchemeName   : [in] schema name.
	// Returns:
	//      CString object.
	//-----------------------------------------------------------------------
	const CString GetSchemaFileName(const CString& strSchemeName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns all schemes names
	// Returns:
	//      Reference to CStringArray.
	//-----------------------------------------------------------------------
	CStringArray& GetSchemes();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to get schemas list.
	// Returns:
	//      A reference to CXTPSyntaxEditSchemaFileInfoList object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditSchemaFileInfoList& GetSchemaList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Finds a schema by file extension.
	// Parameters:
	//      strFileExt : [in] file extension string.
	// Returns:
	//      CXTPSyntaxEditTextSchemaPtr object.
	// See also:
	//      CXTPSyntaxEditTextSchemaPtr.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextSchemaPtr FindSchema(const CString& strFileExt);

	//-----------------------------------------------------------------------
	// Summary:
	//      Applys theme to all text schemes.
	// Parameters:
	//      pTheme : [in] Pointer to theme to be set.
	// See also:
	//      class CXTPSyntaxEditColorTheme.
	//-----------------------------------------------------------------------
	void SetTheme(CXTPSyntaxEditColorTheme* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//      Reloads configuration settings from the specified configuration file.
	// Parameters:
	//
	// Does nothing if specified file is not found in the collection of files
	// from the main configuration file
	//-----------------------------------------------------------------------
	BOOL ReloadFile(const CString& csFileName, int nCfgFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	// Remove all loaded data.
	//-----------------------------------------------------------------------
	void RemoveAll();

private:
	CXTPSyntaxEditMapSchemes        m_mapSchemes;
	CXTPSyntaxEditSchemaFileInfoList    m_listSchemes;
	CStringArray                    m_arSchemeNames;
	CCriticalSection                m_csReadFile;
};

AFX_INLINE CStringArray& CXTPSyntaxEditTextSchemesManager::GetSchemes() {
	return m_arSchemeNames;
}
AFX_INLINE CXTPSyntaxEditSchemaFileInfoList& CXTPSyntaxEditTextSchemesManager::GetSchemaList() {
	return m_listSchemes;
}

//===========================================================================
// Summary: This class manages all configuration parameters of the control.
//          It worries about read configuration files from disk when the Edit
//          control is starting, store it in internal structures and refresh
//          parameters in case configuration files are changed after control
//          was started.
// See Also: CXTPSyntaxEditFileChangesMonitor, CXTPSyntaxEditColorThemesManager
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditConfigurationManager : public CXTPCmdTarget
{
public:
	// Configuration manager sub-objects
	enum XTPSyntaxEditCfgObjects
	{
		xtpEditCfgObjSchMan     = 0x0001, // Schemes manager
		xtpEditCfgObjThemeMan   = 0x0002, // Themes manager
		xtpEditCfgObjMainConfig = 0x0004, // Main configuration file
	};

	//-----------------------------------------------------------------------
	// Summary: Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditConfigurationManager();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPSyntaxEditConfigurationManager() object,
	//          handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditConfigurationManager();

	//-----------------------------------------------------------------------
	// Input:   pParser - Pointer to the CXTPSyntaxEditLexParser object.
	// Summary: Sets lex parser.
	// See Also: CXTPSyntaxEditLexParser
	//-----------------------------------------------------------------------
	//void SetParser(CXTPSyntaxEditLexParser* pParser);

	//-----------------------------------------------------------------------
	// Input:   strConfigFilePath - string of file's full path.
	// Summary: Reloads settings from provided file.
	// Remarks: Reloads configuration settings from the specified new config file.
	//-----------------------------------------------------------------------
	void ReloadConfig(const CString& strConfigFilePath);

	//-----------------------------------------------------------------------
	// Reloads configuration settings from the stored main config file.
	//-----------------------------------------------------------------------
	void ReloadConfig();

	//-----------------------------------------------------------------------
	// Asynchronously reloads configuration settings from the stored main config file.
	//-----------------------------------------------------------------------
	void ReloadConfigAsync(int nAsyncThreadPriority = THREAD_PRIORITY_NORMAL);

	//--------------------------------------------------------------------
	// Summary:     Get main configuration file name.
	// Remarks:     Return configuration file name previously stored by
	//              ReloadConfig(<configFile>).
	// Returns:     Main configuration file name.
	// See also:    CXTPSyntaxEditConfigurationManager::ReloadConfig().
	//--------------------------------------------------------------------
	CString GetConfigFile() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Reloads configuration settings from the specified configuration file.
	// Parameters:
	//      csFileName : [in] configuration file name.
	//      nCfgFlags   : [in] action flags.
	// Returns:
	//      Reloaded Theme name if found, or empty string if theme was not found.
	// Remarks:
	//      Does nothing if specified file is not found in the collection of
	//      files from the main configuration file.
	// See also:
	//      enum XTPSyntaxEditCfgFileRemove.
	//-----------------------------------------------------------------------
	void ReloadFile(CString csFileName, DWORD dwOwnerFlags, int nCfgFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns color theme manager.
	// Returns:
	//      Reference to CXTPSyntaxEditColorThemesManager object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditColorThemesManager& GetThemeManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns text theme manager.
	// Returns:
	//      Reference to CXTPSyntaxEditTextSchemesManager object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextSchemesManager& GetTextSchemesManager();

	//-----------------------------------------------------------------------
	// Summary:
	//-----------------------------------------------------------------------
	void SetTheme(const CString& strThemeName, CXTPSyntaxEditTextSchema* pActiveSch = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Closes configuration monitor session.
	//-----------------------------------------------------------------------
	void Close();
	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current theme name.
	//-----------------------------------------------------------------------
	const CString& GetCurrentTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns notification connection.
	// Returns:
	//      CXTPNotifyConnectionPtr pointer.
	//-----------------------------------------------------------------------
	CXTPNotifyConnection* GetConnection();

private:

	static UINT AFX_CDECL ReloadConfigAsync_Proc(LPVOID pThis);

	CString m_strMainIniFilename;   // Stores main configuration file name.

	CXTPSyntaxEditFileChangesMonitor m_FolderMonitor;   // Stores folder monitor object.
	CXTPSyntaxEditColorThemesManager m_ColorThemeManager;   // Stores color theme manager.
	CXTPSyntaxEditTextSchemesManager m_TextSchemesManager;  // Stores text theme manager.

	CString m_strCurrentThemeName;  // Current theme name

	CXTPNotifyConnectionMT*         m_pConnectMT; // Notification connection.
	CCriticalSection                m_DataLockerCS; // Critical section

	HANDLE  m_hReloadThread;        // Stores handle of monitoring thread.
	CEvent* m_pBreakReloadEvent;    // Stores pointer to event.
};

AFX_INLINE const CString CXTPSyntaxEditColorInfo::GetClassName() const {
	return m_strClassName;
}

AFX_INLINE CXTPSyntaxEditColorThemesManager& CXTPSyntaxEditConfigurationManager::GetThemeManager() {
	return m_ColorThemeManager;
}

AFX_INLINE CXTPSyntaxEditTextSchemesManager& CXTPSyntaxEditConfigurationManager::GetTextSchemesManager() {
	return m_TextSchemesManager;
}

AFX_INLINE CXTPNotifyConnection* CXTPSyntaxEditConfigurationManager::GetConnection() {
	return m_pConnectMT;
}

AFX_INLINE const CString& CXTPSyntaxEditConfigurationManager::GetCurrentTheme() {
	return m_strCurrentThemeName;
}

#endif // !defined(__XTPSYNTAXEDITLEXCOLORFILEREADER_H__)
