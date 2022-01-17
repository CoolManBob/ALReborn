/*======================================================

	AuRegistry.cpp

======================================================*/

#include "AuRegistry.h"

/****************************************************/
/*		The Implementation of Registry class		*/
/****************************************************/
//
#define _IMAX_VALUE		(512+1)

static CHAR g_szApplication[_IMAX_VALUE] = {0, };


void AuRegistry::ModuleInitialize(CHAR *pszApplication)
	{
	strcpy(g_szApplication, pszApplication);
	}


void AuRegistry::ModuleCleanup()
	{
	}


AuRegistry::~AuRegistry()
	{
	Close();
	}


BOOL AuRegistry::Open(CHAR *pszSection, BOOL fWrite)
	{
	CHAR sz[_IMAX_VALUE];
	strcpy(sz, g_szApplication);
	if (NULL != pszSection)
		{
		strcat(sz, _T("\\"));
		strcat(sz, pszSection);
		}

	m_fWrite = fWrite;
	m_hKey = CreateRegistryKey(HKEY_LOCAL_MACHINE, sz, fWrite);

	return (NULL != m_hKey);
	}


BOOL AuRegistry::Open(HKEY hKeyParent, CHAR *pszSection, BOOL fWrite)
	{
	m_fWrite = fWrite;
	m_hKey = CreateRegistryKey(hKeyParent, pszSection, fWrite);

	return (NULL != m_hKey);
	}


void AuRegistry::Close()
	{
	if (NULL != m_hKey)
		{
		DestroyRegistryKey(m_hKey);
		m_hKey = NULL;
		}
	}




//	Internal Helper
//================================================
//
HKEY AuRegistry::CreateRegistryKey(HKEY hkeyParent, CHAR *lpszSubKey, BOOL fWrite)
	{
	HKEY	hKey;
	DWORD	Disposition;
	LONG	Result;

	CHAR	szSubKey[_IMAX_VALUE];
	if (HKEY_LOCAL_MACHINE == hkeyParent)
		{
		strcpy(szSubKey, _T("SOFTWARE"));
		if (NULL != lpszSubKey)
			{
			strcat(szSubKey, _T("\\"));
			strcat(szSubKey, lpszSubKey);
			}
		}
	else
		strcpy(szSubKey, lpszSubKey);

	if (fWrite)
		{
		Result = RegCreateKeyEx(hkeyParent, szSubKey,
					0, NULL, REG_OPTION_NON_VOLATILE,
					KEY_WRITE, NULL,
					&hKey, &Disposition);

		// if we failed, note it, and leave 
		if (ERROR_SUCCESS != Result)
			{
//			TRACE(_T("!!! ERROR : Failed to create key for registery in AuRegistry::CreateRegistryKeyForWrite()\n"));
			return NULL;
			}
		else
			return hKey;
		}
	else
		{
		Result = RegOpenKeyEx(hkeyParent, szSubKey,
					0, KEY_READ, &hKey);
		return (ERROR_SUCCESS != Result ? NULL: hKey);
		}
	}

void AuRegistry::DestroyRegistryKey(HKEY hKey)
	{
	RegCloseKey(hKey);
	}

BOOL AuRegistry::WriteString(CHAR *pszValueName, CHAR *lpszValue, INT32 lValue)
	{
	if (lValue < 0)
		lValue = lstrlen(lpszValue) + 1;

	LONG	Result = RegSetValueEx(m_hKey, pszValueName, 
                             0, REG_SZ,
                             (CONST BYTE *)lpszValue, (DWORD)(lValue*sizeof(TCHAR)));
 
	if (ERROR_SUCCESS != Result)
		{
//		TRACE(_T("!!! ERROR : Failed to write value string for registery in AuRegistry::WriteRegisterString()\n"));
		return FALSE;
		}

	return TRUE;
	}

BOOL AuRegistry::WriteNumber(CHAR *pszValueName, INT32 lValue)
	{
	LONG	Result = RegSetValueEx(m_hKey, pszValueName, 
                             0, REG_DWORD, 
                             (CONST BYTE *)&lValue, (DWORD)4);
 
	if (ERROR_SUCCESS != Result)
		{
//		TRACE(_T("!!! ERROR : Failed to write value for registery in AuRegistry::CreateRegistryKeyForWrite()\n"));
		return FALSE;
		}

	return TRUE;
	}


BOOL AuRegistry::ReadString(CHAR *pszValueName, CHAR *lpValue, UINT32 &ulSize)
	{
	DWORD	Type;
	LONG	Result = RegQueryValueEx(m_hKey, pszValueName, 
                             0, &Type,
                             (BYTE *)lpValue, (LPDWORD)&ulSize);
  #ifdef _UNICODE
	ulSize /= sizeof(TCHAR);
  #endif
 
	if (ERROR_SUCCESS == Result && REG_SZ == Type)
		{
		lpValue[ulSize] = _T('\0');
		return TRUE;
		}
	else
		return FALSE;
	}
/*
BOOL AuRegistry::ReadString(CHAR *pszValueName, CString& sz)
	{
	if (m_fWrite)
		return WriteString(pszValueName, sz);
	else
		{
		TCHAR	buf[_IMAX_VALUE];
		if (!ReadString(pszValueName, buf, _IMAX_VALUE))
			return FALSE;

		sz = buf;
		return TRUE;
		}
	}
*/

BOOL AuRegistry::ReadNumber(CHAR *pszValueName, INT32 *plValue)
	{
	DWORD	nValue = 4;
	DWORD	Type;
	LONG	Result = RegQueryValueEx(m_hKey, pszValueName, 
                             0, &Type,
                             (BYTE *)plValue, &nValue);
	return (ERROR_SUCCESS == Result && REG_DWORD == Type);
	}
/*
BOOL AuRegistry::WriteStringArray(CHAR *pszValueName, const CStringArray& szs, const CString* pszDef)
	{
	LONG	Result;

	// ... delete the previous ones
	::RegDeleteKey(m_hKey, pszValueName);

	// ... create Key
	DWORD	Disposition;
	HKEY hKey;
	Result = RegCreateKeyEx(m_hKey, pszValueName,
				0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_WRITE, NULL,
				&hKey, &Disposition);

	if (ERROR_SUCCESS != Result)
		{
		TRACE(_T("!!! ERROR : Failed to create key for registery in AuRegistry::WriteStringArray()\n"));
		return FALSE;
		}

	// ... write string array
	TCHAR szName[128];
	for (int i=0; i<szs.GetSize(); i++)
		{
		_itoa(i, szName, 10);	// generate name

		Result = ::RegSetValueEx(hKey, szName,
                             0, REG_SZ, 
                             (CONST BYTE *)(LPCTSTR)szs[i], (DWORD)(szs[i].GetLength()*sizeof(TCHAR)));
 		if (ERROR_SUCCESS != Result)
			{
//			TRACE(_T("!!! ERROR : failed to write value string for registery in AuRegistry::WriteStringArray()\n"));
			return FALSE;
			}
		}

	// ... write the default
	if (NULL != pszDef)
		{
		Result = ::RegSetValueEx(hKey, NULL,
                             0, REG_SZ, 
                             (CONST BYTE *)(LPCTSTR)(*pszDef), (DWORD)(pszDef->GetLength()*sizeof(TCHAR)));
 		if (ERROR_SUCCESS != Result)
			{
//			TRACE(_T("!!! ERROR : failed to write value string for registery in AuRegistry::WriteStringArray()\n"));
			return FALSE;
			}
		}

	// ... clean up
	::RegCloseKey(hKey);
	return TRUE;
	}

BOOL AuRegistry::ReadStringArray(CHAR *pszValueName, CStringArray& szs, CString* pszDef)
	{
	LONG	Result;

	// ... create Key
	HKEY hKey;
	Result = RegOpenKeyEx(m_hKey, pszValueName,
				0, KEY_READ, &hKey);

	if (ERROR_SUCCESS != Result)
		{
//		TRACE(_T("!!! ERROR : failed to create key for registery in AuRegistry::ReadStringArray()\n"));
		return FALSE;
		}

	// ... read string array
	DWORD	Type;
	TCHAR szName[128];
	TCHAR szValue[_IMAX_VALUE];
	DWORD nValue;
	for (int i=0; ; i++)
		{
		_itoa(i, szName, 10);	// generate name

		nValue = _IMAX_VALUE;
		Result = RegQueryValueEx(hKey, szName, 
                             0, &Type,
                             (BYTE *)szValue, &nValue);
	  #ifdef IUNICODE
		nValue /= sizeof(TCHAR);
	  #endif
 		if (ERROR_SUCCESS != Result || REG_SZ != Type)
			{	// not error
			break;
			}

		szValue[nValue] = _TCHAR('\0');
		szs.Add(szValue);
		}

	// ... read the default
	if (NULL != pszDef)
		{
		Result = RegQueryValueEx(hKey, NULL, 
                             0, &Type,
                             (BYTE *)szValue, &nValue);
	  #ifdef IUNICODE
		nValue /= sizeof(TCHAR);
	  #endif
 		if (ERROR_SUCCESS == Result && REG_SZ == Type)
			{
			szValue[nValue] = _TCHAR('\0');
			*pszDef = szValue;
			}
		}

	// ... clean up
	::RegCloseKey(hKey);
	return TRUE;
	}
*/
#undef _IMAX_VALUE

BOOL AuRegistry::Int(CHAR *pszValueName, INT32 &i)
	{
	if (m_fWrite)
		return WriteNumber(pszValueName, i);
	else
		{
		if (ReadNumber(pszValueName, &i))
			{
			return TRUE;
			}
		else
			return FALSE;
		}
	}

BOOL AuRegistry::Bool(CHAR *pszValueName, BOOL& f)
	{
	if (m_fWrite)
		return WriteNumber(pszValueName, f);
	else
		{
		INT32 i;
		if (ReadNumber(pszValueName, &i))
			{
			f = (BOOL)i;
			return TRUE;
			}
		else
			return FALSE;
		}
	}

/*
#define _MAX_NUMBER_FORMAT_	256
BOOL AuRegistry::Point(CHAR *pszValueName, LPPOINT lpPoint)
	{
	TCHAR	buf[_MAX_NUMBER_FORMAT_];

	if (m_fWrite)
		{
		wsprintf(buf, _T("%d %d"), lpPoint->x, lpPoint->y);
		return WriteString(pszValueName, buf);
		}
	else
		{
		if (!ReadString(pszValueName, buf, _MAX_NUMBER_FORMAT_))
			return FALSE;

	  #ifdef IUNICODE
		swscanf(buf, _T("%d %d"), &lpPoint->x, &lpPoint->y);
	  #else
		sscanf(buf, _T("%d %d"), &lpPoint->x, &lpPoint->y);
	  #endif
		return TRUE;
		}
	}

BOOL AuRegistry::Size(CHAR *pszValueName, LPSIZE lpSize)
	{
	TCHAR	buf[_MAX_NUMBER_FORMAT_];

	if (m_fWrite)
		{
		wsprintf(buf, _T("%d %d"), lpSize->cx, lpSize->cy);
		return WriteString(pszValueName, buf);
		}
	else
		{
		if (!ReadString(pszValueName, buf, _MAX_NUMBER_FORMAT_))
			return FALSE;

	  #ifdef IUNICODE
		swscanf(buf, _T("%d %d"), &lpSize->cx, &lpSize->cy);
	  #else
		sscanf(buf, _T("%d %d"), &lpSize->cx, &lpSize->cy);
	  #endif
		return TRUE;
		}
	}

BOOL AuRegistry::Rect(CHAR *pszValueName, LPRECT lprc)
	{
	TCHAR	buf[_MAX_NUMBER_FORMAT_];

	if (m_fWrite)
		{
		wsprintf(buf, _T("%d %d %d %d"), lprc->left, lprc->top, lprc->right, lprc->bottom);
		return WriteString(pszValueName, buf);
		}
	else
		{
		if (!ReadString(pszValueName, buf, _MAX_NUMBER_FORMAT_))
			return FALSE;

	  #ifdef IUNICODE
		swscanf(buf, _T("%d %d %d %d"), &lprc->left, &lprc->top, &lprc->right, &lprc->bottom);
	  #else
		sscanf(buf, _T("%d %d %d %d"), &lprc->left, &lprc->top, &lprc->right, &lprc->bottom);
	  #endif
		return TRUE;
		}
	}
#undef _MAX_NUMBER_FORMAT_
*/
