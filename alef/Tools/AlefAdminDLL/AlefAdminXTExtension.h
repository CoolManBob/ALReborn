// AlefAdminXTExtension.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 02. 17.

//
// Extreme ToolKitPro 에서 확장해야 할 클래스들을 정의한다.
//

#pragma once

// 여기서 부터 내꺼 인클루드
//#include <XTToolkitPro.h>      // Xtreme Toolkit MFC Extensions

#define TABCTRLEX_SELCHANGE		WM_USER + 1

#define MAX_MACRO_COMMAND_LENGTH		255
#define MAX_MACRO_PARAMETER_LENGTH		128

typedef enum _eMacroType
{
	MACROTYPE_NONE = 0,
	MACROTYPE_COMMAND,
	MACROTYPE_FUNCTION,
	MACROTYPE_MAX,
} eMacroType;

typedef enum _eMacroFunctionType
{
	MACROFUNCTIONTYPE_NONE = 0,
	MACROFUNCTIONTYPE_SOS,
	MACROFUNCTIONTYPE_DISCONNECT,
	MACROFUNCTIONTYPE_ALL_SKILL_LEARN,
	MACROFUNCTIONTYPE_ALL_SKILL_UPGRADE,
	MACROFUNCTIONTYPE_MAX,
} eMacroFunctionType;

#define MACRO_TYPE_NAME					"MacroType"
#define MACRO_FUNCTION_TYPE_NAME		"MacroFunctionType"
#define MACRO_COMMAND_NAME				"MacroCommand"
#define MACRO_DESCRIPTION_NAME			"MacroDescription"

typedef BOOL (*MACROFUNCTION)(PVOID pData, PVOID pClass, PVOID pCustData);

#define MACRO_DEFAULT_FILENAME			"AlefAdminMacro.txt"


// CTabCtrlEx class
class CTabCtrlEx : public CXTTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)

// Construction / destruction
public:

	// Constructs a CTabCtrlEx object.
	CTabCtrlEx();

	// Destroys a CTabCtrlEx object, handles cleanup and de-allocation.
	virtual ~CTabCtrlEx();

// Member variables
protected:

// Member functions
public:

	//{{AFX_VIRTUAL(CTabCtrlEx)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTabCtrlEx)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//virtual void OnSelChange();
	//virtual BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
};

// Class definition for Property Grid::SetValue, For Extreme ToolKit Pro
class CXTPSetValueGridItem : public CXTPPropertyGridItem
{
public:
	CXTPSetValueGridItem(CString strCaption, CString strValue);

	virtual void SetValue(CString strValue);
};



// Macro
class CAlefAdminMacro
{
public:
	eMacroType m_eMacroType;
	eMacroFunctionType m_eFunctionType;

	CHAR* m_szCommand;
	CHAR* m_szDescription;
	CHAR* m_szParameter;

	MACROFUNCTION m_pfFuction;

public:
	CAlefAdminMacro();
	virtual ~CAlefAdminMacro();

	virtual void Execute();
};
