// XTPSyntaxEditLexCfgFileReader.h
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
#if !defined(__XTPSYNTAXEDITLEXCFGFILEREADER_H__)
#define __XTPSYNTAXEDITLEXCFGFILEREADER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditLexCfgFileReader;

namespace XTPSyntaxEditLexAnalyser
{
//{{AFX_CODEJOCK_PRIVATE
	//===========================================================================
	// Summary:
	//     XTPSyntaxEditTokenType type defines type of tokens that read by
	//     CXTPSyntaxEditLexCfgFileReader class from configuration files.
	// See also:
	//     CXTPSyntaxEditLexCfgFileReader
	//===========================================================================
	enum XTPSyntaxEditTokenType
	{
		xtpEditTokType_Unknown,
		xtpEditTokType_Delim,
		xtpEditTokType_Name,
		xtpEditTokType_Value,
		xtpEditTokType_Quoted,
		xtpEditTokType_Comment,
		xtpEditTokType_EOL,
		xtpEditTokType_Control
	};

	//===========================================================================
	// Summary:
	//     XTP_EDIT_LEXPROPINFO helper structure is used to group information
	//     which describes one Lex Class property pair. The property pair is
	//     property name and property value.
	//===========================================================================
	struct _XTP_EXT_CLASS XTP_EDIT_LEXPROPINFO
	{
		XTP_EDIT_LEXPROPINFO();

		XTP_EDIT_LEXPROPINFO(const XTP_EDIT_LEXPROPINFO& rSrc);

		const XTP_EDIT_LEXPROPINFO& operator = (const XTP_EDIT_LEXPROPINFO& rSrc);

		CStringArray    arPropName;     // stores property name chain
		CStringArray    arPropValue;    // stores value for property names with  corresponding index
		int             nLine;
		int             nOffset;
		int             nPropertyLen;
	};

	//===========================================================================
	// Summary:
	//      CXTPSyntaxEditLexPropInfoArray type defines custom array type based on
	//      CArray to store XTP_EDIT_LEXPROPINFO structures.
	// See also:
	//      XTP_EDIT_LEXPROPINFO
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexPropInfoArray : public CArray<XTP_EDIT_LEXPROPINFO, XTP_EDIT_LEXPROPINFO&>
	{
	public:
		CXTPSyntaxEditLexPropInfoArray();

		CXTPSyntaxEditLexPropInfoArray(const CXTPSyntaxEditLexPropInfoArray& rSrc);

		const CXTPSyntaxEditLexPropInfoArray& operator = (const CXTPSyntaxEditLexPropInfoArray& rSrc);
	};

	//===========================================================================
	// Summary:
	//     XTP_EDIT_LEXCLASSINFO structure is used to group information which describes
	//     a set of properties for one Lex Class.
	// See also:
	//     XTP_EDIT_LEXPROPINFO
	//===========================================================================
	struct _XTP_EXT_CLASS XTP_EDIT_LEXCLASSINFO
	{
		XTP_EDIT_LEXCLASSINFO();

		XTP_EDIT_LEXCLASSINFO(const XTP_EDIT_LEXCLASSINFO& rSrc);

		const XTP_EDIT_LEXCLASSINFO& operator = (const XTP_EDIT_LEXCLASSINFO& rSrc);

		CString                         csClassName;        // name to identify lex class
		CXTPSyntaxEditLexPropInfoArray  arPropertyDesc;     // array to store a set of lex class properties
		int                             nStartLine;
		int                             nEndLine;
	};

	//===========================================================================
	// Summary:
	//     Determines custom array type to store XTP_EDIT_LEXCLASSINFO structures.
	// See also:
	//     XTP_EDIT_LEXCLASSINFO.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexClassInfoArray : public CArray<XTP_EDIT_LEXCLASSINFO, XTP_EDIT_LEXCLASSINFO&>
	{
	public:
		CXTPSyntaxEditLexClassInfoArray();

		CXTPSyntaxEditLexClassInfoArray(const CXTPSyntaxEditLexClassInfoArray& rSrc);

		const CXTPSyntaxEditLexClassInfoArray& operator = (const CXTPSyntaxEditLexClassInfoArray& rSrc);

		BOOL m_bModified;
	};
//}}AFX_CODEJOCK_PRIVATE
}

using namespace XTPSyntaxEditLexAnalyser;

//===========================================================================
// Summary: This class provides functionality to read configuration information
//          from configuration files with predefined structure. Its information
//          describes lex class instances that represent various lexical items
//          for given language.
//          You construct instance of CXTPSyntaxEditLexCfgFileReader by calling
//          constructor. Then you should call ReadSource() member function
//          to read data from persistent storage (file for this implementation),
//          and process raw data into XTP_EDIT_LEXCLASSINFO structures. Call
//          GetLexClassInfoArray() member function to return a pointer to an array
//          of XTP_EDIT_LEXCLASSINFO data structures.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditLexCfgFileReader
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSyntaxEditLexCfgFileReader object, handles cleanup and
	//     de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditLexCfgFileReader();

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads data from the configuration file.
	// Parameters:
	//     csFileName: [in] string, containing full path to the
	//                          configuration file.
	// Remarks:
	//     Use this member function to read configuration data from configuration
	//     file with predefined structure. Data loaded into internal data buffer.
	//-----------------------------------------------------------------------
	void ReadSource(const CString& csFileName, BOOL bSaveInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads data from resources.
	// Parameters:
	//     nResourceID:  [in] Resource ID of the HTML configuration data.
	// Remarks:
	//     Use this member function to read configuration data from application
	//     resources with predefined structure. Data loaded into internal data buffer.
	//-----------------------------------------------------------------------
	void ReadSource(UINT nResourceID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns pointer to the internal array with XTP_EDIT_LEXCLASSINFO
	//     structures.
	// Remarks:
	//     Use this member function to get pointer to the internal array with
	//     XTP_EDIT_LEXCLASSINFO structures, then you may operate with array members.
	// Returns:
	//     Pointer to CXTPSyntaxEditLexClassInfoArray array.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditLexClassInfoArray& GetLexClassInfoArray();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns pointer to the internal map with CXTPSyntaxEditLexClassInfoArray
	//     objects.
	// Remarks:
	//     The key is lex class name, the data is pointer to
	//     CXTPSyntaxEditLexClassInfoArray object.
	// Returns:
	//     Pointer to CMapStringToPtr map.
	//-----------------------------------------------------------------------
	CMapStringToPtr& GetLexClassInfoMap() {
		return m_mapLexClassInfo;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to clear map and delete its data.
	// Parameters:
	//      mapInfo - A reference to map object.
	// See Also:
	//      CopyInfoMap
	//-----------------------------------------------------------------------
	void CleanInfoMap(CMapStringToPtr& mapInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to internal classes map and its data.
	//      Use CleanInfoMap to free (delete) map data.
	// Parameters:
	//      mapInfo - [out] A reference to map object.
	// See Also:
	//      CleanInfoMap
	//-----------------------------------------------------------------------
	void CopyInfoMap(CMapStringToPtr& mapInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     Writes LexClasses to given schema configuration file
	// Parameters:
	//      csFileName      - Configuration file name.
	//      arLexClassDesc  - Array with lex class data (description) to save.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL WriteCfgFile(const CString& csFileName, CXTPSyntaxEditLexClassInfoArray& arLexClassDesc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts escape char sequences to corresponding char.
	// Parameters:
	//      strSrc:  [in] CString with text to convert.
	//      bQuoted: [in] Boolean flag determines that text should be interpreted
	//               as text enclosed by single quotes(')Boolean flag determines
	// Remarks:
	//     Use this member function to convert text with escape char sequences
	//     (like \\n, \\t, etc) to corresponding char
	// Returns:
	//     CString with converted text.
	//-----------------------------------------------------------------------
	CString StrToES(CString strSrc, BOOL bQuoted);

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts char to corresponding escape char sequences.
	// Parameters:
	//      strSrc:  [in] CString with text to convert.
	//      bQuoted: [in] Boolean flag determines that text should be interpreted
	//               as text enclosed by single quotes(')
	// Remarks:
	//     Use this member function to convert text with chars that are represented
	//     by escape sequences  to corresponding escape char sequences
	//     (like \\n, \\t, etc)
	// Returns:
	//     CString with converted text.
	//-----------------------------------------------------------------------
	CString ESToStr(CString strSrc, BOOL bQuoted);

private:

	// A map describes whether property description has a flag or not.
	typedef CMap<void*, void*, bool, bool> CMapPtrToBool;
#ifdef _DEBUG
	// Process error messages of file processing. Writes messages into debug window.
	void ProcessFileException(CFileException* pExc);
#endif
	// Process next token from file.
	int GetToken();
	// Process next lex class token from file.
	int GetLexToken();
	// Process corresponding token
	int ProcessUnknowToken(TCHAR tchCurrSymbol);
	int ProcessDelimToken(TCHAR tchCurrSymbol);
	int ProcessNameToken(TCHAR tchCurrSymbol);
	int ProcessValueToken(TCHAR tchCurrSymbol);
	int ProcessQuotedToken(TCHAR tchCurrSymbol);
	int ProcessCommentToken(TCHAR tchCurrSymbol);
	int ProcessEOLToken(TCHAR tchCurrSymbol);
	int ProcessControlToken(TCHAR tchCurrSymbol);
	// Parses single lex class.
	void ParseLexClass(XTP_EDIT_LEXCLASSINFO& infoClass);
	// Searches for lex class.
	XTP_EDIT_LEXCLASSINFO* FindClassDesc(CXTPSyntaxEditLexClassInfoArray& arInfoClass, const CString& csClassName);
	// Searches for property of lex class.
	XTP_EDIT_LEXPROPINFO* FindPropDesc(XTP_EDIT_LEXCLASSINFO* pInfoClass, XTP_EDIT_LEXPROPINFO* pInfoProp, CMapPtrToBool& mapUsed);
	// Read source string by string into string array.
	void ReadSource2(const CString& csFileName, CStringArray& arBuffer);
	// Writes single string into file.
	void WriteString(CFile& file, LPCTSTR pcszString);
	// Writes a number of string into file.
	void WriteStrings(CFile& file, CStringArray& arBuffer, int nFrom, int nTo);
	// Writes a whole property description into file.
	void WriteProp(CFile& file, CString& csOffset, const XTP_EDIT_LEXPROPINFO& newInfoProp);
	void WriteProp(CFile& file, CString& csOffset, const XTP_EDIT_LEXPROPINFO& oldInfoProp, const XTP_EDIT_LEXPROPINFO& newInfoProp, const CStringArray& arBuffer);
	// Process configuration data.
	void Parse(CXTPSyntaxEditLexClassInfoArray& arLexClassDesc);

private:

	int                             m_nCurrLine;        // Current processing line identifier.
	int                             m_nCurrLine_pos;    // Position in current line.
	int                             m_nCurrPos;         // Current position in data buffer.
	int                             m_nEOFPos;          // EOF position.
	int                             m_nPrevPos;         // Previous position in data buffer.
	int                             m_nTokenType;       // Type of processed token.
	BOOL                            m_bReadNames;       // Flag to read names of properties.
	BOOL                            m_bES;
	CString                         m_csDataBuffer;     // Internal buffer to rear file to.
	CString                         m_strToken;         // Text of processed token.
	CMapStringToPtr                 m_mapLexClassInfo;
	CXTPSyntaxEditLexClassInfoArray m_arLexClassInfo;   // lex classes array.

	// Singleton instantiation.
	CXTPSyntaxEditLexCfgFileReader();
	static CXTPSyntaxEditLexCfgFileReader& AFX_CDECL Instance();
	friend CXTPSyntaxEditLexCfgFileReader* AFX_CDECL XTPSyntaxEditLexConfig();
};

//===========================================================================
// Summary:
//      Use this function to get CXTPSyntaxEditLexCfgFileReader global object.
// Returns:
//      A pointer to CXTPSyntaxEditLexCfgFileReader object.
//===========================================================================
AFX_INLINE CXTPSyntaxEditLexCfgFileReader* AFX_CDECL XTPSyntaxEditLexConfig() {
	return &CXTPSyntaxEditLexCfgFileReader::Instance();
}

//===========================================================================
// Summary:
//      This class provides ability of monitoring changes in configuration files
//      and read new data from monitored files in time.
//===========================================================================
class CXTPSyntaxEditConfigurationManager;

// this class monitors files changes
class _XTP_EXT_CLASS CXTPSyntaxEditFileChangesMonitor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditFileChangesMonitor();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSyntaxEditFileChangesMonitor() object, handles cleanup
	//     and de-allocation. Stops monitoring if it doesn't stop yet.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditFileChangesMonitor();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets default monitoring folder.
	// Parameters:
	//     strPath: [in] path string.
	//-----------------------------------------------------------------------
	void SetDefaultFolder(const CString& strPath);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets associated ConfigurationManager object.
	// Parameters:
	//     pConfigMgr: [in] Pointer to the CXTPSyntaxEditConfigurationManager
	//                      object.
	// See Also: CXTPSyntaxEditConfigurationManager
	//-----------------------------------------------------------------------
	void SetConfigurationManager(CXTPSyntaxEditConfigurationManager* pConfigMgr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Add file to to be monitored.
	// Parameters:
	//     strFilename: [in] file name string.
	// Remarks:
	//     Add file to the bunch of files to monitor changes at. First, try to
	//     check file by provided file  name string, then tries file in the
	//     default folder. Ignores bad file names.
	// See Also: CXTPSyntaxEditConfigurationManager
	//-----------------------------------------------------------------------
	BOOL AddMonitorFile(CString& strFilename, DWORD dwOwnerFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Starts thread which will be monitoring folder changes.
	//-----------------------------------------------------------------------
	void StartMonitoring();

	//-----------------------------------------------------------------------
	// Summary:
	//     Stops thread which is monitoring folder changes.
	//-----------------------------------------------------------------------
	void StopMonitoring();

	//-----------------------------------------------------------------------
	// Summary:
	//     Refreshes contents in the folder, update file's status and re-read
	//     changed configuration files.
	//-----------------------------------------------------------------------
	void RefreshFiles();

private:

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove all files which are monitored at the moment
	//-----------------------------------------------------------------------
	void RemoveAll();

private:
	CString m_strPath;
	CWinThread* m_pThread;
	HANDLE      m_evExitThread;
	static UINT AFX_CDECL ThreadMonitorProc(LPVOID);
	BOOL m_bES;

	CXTPSyntaxEditConfigurationManager* m_pConfigMgr; // pointer to the associated configuration manager

	// this class contain information about monitoring file
	class _XTP_EXT_CLASS CFMFileInfo
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Default object constructor.
		//-----------------------------------------------------------------------
		CFMFileInfo();

		CString                     m_strFileName;  // monitoring file name

		BY_HANDLE_FILE_INFORMATION  m_sysFileInfo;  // File information
		BOOL                        m_bExists;      // Is file exist
		DWORD                       m_dwOwnerFlags; // Additional flags

		//-----------------------------------------------------------------------
		// Summary:
		//     Copy operator.
		//-----------------------------------------------------------------------
		const CFMFileInfo& operator = (const CFMFileInfo& rSrc);
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Get file information using file name.
	//-----------------------------------------------------------------------
	BOOL GetFileInfo(LPCTSTR pcszFilePath, BY_HANDLE_FILE_INFORMATION* pInfo);

	CArray<CFMFileInfo, CFMFileInfo&> m_arFiles; // a bunch of filenames to monitor changes at
};

AFX_INLINE void CXTPSyntaxEditFileChangesMonitor::SetConfigurationManager(CXTPSyntaxEditConfigurationManager* pConfigMgr) {
	m_pConfigMgr = pConfigMgr;
}

#endif // !defined(__XTPSYNTAXEDITLEXCFGFILEREADER_H__)
